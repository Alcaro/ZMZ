/*
Config file handler header generated by Nach's Config file handler creator.
*/

#ifdef __cplusplus
  extern "C" {
#endif

unsigned char read_cfg_vars(const char *);
unsigned char write_cfg_vars(const char *);

extern unsigned char GUIComboGameSpec;
extern unsigned char GameSpecificInput;
extern unsigned char AllowMMX;
#ifdef __WIN32__
extern unsigned char PauseFocusChange;
extern unsigned char HighPriority;
extern unsigned char DisableScreenSaver;
#endif
#ifndef NO_PNG
#ifdef __MSDOS__
#endif
#else
#endif
#ifndef NO_PNG
extern unsigned char ScreenShotFormat;
#else
extern unsigned char ScreenShotFormat;
#endif
extern unsigned char AutoPatch;
extern unsigned char DisplayInfo;
extern unsigned char RomInfo;
extern unsigned char FPSAtStart;
extern unsigned char TimerEnable;
extern unsigned char TwelveHourClock;
extern unsigned char ClockBox;
extern unsigned char SmallMsgText;
extern unsigned char GUIEnableTransp;
extern unsigned char NativeFilePicker;
extern unsigned char EmulateSavestates;
extern char LibretroPath[256];
#ifdef __MSDOS__
extern unsigned char Palette0;
#endif
#ifdef __WIN32__
extern unsigned char cvidmode;
extern unsigned char PrevWinMode;
extern unsigned char PrevFSMode;
#elif defined(__UNIXSDL__)
#ifndef __OPENGL__
#else
#endif
#ifdef __OPENGL__
#endif
extern unsigned char cvidmode;
extern unsigned char PrevWinMode;
extern unsigned char PrevFSMode;
#elif defined(__MSDOS__)
extern unsigned char cvidmode;
#endif
#ifndef __MSDOS__
extern unsigned int CustomResX;
extern unsigned int CustomResY;
#endif
#ifdef __OPENGL__
#else
#endif
#ifdef __MSDOS__
#endif
extern unsigned char antienab;
#ifdef __OPENGL__
extern unsigned char BilinearFilter;
#endif
#ifndef __MSDOS__
#else
#endif
extern unsigned char NTSCFilter;
#ifndef __MSDOS__
#endif
extern unsigned char NTSCBlend;
extern unsigned char NTSCRef;
#ifndef __MSDOS__
#endif
extern char NTSCHue;
extern char NTSCSat;
extern char NTSCCont;
extern char NTSCBright;
extern char NTSCSharp;
extern char NTSCGamma;
extern char NTSCRes;
extern char NTSCArt;
extern char NTSCFringe;
extern char NTSCBleed;
extern char NTSCWarp;
extern unsigned char En2xSaI;
#ifndef __MSDOS__
#else
#endif
extern unsigned char hqFilter;
#ifndef __MSDOS__
#ifdef __OPENGL_
#endif
extern unsigned char hqFilterlevel;
#endif
#ifdef __MSDOS__
#endif
extern unsigned char scanlines;
extern unsigned char GrayscaleMode;
extern unsigned int Mode7HiRes16b;
#ifndef __UNIXSDL__
extern unsigned char vsyncon;
#endif
#ifdef __WIN32__
extern unsigned char TripleBufferWin;
#elif defined(__MSDOS__)
extern unsigned char Triplebufen;
#endif
#ifdef __WIN32__
extern unsigned char ForceRefreshRate;
extern unsigned char SetRefreshRate;
extern unsigned char KitchenSync;
extern unsigned char KitchenSyncPAL;
#endif
#ifndef __MSDOS__
#ifdef __WIN32__
#elif defined(__UNIXSDL__)
#endif
extern unsigned char Keep4_3Ratio;
#else
extern unsigned int smallscreenon;
extern unsigned char ScreenScale;
#endif
extern unsigned char gammalevel;
#ifdef __UNIXSDL__
extern char libAoDriver[10];
#endif
extern unsigned char soundon;
extern unsigned char StereoSound;
extern unsigned char RevStereo;
extern unsigned char Surround;
#ifdef __WIN32__
extern unsigned char PrimaryBuffer;
#endif
#ifdef __MSDOS__
extern unsigned char Force8b;
#endif
#ifdef __MSDOS__
#endif
extern unsigned int SoundQuality;
extern unsigned char MusicRelVol;
extern unsigned char SoundInterpType;
extern unsigned char LowPassFilterType;
#ifdef __MSDOS__
extern unsigned char DisplayS;
#endif
extern unsigned char EchoDis;
extern unsigned char RewindType;
extern unsigned int RewindStates;
extern unsigned char RewindFrames;
extern unsigned char nosaveSRAM;
extern unsigned char SRAMSave5Sec;
extern unsigned char SRAMState;
extern unsigned char LatestSave;
extern unsigned char AutoIncSaveSlot;
extern unsigned char AutoIncSaveSlotBlock;
extern unsigned char AutoState;
extern unsigned char PauseLoad;
extern unsigned char PauseRewind;
extern unsigned int per2exec;
extern unsigned char HacksDisable;
extern unsigned char frameskip;
extern unsigned char maxskip;
extern unsigned char FastFwdToggle;
extern unsigned char FFRatio;
extern unsigned char SDRatio;
extern unsigned char EmuSpeed;
#ifndef __UNIXSDL__
#else
#endif
#ifdef __MSDOS__
extern char ROMPath[256];
extern char SRAMPath[256];
extern char SnapPath[256];
extern char SPCPath[256];
extern char BSXPath[256];
extern char STPath[256];
extern char SGPath[256];
extern char GNextPath[256];
extern char FEOEZPath[256];
extern char SJNSPath[256];
extern char MDHPath[256];
extern char SPL4Path[256];
#else
extern char ROMPath[1024];
extern char SRAMPath[1024];
extern char SnapPath[1024];
extern char SPCPath[1024];
extern char BSXPath[1024];
extern char STPath[1024];
extern char SGPath[1024];
extern char GNextPath[1024];
extern char FEOEZPath[1024];
extern char SJNSPath[1024];
extern char MDHPath[1024];
extern char SPL4Path[1024];
#endif
extern unsigned char guioff;
extern unsigned char showallext;
#ifdef __MSDOS__
extern unsigned char GUIloadfntype;
#else
extern unsigned char GUIloadfntype;
#endif
extern unsigned char prevloadiname[280];
extern unsigned char prevloaddnamel[5120];
extern unsigned char prevloadfnamel[5120];
extern unsigned char prevlfreeze;
extern unsigned char GUIRClick;
extern unsigned char lhguimouse;
extern unsigned char mouseshad;
#ifndef __MSDOS__
#endif
extern unsigned char mousewrap;
#ifdef __WIN32__
extern unsigned char TrapMouseCursor;
extern unsigned char MouseWheel;
#endif
extern unsigned char esctomenu;
extern unsigned char JoyPad1Move;
extern unsigned char FilteredGUI;
extern unsigned char newfont;
extern unsigned char savewinpos;
extern int GUIwinposx[];
extern int GUIwinposy[];
extern unsigned char GUIEffect;
extern unsigned char GUIRAdd;
extern unsigned char GUIGAdd;
extern unsigned char GUIBAdd;
extern unsigned char GUITRAdd;
extern unsigned char GUITGAdd;
extern unsigned char GUITBAdd;
extern unsigned char GUIWRAdd;
extern unsigned char GUIWGAdd;
extern unsigned char GUIWBAdd;
#ifdef __WIN32__
extern unsigned char AlwaysOnTop;
extern unsigned char SaveMainWindowPos;
extern short MainWindowX;
extern short MainWindowY;
extern unsigned char AllowMultipleInst;
#endif
extern unsigned char AutoLoadCht;
extern unsigned char CheatSrcByteSize;
extern unsigned char CheatSrcByteBase;
extern unsigned char CheatSrcSearchType;
extern unsigned char CheatUpperByteOnly;
extern unsigned char MovieDisplayFrame;
extern unsigned char MovieStartMethod;
extern unsigned char MZTForceRTR;
extern unsigned char MovieVideoMode;
extern unsigned char MovieAudio;
extern unsigned char MovieAudioCompress;
extern unsigned char MovieVideoAudio;
extern unsigned int KeyExtraEnab1;
extern unsigned int KeyExtraEnab2;
extern unsigned int KeySaveState;
extern unsigned int KeyStateSelct;
extern unsigned int KeyLoadState;
extern unsigned int KeyIncStateSlot;
extern unsigned int KeyDecStateSlot;
extern unsigned int KeyStateSlc0;
extern unsigned int KeyStateSlc1;
extern unsigned int KeyStateSlc2;
extern unsigned int KeyStateSlc3;
extern unsigned int KeyStateSlc4;
extern unsigned int KeyStateSlc5;
extern unsigned int KeyStateSlc6;
extern unsigned int KeyStateSlc7;
extern unsigned int KeyStateSlc8;
extern unsigned int KeyStateSlc9;
extern unsigned int KeyRewind;
extern unsigned int KeyFastFrwrd;
extern unsigned int KeySlowDown;
extern unsigned int KeyFRateUp;
extern unsigned int KeyFRateDown;
extern unsigned int KeyEmuSpeedUp;
extern unsigned int KeyEmuSpeedDown;
extern unsigned int KeyResetSpeed;
extern unsigned int EMUPauseKey;
extern unsigned int INCRFrameKey;
extern unsigned int KeyBGDisble0;
extern unsigned int KeyBGDisble1;
extern unsigned int KeyBGDisble2;
extern unsigned int KeyBGDisble3;
extern unsigned int KeySprDisble;
extern unsigned int KeyDisableSC0;
extern unsigned int KeyDisableSC1;
extern unsigned int KeyDisableSC2;
extern unsigned int KeyDisableSC3;
extern unsigned int KeyDisableSC4;
extern unsigned int KeyDisableSC5;
extern unsigned int KeyDisableSC6;
extern unsigned int KeyDisableSC7;
extern unsigned int KeyVolUp;
extern unsigned int KeyVolDown;
extern unsigned int KeyQuickExit;
extern unsigned int KeyQuickLoad;
extern unsigned int KeyQuickRst;
extern unsigned int KeyResetAll;
extern unsigned int KeyQuickClock;
extern unsigned int KeyQuickChat;
extern unsigned int KeyQuickSnapShot;
extern unsigned int KeyQuickSaveSPC;
extern unsigned int KeyUsePlayer1234;
extern unsigned int KeyDisplayFPS;
extern unsigned int KeyDisplayBatt;
extern unsigned int KeyIncreaseGamma;
extern unsigned int KeyDecreaseGamma;
extern unsigned int KeyInsrtChap;
extern unsigned int KeyPrevChap;
extern unsigned int KeyNextChap;
extern unsigned int KeyRTRCycle;
extern unsigned char TimeChecker;
extern unsigned int PrevBuildNum;
extern unsigned char FirstTimeData;
#ifndef NO_DEBUGGER
extern unsigned char debuggeron;
#endif
extern unsigned char cfgdontsave;

#ifdef __cplusplus
             }
#endif

