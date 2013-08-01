/*
Copyright (C) 1997-2007 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )

http://www.zsnes.com
http://sourceforge.net/projects/zsnes
https://zsnes.bountysource.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/



#ifdef __UNIXSDL__
#include "gblhdr.h"
#define DIR_SLASH "/"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <zlib.h>
#include <time.h>
#ifdef __WIN32__
#include <io.h>
#else
#include <unistd.h>
#endif
#define DIR_SLASH "\\"
#endif
#include <stdarg.h>
#include "gblvars.h"
#include "asm_call.h"
#include "zpath.h"
#include "cfg.h"
#include "zmovie.h"
#include "input.h"

#define NUMCONV_FR3
#define NUMCONV_FW3
#include "numconv.h"

#ifdef __MSDOS__
#define clim() __asm__ __volatile__ ("cli");
#define stim() __asm__ __volatile__ ("sti");
#else
#define clim()
#define stim()
#endif

void SA1UpdateDPageC(), unpackfunct(), repackfunct();
void PrepareOffset(), ResetOffset(), initpitch(), UpdateBanksSDD1();
void procexecloop(), outofmemory();

extern unsigned char cacheud, ccud, intrset, cycpl, cycphb, xdbt, xpbt, xp;
extern unsigned char xe, xirqb, debugger, curnmi;
extern unsigned short curypos, stackand, stackor, xat, xst, xdt, xxt, xyt, xpc;
extern unsigned int Curtableaddr, cycpblt;

static size_t load_save_size;

enum copy_state_method { csm_save_zst_new,
                         csm_load_zst_new,
                         csm_load_zst_old,
                         csm_save_rewind,
                         csm_load_rewind };

extern unsigned char *SPC7110PackPtr;

extern bool EmulateSavestates;

const char zst_header[]="ZSNES Save State File V143\x1a\x8fZMZ ZMZ ZMZ ZMZ";
const int zst_header_size=sizeof(zst_header)-1;//If states become nearly playable, stick the ZMZ signature at 0x438.
const int zst_size=276315;

static bool s9x_to_zst(const unsigned char * s9x, unsigned int s9xlen, unsigned char * zst, unsigned int zstlen)
{
	if (memcmp(s9x, "#!s9xsnp:", strlen("#!s9xsnp:"))) return false;
	
	bool gotanything=false;
	
	memset(zst, 0, zstlen);
	memcpy(zst, zst_header, zst_header_size);
	
	const unsigned char * curblk=s9x+strlen("#!s9xsnp:0007\n");
	
	const unsigned char * block_VRA=NULL;
	int block_VRA_len=0;
	const unsigned char * block_PPU=NULL;
	int block_PPU_len=0;
	
	const unsigned char * block_RAM=NULL;
	int block_RAM_len=0;
	
	const char * block_SND=NULL;
	int block_SND_len=0;
	
	while (curblk<s9x+s9xlen)
	{
		int len=strtol(curblk+strlen("CPU:"), NULL, 10);
		if (!strncmp(curblk, "VRA", 3)) { block_VRA=curblk+strlen("VRA:123456:"); block_VRA_len=len; }
		if (!strncmp(curblk, "PPU", 3)) { block_PPU=curblk+strlen("PPU:123456:"); block_PPU_len=len; }
		if (!strncmp(curblk, "RAM", 3)) { block_RAM=curblk+strlen("RAM:123456:"); block_RAM_len=len; }
		if (!strncmp(curblk, "SND", 3)) { block_SND=curblk+strlen("SND:123456:"); block_SND_len=len; }
		curblk+=strlen("CPU:000048:")+len;
	}
	
	if (block_VRA_len==65536 && block_PPU_len==2649)
	{
//FILE * f=fopen("ppu.bin", "wb");
//fwrite(block_PPU, 1,block_PPU_len, f);
//fclose(f);
		memcpy(zst+0x20C13, block_VRA, 65536);
		
		const unsigned char * ppu=block_PPU;
		
		zst[0x66]=ppu[0x3A]; // BG mode
		
		int layer;
		for (layer=0;layer<4;layer++)
		{
			int tilemapsize=ppu[0x18+layer*11];
			int tilemapaddr=ppu[0xE +layer*11]*2;
			const int offXl[]={0, 0x800, 0, 0x800};
			const int offYl[]={0, 0, 0x800, 0x1000};
			int offX=offXl[tilemapsize];
			int offY=offYl[tilemapsize];
			zst[0x6C+layer*2]=tilemapaddr;
			zst[0x74+layer*2]=tilemapaddr+offX;//why do these three exist when bg#scsize already exists
			zst[0x7C+layer*2]=tilemapaddr+offY;//cpu/memtable.c::repackfunct could regenerate them instead, far saner
			zst[0x84+layer*2]=tilemapaddr+offX+offY;
			
			zst[0x8B+layer  ]=tilemapsize;
			
			zst[0x68       ]|=ppu[0x14+layer*11]<<layer;//'tilemaps are 16x16' flag
			zst[0x90+layer*2]=ppu[0x15+layer*11]*2;//tile address (character data, not tilemap)
		}
		
		int i;//CGRAM
		for (i=0;i<256;i++)
		{
			zst[0x618+i*2+0]=ppu[0x3F+i*2+1];//it's little endian in one and big in the other for some dumb reason
			zst[0x618+i*2+1]=ppu[0x3F+i*2+0];
		}
		
		memcpy(zst+0x818, zst+0x618, 512);//copy of CGRAM (what)
		
		zst[0x20A]=ppu[0xA4C];//fixed color R
		zst[0x20B]=ppu[0xA4D];//fixed color G
		zst[0x20C]=ppu[0xA4E];//fixed color B
		
		gotanything=true;
	}
	
	if (block_RAM_len==13072)
	{
		memcpy(zst+0xC13, block_RAM, 131072);
		gotanything=true;
	}
	
	if (block_SND_len==66560)
	{
		//do nothing for now
	}
	
	return gotanything;
}

static bool copy_state_data(unsigned char *buffer, void (*copy_func)(unsigned char **, void *, size_t), enum copy_state_method method)
{
	bool ok=true;
	
	unsigned int zmz_state_size();
	bool zmz_state_save(void * data, unsigned int size);
	bool zmz_state_load(const void * data, unsigned int size);
	bool zmz_core_is_snes9x();
	
	static void * state_tmp=NULL;
	static unsigned int state_tmp_size=0;
	
	unsigned int state_size=zmz_state_size();
	
	if (state_size!=state_tmp_size)
	{
		state_tmp_size=state_size;
		state_tmp=realloc(state_tmp, state_size);
	}
	
	if (method==csm_save_rewind)
	{
		ok&=zmz_state_save(state_tmp, state_size);
		copy_func(&buffer, state_tmp, state_size);
	}
	if (method==csm_load_rewind)
	{
		copy_func(&buffer, state_tmp, state_size);
		ok&=zmz_state_load(state_tmp, state_size);
	}
	
	if (method==csm_save_zst_new)
	{
		ok&=zmz_state_save(state_tmp, state_size);
		if (EmulateSavestates && zmz_core_is_snes9x())
		{
			unsigned char * zst=malloc(zst_size);
			if (s9x_to_zst(state_tmp, state_size, zst, zst_size))
			{
				copy_func(&buffer, zst, zst_size);
			}
			free(zst);
		}
		copy_func(&buffer, state_tmp, state_size);
	}
	if (method==csm_load_zst_new)
	{
		copy_func(&buffer, state_tmp, zst_header_size);
		if (strncmp(state_tmp, zst_header, zst_header_size))
		{
			//not fake zsnes state
			copy_func(&buffer, state_tmp+zst_header_size, state_size-zst_header_size);
		}
		else
		{
			//is fake zsnes state
			int trash_bytes_remaining=zst_size-zst_header_size;
			while (trash_bytes_remaining>state_size)
			{
				copy_func(&buffer, state_tmp, state_size);//repeatedly read it into this buffer and discard it
				trash_bytes_remaining-=state_size;
			}
			if (trash_bytes_remaining) copy_func(&buffer, state_tmp, trash_bytes_remaining);
			
			copy_func(&buffer, state_tmp, state_size);
		}
		ok&=zmz_state_load(state_tmp, state_size);
	}
	
	if (method==csm_load_zst_old)
	{
		ok=false; // no such thing
	}
	
	return ok;
}

static void memcpyinc(unsigned char **dest, void *src, size_t len)
{
  memcpy(*dest, src, len);
  *dest += len;
}

static void memcpyrinc(unsigned char **src, void *dest, size_t len)
{
  memcpy(dest, *src, len);
  *src += len;
}

extern unsigned int RewindTimer, DblRewTimer;
extern unsigned char EMUPause;

unsigned char *StateBackup = 0;
unsigned int AllocatedRewindStates, LatestRewindPos, EarliestRewindPos;
bool RewindPosPassed;

size_t rewind_state_size, cur_zst_size, old_zst_size;

extern unsigned char romispal;
void zmv_rewind_save(size_t, bool);
void zmv_rewind_load(size_t, bool);

void ClearCacheCheck()
{
  memset(vidmemch2, 1, sizeof(vidmemch2));
  memset(vidmemch4, 1, sizeof(vidmemch4));
  memset(vidmemch8, 1, sizeof(vidmemch8));
}

//Code to handle special frames for pausing, and desync checking
unsigned char *SpecialPauseBackup = 0, PauseFrameMode = 0;
/*
Pause frame modes

0 - no pause frame stored
1 - pause frame ready to be stored
2 - pause frame stored
3 - pause frame ready for reload
*/

