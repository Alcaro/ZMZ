#include "netplay.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#include "udpsock.c"

#ifdef __GNUC__
#pragma GCC diagnostic error "-Wpadded"//putting it here is undefined behaviour in gcc 4.5; putting it earlier throws for padding in libretro.h and I can't change that.
//#pragma GCC diagnostic push//only exists in gcc 4.6
//#pragma GCC diagnostic error "-Wpadded"
#endif

#define WIREVERSION 0
#ifndef DEBUG
#error no seriously.
#endif

struct packet_base {
	uint32 packtype;
	uint32 signature;
};

struct pack_setup {
	uint32 packtype;//=1
	//if little endian communicates with big endian, packet type is backwards too and the receiver can turn it back; however, big endian is rare and can't be tested
	uint32 signature;//randomly generated from whatever the app feels like; packets with wrong signature are ignored
	
	uint32 format_version;//0
	uint32 core_hash;
	uint32 core_version_hash;
	uint32 game_hash;
	uint32 sram_size;
	//the above must be identical or both abort
	
	char nick[64];
};

struct pack_setup_data {
	uint32 packtype;//=2
	uint32 my_signature;
	
	//it is safe to not tell what this is since it's used for sram only
	uint32 start;
	uint8 data[1024];//udp supports up to slightly below 64KB; slightly above 1KB is fully safe
};

struct pack_setup_ack {
	uint32 packtype;//=3
	uint32 my_signature;
	
	uint8 ack[];//one bit per kilobyte; ack[0]&0x01 for first, ack[0]&0x02 for second
	//this allows 8MB SRAM for 1KB of ACKs; more than that doesn't exist in any legit retro game
};

struct pack_gameplay {
	uint32 packtype;//=4
	uint32 my_signature;
	
	uint32 this_frame;//how many frames the sender has predictively emulated
	uint32 acknowledge_frame;//how much the packet sender has received from the other party and conclusively emulated
	uint32 num_frames;//number of frames in this packet; should be identical to the difference
	uint16 data[64];//most of this should remain unused
	
	uint32 chat_len;
	uint32 chat_frame;//when the message was sent; multiple messages can not be sent without a roundtrip
	uint8 chatmsg[];//this should not be an issue because if it is then you're spamming.
};

struct pack_abort {
	uint32 packtype;//=5
	uint32 my_signature;
	
	uint32 why;
};

const char * abortreasons[]={
	"Libretro core mismatch",
	"Libretro core version mismatch",
	"Game mismatch",//used for all SRAM errors too
};

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wpadded"
//#pragma GCC diagnostic pop
#endif

struct netplay_setup {
	//splitting this off so it can be freed once the channel is open
	int frames_until_resend;
	
	void * sram;//we can just ask the core for this, but lazy...
	uint32 sram_size;
	uint8 * sram_ack;
	//server:
	//0=not sent
	//1=not acknowledged
	//2=acknowledged
	//client:
	//same, except 1 is impossible
	
	uint32 corehash;
	uint32 coreverhash;
	uint32 gamehash;
	
	uint16 * videodata;
};

struct netplay {
	struct socket sock;
	int state;
#define state_s_waitcon 1
#define state_s_sendsetup 2
#define state_s_sendsram 3
#define state_c_sendsetup 4
#define state_c_getsram 5
#define state_playing 6
	uint32 signature;
	uint32 myplayerid;//0 or 1
	struct netplay_setup * setup;
	struct retro_callbacks * cb;
	
	char mynick[65];
	char othernick[65];
};

static netplay* ghandle=NULL;

extern unsigned int CRC32;

#include <zlib.h>

