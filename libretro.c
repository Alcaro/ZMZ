#if 0
Questions to libretro/etc devs:

ToadKing, snes9x-next dev:
- Color HDMA in snes9x-next (show wicked star story)
 - turns out it should be that way, SUPER MARIOWORLD specific hack

maister, snes9x-libretro dev:
- If input from player two should be ignored in favor of input from player one
- Disable layers in snes9x (may need to defer to maister)
& - Disable sound channels in snes9x
 - asked, answer was not no; may appear later
- If I am supposed to be unable to unplug a controller

OV2, snes9x original dev:
- If snes9x should die in an infinite loop of CMemory::LoadROMInt returning FALSE on tiny files; floating.muncher.se/temp/die.zip

maister, libretro dev:
- Return value of retro_audio_sample_batch (number of used samples?)
- Whether there is any way to tell rarch to give me more than two sound channels
#endif

#include "libretro.h"
#include <windows.h>
#include "cfg.h"


struct libretro {
	HMODULE h;
	void (*set_environment)(retro_environment_t);
	void (*set_video_refresh)(retro_video_refresh_t);
	void (*set_audio_sample)(retro_audio_sample_t);
	void (*set_audio_sample_batch)(retro_audio_sample_batch_t);
	void (*set_input_poll)(retro_input_poll_t);
	void (*set_input_state)(retro_input_state_t);
	void (*init)(void);
	void (*deinit)(void);
	unsigned (*api_version)(void);
	void (*get_system_info)(struct retro_system_info *info);
	void (*get_system_av_info)(struct retro_system_av_info *info);
	void (*set_controller_port_device)(unsigned port, unsigned device);
	void (*reset)(void);
	void (*run)(void);
	size_t (*serialize_size)(void);
	bool (*serialize)(void *data, size_t size);
	bool (*unserialize)(const void *data, size_t size);
	void (*cheat_reset)(void);
	void (*cheat_set)(unsigned index, bool enabled, const char *code);
	bool (*load_game)(const struct retro_game_info *game);
	bool (*load_game_special)(unsigned game_type, const struct retro_game_info *info, size_t num_info);
	void (*unload_game)(void);
	unsigned (*get_region)(void);
	void* (*get_memory_data)(unsigned id);
	size_t (*get_memory_size)(unsigned id);
};

struct libretro retro;

void retro_set_environment(retro_environment_t p)
{
	retro.set_environment(p);
}

void retro_set_video_refresh(retro_video_refresh_t p)
{
	retro.set_video_refresh(p);
}

void retro_set_audio_sample(retro_audio_sample_t p)
{
	retro.set_audio_sample(p);
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t p)
{
	retro.set_audio_sample_batch(p);
}

void retro_set_input_poll(retro_input_poll_t p)
{
	retro.set_input_poll(p);
}

void retro_set_input_state(retro_input_state_t p)
{
	retro.set_input_state(p);
}

void retro_init(void)
{
	retro.init();
}

void retro_deinit(void)
{
	retro.deinit();
	FreeLibrary(retro.h);
	retro.h=NULL;
}

unsigned retro_api_version(void)
{
	return retro.api_version();
}

void retro_get_system_info(struct retro_system_info *info)
{
	retro.get_system_info(info);
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
	retro.get_system_av_info(info);
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
	retro.set_controller_port_device(port, device);
}

void retro_reset(void)
{
	retro.reset();
}

void retro_run(void)
{
	retro.run();
}

size_t retro_serialize_size(void)
{
	return retro.serialize_size();
}

bool retro_serialize(void *data, size_t size)
{
	return retro.serialize(data, size);
}

bool retro_unserialize(const void *data, size_t size)
{
	return retro.unserialize(data, size);
}

void retro_cheat_reset(void)
{
	retro.cheat_reset();
}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
	retro.cheat_set(index, enabled, code);
}

bool retro_load_game(const struct retro_game_info *game)
{
	return retro.load_game(game);
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
	return retro.load_game_special(game_type, info, num_info);
}

void retro_unload_game(void)
{
	retro.unload_game();
}

unsigned retro_get_region(void)
{
	return retro.get_region();
}

void* retro_get_memory_data(unsigned id)
{
	return retro.get_memory_data(id);
}

size_t retro_get_memory_size(unsigned id)
{
	return retro.get_memory_size(id);
}

