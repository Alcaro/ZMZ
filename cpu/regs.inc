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



EXTSYM SFXEnable,regptr,initsfxregsr,initSA1regs,SA1Enable,initSDD1regs
EXTSYM SPC7110Enable,initSPC7110regs,RTCEnable,RTCReset,curypos,cycpl
EXTSYM pdh,vram,romispal,reg1read,spcnumread,spcon,reg2read,reg3read
EXTSYM reg4read,JoyEOrig,JoyENow,device2,cycphb,joycontren,totlines

%ifndef NO_DEBUGGER
EXTSYM debuggeron
%endif

SECTION .text

NEWSYM initregr
int 3

; global variables
SECTION .data
NEWSYM invreg,    dw 0
NEWSYM sndrot,    db 0             ; rotates to use A,X or Y for sound skip
NEWSYM sndrot2,   db 0             ; rotates a random value for sound skip
NEWSYM INTEnab,   db 0             ; enables NMI(7)/VIRQ(5)/HIRQ(4)/JOY(0)
NEWSYM NMIEnab,   db 1             ; controlled in e65816 loop.  Sets to 81h
NEWSYM VIRQLoc,   dw 0             ; VIRQ Y location
NEWSYM vidbright, db 0             ; screen brightness (0 .. 15)
NEWSYM previdbr,  db 0             ; previous screen brightness
NEWSYM forceblnk, db 80h           ; force blanking on/off ($80=on)
NEWSYM objptr,    dd 0             ; pointer to object data in VRAM
NEWSYM objptrn,   dd 0             ; pointer2 to object data in VRAM
NEWSYM objsize1,  db 1             ; 1 = 8dot, 4=16 dot, 16=32 dot, 64=64dot
NEWSYM objsize2,  db 4             ; large object size
NEWSYM objmovs1,  db 2             ; number of bytes to move/paragraph
NEWSYM objadds1,  dw 14            ; number of bytes to add/paragraph
NEWSYM objmovs2,  db 2             ; number of bytes to move/paragraph
NEWSYM objadds2,  dw 14            ; number of bytes to add/paragraph
NEWSYM oamaddrt,  dw 0             ; oam address
NEWSYM oamaddrs,  dw 0             ; oam address at beginning of vblank
NEWSYM objhipr,   db 0             ; highest priority object #
NEWSYM bgmode,    db 0             ; graphics mode ( 0 .. 7 )
NEWSYM bg3highst, db 0             ; is 1 if background 3 has the highest priority
NEWSYM bgtilesz,  db 0             ; 0 = 8x8, 1 = 16x16, bit 0=bg1,bit1=bg2,etc
NEWSYM mosaicon,  db 0             ; mosaic on, bit 0=bg1,bit1=bg2, etc
NEWSYM mosaicsz,  db 0             ; mosaic size in pixels
NEWSYM bg1ptr,    dw 0             ; pointer to background1
NEWSYM bg2ptr,    dw 0             ; pointer to background2
NEWSYM bg3ptr,    dw 0             ; pointer to background3
NEWSYM bg4ptr,    dw 0             ; pointer to background4
NEWSYM bg1ptrb,   dw 0             ; pointer to background1
NEWSYM bg2ptrb,   dw 0             ; pointer to background2
NEWSYM bg3ptrb,   dw 0             ; pointer to background3
NEWSYM bg4ptrb,   dw 0             ; pointer to background4
NEWSYM bg1ptrc,   dw 0             ; pointer to background1
NEWSYM bg2ptrc,   dw 0             ; pointer to background2
NEWSYM bg3ptrc,   dw 0             ; pointer to background3
NEWSYM bg4ptrc,   dw 0             ; pointer to background4
NEWSYM bg1ptrd,   dw 0             ; pointer to background1
NEWSYM bg2ptrd,   dw 0             ; pointer to background2
NEWSYM bg3ptrd,   dw 0             ; pointer to background3
NEWSYM bg4ptrd,   dw 0             ; pointer to background4
NEWSYM bg1scsize, db 0             ; bg #1 screen size (0=1x1,1=1x2,2=2x1,3=2x2)
NEWSYM bg2scsize, db 0             ; bg #2 screen size (0=1x1,1=1x2,2=2x1,3=2x2)
NEWSYM bg3scsize, db 0             ; bg #3 screen size (0=1x1,1=1x2,2=2x1,3=2x2)
NEWSYM bg4scsize, db 0             ; bg #4 screen size (0=1x1,1=1x2,2=2x1,3=2x2)
NEWSYM bg1objptr, dw 0             ; pointer to tiles in background1
NEWSYM bg2objptr, dw 0             ; pointer to tiles in background2
NEWSYM bg3objptr, dw 0             ; pointer to tiles in background3
NEWSYM bg4objptr, dw 0             ; pointer to tiles in background4
NEWSYM bg1scrolx, dw 0             ; background 1 x position
NEWSYM bg2scrolx, dw 0             ; background 2 x position
NEWSYM bg3scrolx, dw 0             ; background 3 x position
NEWSYM bg4scrolx, dw 0             ; background 4 x position
NEWSYM bg1sx,     dw 0             ; Temporary Variable for Debugging purposes
NEWSYM bg1scroly, dw 0             ; background 1 y position
NEWSYM bg2scroly, dw 0             ; background 2 y position
NEWSYM bg3scroly, dw 0             ; background 3 y position
NEWSYM bg4scroly, dw 0             ; background 4 y position
NEWSYM addrincr,  dw 2             ; vram increment (2,64,128,256)
NEWSYM vramincr,  db 0             ; 0 = inrement at 2118/2139, 1 = 2119,213A
NEWSYM vramread,  db 0             ; previous character for vram read
NEWSYM vramaddr,  dd 0             ; vram address
NEWSYM cgaddr,    dw 0             ; cg (palette) address
NEWSYM cgmod,     db 0             ; if cgram is modified or not
NEWSYM scrnon,    dw 0             ; main & sub screen on
NEWSYM scrndist,  db 0             ; which background is disabled
NEWSYM resolutn,  dw 224           ; screen resolution
NEWSYM multa,     db 0             ; multiplier A
NEWSYM diva,      dw 0             ; divisor C
NEWSYM divres,    dw 0             ; quotent of divc/divb
NEWSYM multres,   dw 0             ; result of multa * multb/remainder of divc/divb
NEWSYM latchx,    dw 0             ; latched x value
NEWSYM latchy,    dw 0             ; latched y value
NEWSYM latchxr,   db 0             ; low or high byte read for x value
NEWSYM latchyr,   db 0             ; low or high byte read for y value
NEWSYM frskipper, db 0             ; used to control frame skipping
NEWSYM winl1,     db 0             ; window 1 left position
NEWSYM winr1,     db 0             ; window 1 right position
NEWSYM winl2,     db 0             ; window 2 left position
NEWSYM winr2,     db 0             ; window 2 right position
NEWSYM winbg1en,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on BG1
NEWSYM winbg2en,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on BG2
NEWSYM winbg3en,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on BG3
NEWSYM winbg4en,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on BG4
NEWSYM winobjen,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on sprites
NEWSYM wincolen,  db 0             ; Win1 on (IN/OUT) or Win2 on (IN/OUT) on backarea
NEWSYM winlogica, db 0             ; Window logic type for BG1 to 4
NEWSYM winlogicb, db 0             ; Window logic type for Sprites and Backarea
NEWSYM winenabm,  db 0             ; Window logic enable for main screen
NEWSYM winenabs,  db 0             ; Window logic enable for sub screen
NEWSYM mode7set,  db 0             ; mode 7 settings
NEWSYM mode7A,    dw 0             ; A value for Mode 7
NEWSYM mode7B,    dw 0             ; B value for Mode 7
NEWSYM mode7C,    dw 0             ; C value for Mode 7
NEWSYM mode7D,    dw 0             ; D value for Mode 7
NEWSYM mode7X0,   dw 0             ; Center X for Mode 7
NEWSYM mode7Y0,   dw 0             ; Center Y for Mode 7
NEWSYM JoyAPos,   db 0             ; Old-Style Joystick Read Position for Joy 1 & 3
NEWSYM JoyBPos,   db 0             ; Old-Style Joystick Read Position for Joy 2 & 4
NEWSYM compmult,  dd 0             ; Complement Multiplication for Mode 7
NEWSYM joyalt,    db 0             ; temporary joystick alternation

