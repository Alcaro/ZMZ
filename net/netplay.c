#include "netplay.h"
#include <stdint.h>
#include <string.h>
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#include "udpsock.c"

//standard port ID is 21101

#ifdef __GNUC__
#pragma GCC diagnostic error "-Wpadded"//putting it here is undefined behaviour in gcc 4.5, but seems to work; putting it earlier throws for padding in libretro.h and I can't change that.
//#pragma GCC diagnostic push//only exists in gcc 4.6
//#pragma GCC diagnostic error "-Wpadded"
#endif

#define WIRESIGNATURE 0x52696E6D
#define WIREVERSION 0
#ifndef DEBUG
#error no seriously.
#endif

struct packet_base {
	uint32 packtype;
	uint32 signature;
};

//0 is banned since it's the same if byteswapped
#define pack_setup_id 1
struct pack_setup {
	uint32 packtype;
	//if little endian communicates with big endian, packet type is backwards too and the receiver can turn it back; however, big endian is rare and can't be tested
	uint32 signature;//randomly generated from whatever the app feels like; packets with wrong signature are ignored
	
	uint32 format_signature;//0x6D6E6952
	uint32 format_version;//0
	uint32 core_hash;
	uint32 core_version_hash;
	uint32 game_hash;
	uint32 sram_size;
	//the above must be identical or both abort
	
	uint8 nick[64];
};

#define pack_setup_data_id 2
struct pack_setup_data {
	uint32 packtype;
	uint32 signature;
	
	//it is safe to not tell what this is since it's used for sram only
	uint32 start;
	uint32 len;//if sram isn't an even number of kilobytes
	uint8 data[1024];//udp supports up to slightly below 64KB; slightly above 1KB is fully safe
};

#define pack_setup_ack_id 3
struct pack_setup_ack {
	uint32 packtype;
	uint32 signature;
	
	uint8 ack[];//one bit per kilobyte; ack[0]&0x01 for first, ack[0]&0x02 for second
	//this allows 8MB SRAM for 1KB of ACKs; more than that doesn't exist in any legit retro game
};

#define pack_gameplay_id 4
struct pack_gameplay {
	uint32 packtype;
	uint32 signature;
	
	uint32 acknowledge_frame;//how much the packet sender has received from the other party and conclusively emulated
	
	uint32 start_frame;//where the input data starts
	uint32 this_frame;//how many frames the sender has predictively emulated
	uint16 data[64];//size is equal to the difference between the above; should remain mostly unused
	
	uint32 chat_len;
	uint32 chat_frame;//when the message was sent (so it can be acknowledged); multiple messages can not be sent without a roundtrip
	uint8 chatmsg[];//this should not be an issue because if it is then you're spamming.
};

#define pack_abort_id 5
struct pack_abort {
	uint32 packtype;
	uint32 signature;
	
	uint32 why;
};

const char * abortreasons[]={
	//these aren't really in any valid order.
#define abort_no_host_ip 0
	"Invalid IP address",
#define abort_no_host 1
	"No server on this IP",
#define abort_not_host 2
	"Server is not ZMZ",
#define abort_bad_host 3
	"ZMZ version mismatch",
#define abort_bad_core 4
	"Libretro core mismatch",
#define abort_bad_core_ver 5
	"Libretro core version mismatch",
#define abort_bad_game 6
	"Game mismatch",//used for SRAM size mismatch too since, well, it is wrong game.
#define abort_sore_loser 7
	"User request",
#define abort_ping_timeout 8
	"Connection lost",
#define abort_bad_endian 9
	"Endianness mismatch",//ZMZ only runs on Windows, which is only little endian (http://support.microsoft.com/kb/102025), so this won't happen, but better unused features than lacking an used one
#define abort_count 10
};

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wpadded"
//#pragma GCC diagnostic pop
#endif

struct netplay_setup {
	//splitting this off so it can be freed once the channel is open
	int frames_until_resend;
	
	void * sram_ptr;//we can repeatedly ask the core for this, but this seems easier.
	uint32 sram_size;
	uint32 sram_ack_size_without_header;
	uint32 sram_ack_size_with_header;
	struct pack_setup_ack * sram_ack;
	//these are used only by server
	uint32 sram_send_at;
	struct pack_setup_ack * sram_sent_noack;//this one contains an unused header; harmless to keep
	
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
#define state_aborted 7//if this one hits, myplayerid is the reason why; no reason to waste bytes
	uint32 signature;
	uint32 myplayerid;//0 or 1
	struct netplay_setup * setup;
	struct retro_callbacks * cb;
	