bool retro_load(struct libretro * retro, const char * lib)
{
	retro->h=LoadLibrary(lib);
	if (!retro->h) return false;
#define die() do { FreeLibrary(retro->h); return false; } while(0)
#define libload(name) GetProcAddress(retro->h, name)
#ifdef __GNUC__
#define load(name) if (!(retro->name=(__typeof(retro->name))libload("retro_"#name))) die()//shut up about that damn type punning
#else
#define load(name) if (!(*(void**)(&retro->name)=(void*)libload("retro_"#name))) die()
#endif
	load(set_environment);
	load(set_video_refresh);
	load(set_audio_sample);
	load(set_audio_sample_batch);
	load(set_input_poll);
	load(set_input_state);
	load(init);
	load(deinit);
	load(api_version);
	load(get_system_info);
	load(get_system_av_info);
	load(set_controller_port_device);
	load(reset);
	load(run);
	load(serialize_size);
	load(serialize);
	load(unserialize);
	load(cheat_reset);
	load(cheat_set);
	load(load_game);
	load(load_game_special);
	load(unload_game);
	load(get_region);
	load(get_memory_data);
	load(get_memory_size);
	
	if (retro->api_version()!=RETRO_API_VERSION) die();
	
	return true;
}

void retro_unload(struct libretro * retro)
{
	if (retro->h)
	{
		retro->deinit();
		FreeLibrary(retro->h);
		retro->h=NULL;
	}
}

enum replaceresult {
	repl_fail,
	repl_ok,
	repl_same
};
enum replaceresult retro_replace_core(struct libretro * retro, const char * core)
{
	struct libretro tmpretro;
	HMODULE h=LoadLibrary(core);
	if (h==retro->h)
	{
		FreeLibrary(h);//can't load the same one twice
		return repl_same;
	}
	if (!retro_load(&tmpretro, core)) return repl_fail;
	retro_unload(retro);
	memcpy(retro, &tmpretro, sizeof(struct libretro));
	return repl_ok;
}

bool assign_no_core(struct libretro * retro);







#include <stdio.h>

extern void showvideo();
extern void cachevideo();
extern char * vidbuffer;//size = 512*296*4+4096+512*296
extern unsigned short resolutn;

extern const char * Msgptr;
extern unsigned int MsgCount;
extern unsigned int MessageOn;

static enum retro_pixel_format fmt=RETRO_PIXEL_FORMAT_0RGB1555;
static int fmt_shift;

// Callbacks
//
// Environment callback. Gives implementations a way of performing uncommon tasks. Extensible.
static bool retro_environment(unsigned cmd, void *data)
{
	if (cmd==RETRO_ENVIRONMENT_SET_PIXEL_FORMAT)
	{
		enum retro_pixel_format newfmt = *(enum retro_pixel_format *)data;
		if (newfmt==RETRO_PIXEL_FORMAT_0RGB1555 || newfmt==RETRO_PIXEL_FORMAT_RGB565 ||
				newfmt==RETRO_PIXEL_FORMAT_XRGB8888/*fuck bsnes for violating the libretro spec and not accepting 0RGB1555*/)
		{
			fmt=newfmt;
			fmt_shift=(fmt==RETRO_PIXEL_FORMAT_0RGB1555?1:0);
			return true;
		}
	}
	return false;
}

// Render a frame. Pixel format is 15-bit 0RGB1555 native endian unless changed (see RETRO_ENVIRONMENT_SET_PIXEL_FORMAT).
// Width and height specify dimensions of buffer.
// Pitch specifices length in bytes between two lines in buffer.
// For performance reasons, it is highly recommended to have a frame that is packed in memory, i.e. pitch == width * byte_per_pixel.
// Certain graphic APIs, such as OpenGL ES, do not like textures that are not packed in memory.
#define HIMASK (7<<11<<2 | 7<<6<<2 | 7<<0<<2)
#define LOMASK (~HIMASK&0xFFFF)
static inline int blend(int pix1, int pix2)
{
	int high31=(pix1&HIMASK);
	int low31=(pix1&LOMASK);
	int high32=(pix2&HIMASK);
	int low32=(pix2&LOMASK);
	int high3=(high31+high32)>>1;
	int low3=((low31+low32)>>1 & LOMASK);
	return high3+low3;
}

static inline int blend4(int pix1, int pix2, int pix3, int pix4)
{
	int high31=(pix1&HIMASK);
	int low31=(pix1&LOMASK);
	int high32=(pix2&HIMASK);
	int low32=(pix2&LOMASK);
	int high33=(pix3&HIMASK);
	int low33=(pix3&LOMASK);
	int high34=(pix4&HIMASK);
	int low34=(pix4&LOMASK);
	int high3=(high31+high32+high33+high34)>>2;
	int low3=((low31+low32+low33+low34)>>2 & LOMASK);
	return high3+low3;
}
#undef HIMASK
#undef LOMASK