void BackupPauseFrame()
{
  if (SpecialPauseBackup)
  {
    copy_state_data(SpecialPauseBackup, memcpyinc, csm_save_rewind);
    PauseFrameMode = 2;
  }
}

void RestorePauseFrame()
{
  if (SpecialPauseBackup)
  {
    copy_state_data(SpecialPauseBackup, memcpyrinc, csm_load_rewind);
    //ClearCacheCheck();
    PauseFrameMode = 0;
  }
}

void DeallocPauseFrame()
{
  if (SpecialPauseBackup) { free(SpecialPauseBackup); }
}

#define ActualRewindFrames (unsigned int)(RewindFrames * (romispal ? 10 : 12))

void BackupCVFrame()
{
  unsigned char *RewindBufferPos = StateBackup + LatestRewindPos*rewind_state_size;

  if (MovieProcessing == MOVIE_PLAYBACK) { zmv_rewind_save(LatestRewindPos, true); }
  else if (MovieProcessing == MOVIE_RECORD) { zmv_rewind_save(LatestRewindPos, false); }
  copy_state_data(RewindBufferPos, memcpyinc, csm_save_rewind);

  if (RewindPosPassed)
  {
    EarliestRewindPos = (EarliestRewindPos+1)%AllocatedRewindStates;
    RewindPosPassed = false;
  }
//  printf("Backing up in #%u, earliest: #%u, allocated: %u\n", LatestRewindPos, EarliestRewindPos, AllocatedRewindStates);

  LatestRewindPos = (LatestRewindPos+1)%AllocatedRewindStates;

  if (LatestRewindPos == EarliestRewindPos) { RewindPosPassed = true; }

  RewindTimer = ActualRewindFrames;
  DblRewTimer += (DblRewTimer) ? 0 : ActualRewindFrames;
//  printf("New backup slot: #%u, timer %u, check %u\n", LatestRewindPos, RewindTimer, DblRewTimer);
}

