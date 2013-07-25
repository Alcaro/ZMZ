;Copyright (C) 1997-2007 ZSNES Team ( zsKnight, _Demo_, pagefault, Nach )
;
;http://www.zsnes.com
;http://sourceforge.net/projects/zsnes
;https://zsnes.bountysource.com
;
;This program is free software; you can redistribute it and/or
;modify it under the terms of the GNU General Public License
;version 2 as published by the Free Software Foundation.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program; if not, write to the Free Software
;Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

%include "macros.mac"

EXTSYM KeyRewind,statesaver,Voice0Status,UpdateDPage
EXTSYM StartGUI,romdata,initvideo,DosExit,sfxramdata,deinitvideo
EXTSYM vidbufferofsa,device2,RawDumpInProgress
EXTSYM KeySaveState,KeyLoadState,KeyQuickExit,KeyQuickLoad,KeyQuickRst
EXTSYM GUIDoReset,GUIReset,KeyOnStA,KeyOnStB,ProcessKeyOn,C4Enable,KeyQuickClock
EXTSYM KeyQuickSaveSPC,TimerEnable,splitflags,joinflags
EXTSYM KeyQuickSnapShot,csounddisable,videotroub,ResetTripleBuf
EXTSYM InitPreGame,Curtableaddr,curcyc,debugdisble,dmadata,guioff,memtabler8
EXTSYM SetupPreGame,memtablew8,regaccessbankr8,showmenu,snesmap2,snesmmap
EXTSYM DeInitPostGame,spcPCRam,xp,xpb,xpc,tablead
EXTSYM tableadc,SA1UpdateDPage,Makemode7Table,nextmenupopup,MovieProcessing
EXTSYM SFXEnable,wramdata,cycpbl,cycpblt,irqon,spcon
EXTSYM multchange,romispal,scrndis,sprlefttot,sprleftpr,processsprites
EXTSYM cachesprites,opcjmptab,CheatOn,Output_Text,Check_Key,Get_Key,
EXTSYM INTEnab,JoyCRead,NMIEnab,NumCheats,CurrentExecSA1,ReadInputDevice
EXTSYM StartDrawNewGfx,VIRQLoc,cachevideo,cfield,cheatdata,curblank,curnmi
EXTSYM curypos,cycpl,doirqnext,drawline,exechdma,hdmadelay,intrset,newengen
EXTSYM oamaddr,oamaddrs,resolutn,showvideo,starthdma,switchtonmi
EXTSYM switchtovirq,totlines,updatetimer,SA1Swap,SA1DoIRQ,JoyAOrig,JoyANow
EXTSYM JoyBOrig,JoyBNow,JoyCOrig,JoyCNow,JoyDOrig,JoyDNow,JoyEOrig,JoyENow
EXTSYM SA1Message,MultiTapStat,idledetectspc,SA1Control,SA1Enable,SA1IRQEnable
EXTSYM SPC700read,SPC700write,numspcvblleft,spc700idle,SA1IRQExec,ForceNewGfxOff
EXTSYM LethEnData,GUIQuit,IRAM,SA1Ptr,SA1BWPtr,outofmemfix
EXTSYM yesoutofmemory,ProcessMovies,MovieStop,ppustatus,C4VBlank
EXTSYM ReturnFromSPCStall,scanlines,MainLoop,MoviePassWaiting,MovieDumpRaw
EXTSYM NumberOfOpcodes,SfxCLSR,SfxSCMR,SfxPOR,sfx128lineloc,sfx160lineloc
EXTSYM sfx192lineloc,sfxobjlineloc,sfxclineloc,PLOTJmpa,PLOTJmpb,FxTable
EXTSYM FxTableb,FxTablec,FxTabled,SfxPBR,SCBRrel,SfxSCBR,SfxCOLR,SFXCounter
EXTSYM fxbit01,fxbit01pcal,fxbit23,fxbit23pcal,fxbit45,fxbit45pcal,fxbit67
EXTSYM fxbit67pcal,SfxSFR,nosprincr,cpucycle,switchtovirqdeb,switchtonmideb
EXTSYM MovieSeekBehind,BackupCVFrame,RestoreCVFrame,loadstate
EXTSYM KeyInsrtChap,KeyNextChap,KeyPrevChap,MovieInsertChapter,MovieSeekAhead
EXTSYM ResetDuringMovie,EMUPauseKey,INCRFrameKey,MovieWaiting,NoInputRead
EXTSYM AllocatedRewindStates,PauseFrameMode,RestorePauseFrame,BackupPauseFrame
EXTSYM RewindType