static inline int shiftpix(int pix)
{
	return ((pix&0xFFE0)<<1) | (pix&0x001F);
}

static inline int fixpix(int pix)
{
	return ((pix&0xFFE0)<<fmt_shift) | (pix&0x001F);
}

static void retro_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
static void retro_video_refresh_weird_size(const char * data, unsigned width, unsigned height, size_t pitch);
static void retro_video_refresh_too_weird_size(const char * data, unsigned width, unsigned height, size_t pitch);

static void retro_video_finalize();

static void retro_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch)
{
	if (fmt==RETRO_PIXEL_FORMAT_XRGB8888)
	{
		//bsnes, follow the libretro spec and give me 0RGB1555 instead of demanding such bizarre formats.
		static unsigned short * rgb565pixels=NULL;
		static int num565pixels=0;
		if (num565pixels<width*height)
		{
			num565pixels=width*height;
			free(rgb565pixels);
			rgb565pixels=malloc(num565pixels*sizeof(unsigned short));
		}
		int y;
		int x;
		for (y=0;y<height;y++)
		{
			unsigned short * outpixels=rgb565pixels+(y*width);
			unsigned int * inpixels=(unsigned int*)((char*)data+(pitch*y));
			for (x=0;x<width;x++)
			{
				*outpixels=
								((*inpixels&0x0000F8)>>3)|
								((*inpixels&0x00FC00)>>5)|
								((*inpixels&0xF80000)>>8);
				outpixels++;
				inpixels++;
			}
		}
		data=(void*)rgb565pixels;
		pitch=width*2;
	}
	if (height==224 || height==239) resolutn=height;
	else if (height==448 || height==478) resolutn=height/2;
	else
	{
		retro_video_refresh_weird_size((char*)data, width, height, pitch);
		retro_video_finalize();
		return;
	}
	
	if (width!=256 && width!=512)
	{
		retro_video_refresh_weird_size((char*)data, width, height, pitch);
		retro_video_finalize();
		return;
	}
	
	unsigned short * outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);
//int qq;
//int random=rand();
//for(qq=-288;qq<288*(resolutn+2);qq++)outpixels[qq]=random;
	
	int y;
	int x;
	if (height<300 && width==256)
	{
		if (fmt!=RETRO_PIXEL_FORMAT_0RGB1555)
		{
			for (y=0;y<resolutn;y++)
			{
				unsigned short * pixels=(unsigned short*)((char*)data+(pitch*y));
				memcpy(outpixels, pixels, 256*2);
				outpixels+=288;
			}
		}
		else
		{
			for (y=0;y<resolutn;y++)
			{
				unsigned short * pixels=(unsigned short*)((char*)data+(pitch*y));
				for (x=0;x<256;x++)
				{
					outpixels[x]=shiftpix(pixels[x]);
				}
				outpixels+=288;
			}
		}
	}
	if (height<300 && width==512)
	{
		for (y=0;y<resolutn;y++)
		{
			unsigned short * pixels=(unsigned short*)((char*)data+(pitch*y));
			for (x=0;x<256;x++)
			{
				outpixels[x]=blend(fixpix(pixels[x*2+0]), fixpix(pixels[x*2+1]));
			}
			outpixels+=288;
		}
	}
	if (height>300 && width==256)
	{
		for (y=0;y<resolutn;y++)
		{
			unsigned short * pixels1=(unsigned short*)((char*)data+(pitch*(y*2+0)));
			unsigned short * pixels2=(unsigned short*)((char*)data+(pitch*(y*2+1)));
			for (x=0;x<256;x++)
			{
				outpixels[x]=blend(fixpix(pixels1[x]), fixpix(pixels2[x]));
			}
			outpixels+=288;
		}
	}
	if (height>300 && width==512)
	{
		for (y=0;y<resolutn;y++)
		{
			unsigned short * pixels1=(unsigned short*)((char*)data+(pitch*(y*2+0)));
			unsigned short * pixels2=(unsigned short*)((char*)data+(pitch*(y*2+1)));
			for (x=0;x<256;x++)
			{
				//outpixels[x]=pixels2[x*2];
				outpixels[x]=blend4(fixpix(pixels1[x*2]), fixpix(pixels1[x*2+1]), fixpix(pixels2[x*2]), fixpix(pixels2[x*2+1]));
			}
			outpixels+=288;
		}
	}
	
	retro_video_finalize();
}