	uint32 time_since_last;
	
	char mynick[64+1];
	char othernick[64+1];
	
	//these start being used once gameplay starts
	
	uint32 speculative_frames;
	uint32 final_frames;
	
	uint32 send_from_frame;
	
	uint8 my_input_lag;
	
	void * savestates[128];
	uint16 my_input[128];
	uint32 savestate_size;
	//index to this is speculative_frames%128
	
	bool is_replay;
	uint16 inputs[2];
	
	//reset when !speculative_frames%60
	int out_packets; int out_frames;
	int in_packets; int in_frames;
	//if (in_frames/in_packets)-(out_frames/out_packets)>1.5 then we're too much in front, skip frame
	
	//checked and reset when !speculative_frames%120
	uint64 fpscheck;
};

static netplay* ghandle=NULL;

extern unsigned int CRC32;

#include <zlib.h>

void zmz_show_message(const char * msg);

static void netplay_free_setup(netplay* handle)
{
	if (handle->setup)
	{
		free(handle->setup->sram_ack);
		free(handle->setup->sram_sent_noack);
		free(handle->setup);
		handle->setup=NULL;
	}
}

static void netplay_abort(netplay* handle, int why)
{
	if (handle->state==state_aborted) return;
	handle->state=state_aborted;
	handle->myplayerid=why;
	netplay_free_setup(handle);
	
	int i;
	for (i=0;i<128;i++)
	{
		free(handle->savestates[i]);
	}
	
	zmz_show_message(abortreasons[why]);
}

netplay* netplay_create(const char * mynick, const char * host, unsigned short port, struct retro_callbacks * cb)
{
	netplay* handle=malloc(sizeof(struct netplay));
	memset(handle, 0, sizeof(struct netplay));
	
	if (!socket_new(&handle->sock, host, port))
	{
		zmz_show_message(abortreasons[abort_no_host_ip]);
		free(handle);
		return NULL;
	}
	
	handle->setup=malloc(sizeof(struct netplay_setup));
	memset(handle->setup, 0, sizeof(struct netplay_setup));
	
	if (host)
	{
		handle->signature =(rand()&255);
		handle->signature^=(rand()&255)<<8;
		handle->signature^=(rand()&255)<<16;//rand_max is less than uint32_max on windows, so let's mix in some more shit.
		handle->signature^=(rand()&255)<<24;
	}
	
	handle->setup->sram_size=retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
	handle->setup->sram_ptr=retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
	int num_kb=(handle->setup->sram_size+1023)/1024;
	handle->setup->sram_ack_size_without_header=(num_kb+7)/8;
	handle->setup->sram_ack_size_with_header=sizeof(struct pack_setup_ack)+handle->setup->sram_ack_size_without_header;
	
	handle->setup->sram_ack=malloc(handle->setup->sram_ack_size_with_header);
	handle->setup->sram_ack->packtype=pack_setup_ack_id;
	handle->setup->sram_ack->signature=handle->signature;
	memset(&handle->setup->sram_ack->ack, 0, handle->setup->sram_ack_size_without_header);
	
	if (!host)
	{
		handle->setup->sram_sent_noack=malloc(handle->setup->sram_ack_size_with_header);
		handle->setup->sram_sent_noack->packtype=pack_setup_ack_id;//these two aren't needed, but why not
		handle->setup->sram_sent_noack->signature=handle->signature;//especially the signature is worthless; we're the host so we don't have it at this point!
		memset(&handle->setup->sram_sent_noack->ack, 0, handle->setup->sram_ack_size_without_header);
	}
	else handle->setup->sram_sent_noack=NULL;
	
	handle->setup->videodata=malloc(2*256*224);
	memset(handle->setup->videodata, 0, 2*256*224);
	
	handle->setup->frames_until_resend=1;
	
	struct retro_system_info info;
	retro_get_system_info(&info);
	handle->setup->corehash=crc32(0, info.library_name, strlen(info.library_name));
	handle->setup->coreverhash=crc32(0, info.library_version, strlen(info.library_version));
	handle->setup->gamehash=CRC32;
	
	handle->myplayerid=((bool)host);
	handle->cb=cb;
	strcpy(handle->mynick, mynick);
	strcpy(handle->othernick, "");
	
	if (host) handle->state=state_c_sendsetup;
	else handle->state=state_s_waitcon;
	
	int i;
	for (i=0;i<128;i++)
	{
		handle->savestates[i]=NULL;
	}
	
	return handle;
}

