#include "../libretro.h"

struct retro_callbacks
{
   retro_video_refresh_t video_refresh_cb;
   retro_audio_sample_t audio_sample_cb;
   retro_audio_sample_batch_t audio_sample_batch_cb;
   retro_input_state_t input_state_cb;
   retro_input_poll_t input_poll_cb;
};

typedef struct netplay netplay;
netplay* netplay_create(const char * mynick, const char * host, unsigned short port, struct retro_callbacks * cb);
//bool netplay_connected(netplay* handle, const char * * othernick, const char * * otherip);
void netplay_run(netplay* handle);
void netplay_chat(netplay* handle, const char * message);
void netplay_free(netplay* handle);

void netplay_input_poll(void);
int16_t netplay_input_state(unsigned port, unsigned device, unsigned index, unsigned id);
void netplay_video_refresh(const void *data, unsigned width, unsigned height, size_t pitch);
void netplay_audio_sample(int16_t left, int16_t right);
size_t netplay_audio_sample_batch(const int16_t *data, size_t frames);