%ifndef NO_DEBUGGER
EXTSYM debuggeron,startdebugger
%endif

%ifdef __MSDOS__
EXTSYM dssel,Game60hzcall,NextLineStart,FlipWait,LastLineStart,smallscreenon,ScreenScale
EXTSYM cvidmode,GUI16VID,ScreenShotFormat
%endif

SECTION .data
NEWSYM tempedx, dd 0
NEWSYM tempesi, dd 0
NEWSYM tempedi, dd 0
NEWSYM tempebp, dd 0
NEWSYM RewindTimer, dd 0
NEWSYM BackState, db 1
NEWSYM BackStateSize, dd 6
NEWSYM DblRewTimer, dd 0
SECTION .text

NEWSYM ProcessRewind
    mov al, [RewindType]
    test al, al
    je .rewindPress
.rewindHold
    mov eax,[KeyRewind]
    mov al, byte[pressed+eax]
    test al, al
    je .notokay
    jmp .doit

.rewindPress
    mov eax,[KeyRewind]
    cmp byte[pressed+eax],1
    jne near .notokay
    mov byte[pressed+eax],2
.doit

    pushad
    call RestoreCVFrame
    popad

    cmp byte[PauseFrameMode],1
    jne .notpauserewind
    pushad
    call BackupPauseFrame
    popad
.notpauserewind

    mov esi,[tempesi]
    mov edi,[tempedi]
    mov ebp,[tempebp]
    mov edx,[tempedx]

.notokay
    ret

NEWSYM UpdateRewind
    cmp byte[AllocatedRewindStates],0
    je .norewinds
    cmp dword[KeyRewind],0
    je .norewinds

    mov al, [RewindType]
    test al, al
    jne .notimer
    dec dword[DblRewTimer]
    dec dword[RewindTimer]
    jnz .checkrewind
.notimer

    mov [tempedx],edx
    mov [tempesi],esi
    mov [tempedi],edi
    mov [tempebp],ebp

    mov eax,[KeyRewind]
    cmp byte[pressed+eax],1
    je near .checkrewind

    pushad
    call BackupCVFrame
    popad

.checkrewind
    call ProcessRewind
.norewinds
    ret

SECTION .data
NEWSYM MuteVoiceF, dd 0
SECTION .text

VoiceEndMute:
    mov byte[MuteVoiceF],0
    ret

%macro StartMute 1
    mov al,[Voice0Status+%1]
    or al,al
    jz %%notmuted
    or byte[MuteVoiceF],1 << %1
%%notmuted
%endmacro

VoiceStartMute:
    mov byte[MuteVoiceF],0
    push eax
    StartMute 0
    StartMute 1
    StartMute 2
    StartMute 3
    StartMute 4
    StartMute 5
    StartMute 6
    StartMute 7
    pop eax
    ret


%macro stim 0
%ifdef __MSDOS__
    sti
%endif
%endmacro

%macro ProcessIRQStuff 0
    ; check for VIRQ/HIRQ
    test dl,04h
    jnz %%virqdo
    cmp byte[doirqnext],1
    je near .virq
%%virqdo
    test byte[INTEnab],20h
    jz near %%novirq
    mov ax,[VIRQLoc]
    cmp ax,[resolutn]
    jne %%notres
    dec ax
;    inc ax
%%notres
    cmp ax,0FFFFh
    jne %%notzero
    xor ax,ax
%%notzero
    cmp word[curypos],ax
    jne near %%noirq
    test byte[INTEnab],10h
    jnz %%tryhirq
%%startirq
    cmp byte[intrset],1
    jne %%nointrseta
    mov byte[intrset],2
%%nointrseta
    mov byte[irqon],80h
    test dl,04h
    jnz %%irqd
    mov byte[doirqnext],1
    jmp .virq
%%novirq
    test byte[INTEnab],10h
    jz %%noirq
%%setagain
    cmp byte[intrset],2
    jbe %%nointrseta3
    dec byte[intrset]
    cmp byte[intrset],2
    ja %%noirq