netplay* netplay_create(const char * mynick, const char * host, unsigned short port, struct retro_callbacks * cb)
{
	netplay* handle=malloc(sizeof(struct netplay));
	memset(handle, 0, sizeof(struct netplay));
	
	if (!socket_new(&handle->sock, host, port)) goto err;
	
	handle->setup=malloc(sizeof(struct netplay_setup));
	memset(handle->setup, 0, sizeof(struct netplay_setup));
	
	handle->setup->sram_size=retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
	handle->setup->sram=retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
	handle->setup->sram_ack=malloc(handle->setup->sram_size/1024);
	memset(handle->setup->sram_ack, 0, handle->setup->sram_size/1024);
	
	handle->setup->videodata=malloc(2*256*224);
	memset(handle->setup->videodata, 0, 2*256*224);
	
	handle->setup->frames_until_resend=1;
	
	struct retro_system_info info;
	retro_get_system_info(&info);
	handle->setup->corehash=crc32(0, info.library_name, strlen(info.library_name));
	handle->setup->coreverhash=crc32(0, info.library_version, strlen(info.library_version));
	handle->setup->gamehash=CRC32;
printf("MYHASHES=%.8X,%.8X,%.8X\n",handle->setup->corehash,handle->setup->coreverhash,handle->setup->gamehash);
	
	handle->myplayerid=(!host);
	handle->cb=cb;
	strcpy(handle->mynick, mynick);
	strcpy(handle->othernick, "");
	
	if (host)
	{
		handle->signature =(rand()&255);
		handle->signature^=(rand()&255)<<8;
		handle->signature^=(rand()&255)<<16;//rand_max is less than uin32_max on windows, just shuffle it a bit.
		handle->signature^=(rand()&255)<<24;
		if (handle->signature==0) handle->signature=0x26594131;//signature 0 is invalid, just map it to something random.
	}
	
	if (host) handle->state=state_c_sendsetup;
	else handle->state=state_s_waitcon;
	return handle;
	
err:
	free(handle);
	return NULL;
}

//client (P2) sends struct setup every 200ms until it gets one back
//server (P1) sends struct setup every 200ms until it gets an empty struct setup_ack
//client sends struct setup_ack for 0 bytes until it gets a setup_data
//server sends setup_data for any kilobyte marked unsent every 17ms (); kilobyte is marked sent
//when client gets one, setup_ack is sent
//once server is out of unsent kilobytes, all sent ones are marked unsent if not acknowledged, and resent; however, reset is not done more often than once per 200ms
//once all kilobytes are sent, setup is done

void netplay_run(netplay* handle)
{
	ghandle=handle;
	char raw_packet[2048];
	int packlen;
	while (packlen=socket_read(&handle->sock, raw_packet, 2048))
	{
		struct packet_base * pack_base=(void*)raw_packet;
		if (handle->signature && pack_base->signature!=handle->signature) continue;
		handle->signature=pack_base->signature;
		
		if (pack_base->packtype==1 && handle->state==state_s_waitcon)
		{
			struct pack_setup * packet=(void*)raw_packet;
			
			memcpy(handle->othernick, packet->nick, 64);
			printf("SRC=[%s]", packet->nick);
			
			printf("MYHASHES=%.8X,%.8X,%.8X\n",handle->setup->corehash,handle->setup->coreverhash,handle->setup->gamehash);
			printf("RMHASHES=%.8X,%.8X,%.8X\n",packet->core_hash,packet->core_version_hash,packet->game_hash);
			
			handle->state=state_s_sendsetup;
		}
	}
	if (handle->state!=state_playing)
	{
		if (handle->state==state_c_sendsetup)
		{
			handle->setup->frames_until_resend--;
			if (!handle->setup->frames_until_resend)
			{
				struct pack_setup packet = { 1, handle->signature,
																		WIREVERSION,
																		handle->setup->corehash, handle->setup->coreverhash, handle->setup->gamehash,
																		handle->setup->sram_size };
				strcpy(packet.nick, handle->mynick);
				socket_write(&handle->sock, &packet, sizeof(packet));
				handle->setup->frames_until_resend=60/5;
			}
		}
		//free(handle->setup->sram_ack);
		//free(handle->setup);
		//handle->setup=NULL;
		handle->cb->video_refresh_cb(handle->setup->videodata, 256, 224, 512);
	}
	
	if (handle->state==state_playing)
	{
		retro_run();
	}
}

void netplay_free(netplay* handle)
{
	if (handle->setup)
	{
		if (handle->setup->sram_ack) free(handle->setup->sram_ack);
		free(handle->setup);
	}
	socket_close(&handle->sock);
	free(handle);
}



void netplay_input_poll(void)
{
	ghandle->cb->input_poll_cb();
}

int16_t netplay_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
	return ghandle->cb->input_state_cb(port, device, index, id);
}

void netplay_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
	ghandle->cb->video_refresh_cb(data, width, height, pitch);
}

void netplay_audio_sample(int16_t left, int16_t right)
{
	ghandle->cb->audio_sample_cb(left, right);
}

size_t netplay_audio_sample_batch(const int16_t *data, size_t frames)
{
	return ghandle->cb->audio_sample_batch_cb(data, frames);
}
