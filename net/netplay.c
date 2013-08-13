#include "netplay.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

#define _WIN32_WINNT 0x0501
#include <winsock2.h>
#include <ws2tcpip.h>

struct socket {
	int fd;
	bool connected;
	struct sockaddr * addr;
	int addrlen;
};

static bool socket_new(struct socket * sock, const char * host, unsigned short port)
{
#ifdef _WIN32
	static bool inited=false;
	if (!inited)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		inited=true;
	}
#endif
	
	memset(sock, 0, sizeof(sock));
	sock->fd=-1;
	
	char portstr[16];
	sprintf(portstr, "%i", port);
	
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags=(host?0:AI_PASSIVE);
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	
	struct addrinfo * addr=NULL;
	struct addrinfo * addrwalk;
	
	if (getaddrinfo(NULL, portstr, &hints, &addr)!=0) goto err;
	
	addrwalk=addr;
	
	do {
		sock->fd=socket(addrwalk->ai_family, addrwalk->ai_socktype, addrwalk->ai_protocol);
		if (sock->fd<0)
			continue;
		
		if (host || (bind(sock->fd, addrwalk->ai_addr, addrwalk->ai_addrlen)==0)) break;
		
		close(sock->fd);
	} while((addrwalk=addrwalk->ai_next) != NULL);
	
#ifdef _WIN32
	u_long iMode=1;
	ioctlsocket(sock->fd, FIONBIO, &iMode); 
#endif
#ifndef _WIN32
	fcntl(core->fd, F_SETFL, fcntl(core->fd, F_GETFL, 0)|O_NONBLOCK);
#endif
	
	if (sock->fd<0) goto err;
	
	sock->connected=(host);
	
	sock->addrlen=addr->ai_addrlen;
	sock->addr=malloc(addr->ai_addrlen);
	if (sock->connected) memcpy(sock->addr, addr->ai_addr, addr->ai_addrlen);
	
	freeaddrinfo(addr);
	sock->connected=true;
	return true;
	
err:
	if (addr) freeaddrinfo(addr);
	if (sock->fd>=0) close(sock->fd);
	sock->fd=-1;
	return false;
}

static int socket_read(struct socket * sock, void * buffer, int buflen)
{
	if (sock->connected)
	{
		char addrcopy[sock->addrlen];
		int ret;
		do {
printf("GET2=%i\n",ret);
			ret=recvfrom(sock->fd, buffer, buflen, 0, (struct sockaddr*)addrcopy, &sock->addrlen);
		} while (ret>0 && !!memcmp(sock->addr, addrcopy, sock->addrlen));
		if (ret<=0) ret=0;
		return ret;
	}
	else
	{
		int ret=recvfrom(sock->fd, buffer, buflen, 0, sock->addr, &sock->addrlen);
printf("GET1=%i\n",ret);
		if (ret<=0) ret=0;
		else sock->connected=true;
		return ret;
	}
}

static void socket_write(struct socket * sock, void * buffer, int buflen)
{
#ifndef DEBUG
#error no seriously.
#endif
//if (rand()&1) return;//50% packet loss omg panic! extreme circumstances are the best for testing that stuff works
printf("SEND=%i,%i",sock->connected,buflen);
	if (sock->connected) printf(",%i",sendto(sock->fd, buffer, buflen, 0, sock->addr, sock->addrlen));
puts("");
}

static void socket_close(struct socket * sock)
{
	if (sock->addr) free(sock->addr);
	sock->addr=0;
	sock->addrlen=0;
	if (sock->fd>=0) close(sock->fd);
	sock->fd=-1;
}

static const char * socket_other_end(struct socket * sock)
{
	if (!sock->connected) return NULL;
	static int addrstrlen=16;
	static char * addrstr=NULL;
	if (!addrstr) addrstr=malloc(addrstrlen);
	
retry: ;
	int err=getnameinfo(sock->addr, sock->addrlen, addrstr, addrstrlen, NULL, 0, NI_NUMERICHOST);
	if (!err) return addrstr;
	if (err==WSAEFAULT)
	{
		addrstrlen*=2;
		addrstr=realloc(addrstr, addrstrlen);
		goto retry;
	}
	return NULL;
	
	////wtf, inet_ntop doesn't exist on xp?
	//if (inet_ntop(sock->addr->sa_family, sock->addr, addrstr, addrstrlen)==0/*wtf, this one is documented to return pointer, why doesn't it*/)
	//{
	//	if (WSAGetLastError()==ERROR_INVALID_PARAMETER && addrstrlen<1024)
	//	{
	//		addrstrlen*=2;
	//		addrstr=realloc(addrstr, addrstrlen);
	//		goto retry;
	//	}
	//	return NULL;
	//}
}

#ifdef __GNUC__
#pragma GCC diagnostic error "-Wpadded"//putting it here is undefined behaviour in gcc 4.5; putting it earlier throws for padding in libretro.h and I can't change that.
//#pragma GCC diagnostic push//only exists in gcc 4.6
//#pragma GCC diagnostic error "-Wpadded"
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

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wpadded"
//#pragma GCC diagnostic pop
#endif

struct netplay_setup {
	//splitting this off so it can be freed once the channel is open
	int frames_until_resend;
	
	void * sram;
	uint32 sram_size;
	uint8 * sram_ack;
	//server:
	//0=not sent
	//1=not acknowledged
	//2=acknowledged
	//client:
	//same, 1 is impossible
	
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
	uint32 myplayerid;//0, 1
	struct netplay_setup * setup;
	struct retro_callbacks * cb;
};

static netplay* ghandle=NULL;

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
	
	handle->myplayerid=(!host);
	handle->cb=cb;
	
	if (host)
	{
		handle->signature =rand()&255;
		handle->signature^=rand()&255<<8;
		handle->signature^=rand()&255<<16;//rand_max is less than uin32_max on windows, just shuffle it a bit.
		handle->signature^=rand()&255<<24;
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
printf("SIG=[%.8X:%.8X]",pack_base->signature,handle->signature);
		if (handle->signature && pack_base->signature!=handle->signature) continue;
		handle->signature=pack_base->signature;
		
		if (pack_base->packtype==1 && handle->state==state_s_waitcon)
		{
			struct pack_setup * packet=(void*)raw_packet;
			printf("SRC=[%s]", packet->nick);
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
				struct pack_setup packet = { 1, handle->signature, 0, 42/*corehash*/, 69/*coreverhash*/, 42/*gamehash*/, 69/*sramsize*/, "ZMZ"/*mynick*/ };
				socket_write(&handle->sock, &packet, sizeof(packet));
				handle->setup->frames_until_resend=60/5;
			}
			//free(handle->setup);
			//handle->setup=NULL;
		}
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