NEWSYM wramrwadr, dd 0             ; continuous read/write to wram address
NEWSYM dmadata, times 129 db 0FFh  ; dma data (written from ports 43xx)
NEWSYM irqon,     db 0             ; if IRQ has been called (80h) or not (0)
NEWSYM nexthdma,  db 0             ; HDMA data to execute once vblank ends
NEWSYM curhdma,   db 0             ; Currently executed hdma
NEWSYM hdmadata, times 8*19 db 0   ; 4 dword register addresses, # of bytes to
                           ; transfer/line, address increment (word)
NEWSYM hdmatype,  db 0             ; if first time executing hdma or not
NEWSYM coladdr,   db 0             ; red value of color to add
NEWSYM coladdg,   db 0             ; green value of color to add
NEWSYM coladdb,   db 0             ; blue value of color to add
NEWSYM colnull,   db 0             ; keep this 0 (when accessing colors by dword)
NEWSYM scaddset,  db 0             ; screen/fixed color addition settings
NEWSYM scaddtype, db 0             ; which screen to add/sub
NEWSYM Voice0Disabl2, db 1         ; Disable Voice 0
NEWSYM Voice1Disabl2, db 1         ; Disable Voice 1
NEWSYM Voice2Disabl2, db 1         ; Disable Voice 2
NEWSYM Voice3Disabl2, db 1         ; Disable Voice 3
NEWSYM Voice4Disabl2, db 1         ; Disable Voice 4
NEWSYM Voice5Disabl2, db 1         ; Disable Voice 5
NEWSYM Voice6Disabl2, db 1         ; Disable Voice 6
NEWSYM Voice7Disabl2, db 1         ; Disable Voice 7