%%nointrseta3
    cmp byte[intrset],1
    jne %%nointrseta2
    test byte[INTEnab],80h
    jz %%tryhirq
    mov byte[intrset],8
    jmp %%noirq
%%nointrseta2
    test dl,04h
    jnz %%noirq
%%tryhirq
    jmp %%startirq
%%irqd
    mov byte[doirqnext],1
%%noirq
%endmacro


; .returnfromsfx

; pexecs
; *** Copy to PC whenever a non-relative jump is executed

SECTION .data
NEWSYM romloadskip, db 0
NEWSYM SSKeyPressed, dd 0
NEWSYM SPCKeyPressed, dd 0
NEWSYM NoSoundReinit, dd 0
NEWSYM NextNGDisplay, db 0
NEWSYM TempVidInfo, dd 0
NEWSYM tempdh, db 0

SECTION .text


; this wonderful mess starts up the CPU and initialized the emulation state
NEWSYM start65816

    call initvideo

    cmp byte[videotroub],1
    jne .notrouble
    ret
.notrouble

    mov edi,[vidbufferofsa]
    mov ecx,37518
    xor eax,eax
    rep stosd
    cmp byte[romloadskip],1
    je near StartGUI

NEWSYM continueprog
    ; clear keyboard presses
    mov esi,pressed
    mov ecx,256+128+64
    mov al,0
.loopa
    mov [esi],al
    inc esi
    dec ecx
    jnz .loopa

    mov byte[romloadskip],0
%ifndef NO_DEBUGGER
    mov byte[debuggeron],0
%endif
    mov byte[exiter],0

    call InitPreGame
    jmp reexecute

NEWSYM continueprognokeys
    mov byte[romloadskip],0
%ifndef NO_DEBUGGER
    mov byte[debuggeron],0
%endif
    mov byte[exiter],0

    call InitPreGame
    jmp reexecuteb2

; Incorrect

NEWSYM reexecuteb
%ifndef __MSDOS__
    jmp reexecuteb2
%endif
NEWSYM reexecute

    ; clear keyboard presses
    mov esi,pressed
    mov ecx,256+128+64
    mov al,0
.loopa
    cmp byte[esi],2
    jne .notclear
    mov [esi],al
.notclear
    inc esi
    dec ecx
    jnz .loopa
reexecuteb2:
    cmp byte[NoSoundReinit],1
    je .skippregame
    call SetupPreGame
.skippregame

    ; initialize variables (Copy from variables)
    xor eax,eax
    xor ebx,ebx
    xor ecx,ecx
    xor edx,edx
    mov bl,[xpb]
    mov ax,[xpc]
    test ax,8000h
    jz .loweraddr
    mov esi,[snesmmap+ebx*4]
    jmp .skiplower
.loweraddr
    cmp ax,4300h
    jb .lower
    cmp dword[memtabler8+ebx*4],regaccessbankr8
    je .dma
.lower
    mov esi,[snesmap2+ebx*4]
    jmp .skiplower
.dma
    mov esi,dmadata-4300h
.skiplower
    mov [initaddrl],esi
    add esi,eax                 ; add program counter to address
    mov dl,[xp]                 ; set flags
    mov dh,[curcyc]             ; set cycles

    mov bl,dl

    mov edi,[tableadc+ebx*4]
    or byte[curexecstate],2

    mov ebp,[spcPCRam]

    mov byte[NoSoundReinit],0
    mov byte[csounddisable],0
    mov byte[NextNGDisplay],0

EXTSYM zmz_main
call zmz_main
;    call splitflags

;    call execute

;    call joinflags

    ; de-init variables (copy to variables)

    mov [spcPCRam],ebp
    mov [Curtableaddr],edi
    mov [xp],dl
    mov [curcyc],dh

    mov eax,[initaddrl]
    sub esi,eax                 ; subtract program counter by address
    mov [xpc],si
    call ResetTripleBuf

    mov eax,[KeySaveState]
    test byte[pressed+eax],1
    jnz .soundreinit
    mov eax,[KeyLoadState]
    test byte[pressed+eax],1
    jz .skipsoundreinit
.soundreinit
    mov byte[NoSoundReinit],1
    mov byte[csounddisable],1
.skipsoundreinit

    cmp byte[NoSoundReinit],1
    je .skippostgame
    call DeInitPostGame