static void netplay_run_init(netplay* handle)
{
	netplay_free_setup(handle);
	handle->state=state_playing;
	
#ifdef _WIN32
	GetSystemTimeAsFileTime((FILETIME*)&handle->fpscheck);
#else
#error what
#endif
	
	handle->savestate_size=retro_serialize_size();
	
	int i;
	for (i=0;i<128;i++)
	{
		handle->savestates[i]=malloc(handle->savestate_size);
	}
memset(handle->my_input, 0xFF, sizeof(handle->my_input));
}

static void netplay_run_packet(netplay* handle, struct pack_gameplay* packet)
{
//puts("RPCKET");
//printf("GETPACK=%i->%i ",packet->acknowledge_frame,packet->this_frame);
//printf("THIS=%i=%i->%i ",handle->final_frames,handle->send_from_frame,handle->speculative_frames);
//printf("PID=%i ",handle->myplayerid);
	handle->in_packets++;
	handle->in_frames+=(packet->this_frame - packet->start_frame);
	
	handle->send_from_frame=max(handle->send_from_frame, packet->acknowledge_frame);
	
	int id=0;
	int frame;
	bool must_play=false;
	for (frame=packet->start_frame;frame<packet->this_frame;frame++)
	{
//printf("IN[%i]=%.4X ",frame,packet->data[id]);
		if (frame==handle->final_frames && frame<handle->speculative_frames)
		{
			if (!must_play && handle->inputs[handle->myplayerid^1]!=packet->data[id])
			{
printf("REWIND->%i\n",handle->final_frames);
				retro_unserialize(handle->savestates[handle->final_frames%128], handle->savestate_size);
				must_play=true;
			}
			if (must_play)
			{
				handle->inputs[handle->myplayerid^1]=packet->data[id];
				handle->inputs[handle->myplayerid]=handle->my_input[handle->final_frames%128];
				handle->is_replay=true;
puts("TRUEPLAY");
				retro_run();
				handle->is_replay=false;
			}
			handle->final_frames++;
		}
		id++;
	}
	if (must_play)
	{
		for (frame=handle->final_frames;frame<handle->speculative_frames;frame++)
		{
			//rerun this
			handle->inputs[handle->myplayerid]=handle->my_input[frame%128];
			//can't set handle->inputs[handle->myplayerid^1], it's not known
			retro_serialize(handle->savestates[frame%128], handle->savestate_size);
			handle->is_replay=true;
			retro_run();
puts("RESYNC");
			handle->is_replay=false;
		}
	}
	
	//TODO: chat messages
//printf("NEWTHIS=%i\n",handle->final_frames);
}