static void retro_video_finalize()
{
	unsigned short * outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);
	outpixels+=resolutn*288;
	
	int y;
	int x;
	//extern unsigned char antienab;
	//extern unsigned char En2xSaI;
	//if (antienab || En2xSaI)
	{
		memcpy(outpixels, outpixels-288, 256*2);//bottom row
		
		outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);//right row
		for (y=0;y<=resolutn;y++)
		{
			outpixels[256]=outpixels[255];
			outpixels+=288;
		}
		
		//outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);//right row
		//outpixels[0]=0x1F;
		//outpixels[255]=0x1F;
		//outpixels[223*288]=0x1F;
		//outpixels[223*288+255]=0x1F;
	}
	
	extern unsigned char GrayscaleMode;
	if (GrayscaleMode)
	{
		static unsigned short grayscale[65536];
		static unsigned char grayscale_type=0;
		if (grayscale_type!=GrayscaleMode)
		{
			int r, g, b;
			if (GrayscaleMode==1)
			{
				//Average
				//    The graylevel will be calculated as
				//    Average Brightness = (R + G + B) ÷ 3
				for (r=0;r<64;r+=2)
				for (g=0;g<64;g++)
				for (b=0;b<64;b+=2)
				{
					int brightness=(r+g+b)/3;
					grayscale[(r&0x3E)<<10 | g<<5 | b>>1] = brightness>>1 | brightness<<5 | (brightness&0x3E)<<10;
				}
			}
			if (GrayscaleMode==2)
			{
				//Lightness
				//    The graylevel will be calculated as
				//    Lightness = ½ × (max(R,G,B) + min(R,G,B))
				for (r=0;r<64;r+=2)
				for (g=0;g<64;g++)
				for (b=0;b<64;b+=2)
				{
					int max=r;
					if (g>max) max=g;
					if (b>max) max=b;
					int min=r;
					if (g<min) min=g;
					if (b<min) min=b;
					int brightness=(min+max)/2;
					grayscale[(r&0x3E)<<10 | g<<5 | b>>1] = brightness>>1 | brightness<<5 | (brightness&0x3E)<<10;
				}
			}
			if (GrayscaleMode==3)
			{
				//Luminosity
				//    The graylevel will be calculated as
				//    Luminosity = 0.21 × R + 0.72 × G + 0.07 × B
				for (r=0;r<64;r+=2)
				for (g=0;g<64;g++)
				for (b=0;b<64;b+=2)
				{
					int brightness=((r*21)+(g*72)+(b*7))/100;
					grayscale[(r&0x3E)<<10 | g<<5 | b>>1] = brightness>>1 | brightness<<5 | (brightness&0x3E)<<10;
				}
			}
			grayscale_type=GrayscaleMode;
		}
		unsigned short * line=(unsigned short*)(vidbuffer+16*2+256*2+32*2);
		for (y=0;y<=resolutn;y++)
		{
			for (x=0;x<=256;x++)
			{
				line[x]=grayscale[line[x]];
			}
			line+=288;
		}
	}
	
	showvideo();
	cachevideo();
}

static void retro_video_refresh_weird_size(const char * data, unsigned width, unsigned height, size_t pitch)
{
	int displaywidth=width;
	int displayheight=height;
	bool doublewidth=(width>256);
	bool doubleheight=(height>239);
	int add_y=0;
	
	if (doubleheight)
	{
		displayheight/=2;
		add_y=pitch;
		if (displayheight>239)
		{
			retro_video_refresh_too_weird_size(data, width, height, pitch);
			return;
		}
		if (displayheight>224) resolutn=239;
		else resolutn=224;
	}
	
	if (doublewidth)
	{
		displaywidth/=2;
		if (displaywidth>256)
		{
			retro_video_refresh_too_weird_size(data, width, height, pitch);
			return;
		}
	}
	
	unsigned short * outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);
	memset(outpixels, 0, 2*288*resolutn);
	
	int xoff=(256-displaywidth)/2;
	int yoff=(resolutn-displayheight)/2;
	outpixels+=yoff*288+xoff;
	
	int y;
	int x;
	for (y=0;y<displayheight;y++)
	{
		unsigned short * pixels1=(unsigned short*)data;
		data+=add_y;
		unsigned short * pixels2=(unsigned short*)data;
		data+=pitch;
		if (!doublewidth)
		{
			for (x=0;x<displaywidth;x++)
			{
				outpixels[x]=blend(fixpix(pixels1[x]), fixpix(pixels2[x]));
			}
		}
		else
		{
			for (x=0;x<displaywidth;x++)
			{
				outpixels[x]=blend4(fixpix(pixels1[x*2]), fixpix(pixels1[x*2+1]), fixpix(pixels2[x*2]), fixpix(pixels2[x*2+1]));
			}
		}
		outpixels+=288;
	}
}