.skippostgame

    ;Multipass Movies
    cmp byte[MoviePassWaiting],1
    jne .nomoviepasswaiting
    pushad
    call MovieDumpRaw
    popad
    jmp continueprog
.nomoviepasswaiting

    ; clear all keys
    call Check_Key
    cmp al,0
    je .nokeys
.yeskeys
    call Get_Key
    call Check_Key
    cmp al,0
    jne .yeskeys
.nokeys

    cmp byte[nextmenupopup],1
    je near showmenu
    cmp byte[ReturnFromSPCStall],1
    je near .activatereset
    mov eax,[KeySaveState]
    test byte[pressed+eax],1
    jz .nosavestt
    mov byte[pressed+1],0
    mov byte[pressed+eax],2
    pushad
    call statesaver
    popad
    jmp reexecuteb
.nosavestt
    mov eax,[KeyLoadState]
    test byte[pressed+eax],1
    jz .noloadstt0
    pushad
    call loadstate
    popad
    jmp reexecuteb
.noloadstt0
    mov eax,[KeyInsrtChap]
    test byte[pressed+eax],1
    jz .noinsertchapter
    mov byte[pressed+eax],0
    pushad
    call MovieInsertChapter
    popad
    jmp continueprognokeys
.noinsertchapter
    mov eax,[KeyNextChap]
    test byte[pressed+eax],1
    jz .nonextchapter
    mov byte[pressed+eax],0
    mov byte[multchange],1
    pushad
    call MovieSeekAhead
    popad
    jmp continueprognokeys
.nonextchapter
    mov eax,[KeyPrevChap]
    test byte[pressed+eax],1
    jz .noprevchapter
    mov byte[pressed+eax],0
    mov byte[multchange],1
    pushad
    call MovieSeekBehind
    popad
    jmp continueprognokeys
.noprevchapter
    cmp byte[SSKeyPressed],1
    je near showmenu
    cmp byte[SPCKeyPressed],1
    je near showmenu
%ifndef NO_DEBUGGER
   cmp byte[debugdisble],0
    jne .nodebugger
    test byte[pressed+59],1
    jne near startdebugger
.nodebugger
%endif
    test byte[pressed+59],1
    jne near showmenu
    mov eax,[KeyQuickRst]
    test byte[pressed+eax],1
    jz .noreset
.activatereset
    pushad
    mov byte[GUIReset],1
    cmp byte[MovieProcessing],2 ;Recording
    jne .nomovierecording
    call ResetDuringMovie
    jmp .movieendif
.nomovierecording
    call GUIDoReset
.movieendif
    popad
    mov byte[ReturnFromSPCStall],0
    jmp continueprog
.noreset
    cmp byte[guioff],1
    je near endprog
    mov eax,[KeyQuickExit]
    test byte[pressed+eax],1
    jnz near endprog
    jmp StartGUI

NEWSYM endprog
    call deinitvideo
    pushad
    call MovieStop
    popad

    jmp DosExit

NEWSYM interror
    stim
    call deinitvideo
    mov edx,.nohand         ;use extended
    mov ah,9                ;DOS- API
    call Output_Text                 ;to print a string
    jmp DosExit

SECTION .data
.nohand db 'Cannot process interrupt handler!',13,10,0

; global variables
NEWSYM invalid, db 0
NEWSYM invopcd, db 0
NEWSYM pressed, times 256+128+64 db 0          ; keyboard pressed keys in scancode
NEWSYM exiter, db 0
NEWSYM oldhand9o, dd 0
NEWSYM oldhand9s, dw 0
NEWSYM oldhand8o, dd 0
NEWSYM oldhand8s, dw 0
NEWSYM opcd,      dd 0
NEWSYM pdh,       dd 0
NEWSYM pcury,     dd 0
NEWSYM timercount, dd 0
NEWSYM initaddrl, dd 0                  ; initial address location
NEWSYM NetSent, dd 0
NEWSYM nextframe, dd 0                  ; tick count for timer
NEWSYM curfps,    db 0                  ; frame/sec for current screen
;NEWSYM newgfxerror, db 'NEED MEMORY FOR GFX ENGINE',0
;NEWSYM newgfxerror2, db 'NEED 320x240 FOR NEW GFX 16B',0
;newgfxerror db 'NEW GFX IN 16BIT IS N/A',0
NEWSYM HIRQCycNext,   dd 0
NEWSYM HIRQNextExe,   db 0