void RestoreCVFrame()
{
  unsigned char *RewindBufferPos;

  if (LatestRewindPos != ((EarliestRewindPos+1)%AllocatedRewindStates))
  {
    if (DblRewTimer > ActualRewindFrames)
    {
      if (LatestRewindPos == 1 || AllocatedRewindStates == 1)
        { LatestRewindPos = AllocatedRewindStates-1; }
      else { LatestRewindPos = (LatestRewindPos) ? LatestRewindPos-2 : AllocatedRewindStates-2; }
    }
    else
    {
      LatestRewindPos = (LatestRewindPos) ? LatestRewindPos-1 : AllocatedRewindStates-1;
    }
  }
  else { LatestRewindPos = EarliestRewindPos; }

  RewindBufferPos = StateBackup + LatestRewindPos*rewind_state_size;
  //printf("Restoring from #%u, earliest: #%u\n", LatestRewindPos, EarliestRewindPos);

  if (MovieProcessing == MOVIE_RECORD)
  {
    zmv_rewind_load(LatestRewindPos, false);
  }
  else
  {
    if (MovieProcessing == MOVIE_PLAYBACK)
    {
      zmv_rewind_load(LatestRewindPos, true);
    }

    if (PauseRewind || EMUPause)
    {
      PauseFrameMode = EMUPause = true;
    }
  }

  copy_state_data(RewindBufferPos, memcpyrinc, csm_load_rewind);
  ClearCacheCheck();

  LatestRewindPos = (LatestRewindPos+1)%AllocatedRewindStates;
  RewindTimer = ActualRewindFrames;
  DblRewTimer = 2*ActualRewindFrames;
}

void SetupRewindBuffer()
{
  //For special rewind case to help out pauses
  DeallocPauseFrame();
  SpecialPauseBackup = malloc(rewind_state_size);

  //For standard rewinds
  if (StateBackup) { free(StateBackup); }
  for (; RewindStates; RewindStates--)
  {
    StateBackup = 0;
    StateBackup = (unsigned char *)malloc(rewind_state_size*RewindStates);
    if (StateBackup) { break; }
  }
  AllocatedRewindStates = RewindStates;
}

void DeallocRewindBuffer()
{
  if (StateBackup) { free(StateBackup); }
}

static size_t state_size;

static void state_size_tally(unsigned char **dest, void *src, size_t len)
{
  state_size += len;
}

void InitRewindVars()
{
  state_size = 0;
  copy_state_data(NULL, state_size_tally, csm_save_rewind);
  rewind_state_size = state_size;

  SetupRewindBuffer();
  LatestRewindPos = 0;
  EarliestRewindPos = 0;
  RewindPosPassed = false;
  RewindTimer = 1;
  DblRewTimer = 1;
}