static void retro_video_refresh_too_weird_size(const char * data, unsigned width, unsigned height, size_t pitch)
{
	unsigned short * outpixels=(unsigned short*)(vidbuffer+16*2+256*2+32*2);
	void draw_text_for_video(unsigned short * pixels, int pitch, int xoff, int yoff, const char * text);
	draw_text_for_video(outpixels, 288, (256-11*16)/2, (224-16)/2-8, "UNSUPPORTED");
	draw_text_for_video(outpixels, 288, (256-13*16)/2, (224-16)/2+8, "LIBRETRO CORE");
}


int zmz_toggle_layer(int layer, int newstate)
{
	//layer: 0-4 (4 is sprite)
	//newstate: 0-1 (enable flag)
	//return: 0-1 (support flag)
	
printf("lay%i=%i\n",layer,newstate);
	return 0;
}





DWORD SampleRate=0;
//extern volatile int SPCSize;
//extern int DSPBuffer[1280];
//buf=(int[])DSPBuffer count=256
//
//#define BUFSIZE 4096
//static short samples[BUFSIZE];
//static int sampleread=0;
//static int samplewrite=0;
//
//extern unsigned char MusicVol;
//
//void SoundProcess()
//{
	//int i;
	//for (i=0;i<SPCSize;i++)
	//{
		//DSPBuffer[i]=samples[(i+sampleread)&(BUFSIZE-1)]*MusicVol/127;
	//}
	//int tmp=samplewrite+(sampleread>samplewrite?BUFSIZE:0);
	//if (sampleread+SPCSize<tmp) sampleread=(sampleread+SPCSize)&(BUFSIZE-1);
//printf("%i ",(BUFSIZE+samplewrite-sampleread)&(BUFSIZE-1));
//}

void UpdateSound(const int16_t* data, size_t frames);

// Renders multiple audio frames in one go. One frame is defined as a sample of left and right channels, interleaved.
// I.e. int16_t buf[4] = { l, r, l, r }; would be 2 frames.
// Only one of the audio callbacks must ever be used.
static size_t retro_audio_sample_batch(const int16_t *data, size_t frames)
{
	UpdateSound(data, frames);
	//int i;
	//for (i=0;i<frames;i++)
	//{
		//if ((samplewrite+2)&(BUFSIZE-1) == sampleread) return i;
		//samples[samplewrite++]=data[i*2];
		//samples[samplewrite++]=data[i*2+1];
		//samplewrite&=(BUFSIZE-1);
	//}
	return frames;
}

// Renders a single audio frame. Should only be used if implementation generates a single sample at a time.
// Format is signed 16-bit native endian.
static void retro_audio_sample(int16_t left, int16_t right)
{
	//if ((samplewrite+2)&(BUFSIZE-1) != sampleread)
	//{
		//samples[samplewrite++]=left;
		//samples[samplewrite++]=right;
	//}
	int16_t a[2]={left,right};
	retro_audio_sample_batch(a,1);
}

int zmz_toggle_sound_channel(int channel, int newstate)
{
	//channel: 0-7
	//newstate: 0-1 (enable flag)
	//return: 0-1 (support flag)
	
printf("ch%i=%i\n",channel,newstate);
	return 0;
}





void ReadInputDevice();
extern unsigned int JoyAOrig;
extern unsigned int JoyBOrig;
extern unsigned int JoyCOrig;
extern unsigned int JoyDOrig;
extern unsigned int JoyEOrig;
extern unsigned int JoyANow;
extern unsigned int JoyBNow;
extern unsigned int JoyCNow;
extern unsigned int JoyDNow;
extern unsigned int JoyENow;
extern unsigned char pl1contrl;
extern unsigned char pl2contrl;
extern unsigned char pl3contrl;
extern unsigned char pl4contrl;
extern unsigned char pl5contrl;

extern unsigned char device1;
extern unsigned char device2;

