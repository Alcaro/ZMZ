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



EXTSYM initsfxregsw,reg420Bw,reg420Cw,regptw,initSA1regsw,SDD1Reset
EXTSYM SPC7110Reset,RTCReset2,NextLineCache,vidmemch2,vidmemch4
EXTSYM vidmemch8,vrama,nmirept,SPCRAM,HIRQCycNext,HIRQNextExe,tableadc
EXTSYM cycpb268,cycpb358,cycpbl,cycpblt,opexec268,opexec268cph,opexec358
EXTSYM opexec358cph

%ifndef NO_DEBUGGER
EXTSYM sndwrit,debstop
%endif

SECTION .text

NEWSYM initregw
int 3

SECTION .bss
NEWSYM prevoamptr, resb 1
SECTION .text

SECTION .bss
NEWSYM oamlow, resb 1
SECTION .text

section .data
NEWSYM bgscrolPrev, db 0
NEWSYM bg1scrolx_m7, dw 0
NEWSYM bg1scroly_m7, dw 0

section .text
NEWSYM reg2118
NEWSYM reg2118inc
NEWSYM reg2118inc8
NEWSYM reg2118inc8inc
NEWSYM reg2119
NEWSYM reg2119inc
NEWSYM reg2119inc8
NEWSYM reg2119inc8inc
int 3

SECTION .data
NEWSYM multchange, db 1
NEWSYM m7byte, db 0
SECTION .text

; Joystick 1 & 2 status bytes
SECTION .bss
NEWSYM MultiTapStat, resb 1
SECTION .text