void InitRewindVarsForMovie()
{
  LatestRewindPos = 0;
  EarliestRewindPos = 0;
  RewindPosPassed = false;
  RewindTimer = 1;
  DblRewTimer = 1;
}

extern unsigned int MsgCount, MessageOn;
extern unsigned char AutoIncSaveSlot, cbitmode, NoPictureSave;
extern char *Msgptr;
extern unsigned short PrevPicture[64*56];

static FILE *fhandle;
void CapturePicture();

static void write_save_state_data(unsigned char **dest, void *data, size_t len)
{
  fwrite(data, 1, len, fhandle);
}

void calculate_state_sizes()
{
  state_size = 0;
  copy_state_data(0, state_size_tally, csm_save_zst_new);
  cur_zst_size = state_size;
}

unsigned int current_zst = 0;
unsigned int newest_zst = 0;
time_t newestfiledate;

bool zmz_core_is_snes9x();
char *zst_name()
{
  char base_extension[4];
  if (zmz_core_is_snes9x() && !EmulateSavestates) strcpy(base_extension, "000");
  else strcpy(base_extension, "zst");
  static char buffer[7];
  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_RECORD))
  {
    sprintf(buffer, "%.2d.%s", current_zst, base_extension);
    return(buffer);
  }
  strcpy(buffer, base_extension);
  if (current_zst)
  {
    buffer[2] = (current_zst%10)+'0';
    if (current_zst > 9)
    {
      buffer[1] = (current_zst/10)+'0';
    }
  }
  setextension(ZStateName, buffer);
  return(ZStateName);
}

void zst_determine_newest()
{
  struct stat filestat;

  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_RECORD)) { mzt_chdir_up(); }
  if (!stat_dir(ZSramPath, zst_name(), &filestat) && filestat.st_mtime > newestfiledate)
  {
    newestfiledate = filestat.st_mtime;
    newest_zst = current_zst;
  }
  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_RECORD)) { mzt_chdir_down(); }
}

void zst_init()
{
  if (LatestSave)
  {
    for (current_zst = 0; current_zst < 100; current_zst++)
    {
      zst_determine_newest();
    }
    current_zst = newest_zst;
    zst_name();
  }
}

int zst_exists()
{
  int ret;

  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_RECORD)) { mzt_chdir_up(); }
  ret = access_dir(ZSramPath, zst_name(), F_OK) ? 0 : 1;
  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_RECORD)){ mzt_chdir_down(); }

  return(ret);
}


static bool zst_save_compressed(FILE *fp)
{
  size_t data_size = cur_zst_size;
  unsigned char *buffer = 0;

  bool worked = false;

  if ((buffer = (unsigned char *)malloc(data_size)))
  {
    //Compressed buffer which must be at least 0.1% larger than source buffer plus 12 bytes
    //We devide by 1000 then add an extra 1 as a quick way to get a buffer large enough when
    //using integer division
    unsigned long compressed_size = data_size + data_size/1000 + 13;
    unsigned char *compressed_buffer = 0;

    if ((compressed_buffer = (unsigned char *)malloc(compressed_size)))
    {
      copy_state_data(buffer, memcpyinc, csm_save_zst_new);
      if (compress2(compressed_buffer, &compressed_size, buffer, data_size, Z_BEST_COMPRESSION) == Z_OK)
      {
        fwrite3(compressed_size, fp);
        fwrite(compressed_buffer, 1, compressed_size, fp);
        worked = true;
      }
      free(compressed_buffer);
    }
    free(buffer);
  }

  if (!worked) //Compression failed for whatever reason
  {
    fwrite3(cur_zst_size | 0x00800000, fp); //Uncompressed ZST will never break 8MB
  }

  return(worked);
}

void zst_save(FILE *fp, bool Thumbnail, bool Compress)
{
  if (!Compress || !zst_save_compressed(fp)) //If we don't want compressed or compression failed
  {
    fhandle = fp; //Set global file handle
    copy_state_data(0, write_save_state_data, csm_save_zst_new);

    if (Thumbnail)
    {
      CapturePicture();
      fwrite(PrevPicture, 1, 64*56*sizeof(unsigned short), fp);
    }
  }
}