// Polls input.
static void retro_input_poll(void)
{
	extern unsigned char NoInputRead;
	if (NoInputRead!=1) ReadInputDevice();
}
// Queries for input for player 'port'. device will be masked with RETRO_DEVICE_MASK.
// Specialization of devices such as RETRO_DEVICE_JOYPAD_MULTITAP that have been set with retro_set_controller_port_device()
// will still use the higher level RETRO_DEVICE_JOYPAD to request input.
static int16_t retro_input_state(unsigned port, unsigned device, unsigned index, unsigned id)
{
	if (device==RETRO_DEVICE_JOYPAD)
	{
//if (framecount==350 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==352 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==354 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==356 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==358 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==510 && id==RETRO_DEVICE_ID_JOYPAD_RIGHT) return 1;
//if (framecount==573 && id==RETRO_DEVICE_ID_JOYPAD_UP) return 1;
//if (framecount==637 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount>824 && id==RETRO_DEVICE_ID_JOYPAD_Y) return 1;
//if (framecount>824 && framecount<860 && id==RETRO_DEVICE_ID_JOYPAD_RIGHT) return 1;

//if (framecount==900 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount>900 && id==RETRO_DEVICE_ID_JOYPAD_A) return 1;
//if (framecount==927 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==954 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;
//if (framecount==982 && id==RETRO_DEVICE_ID_JOYPAD_B) return 1;

		unsigned int keystbl[]={JoyAOrig, JoyBOrig, JoyCOrig, JoyDOrig, JoyEOrig};
		unsigned int keys=keystbl[(port==0)?0:(index+1)];
		const unsigned int whichBit[]={
			0x80000000,//RETRO_DEVICE_ID_JOYPAD_B
			0x40000000,//RETRO_DEVICE_ID_JOYPAD_Y
			0x20000000,//RETRO_DEVICE_ID_JOYPAD_SELECT
			0x10000000,//RETRO_DEVICE_ID_JOYPAD_START
			0x08000000,//RETRO_DEVICE_ID_JOYPAD_UP
			0x04000000,//RETRO_DEVICE_ID_JOYPAD_DOWN
			0x02000000,//RETRO_DEVICE_ID_JOYPAD_LEFT
			0x01000000,//RETRO_DEVICE_ID_JOYPAD_RIGHT
			0x00800000,//RETRO_DEVICE_ID_JOYPAD_A
			0x00400000,//RETRO_DEVICE_ID_JOYPAD_X
			0x00200000,//RETRO_DEVICE_ID_JOYPAD_L
			0x00100000,//RETRO_DEVICE_ID_JOYPAD_R
			0x00000000,//RETRO_DEVICE_ID_JOYPAD_L2
			0x00000000,//RETRO_DEVICE_ID_JOYPAD_R2
			0x00000000,//RETRO_DEVICE_ID_JOYPAD_L3
			0x00000000,//RETRO_DEVICE_ID_JOYPAD_R3
		};
		return (keys&whichBit[id])?1:0;
	}
	return 0;
}

const unsigned char retro_device_type[5]={
	RETRO_DEVICE_JOYPAD,//GAMEPAD
	RETRO_DEVICE_NONE,//RETRO_DEVICE_MOUSE,//Mouse
	RETRO_DEVICE_NONE,//RETRO_DEVICE_LIGHTGUN_SUPER_SCOPE,//Super Scope
	RETRO_DEVICE_NONE,//RETRO_DEVICE_LIGHTGUN_JUSTIFIER,//1 Justifier
	RETRO_DEVICE_NONE,//RETRO_DEVICE_LIGHTGUN_JUSTIFIERS,//2 Justifiers
};





static bool romloaded=false;
static bool is_snes9x;

void zmz_set_controllers()
{
	retro_set_controller_port_device(0, pl1contrl?retro_device_type[device1]:RETRO_DEVICE_NONE);
	retro_set_controller_port_device(1, pl2contrl?retro_device_type[device2]:RETRO_DEVICE_NONE);
}

bool zmz_load_core(const char * core)
{
	if (core)
	{
		enum replaceresult repl=retro_replace_core(&retro, core);
		if (repl==repl_fail) return false;
		if (repl==repl_same) return true;
	}
	else
	{
		retro_unload(&retro);//untested, we're not really supposed to replace a real core with the null core
		assign_no_core(&retro);
	}
	
	fmt=RETRO_PIXEL_FORMAT_0RGB1555;
	fmt_shift=1;
	retro_set_environment(retro_environment);
	retro_init();
	retro_set_video_refresh(retro_video_refresh);
	retro_set_audio_sample(retro_audio_sample);
	retro_set_audio_sample_batch(retro_audio_sample_batch);
	retro_set_input_poll(retro_input_poll);
	retro_set_input_state(retro_input_state);
	zmz_set_controllers();
	
	struct retro_system_info sinfo;
	retro_get_system_info(&sinfo);
	is_snes9x=(!strnicmp(sinfo.library_name, "Snes9X", 6));
	
	if (core && core!=LibretroPath) strcpy(LibretroPath, core);
	
	if (romloaded)
	{
		void powercycle(bool sramload, bool romload);
		powercycle(false, true);
	}
	else romloaded=false;
	
	return true;
}