NEWSYM oamram,  times 1024 db 0    ; OAMRAM (544 bytes)
NEWSYM cgram,   times 512 db 0     ; CGRAM
NEWSYM pcgram,  times 512 db 0     ; Previous CGRAM

NEWSYM vraminctype,  db 0

; New variables
NEWSYM vramincby8on,   db 0        ; if increment by 8 is on
NEWSYM vramincby8left, db 0        ; how many left
NEWSYM vramincby8totl, db 0        ; how many in total (32,64,128)
NEWSYM vramincby8rowl, db 0        ; how many left in that row (start at 8)
NEWSYM vramincby8ptri, dw 0        ; increment by how many when rowl = 0

NEWSYM nexthprior,     db 0
NEWSYM doirqnext,      db 0

NEWSYM vramincby8var,  dw 0
NEWSYM screstype,      db 0
NEWSYM extlatch,       db 0
NEWSYM cfield,         db 0
NEWSYM interlval,      db 0

NEWSYM HIRQLoc,   dw 0             ; HIRQ X location

NEWSYM KeyOnStA, db 0
NEWSYM KeyOnStB, db 0

NEWSYM SDD1BankA, db 0
NEWSYM SDD1BankB, db 1
NEWSYM SDD1BankC, db 2
NEWSYM SDD1BankD, db 3
NEWSYM vramread2, db 0             ; previous character for vram read
NEWSYM nosprincr, db 0
NEWSYM poamaddrs, dw 0
NEWSYM ioportval, db 255
NEWSYM iohvlatch, db 0
NEWSYM ppustatus, db 0
NEWSYM hdmastartsc, db 0
NEWSYM hdmarestart, db 0
NEWSYM hdmadelay, db 0
NEWSYM nohdmaframe, db 0           ; no hdma for current frame

NEWSYM tempdat, times 474 db 0     ; expandable area

num2writeppureg equ $-sndrot
; pharos equ hack *sigh*
NEWSYM PHnum2writeppureg, dd num2writeppureg

NEWSYM scrndis,   db 0             ; which background is disabled

ALIGN32
NEWSYM oamaddr,   dd 0             ; oam address

NEWSYM bg1ptrx,   dd 0             ; pointer to background1
NEWSYM bg2ptrx,   dd 0             ; pointer to background2
NEWSYM bg3ptrx,   dd 0             ; pointer to background3
NEWSYM bg4ptrx,   dd 0             ; pointer to background4

NEWSYM bg1ptry,   dd 0             ; pointer to background1
NEWSYM bg2ptry,   dd 0             ; pointer to background2
NEWSYM bg3ptry,   dd 0             ; pointer to background3
NEWSYM bg4ptry,   dd 0             ; pointer to background4

NEWSYM Voice0Disable, db 1         ; Disable Voice 0
NEWSYM Voice1Disable, db 1         ; Disable Voice 1
NEWSYM Voice2Disable, db 1         ; Disable Voice 2
NEWSYM Voice3Disable, db 1         ; Disable Voice 3
NEWSYM Voice4Disable, db 1         ; Disable Voice 4
NEWSYM Voice5Disable, db 1         ; Disable Voice 5
NEWSYM Voice6Disable, db 1         ; Disable Voice 6
NEWSYM Voice7Disable, db 1         ; Disable Voice 7

NEWSYM BG116x16t, db 0
NEWSYM BG216x16t, db 0
NEWSYM BG316x16t, db 0
NEWSYM BG416x16t, db 0
NEWSYM SPC700read, dd 0
NEWSYM SPC700write, dd 0

NEWSYM JoyCRead, db 0

NEWSYM nssdip1, db 0
NEWSYM nssdip2, db 0
NEWSYM nssdip3, db 0
NEWSYM nssdip4, db 0
NEWSYM nssdip5, db 0
NEWSYM nssdip6, db 0

SECTION .text

NEWSYM ClearRegs
int 3

SECTION .bss
NEWSYM MultiTap, resb 1
SECTION .text

SECTION .bss
NEWSYM hblank, resb 1
SECTION .text