/*
Merges all the passed strings into buffer. Make sure to pass an extra parameter as 0 after all the strings.
Copies at most buffer_len characters. Result is always null terminated.
Returns how many bytes are needed to store all strings.
Thus if return is <= buffer_len, everything was copied.
*/
static size_t string_merge(char *buffer, size_t buffer_len, ...)
{
  char *s;
  size_t copied = 0, needed = 0;

  va_list ap;
  va_start(ap, buffer_len);

  if (buffer && buffer_len) { *buffer = 0; }

  while ((s = va_arg(ap, char *)))
  {
    needed += strlen(s);
    if (buffer && (copied+1 < buffer_len))
    {
      strncpy(buffer+copied, s, buffer_len-copied);
      buffer[buffer_len-1] = 0;
      copied += strlen(buffer+copied);
    }
  }

  va_end(ap);
  return(needed+1);
}

static char txtmsg[30];

void set_state_message(char *prefix, char *suffix)
{
  char num[3];
  sprintf(num, "%d", current_zst);
  string_merge(txtmsg, sizeof(txtmsg), prefix, isextension(ZStateName, "zss") ? "AUTO" : num, suffix, 0);

  Msgptr = txtmsg;
  MessageOn = MsgCount;
}

void statesaver()
{
  if (MovieProcessing == MOVIE_RECORD)
  {
    //'Auto increment savestate slot' code
    current_zst += AutoIncSaveSlot;
    current_zst %= 100;

    if (mzt_save(current_zst, (cbitmode && !NoPictureSave) ? true : false, false))
    {
      set_state_message("RR STATE ", " SAVED.");
    }
    else
    {
      current_zst += 100-AutoIncSaveSlot;
      current_zst %= 100;
    }
    return;
  }

  if ((MovieProcessing == MOVIE_PLAYBACK) || (MovieProcessing == MOVIE_DUMPING_NEW))
  {
    //'Auto increment savestate slot' code
    current_zst += AutoIncSaveSlot;
    current_zst %= 100;

    if (mzt_save(current_zst, (cbitmode && !NoPictureSave) ? true : false, true))
    {
      set_state_message("RR STATE ", " SAVED.");
    }
    else
    {
      current_zst += 100-AutoIncSaveSlot;
      current_zst %= 100;
    }
    return;
  }

  clim();

  //'Auto increment savestate slot' code
  if(!isextension(ZStateName, "zss"))
  {
    current_zst += (char) AutoIncSaveSlot;
    current_zst %= 100;
    zst_name();
  }

  if ((fhandle = fopen_dir(ZSramPath, ZStateName, "wb")))
  {
    zst_save(fhandle, (bool)(cbitmode && !NoPictureSave), false);
    fclose(fhandle);

    //Display message onscreen, 'STATE XX SAVED.'
    set_state_message("STATE ", " SAVED.");
  }
  else
  {
    //Display message onscreen, 'UNABLE TO SAVE.'
    Msgptr = "UNABLE TO SAVE.";
    MessageOn = MsgCount;

    if(!isextension(ZStateName, "zss"))
    {
      current_zst += 100-(char) AutoIncSaveSlot;
      current_zst %= 100;
      zst_name();
    }
  }

  stim();
}

extern unsigned int Totalbyteloaded, SfxMemTable[256], SfxCPB;
extern unsigned int SfxPBR, SfxROMBR, SfxRAMBR, SCBRrel, SfxSCBR;
extern unsigned char pressed[256+128+64], multchange, ioportval, SDD1Enable;
extern unsigned char nexthdma;

static void read_save_state_data(unsigned char **dest, void *data, size_t len)
{
  load_save_size += fread(data, 1, len, fhandle);
}

//input, inputlen: A full gzip stream.
//outputlen: Length of the output. Note that it should be initialized to what you think the length should be; being wrong is not an error, but will allocat more memory than necessary. Use 0 if you have no clue, and it'll guess.
//return: Pointer to inflated data, or NULL if error. To be sent to free().
static void * gzipInflate(const void * input, unsigned int inputlen, unsigned int * outputlen)
{
	if (inputlen==0)
	{
		*outputlen=0;
		return malloc(1);
	}
	
	z_stream strm;
	strm.next_in=(void*)input;
	strm.avail_in=inputlen;
	strm.total_out=0;
	strm.zalloc=Z_NULL;
	strm.zfree=Z_NULL;
	
	int outlenguess=*outputlen;
	if (!outlenguess) outlenguess=inputlen*2;
	
	if (inflateInit2(&strm, (16+MAX_WBITS)) != Z_OK) return NULL;
	
	void * output=malloc(outlenguess);
	
	while (true)
	{
		if (strm.total_out >= outlenguess)
		{
			outlenguess*=2;
			output=realloc(output, outlenguess);
		}
		
		strm.next_out = (Bytef *) (output+strm.total_out);
		strm.avail_out = outlenguess-strm.total_out;
		
		// Inflate another chunk.
		int err=inflate(&strm, Z_SYNC_FLUSH);
		if (err==Z_STREAM_END) break;
		else if (err!=Z_OK)
		{
			free(output);
			return NULL;
		}
	}
	
	if (inflateEnd(&strm)!=Z_OK)
	{
		free(output);
		return NULL;
	}
	
	*outputlen=strm.total_out;
	return output;
}