void zmz_init()
{
	const char * libretros[]={
		LibretroPath,
		"snes9x_libretro_x86_20130519.dll",
		"snes9x_next_libretro_x86_20130519.dll",
		"bsnes_libretro_accuracy_x86_20130519.dll",
		"bsnes_libretro_balanced_x86_20130519.dll",
		"bsnes_libretro_performance_x86_20130519.dll",
		"retro.dll",
		NULL,
	};
	int i=0;
	while (!zmz_load_core(libretros[i])) i++;
}

unsigned char * zmz_get_sram()
{
	return retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
}

unsigned int zmz_get_sram_size()
{
	return retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
}

bool zmz_game_is_pal()
{
	return (retro_get_region()==RETRO_REGION_PAL);
}

void zmz_load(unsigned char * ROM, unsigned int NumofBytes)
{
	struct retro_system_info sinfo;
	retro_get_system_info(&sinfo);
	if (sinfo.need_fullpath)
	{
		struct retro_game_info ginfo;
		extern char * ZCartName;
		ginfo.path=ZCartName;
		ginfo.data=NULL;
		ginfo.size=0;
		ginfo.meta=NULL;
		if (!retro_load_game(&ginfo))
		{
			Msgptr="GAME LOAD FAILED";
			MessageOn=MsgCount;
			return;
		}
	}
	else
	{
		struct retro_game_info ginfo;
		ginfo.path=NULL;
		ginfo.data=ROM;
		ginfo.size=NumofBytes;
		ginfo.meta=NULL;
		if (!retro_load_game(&ginfo))
		{
			Msgptr="GAME LOAD FAILED";
			MessageOn=MsgCount;
			return;
		}
	}
	romloaded=true;
	struct retro_system_av_info avinfo;
	memset(&avinfo, 0, sizeof(avinfo));
	retro_get_system_av_info(&avinfo);
	SampleRate=avinfo.timing.sample_rate;
	
	//sampleread=0;
	//samplewrite=0;
	
	zmz_set_controllers();
	
	void reInitSound();
	reInitSound();
}

void zmz_reset()
{
	if (!romloaded) return;
	retro_reset();
}

unsigned int zmz_state_size()
{
	if (!romloaded) return false;
	return retro_serialize_size();
}

bool zmz_state_save(void * data, unsigned int size)
{
	if (!romloaded) return false;
	return retro_serialize(data, size);
}

bool zmz_state_load(const void * data, unsigned int size)
{
	if (!romloaded) return false;
	return retro_unserialize(data, size);
}

bool zmz_core_is_snes9x()
{
	return is_snes9x;
}