static void netplay_run_frame(netplay* handle)
{
//if (handle->speculative_frames!=retro_get_memory_size(0x5A4D5A)) printf("%i!=%i\n",handle->speculative_frames,retro_get_memory_size(0x5A4D5A)),exit(13);
#ifndef DEBUG
#error no seriously.
#endif
//puts("RFRAME");
	if (handle->speculative_frames%60==0)
	{
printf("INOUT= IN=%i,%i OUT=%i,%i\n",handle->in_packets,handle->in_frames,handle->out_packets,handle->out_frames);
if(!handle->in_packets) puts("INOUTDIFF=inf");
else printf("INOUTDIFF=%f\n", ((float)(handle->in_frames*handle->out_packets)-(handle->out_frames*handle->in_packets)) / (handle->in_packets*handle->out_packets/**3/2*/));
		if ((handle->in_frames*handle->out_packets)-(handle->out_frames*handle->in_packets) <- handle->in_packets*handle->out_packets*3/2)
		//if (0)
		{
puts("LAGFRAME");
			handle->out_packets=0;
			handle->out_frames=0;
			handle->in_packets=0;
			handle->in_frames=0;
			return;
		}
puts("NOTLAGFRAME");
		
		handle->out_packets=0;
		handle->out_frames=0;
		handle->in_packets=0;
		handle->in_frames=0;
	}
	if (handle->speculative_frames%120==0)
	{
		uint64 oldfpscheck=handle->fpscheck;
		
#ifdef _WIN32
		GetSystemTimeAsFileTime((FILETIME*)&handle->fpscheck);
		printf("TIMER=%u\n",handle->fpscheck - oldfpscheck);
		if (handle->fpscheck - oldfpscheck > 2100*10000)
#else
#error what
#endif
		{
			handle->my_input_lag++;
		}
	}
	
	uint16 myinput=0;
	int i;
	for (i=0;i<16;i++)//it only goes to 12, but a pile of zeroes is harmless.
	{
		myinput|=(handle->cb->input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, i))<<i;
	}
	
	handle->my_input[(handle->speculative_frames+handle->my_input_lag)%128]=myinput;
	
	handle->inputs[handle->myplayerid]=handle->my_input[handle->speculative_frames%128];
	//keep handle->inputs[handle->myplayerid^1] as what it was last time
	
	retro_serialize(handle->savestates[handle->speculative_frames%128], handle->savestate_size);
	
	handle->speculative_frames++;
	
	if (handle->speculative_frames-handle->final_frames >= 64/6)
	{
printf("%i-%i=%i,DIE\n",handle->speculative_frames,handle->final_frames,handle->speculative_frames-handle->final_frames);
#ifndef DEBUG
#error no seriously. also kill that division.
#endif
exit(12);
		netplay_abort(handle, abort_ping_timeout);
		return;//normally we can just continue, but not when we're about to overflow a buffer.
	}
	struct pack_gameplay packet={
		pack_gameplay_id, handle->signature, handle->final_frames, handle->send_from_frame, handle->speculative_frames
	};
	for (i=0;i<handle->speculative_frames-handle->send_from_frame;i++)
	{
		packet.data[i]=handle->my_input[(handle->send_from_frame+i)%128];
	}
	
	packet.chat_len=0;
	socket_write(&handle->sock, &packet, sizeof(packet));
	
	handle->out_packets++;
	handle->out_frames+=(packet.this_frame - packet.start_frame);
	
	retro_run();
}

//client (P2) sends struct setup every 200ms until it gets one back
//server (P1) sends struct setup every 200ms until it gets an empty struct setup_ack
//client sends struct setup_ack for 0 bytes until it gets a setup_data
//server sends setup_data for any kilobyte marked unsent every 17ms (); kilobyte is marked sent
//when client gets one, setup_ack is sent
//once server is out of unsent kilobytes, all sent ones are marked unsent if not acknowledged, and resent; however, reset is not done more often than once per 200ms
//once all kilobytes are acknowledged, setup is done