SECTION .text

;*******************************************************
; Int 08h vector
;*******************************************************

; sets to either 60Hz or 50Hz depending on PAL/NTSC
NEWSYM init60hz
    cmp byte[romispal],0
    jne .dopal
    mov al,00110110b
    out 43h,al
    mov ax,19900        ; 65536/(60/((65536*24+175)/(60*60*24)))
    mov dword[timercount],19900
    out 40h,al
    mov al,ah
    out 40h,al
    ret
.dopal
    mov al,00110110b
    out 43h,al
    mov ax,23863        ; 65536/(50/((65536*24+175)/(60*60*24)))
    mov dword[timercount],23863
    out 40h,al
    mov al,ah
    out 40h,al
    ret

NEWSYM init18_2hz
    mov al,00110110b
    out 43h,al
    mov ax,0
    mov dword[timercount],65536
    out 40h,al
    mov al,ah
    out 40h,al
    ret

%ifdef __MSDOS__
NEWSYM handler8h
    cli
    push ds
    push eax
;    mov ax,0
    mov ax,[cs:dssel]
NEWSYM handler8hseg
    mov ds,ax
    call Game60hzcall
    mov eax,[timercount]
    sub dword[timeradj],eax
    jnc .noupd
    add dword[timeradj],65536
    pushf
    call far [oldhand8o]
.noupd
    mov al,20h
    out 20h,al
    pop eax
    pop ds
    sti
    iretd
%endif

SECTION .data
NEWSYM timeradj, dd 65536
NEWSYM t1cc, dw 0
SECTION .text

;*******************************************************
; Int 09h vector
;*******************************************************

%ifdef __MSDOS__
SECTION .bss
NEWSYM skipnextkey42, resb 1
SECTION .text

NEWSYM handler9h
    cli
    push ds
    push eax
    push ebx
    mov ax,[cs:dssel]
    mov ds,ax
    xor ebx,ebx
    in al,60h                 ; get keyboard scan code
    cmp al,42
    jne .no42
    cmp byte[skipnextkey42],0
    je .no42
    mov byte[skipnextkey42],0
    jmp .skipkeyrel
.no42
    cmp al,0E0h
    jne .noE0
    mov byte[skipnextkey42],1
    jmp .skipkeyrel
.noE0
    mov byte[skipnextkey42],0
    mov bl,al
    xor bh,bh
    test bl,80h               ; check if bit 7 is on (key released)
    jnz .keyrel
    cmp byte[pressed+ebx],0
    jne .skipa
    mov byte[pressed+ebx],1        ; if not, set key to pressed
.skipa
    jmp .skipkeyrel
.keyrel
    and ebx,7Fh
    cmp ebx,59
    je .skipkeyrel
    cmp ebx,[KeySaveState]
    je .skipkeyrel
    cmp ebx,[KeyLoadState]
    je .skipkeyrel
    cmp ebx,[KeyQuickExit]
    je .skipkeyrel
    cmp ebx,[KeyQuickLoad]
    je .skipkeyrel
    cmp ebx,[KeyQuickRst]
    je .skipkeyrel
    cmp bl,1
    je .skipkeyrel
    mov byte[pressed+ebx],0        ; if not, set key to pressed
.skipkeyrel
    mov byte[pressed],0
    in al,61h
    mov ah,al
    or al,80h
    out 61h,al
    mov al,20h                ; turn off interrupt mode
    out 20h,al
    pop ebx                          ; Pop registers off
    pop eax                          ; stack in correct
    pop ds
    sti
    iretd
%endif

SECTION .data
ALIGN32
NEWSYM soundcycleft, dd 0
NEWSYM curexecstate, dd 0

NEWSYM nmiprevaddrl, dd 0       ; observed address -5
NEWSYM nmiprevaddrh, dd 0       ; observed address +5
NEWSYM nmirept,      dd 0       ; NMI repeat check, if 6 then okay
NEWSYM nmiprevline,  dd 224     ; previous line
NEWSYM nmistatus,    dd 0       ; 0 = none, 1 = waiting for nmi location,
                        ; 2 = found, disable at next line