//static bool zst_load_compressed(FILE *fp, size_t compressed_size)
//{
//  unsigned long data_size = cur_zst_size;
//  unsigned char *buffer = 0;
//  bool worked = false;
//  if ((buffer = (unsigned char *)malloc(data_size)))
//  {
//    unsigned char *compressed_buffer = 0;
//    if ((compressed_buffer = (unsigned char *)malloc(compressed_size)))
//    {
//      fread(compressed_buffer, 1, compressed_size, fp);
//      if (uncompress(buffer, &data_size, compressed_buffer, compressed_size) == Z_OK && data_size==cur_zst_size)
//      {
//        copy_state_data(buffer, memcpyrinc, csm_load_zst_new);
//        worked = true;
//      }
//      free(compressed_buffer);
//    }
//    free(buffer);
//  }
//  return(worked);
//}

bool zst_load(FILE *fp, size_t Compressed)
{
  fseek(fp, 0, SEEK_SET);
  unsigned char signature[3];
  fread(&signature, 3,1, fp);
  if (signature[0]==0x1F && signature[1]==0x8B && signature[2]==0x08)
  {
    bool ret=false;
    fseek(fp, 0, SEEK_END);
    int compressedlen=ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char * compresseddata=malloc(compressedlen);
    fread(compresseddata, 1,compressedlen, fp);
    
    unsigned int decompressedlen=cur_zst_size;
    unsigned char * state=gzipInflate(compresseddata, compressedlen, &decompressedlen);
    if (state)
    {
      ret=copy_state_data(state, memcpyinc, csm_load_zst_new);
    }
    free(state);
    free(compresseddata);
    if (ret) return true;
  }
  fseek(fp, 0, SEEK_SET);

  load_save_size = 0;
  fhandle = fp; //Set global file handle
  bool ok=copy_state_data(0, read_save_state_data, csm_load_zst_new);
  Totalbyteloaded += load_save_size;

  //Clear cache check if state loaded
  ClearCacheCheck();

  if (MovieProcessing != MOVIE_RECORD)
  {
    nexthdma = 0;
  }

  procexecloop();

  return ok;
}

//Wrapper for above
bool zst_compressed_loader(FILE *fp)
{
  size_t data_size = fread3(fp);
  return((data_size & 0x00800000) ? zst_load(fp, 0) : zst_load(fp, data_size));
}

extern unsigned char Voice0Disable, Voice1Disable, Voice2Disable, Voice3Disable;
extern unsigned char Voice4Disable, Voice5Disable, Voice6Disable, Voice7Disable;

void stateloader(char *statename, bool keycheck, bool xfercheck)
{
  extern unsigned char PauseLoad;

  if (keycheck)
  {
    pressed[1] = 0;
    pressed[KeyLoadState] = 2;
    multchange = 1;
    MessageOn = MsgCount;
  }

  if (MZTForceRTR == RTR_REPLAY_TO_RECORD && (MovieProcessing == MOVIE_PLAYBACK))
  {
    MovieRecord();
  }
  else if (MZTForceRTR == RTR_RECORD_TO_REPLAY && (MovieProcessing == MOVIE_RECORD))
  {
    MovieStop();
    MoviePlay();
  }

  switch (MovieProcessing)
  {
    case MOVIE_PLAYBACK:
      if (mzt_load(current_zst, true))
      {
        Msgptr = "CHAPTER LOADED.";
        MessageOn = MsgCount;
      }
      else
      {
        set_state_message("UNABLE TO LOAD STATE ", ".");
      }
      return;
    case MOVIE_RECORD:
      if (mzt_load(current_zst, false))
      {
        set_state_message("RR STATE ", " LOADED.");

        if (PauseLoad || EMUPause)
        {
          PauseFrameMode = EMUPause = true;
        }
      }
      else
      {
        set_state_message("UNABLE TO LOAD STATE ", ".");
      }
      return;
    case MOVIE_OLD_PLAY:
    {
      extern unsigned char CMovieExt;
      size_t fname_len = strlen(statename);
      setextension(statename, "zmv");
      if (isdigit(CMovieExt)) { statename[fname_len-1] = CMovieExt; }
    }
    case MOVIE_ENDING_DUMPING: case MOVIE_DUMPING_NEW: case MOVIE_DUMPING_OLD:
      return;
      break;
  }

  clim();

  if(!isextension(ZStateName, "zss"))
  {
    zst_name();
  }

  //Actual state loading code
  if ((fhandle = fopen_dir(ZSramPath, statename, "rb")))
  {
    if (xfercheck) { Totalbyteloaded = 0; }

    if (zst_load(fhandle, 0))
    {
      set_state_message("STATE ", " LOADED."); // 'STATE XX LOADED.'

      if (PauseLoad || EMUPause)
      {
        PauseFrameMode = EMUPause = true;
      }
    }
    else
    {
      set_state_message("UNABLE TO LOAD STATE ", ".");
    }
    fclose(fhandle);
  }
  else
  {
    set_state_message("UNABLE TO LOAD STATE ", "."); // 'UNABLE TO LOAD STATE XX.'
  }

  Voice0Disable = Voice1Disable = Voice2Disable = Voice3Disable = 1;
  Voice4Disable = Voice5Disable = Voice6Disable = Voice7Disable = 1;

  stim();
}