extern unsigned char pressed[256+128+64];
void zmz_main()
{
	if (!romloaded) return;
	zmz_set_controllers();
	while (true)
	{
		retro_run();
		
	nonewgfx: ;
		extern unsigned char GUIQuit;
		if (GUIQuit==1) break;
#define key(keyid, code) if (keyid && pressed[keyid]==1) { pressed[keyid]=2; code; }
#define keybyte(keyid, byte) key(keyid, extern unsigned char byte; byte=1)
#define keybytetoggle(keyid, byte) key(keyid, extern unsigned char byte; byte^=1)
#define keybytereturn(keyid, byte) key(keyid, extern unsigned char byte; byte=1; break)
		keybytereturn(KeyQuickSnapShot, SSKeyPressed);
		keybytetoggle(KeyQuickClock, TimerEnable);
		keybytereturn(KeyQuickSaveSPC, SPCKeyPressed);
		keybytetoggle(EMUPauseKey, EMUPause);
		keybytetoggle(INCRFrameKey, INCRFrame);
		extern unsigned char nextmenupopup; if (pressed[1]==1 || pressed[59]==1 || nextmenupopup==1) break;
		if (nextmenupopup>=2) nextmenupopup--;
		
		#define statekey(keyid) if (keyid && (pressed[keyid]&1)) break;
		statekey(KeySaveState);
		statekey(KeyLoadState);
		statekey(KeyInsrtChap);
		statekey(KeyPrevChap);
		statekey(KeyNextChap);
		statekey(KeyQuickRst);
		statekey(KeyQuickExit);
		statekey(KeyQuickLoad);
		#undef statekey
#undef key
#undef keybyte
#undef keybytetoggle
#undef keybytereturn
		
		extern unsigned char ExecExitOkay; if (ExecExitOkay) ExecExitOkay--;
		extern short curypos; curypos++;
		
		extern unsigned char PauseFrameMode;
		if (PauseFrameMode==3)
		{
			extern void RestorePauseFrame();
			RestorePauseFrame();
		}
		
		extern unsigned char EMUPause;
		extern unsigned char RawDumpInProgress;
		if (EMUPause==1 && RawDumpInProgress!=1)
		{
			if (PauseFrameMode==1)
			{
				extern void BackupPauseFrame();
				BackupPauseFrame();
			}
			extern void ProcessRewind();
			ProcessRewind();
			if (PauseFrameMode==2)
			{
				PauseFrameMode=3;
				goto noprocmovie;
			}
			
			extern unsigned char INCRFrame;
			if (INCRFrame!=1)
			{
				ReadInputDevice();
				//memset(samples, 0, sizeof(samples));
				goto nonewgfx;
			}
			INCRFrame^=1;
		}
		
		extern void UpdateRewind();
		UpdateRewind();
		
		extern unsigned char MovieProcessing;
		if (MovieProcessing)
		{
			extern void ProcessMovies();
			ProcessMovies();
			extern unsigned char GUIReset;
			extern unsigned char ZMVZClose;
			if (GUIReset==1)
			{
				extern unsigned char MovieWaiting;
				extern unsigned int KeyQuickRst;
				MovieWaiting=1;
				pressed[KeyQuickRst]=1;
				break;
			}
			else if (!MovieProcessing && ZMVZClose==1)
			{
				extern void DosExit();
				DosExit();
			}
		}
	noprocmovie: ;
		
		extern unsigned char device2;
		extern unsigned int JoyBNow;
		extern unsigned int LethEnData;
		
		if (device2==3 || device2==4) JoyBNow=LethEnData;
	}
	
	//memset(samples, 0, sizeof(samples));
}




void choose_file_native()
{
	extern unsigned char GUIwinactiv[];
	extern unsigned char GUIwinorder[];
	extern unsigned char GUIwinptr;
	GUIwinactiv[1] = 0; // close load dialog
	GUIwinorder[--GUIwinptr] = 0;
	
	extern HWND hMainWindow;
	
	char path_buff[MAX_PATH];
	*path_buff=0;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=hMainWindow;
	ofn.lpstrFilter="ROM files or archives\0"
										"*.sfc;*.jma;*.zip;*.gz;*.st;*.bs;*.smc;*.swc;*.fig;"
											"*.dx2;*.ufo;*.gd3;*.gd7;*.mgd;*.mgh;*.048;*.058;*.078;*.bin;*.usa;*.eur;*.jap;*.aus;*.1;*.a\0"
									"All Files (*.*)\0"
										"*.*\0";
	ofn.lpstrFile=path_buff;
	ofn.nMaxFile=MAX_PATH;
	//ofn.lpstrTitle=title;
	ofn.Flags=OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt=TEXT("smc");
	if (!GetOpenFileName(&ofn)) return;
	
	bool init_rom_path(char *path);
	void powercycle(bool sramload, bool romload);
	if (init_rom_path(path_buff)) { powercycle(false, true); }
}

void choose_libretro_native()
{
	extern unsigned char GUIwinactiv[];
	extern unsigned char GUIwinorder[];
	extern unsigned char GUIwinptr;
	GUIwinactiv[22] = 0; // close load dialog
	GUIwinorder[--GUIwinptr] = 0;
	
	extern HWND hMainWindow;
	
	char path_buff[MAX_PATH];
	*path_buff=0;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=hMainWindow;
	ofn.lpstrFilter="Libretro cores\0*.dll\0";
	ofn.lpstrFile=path_buff;
	ofn.nMaxFile=MAX_PATH;
	//ofn.lpstrTitle=title;
	ofn.Flags=OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt=TEXT("dll");
	if (!GetOpenFileName(&ofn)) return;
	
	zmz_load_core(path_buff);
	
	void powercycle(bool sramload, bool romload);
	if (romloaded) powercycle(false, false);
}
