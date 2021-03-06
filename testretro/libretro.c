#include "libretro.h"
#include <string.h>
#include <stdio.h>

#include <windows.h>

retro_video_refresh_t video;
retro_input_state_t input;
retro_input_poll_t inputpoll;

uint16_t pixels[224][256];

//blatant zsnes copypasta
unsigned char zfont[]={
0,0,0,0,0,
0x70,0x98,0xA8,0xC8,0x70, 0x20,0x60,0x20,0x20,0x70, 0x70,0x88,0x30,0x40,0xF8, 0x70,0x88,0x30,0x88,0x70,
0x50,0x90,0xF8,0x10,0x10, 0xF8,0x80,0xF0,0x08,0xF0, 0x70,0x80,0xF0,0x88,0x70, 0xF8,0x08,0x10,0x10,0x10,
0x70,0x88,0x70,0x88,0x70, 0x70,0x88,0x78,0x08,0x70, 0x70,0x88,0xF8,0x88,0x88, 0xF0,0x88,0xF0,0x88,0xF0,
0x70,0x88,0x80,0x88,0x70, 0xF0,0x88,0x88,0x88,0xF0, 0xF8,0x80,0xF0,0x80,0xF8, 0xF8,0x80,0xF0,0x80,0x80,
0x78,0x80,0x98,0x88,0x70, 0x88,0x88,0xF8,0x88,0x88, 0xF8,0x20,0x20,0x20,0xF8, 0x78,0x10,0x10,0x90,0x60,
0x90,0xA0,0xE0,0x90,0x88, 0x80,0x80,0x80,0x80,0xF8, 0xD8,0xA8,0xA8,0xA8,0x88, 0xC8,0xA8,0xA8,0xA8,0x98,
0x70,0x88,0x88,0x88,0x70, 0xF0,0x88,0xF0,0x80,0x80, 0x70,0x88,0xA8,0x90,0x68, 0xF0,0x88,0xF0,0x90,0x88,
0x78,0x80,0x70,0x08,0xF0, 0xF8,0x20,0x20,0x20,0x20, 0x88,0x88,0x88,0x88,0x70, 0x88,0x88,0x50,0x50,0x20,
0x88,0xA8,0xA8,0xA8,0x50, 0x88,0x50,0x20,0x50,0x88, 0x88,0x50,0x20,0x20,0x20, 0xF8,0x10,0x20,0x40,0xF8,
0x00,0x00,0xF8,0x00,0x00, 0x00,0x00,0x00,0x00,0xF8, 0x68,0x90,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x20,
0x08,0x10,0x20,0x40,0x80, 0x10,0x20,0x40,0x20,0x10, 0x40,0x20,0x10,0x20,0x40, 0x70,0x40,0x40,0x40,0x70,
0x70,0x10,0x10,0x10,0x70, 0x00,0x20,0x00,0x20,0x00, 0x60,0x98,0x70,0x98,0x68, 0x20,0x20,0xA8,0x70,0x20,
0x50,0xF8,0x50,0xF8,0x50, 0x00,0xF8,0x00,0xF8,0x00, 0x48,0x90,0x00,0x00,0x00, 0x80,0x40,0x20,0x10,0x08,
0xA8,0x70,0xF8,0x70,0xA8, 0x70,0x88,0x30,0x00,0x20, 0x88,0x10,0x20,0x40,0x88, 0x20,0x20,0xF8,0x20,0x20,
0x00,0x00,0x00,0x20,0x40, 0x30,0x40,0x40,0x40,0x30, 0x60,0x10,0x10,0x10,0x60, 0x70,0x98,0xB8,0x80,0x70,
0x20,0x40,0x00,0x00,0x00, 0x20,0x20,0x20,0x00,0x20, 0x78,0xA0,0x70,0x28,0xF0, 0x00,0x20,0x00,0x20,0x40,
0x40,0x20,0x00,0x00,0x00, 0x20,0x50,0x00,0x00,0x00, 0x30,0x40,0xC0,0x40,0x30, 0x60,0x10,0x18,0x10,0x60,
0x20,0x20,0x70,0x70,0xF8, 0xF8,0x70,0x70,0x20,0x20, 0x08,0x38,0xF8,0x38,0x08, 0x80,0xE0,0xF8,0xE0,0x80,
0x20,0x60,0xF8,0x60,0x20, 0x38,0x20,0x30,0x08,0xB0, 0xFC,0x84,0xFC,0x00,0x00, 0x00,0xFC,0x00,0x00,0x00,
0xF8,0x88,0x88,0x88,0xF8,
};

unsigned char convtable[256]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x3E,0x33,0x31,0x3F,0x37,0x2F,0x3D,0x3A,0x3B,0x35,0x38,0x39,0x25,0x28,0x29,
0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x2E,0x40,0x2A,0x32,0x2B,0x36,
0x3C,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x2C,0x34,0x2D,0x42,0x26,
0x41,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x43,0x00,0x44,0x27,0x00,
0x0D,0x1F,0x0F,0x0B,0x0B,0x0B,0x0B,0x0D,0x0F,0x0F,0x0F,0x13,0x13,0x13,0x0B,0x0B,
0x0F,0x0B,0x0B,0x19,0x19,0x19,0x1F,0x1F,0x23,0x19,0x1F,0x0D,0x10,0x23,0x1A,0x10,
0x0B,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,
0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,
0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,
0x7D,0x7E,0x7F,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4D,0x4C,0x4B,0x4A,0x45,0x46,0x47,0x48,0x49,
};