void debugloadstate()
{
  stateloader(ZStateName, 0, 0);
}

void loadstate()
{
  stateloader(ZStateName, 1, 0);
}

void loadstate2()
{
  stateloader(ZStateName, 0, 1);
}

void LoadSecondState()
{
  setextension(ZStateName, "zss");
  loadstate2();
  zst_name();
}

void SaveSecondState()
{
  setextension(ZStateName, "zss");
  statesaver();
  zst_name();
}

extern unsigned char CHIPBATT, sramsavedis, *sram2, nosaveSRAM;
void SaveCombFile();

unsigned char * zmz_get_sram();

// Sram saving
void SaveSramData()
{
  extern unsigned int sramb4save;
  if (*ZSaveName && (!SRAMSave5Sec || sramb4save))
  {
    FILE *fp = 0;
    unsigned int *data_to_save;

    setextension(ZSaveName, "srm");

    if (ramsize && !sramsavedis)
    {
      data_to_save=(unsigned int*)zmz_get_sram();

      clim();
      if (!nosaveSRAM && (fp = fopen_dir(ZSramPath, ZSaveName,"wb")))
      {
        fwrite(data_to_save, 1, ramsize, fp);
        fclose(fp);
      }
      stim();
    }
    sramb4save = 0;
  }
  SaveCombFile();
}

extern bool SramExists;
void OpenSramFile()
{
  FILE *fp;

  setextension(ZSaveName, "srm");
  if ((fp = fopen_dir(ZSramPath, ZSaveName, "rb")))
  {
    fread(zmz_get_sram(), 1, ramsize, fp);
    fclose(fp);

    SramExists = true;

    //if (*ZSaveST2Name && (fp = fopen_dir(ZSramPath, ZSaveST2Name, "rb")))
    //{
    //  fread(sram2, 1, ramsize, fp);
    //  fclose(fp);
    //}
  }
  else
  {
    SramExists = false;
  }
}

/*
SPC File Format - Invented by _Demo_ & zsKnight
Cleaned up by Nach

00000h-00020h - File Header : SNES-SPC700 Sound File Data v0.00 (33 bytes)
00021h-00023h - 0x1a,0x1a,0x1a (3 bytes)
00024h        - 10 (1 byte)
00025h        - PC Register value (1 Word)
00027h        - A Register Value (1 byte)
00028h        - X Register Value (1 byte)
00029h        - Y Register Value (1 byte)
0002Ah        - Status Flags Value (1 byte)
0002Bh        - Stack Register Value (1 byte)
0002Ch-0002Dh - Reserved (1 byte)
0002Eh-0004Dh - SubTitle/Song Name (32 bytes)
0004Eh-0006Dh - Title of Game (32 bytes)
0006Eh-0007Dh - Name of Dumper (32 bytes)
0007Eh-0009Dh - Comments (32 bytes)
0009Eh-000A1h - Date the SPC was Dumped (4 bytes)
000A2h-000A8h - Reserved (7 bytes)
000A9h-000ACh - Length of SPC in seconds (4 bytes)
000ADh-000AFh - Fade out length in milliseconds (3 bytes)
000B0h-000CFh - Author of Song (32 bytes)
000D0h        - Default Channel Disables (0 = enable, 1 = disable) (1 byte)
000D1h        - Emulator used to dump .spc file (1 byte)
                (0 = UNKNOWN, 1 = ZSNES, 2 = SNES9X)
                (Note : Contact the authors if you're an snes emu author with
                 an .spc capture in order to assign you a number)
000D2h-000FFh - Reserved (46 bytes)
00100h-100FFh - SPCRam (64 KB)
10100h-101FFh - DSPRam (256 bytes)
*/