NEWSYM joycontren,   dd 0       ; joystick read control check
NEWSYM NextLineCache, db 0
NEWSYM ZMVZClose, db 0

SECTION .text

Donextlinecache:
    cmp word[curypos],0
    je .nocache
    mov ax,[resolutn]
    dec ax
    cmp word[curypos],ax
    jae .nocache
    test byte[scrndis],10h
    jnz .nocache
    cmp byte[curblank],0h
    jne .nocache
    push ecx
    push ebx
    push esi
    push edi
    xor ecx,ecx
    mov cl,[curypos]
    push edx
.next
    mov byte[sprlefttot+ecx],0
    mov dword[sprleftpr+ecx*4],0
    inc cl
    jnz .next
    call processsprites
    call cachesprites
    pop edx
    pop edi
    pop esi
    pop ebx
    pop ecx
.nocache
    mov byte[NextLineCache],0
    ret

;*******************************************************
; 65816 execution
;*******************************************************

SECTION .text

NEWSYM exitloop2
   mov byte[ExecExitOkay],0
NEWSYM exitloop
   ret

ALIGN16

%macro FlipCheck 0
%ifdef __MSDOS__
   cmp byte[FlipWait],0
   je %%noflip
   push edx
   push eax
   mov dx,3DAh             ;VGA status port
   in al,dx
   test al,8
   jz %%skipflip
   push ebx
   push ecx
   mov ax,4F07h
   mov bh,00h
   mov bl,00h
   xor cx,cx
   mov dx,[NextLineStart]
   mov [LastLineStart],dx
   int 10h
   mov byte[FlipWait],0
   pop ecx
   pop ebx
%%skipflip
   pop eax
   pop edx
%%noflip
%endif
%endmacro

NEWSYM execute
NEWSYM execloop
   mov bl,dl
   test byte[curexecstate],2
   jnz .sound
.startagain
   call dword near [edi+ebx*4]
.cpuover
   jmp cpuover
.sound
   mov edi,[tableadc+ebx*4]
%ifdef OPENSPC
   pushad
   mov bl,[esi]
   movzx eax,byte[cpucycle+ebx]
   mov ebx,0xC3A13DE6
   mul ebx
   add [ospc_cycle_frac],eax
   adc [SPC_Cycles],edx
   call OSPC_Run
   popad
%else
   sub dword[cycpbl],55
   jnc .skipallspc
   mov eax,[cycpblt]
   mov bl,[ebp]
   add dword[cycpbl],eax
   ; 1260, 10000/12625
   inc ebp
   call dword near [opcjmptab+ebx*4]
   xor ebx,ebx
.skipallspc
%endif
   mov bl,[esi]
   inc esi
   sub dh,[cpucycle+ebx]
   jc .cpuovers
   call dword near [edi+ebx*4]
.cpuovers
   jmp cpuover



SECTION .data
ALIGN32
NEWSYM ExecExitOkay, db 1
NEWSYM JoyABack, dd 0
NEWSYM JoyBBack, dd 0
NEWSYM JoyCBack, dd 0
NEWSYM JoyDBack, dd 0
NEWSYM JoyEBack, dd 0
NEWSYM NetCommand, dd 0
NEWSYM spc700read, dd 0
NEWSYM lowestspc,  dd 0
NEWSYM highestspc, dd 0
NEWSYM SA1UBound,  dd 0
NEWSYM SA1LBound,  dd 0
NEWSYM SA1SH,      dd 0
NEWSYM SA1SHb,     dd 0
NEWSYM NumberOfOpcodes2, dd 0
NEWSYM ChangeOps, dd 0
NEWSYM SFXProc,    dd 0
NEWSYM EMUPause, db 0
NEWSYM INCRFrame, db 0
NEWSYM NoHDMALine, db 0
SECTION .text

NEWSYM cpuover
int 3

SECTION .bss
.numcheat resb 1
SECTION .text

ALIGN16

NEWSYM pexecs
NEWSYM pexecs2
NEWSYM UpdatePORSCMR
NEWSYM UpdateSCBRCOLR
NEWSYM UpdateCLSR
NEWSYM UpdateSFX
NEWSYM StartSFX
NEWSYM StartSFXdebug
NEWSYM StartSFXdebugb
NEWSYM StartSFXret
NEWSYM execloopdeb
NEWSYM execsingle
int 3