// Karl Malbrain's compact CRC-32.
// See "A compact CCITT crc16 and crc32 C implementation that balances processor cache usage against speed":
// http://www.geocities.ws/malbrain/

unsigned long Crc32[] = {
0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

unsigned long crc32_calc (unsigned char *ptr, unsigned cnt, unsigned long crc)
{
    while( cnt-- ) {
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr & 0xf)];
        crc = ( crc >> 4 ) ^ Crc32[(crc & 0xf) ^ (*ptr++ >> 4)];
    }
 
    return crc;
}

struct {
	uint16_t state[2];
	uint16_t frame;
	char lines[28][32];
} state;

void retro_set_environment(retro_environment_t cb)
{
	//not sure if this is needed, but why not
	bool yes=true;
	cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &yes);
	//I could enable frame duping too, but really, what's the point spending effort on ridiculous parts of a test core...
}

void retro_set_video_refresh(retro_video_refresh_t cb) { video=cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) {}
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {}
void retro_set_input_poll(retro_input_poll_t cb) { inputpoll=cb; }
void retro_set_input_state(retro_input_state_t cb) { input=cb; }

void retro_init(void) {}
void retro_deinit(void) {}

unsigned retro_api_version(void) { return RETRO_API_VERSION; }

void retro_get_system_info(struct retro_system_info *info)
{
	info->library_name="TEST";
	info->library_version="1";
	info->valid_extensions="smc|sfc";
	info->need_fullpath=false;
	info->block_extract=false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	//let's use the snes sizes; I don't think zmz uses these, but if rarch wants to use this too, then it should be allowed to.
	info->geometry.base_width=256;
	info->geometry.base_height=224;
	info->geometry.max_width=256;
	info->geometry.max_height=224;
	info->geometry.aspect_ratio=0;
	
	info->timing.fps=60;
	info->timing.sample_rate=32040;//there is no sound; is there any better way to say that than constant underruns? sample rate 0 probably makes something blow up.
}

void retro_set_controller_port_device(unsigned port, unsigned device) {}//only retropad supported

void retro_reset(void)
{
	memset(&state, 0, sizeof(state));
	memset(&pixels, 0, sizeof(pixels));
}

void renderchr(int chr, int x, int y)
{
	int ix;
	int iy;
	
	for (iy=0;iy<5;iy++)
	for (ix=0;ix<8;ix++)
	{
		if ((zfont[convtable[chr]*5 + iy]>>ix)&1)
		{
			pixels[y+iy][x+(ix^7)]=0x7FFF;
		}
	}
}

void renderstr(const char * str, int x, int y)
{
	int i;
	for (i=0;str[i];i++)
	{
		renderchr(str[i], x+i*8, y);
	}
}

void retro_run(void)
{
	inputpoll();
	
	uint16_t newstate[2]={0,0};
	int i;
	for (i=0;i<16;i++)//it only goes to 12, but a pile of zeroes is harmless.
	{
		newstate[0]|=(input(0, RETRO_DEVICE_JOYPAD, 0, i))<<i;
		newstate[1]|=(input(1, RETRO_DEVICE_JOYPAD, 0, i))<<i;
	}
	if (state.state[0]!=newstate[0] || state.state[1]!=newstate[1])
	{
		for (i=0;i<27;i++)
		{
			strcpy(state.lines[i], state.lines[i+1]);
		}
		
		state.state[0]=newstate[0];
		state.state[1]=newstate[1];
	}
	
	sprintf(state.lines[27], "%i: %.4X %.4X", state.frame, newstate[0], newstate[1]);
	
	uint16_t color=(~crc32_calc((void*)state.lines, 32*27, ~0U))&0x3DEF;
	for (i=0;i<256*224;i++) pixels[0][i]=color;
	
	for (i=0;i<28;i++)
	{
		renderstr(state.lines[i], 0, i*8);
	}
	
	video(pixels, 256, 224, 512);
	
	state.frame++;
	
	//ensure reemulating isn't free
	unsigned long long a;
	unsigned long long b;
	GetSystemTimeAsFileTime((FILETIME*)&a);
	GetSystemTimeAsFileTime((FILETIME*)&b);
	while (b-a < 10*10000) GetSystemTimeAsFileTime((FILETIME*)&b);
}

size_t retro_serialize_size(void)
{
	return sizeof(state);
}

bool retro_serialize(void *data, size_t size)
{
	memcpy(data, &state, sizeof(state));
}

bool retro_unserialize(const void *data, size_t size)
{
	memcpy(&state, data, sizeof(state));
}

void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {}

bool retro_load_game(const struct retro_game_info *game)
{
	retro_reset();
	return true;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) { return false; }

void retro_unload_game(void) {}

unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }

void *retro_get_memory_data(unsigned id) { return NULL; }
size_t retro_get_memory_size(unsigned id)
{
	if (id==0x5A4D5A) return state.frame;//most obvious way to extract this... okay I could mess with the states, but this seems easier.
	return 0;
}