char spcsaved[64];
void savespcdata()
{
  if (!zmz_core_is_snes9x())
  {
    strcpy(spcsaved, "ONLY AVAILABLE WITH SNES9X CORE.");
    return;
  }
  
  size_t fname_len;
  unsigned int i = 0;

  setextension(ZSaveName, "spc");
  fname_len = strlen(ZSaveName);

  while (i < 100)
  {
    if (i)
    {
      sprintf(ZSaveName-1+fname_len - ((i < 10) ? 0 : 1), "%d", i);
    }
    if (access_dir(ZSpcPath, ZSaveName, F_OK))
    {
      break;
    }
    i++;
  }
  if (i < 100)
  {
    char * state=malloc(cur_zst_size);
    bool zmz_state_save(void * data, unsigned int size);
    zmz_state_save(state, cur_zst_size);
    
    const char * curblk=state+strlen("#!s9xsnp:0007\n");
    
    const char * block_SND=NULL;
    int block_SND_len=0;
    
    while (curblk<state+cur_zst_size)
    {
      int len=strtol(curblk+strlen("CPU:"), NULL, 10);
      if (!strncmp(curblk, "SND", 3)) { block_SND=curblk+strlen("SND:123456:"); block_SND_len=len; }
      curblk+=strlen("CPU:000048:")+len;
    }
    
    char SPCHeader[256];
    const char * SPCMainRam=NULL;
    char SPCFooter[256];
    memset(SPCHeader, 0, sizeof(SPCHeader));
    memset(SPCFooter, 0, sizeof(SPCFooter));
    
    extern const unsigned char SPCROM[64];
    
    if (block_SND_len==66560)
    {
      strcpy(SPCHeader, "SNES-SPC700 Sound File Data v0.30");
      SPCHeader[0x21]=0x1A;
      SPCHeader[0x22]=0x1A;
      SPCHeader[0x23]=0x1A;
      SPCHeader[0x24]=0x0A;
      
      SPCMainRam=block_SND;
      
      const char * smpregs=block_SND+65536;
      SPCHeader[0x25]=smpregs[0x0C];
      SPCHeader[0x26]=smpregs[0x0D];
      SPCHeader[0x27]=smpregs[0x14];
      SPCHeader[0x28]=smpregs[0x18];
      SPCHeader[0x29]=smpregs[0x1C];
      SPCHeader[0x2A]=(smpregs[0x20]<<7 | smpregs[0x24]<<6 | smpregs[0x28]<<5 | smpregs[0x2C]<<4 |
                       smpregs[0x30]<<3 | smpregs[0x34]<<2 | smpregs[0x38]<<1 | smpregs[0x3C]<<0);
      SPCHeader[0x2B]=smpregs[0x10];
      
      const char * dspdata=block_SND+65536+(4*41);
      memcpy(SPCFooter, dspdata, 128);
      memset(SPCFooter+128, 0, 64);
      memcpy(SPCFooter+192, SPCROM, 64);
    }

    extern unsigned int infoloc;
    memcpy(SPCHeader+0x4E, ((unsigned char *)romdata)+infoloc, 21);

    //0009Eh-000A1h - Date the SPC was Dumped
    time_t t = time(0);
    struct tm *lt = localtime(&t);
    SPCHeader[0x9E]=lt->tm_mday;
    SPCHeader[0x9F]=lt->tm_mon+1;
    SPCHeader[0xA0]=(lt->tm_year+1900)&0xFF;
    SPCHeader[0xA1]=((lt->tm_year+1900)>>8)&0xFF;
    
    if (SPCMainRam)
    {
      FILE* fp = fopen_dir(ZSpcPath, ZSaveName, "wb");
      if (fp)
      {
        fwrite(SPCHeader, 1,256, fp);
        fwrite(SPCMainRam, 1,65536, fp);
        fwrite(SPCFooter, 1,256, fp);
        fclose(fp);
        
        sprintf(spcsaved, "%s FILE SAVED.", ZSaveName+fname_len-3);
      }
      else sprintf(spcsaved, "%s FILE OPEN FAILED.", ZSaveName+fname_len-3);
    }
    else strcpy(spcsaved, "INCOMPATIBLE SNES9X CORE.");
    free(state);
  }
  else strcpy(spcsaved, "TOO MANY SPCS IN FOLDER.");
}

void SaveGameSpecificInput()
{
  if (!*ZSaveName)
  {
    psr_cfg_run(write_input_vars, ZCfgPath, "zinput.cfg");
  }

  if (GameSpecificInput && *ZSaveName)
  {
    setextension(ZSaveName, "inp");
    psr_cfg_run(write_input_vars, ZSramPath, ZSaveName);
  }
}

void LoadGameSpecificInput()
{
  if (GameSpecificInput && *ZSaveName)
  {
    psr_cfg_run(read_input_vars, ZCfgPath, "zinput.cfg");

    setextension(ZSaveName, "inp");
    psr_cfg_run(read_input_vars, ZSramPath, ZSaveName);
  }
}