void netplay_run(netplay* handle)
{
	ghandle=handle;
	while (true)
	{
		char raw_packet[2048];
		int packlen=socket_read(&handle->sock, raw_packet, 2048);
		if (!packlen) break;
		memset(raw_packet+packlen, 0, 2048-packlen);
		
		struct packet_base * pack_base=(void*)raw_packet;
		if (pack_base->packtype&0xFF000000 && (handle->state==state_s_waitcon || handle->state==state_c_sendsetup))
		{
			handle->signature=pack_base->signature;
			netplay_abort(handle, abort_bad_endian);
			//the other party won't understand our aborts, so it'll start aborting too
			//signature will be right since it will be swapped back
			//it won't pong loop since aborts are not ponged
		}
		
		if (pack_base->packtype==pack_setup_id && handle->state==state_s_waitcon)
		{
			handle->signature=pack_base->signature;
			struct pack_setup * packet=(void*)raw_packet;
			
			handle->state=state_s_sendsetup;
			handle->setup->frames_until_resend=1;
			
			if (packet->format_signature!=WIRESIGNATURE) netplay_abort(handle, abort_not_host);
			if (packet->format_version!=WIREVERSION) netplay_abort(handle, abort_bad_host);
			
			int i; for (i=0;i<64;i++) handle->othernick[i]=packet->nick[i];
			
			if (handle->setup->corehash!=packet->core_hash) netplay_abort(handle, abort_bad_core);
			if (handle->setup->coreverhash!=packet->core_version_hash) netplay_abort(handle, abort_bad_core_ver);
			if (handle->setup->gamehash!=packet->game_hash) netplay_abort(handle, abort_bad_game);
			if (handle->setup->sram_size!=packet->sram_size) netplay_abort(handle, abort_bad_game);
			continue;
		}
		
		if (pack_base->signature!=handle->signature) continue;
		
		if (pack_base->packtype&0xFF) handle->time_since_last=0;
		
		if (pack_base->packtype==pack_setup_id && (handle->state==state_c_sendsetup || handle->state==state_c_getsram))
		{
			struct pack_setup * packet=(void*)raw_packet;
			
			int i; for (i=0;i<64;i++) handle->othernick[i]=packet->nick[i];
			handle->state=state_c_getsram;
			
			//these were checked on the other end already, but why not
			if (packet->format_signature!=WIRESIGNATURE) netplay_abort(handle, abort_not_host);
			if (packet->format_version!=WIREVERSION) netplay_abort(handle, abort_bad_host);
			
			if (handle->setup->corehash!=packet->core_hash) netplay_abort(handle, abort_bad_core);
			if (handle->setup->coreverhash!=packet->core_version_hash) netplay_abort(handle, abort_bad_core_ver);
			if (handle->setup->gamehash!=packet->game_hash) netplay_abort(handle, abort_bad_game);
			if (handle->setup->sram_size!=packet->sram_size) netplay_abort(handle, abort_bad_game);
			
			struct pack_setup_ack retpacket={ pack_setup_ack_id, handle->signature };
			socket_write(&handle->sock, &retpacket, sizeof(retpacket));
			continue;
		}
		
		if (pack_base->packtype==pack_setup_ack_id && handle->state==state_s_sendsetup)
		{
			struct pack_setup_ack * packet=(void*)raw_packet;
			if (!handle->setup->sram_size) netplay_run_init(handle);
			else handle->state=state_s_sendsram;
			continue;
		}
		
		if (pack_base->packtype==pack_setup_data_id && handle->state==state_c_getsram)
		{
			struct pack_setup_data * packet=(void*)raw_packet;
			
			if (packet->len>1024 || packet->start*1024+1024 > handle->setup->sram_size)
			{
				netplay_abort(handle, abort_not_host);
				continue;
			}
			
			memcpy(handle->setup->sram_ptr+packet->start, packet->data, packet->len);
			handle->setup->sram_ack->ack[packet->start/8]|=1<<(packet->start&7);
			
			socket_write(&handle->sock, handle->setup->sram_ack, handle->setup->sram_ack_size_with_header);
			continue;
		}
		
		if (pack_base->packtype==pack_setup_ack_id && handle->state==state_s_sendsram)
		{
			struct pack_setup_ack * packet=(void*)raw_packet;
			
			memcpy(handle->setup->sram_ack, packet, handle->setup->sram_ack_size_with_header);
			
			bool done=true;
			int i;
			for (i=0;i<handle->setup->sram_ack_size_without_header;i++)
			{
				if (handle->setup->sram_ack->ack[i]!=0xFF) done=false;
			}
			if (done)
			{
				netplay_run_init(handle);
			}
			continue;
		}
		
		if (pack_base->packtype==pack_abort_id)
		{
			struct pack_abort * packet=(void*)raw_packet;
			if (packet->why>=abort_count) netplay_abort(handle, abort_not_host);
			else netplay_abort(handle, packet->why);
		}
		if (handle->state==state_aborted && pack_base->packtype!=pack_abort_id && pack_base->packtype!=(pack_abort_id<<24))
		{
			struct pack_abort packet = { pack_abort_id, handle->signature,
																		handle->myplayerid };
			socket_write(&handle->sock, &packet, sizeof(packet));
		}
		
		if (pack_base->packtype==pack_setup_data_id && handle->state==state_c_getsram)
		{
			struct pack_setup_data * packet=(void*)raw_packet;
			
			if (packet->len>1024 || packet->start*1024+1024 > handle->setup->sram_size)
			{
				netplay_abort(handle, abort_not_host);
				continue;
			}
			
			memcpy(handle->setup->sram_ptr+packet->start, packet->data, packet->len);
			handle->setup->sram_ack->ack[packet->start/8]|=1<<(packet->start&7);
			
			socket_write(&handle->sock, handle->setup->sram_ack, handle->setup->sram_ack_size_with_header);
			continue;
		}
		
		if (pack_base->packtype==pack_gameplay_id && handle->state==state_c_getsram)
		{
			netplay_run_init(handle);
			//fall through
		}
		if (pack_base->packtype==pack_gameplay_id && handle->state==state_playing)
		{
			struct pack_gameplay * packet=(void*)raw_packet;
			netplay_run_packet(handle, packet);
		}
	}
	if (handle->state!=state_s_waitcon && handle->state!=state_aborted)
	{
		handle->time_since_last++;
		if (handle->time_since_last>=120)
		{
			if (handle->state!=state_c_sendsetup) netplay_abort(handle, abort_no_host);
			else netplay_abort(handle, abort_ping_timeout);
		}
	}
	if (handle->state!=state_playing && handle->state!=state_aborted)
	{
		if (handle->state==state_s_sendsram)
		{
			int byteat=handle->setup->sram_send_at;
			int lastbyte=handle->setup->sram_ack_size_without_header;
			while (byteat<lastbyte && handle->setup->sram_sent_noack->ack[byteat]==0xFF) byteat++;
			if (handle->setup->frames_until_resend) handle->setup->frames_until_resend--;
			if (byteat==lastbyte)
			{
				if (!handle->setup->frames_until_resend)
				{
					handle->setup->frames_until_resend=60/5;
					memcpy(handle->setup->sram_sent_noack, handle->setup->sram_ack, handle->setup->sram_ack_size_with_header);
					byteat=0;
				}
			}
			handle->setup->sram_send_at=byteat;
			if (byteat<lastbyte)
			{
				int bitat;
				for (bitat=0;handle->setup->sram_sent_noack->ack[byteat]&(1<<bitat);bitat++) {}
				handle->setup->sram_sent_noack->ack[byteat]|=(1<<bitat);
				
				struct pack_setup_data packet;
				packet.packtype=pack_setup_data_id;
				packet.signature=handle->signature;
				packet.start=byteat*8+bitat;
				packet.len=handle->setup->sram_size-(packet.start*1024);
				if (packet.len>1024) packet.len=1024;
				memcpy(packet.data, handle->setup->sram_ptr+(packet.start*1024), packet.len);
				memset(packet.data+packet.len, 0, 1024-packet.len);
				socket_write(&handle->sock, &packet, sizeof(packet));
			}
		}
		if (handle->state==state_c_sendsetup || handle->state==state_s_sendsetup)
		{
			handle->setup->frames_until_resend--;
			if (!handle->setup->frames_until_resend)
			{
				struct pack_setup packet = { pack_setup_id, handle->signature,
																		WIRESIGNATURE, WIREVERSION,
																		handle->setup->corehash, handle->setup->coreverhash, handle->setup->gamehash,
																		handle->setup->sram_size };
				int i; for (i=0;i<64;i++) packet.nick[i]=handle->mynick[i];
				socket_write(&handle->sock, &packet, sizeof(packet));
				handle->setup->frames_until_resend=60/5;
			}
		}
		handle->cb->video_refresh_cb(handle->setup->videodata, 256, 224, 512);
	}
	
	if (handle->state==state_playing)
	{
		netplay_run_frame(handle);
	}
	
	if (handle->state==state_aborted)
	{
		retro_run(); 
	}
}

void netplay_free(netplay* handle)
{
	netplay_abort(handle, abort_sore_loser);
	socket_close(&handle->sock);
	free(handle);
}



void netplay_input_poll(void)
{
	if (ghandle->is_replay) return;
	ghandle->cb->input_poll_cb();
}

int16_t netplay_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
	if (device==RETRO_DEVICE_JOYPAD && index==0)
	{
		return (ghandle->inputs[port]>>id)&1;
	}
	return 0;
}

void netplay_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
	if (ghandle->is_replay) return;
	ghandle->cb->video_refresh_cb(data, width, height, pitch);
}

void netplay_audio_sample(int16_t left, int16_t right)
{
	if (ghandle->is_replay) return;
	ghandle->cb->audio_sample_cb(left, right);
}

size_t netplay_audio_sample_batch(const int16_t *data, size_t frames)
{
	if (ghandle->is_replay) return;
	return ghandle->cb->audio_sample_batch_cb(data, frames);
}

#ifndef DEBUG
#error no seriously go fix guiwindp.inc line 3473.
#endif
