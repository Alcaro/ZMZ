#include "libretro.h"
#include <windows.h>

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
static void none_set_audio_sample(retro_audio_sample_t p) {}
static void none_set_audio_sample_batch(retro_audio_sample_batch_t p) {}
static void none_set_input_poll(retro_input_poll_t p) {}
static void none_set_input_state(retro_input_state_t p) {}
static void none_init(void) {}
static void none_deinit(void) {}
static unsigned none_api_version(void) { return 1; }
static void none_get_system_av_info(struct retro_system_av_info *info) {}
static void none_set_controller_port_device(unsigned port, unsigned device) {}
static void none_reset(void) {}
static size_t none_serialize_size(void) { return 0; }
static bool none_serialize(void *data, size_t size) { return false; }
static bool none_unserialize(const void *data, size_t size) { return false; }
static void none_cheat_reset(void) {}
static void none_cheat_set(unsigned index, bool enabled, const char *code) {}
static bool none_load_game(const struct retro_game_info *game) { return true; }
static bool none_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) { return false; }
static void none_unload_game(void) {}
static unsigned none_get_region(void) { return 0; }
static void* none_get_memory_data(unsigned id) { return NULL; }
static size_t none_get_memory_size(unsigned id) { return 0; }

static void none_set_environment(retro_environment_t p)
{
	enum retro_pixel_format fmt=RETRO_PIXEL_FORMAT_RGB565;
	p(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt);
}

static void none_get_system_info(struct retro_system_info *info)
{
	info->library_name="NULL";
	info->library_version="0";
	info->valid_extensions="";
	info->need_fullpath=false;
	info->block_extract=false;
}

static retro_video_refresh_t none_video_cb;
static void none_set_video_refresh(retro_video_refresh_t p) { none_video_cb=p; }

void draw_text_for_video(unsigned short * pixels, int pitch, int xoff, int yoff, const char * text)
{
	extern unsigned char GUIFontData1[705];
	extern unsigned char ASCII2Font[128];
	int i;
	int x;
	int y;
	for (i=0;text[i];i++)
	{
		for (y=0;y<5;y++)
		{
			int bits=GUIFontData1[ASCII2Font[text[i]]*5+y];
			for (x=0;x<8;x++)
			{
				if (bits&128)
				{
					pixels[(yoff+y*2+0)*pitch + xoff+i*16+x*2+0]=0xFFFF;
					pixels[(yoff+y*2+0)*pitch + xoff+i*16+x*2+1]=0xFFFF;
					pixels[(yoff+y*2+1)*pitch + xoff+i*16+x*2+0]=0xFFFF;
					pixels[(yoff+y*2+1)*pitch + xoff+i*16+x*2+1]=0xFFFF;
				}
				bits<<=1;
			}
		}
	}
}

static void none_run(void)
{
	unsigned short pixels[256*224];
	memset(pixels, 0, sizeof(pixels));
	draw_text_for_video(pixels, 256, (256-11*16)/2, (224-16)/2-8, "NO LIBRETRO");
	draw_text_for_video(pixels, 256, (256-11*16)/2, (224-16)/2+8, "CORE LOADED");
	none_video_cb(pixels, 256, 224, 256*2);
}

bool assign_no_core(struct libretro * retro)
{
	retro->h=NULL;
#define none(which) retro->which=none_##which;
	none(set_environment);
	none(set_video_refresh);
	none(set_audio_sample);
	none(set_audio_sample_batch);
	none(set_input_poll);
	none(set_input_state);
	none(init);
	none(deinit);
	none(api_version);
	none(get_system_info);
	none(get_system_av_info);
	none(set_controller_port_device);
	none(reset);
	none(run);
	none(serialize_size);
	none(serialize);
	none(unserialize);
	none(cheat_reset);
	none(cheat_set);
	none(load_game);
	none(load_game_special);
	none(unload_game);
	none(get_region);
	none(get_memory_data);
	none(get_memory_size);
#undef none
}
