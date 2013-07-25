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

EXTSYM winbgdata

SECTION .bss
NEWSYM bgcoloradder, resb 1
NEWSYM res512switch, resb 1

SECTION .text

SECTION .data
NEWSYM MosaicYAdder, dw 0,0,0,1,0,2,1,0,0,4,2,2,3,1,0,7

NEWSYM cwinptr,    dd winbgdata

SECTION .bss
NEWSYM pwinbgenab, resb 1
NEWSYM pwinbgtype, resd 1
NEWSYM winonbtype, resb 1
NEWSYM dualwinbg,  resb 1
NEWSYM pwinspenab, resb 1
NEWSYM pwinsptype, resd 1
NEWSYM winonstype, resb 1
NEWSYM dualwinsp,  resb 1
NEWSYM dwinptrproc, resd 1
SECTION .text

NEWSYM makewindow
NEWSYM makedualwin
NEWSYM dualstartprocess
NEWSYM dualwinand
NEWSYM dualwinor
NEWSYM dualwinxor
NEWSYM dualwinxnor
int 3

SECTION .bss
NEWSYM winonsp, resb 1
SECTION .text

NEWSYM makewindowsp
NEWSYM makedualwinsp
int 3

; window logic data
SECTION .bss
NEWSYM windowdata, resb 16
NEWSYM numwin, resb 1
NEWSYM multiwin, resb 1
NEWSYM multiclip, resb 1
NEWSYM multitype, resb 1
SECTION .text

NEWSYM procspritessub
NEWSYM procspritesmain
int 3

SECTION .bss
NEWSYM curbgnum, resb 1
SECTION .text

NEWSYM drawbackgrndsub
NEWSYM drawbackgrndmain
NEWSYM procbackgrnd
int 3

SECTION .bss
NEWSYM nextprimode, resb 1
NEWSYM cursprloc,   resd 1
NEWSYM curcolor,    resb 1
NEWSYM curtileptr,  resw 1
; esi = pointer to video buffer
; edi = pointer to tile data
; ebx = cached memory
; al = current x position
NEWSYM bg1vbufloc,  resd 1
NEWSYM bg2vbufloc,  resd 1
NEWSYM bg3vbufloc,  resd 1
NEWSYM bg4vbufloc,  resd 1
NEWSYM bg1tdatloc,  resd 1
NEWSYM bg2tdatloc,  resd 1
NEWSYM bg3tdatloc,  resd 1
NEWSYM bg4tdatloc,  resd 1
NEWSYM bg1tdabloc,  resd 1
NEWSYM bg2tdabloc,  resd 1
NEWSYM bg3tdabloc,  resd 1
NEWSYM bg4tdabloc,  resd 1
NEWSYM bg1cachloc,  resd 1
NEWSYM bg2cachloc,  resd 1
NEWSYM bg3cachloc,  resd 1
NEWSYM bg4cachloc,  resd 1
NEWSYM bg1yaddval,  resd 1
NEWSYM bg2yaddval,  resd 1
NEWSYM bg3yaddval,  resd 1
NEWSYM bg4yaddval,  resd 1
NEWSYM bg1xposloc,  resd 1
NEWSYM bg2xposloc,  resd 1
NEWSYM bg3xposloc,  resd 1
NEWSYM bg4xposloc,  resd 1
NEWSYM alreadydrawn, resb 1
SECTION .text

NEWSYM fillwithnothing
int 3

SECTION .bss
NEWSYM bg3draw, resb 1
NEWSYM maxbr,   resb 1
SECTION .text

NEWSYM blanker
int 3

ALIGN32
SECTION .bss
NEWSYM bg3high2, resd 1
NEWSYM cwinenabm, resd 1
SECTION .text

NEWSYM drawline
int 3

;NEWSYM nodrawline
    ret

NEWSYM priority2
int 3

ALIGN32
SECTION .bss
NEWSYM tempbuffer, resd 33
NEWSYM currentobjptr, resd 1
NEWSYM curmosaicsz,   resd 1
NEWSYM extbgdone, resb 1
SECTION .text


NEWSYM processmode7
NEWSYM drawsprites
NEWSYM drawspriteswinon
NEWSYM drawspritesprio
NEWSYM drawspritespriowinon
int 3

SECTION .data
NEWSYM prfixobjl, db 0
NEWSYM csprbit, db 1
NEWSYM csprprlft, db 0
SECTION .text

NEWSYM proc8x8
NEWSYM proc16x8
int 3

SECTION .bss
NEWSYM drawn, resb 1
NEWSYM curbgpr, resb 1    ; 00h = low priority, 20h = high priority
SECTION .text


SECTION .bss
NEWSYM winptrref, resd 1
SECTION .text

NEWSYM draw8x8
NEWSYM draw8x8winon
int 3

SECTION .bss
NEWSYM alttile, resb 1
NEWSYM hirestiledat, resb 256
SECTION .text

NEWSYM draw16x8
NEWSYM draw16x8b
NEWSYM draw16x8winon
NEWSYM draw16x8bwinon
int 3

SECTION .data
NEWSYM extraleft, db 0,0,0,1,0,1,2,2,0,2,3,1,2,4,2,1
SECTION .text

NEWSYM domosaic
NEWSYM domosaicwin
NEWSYM dowindow
int 3

ALIGN32

SECTION .bss
NEWSYM yadder,     resd 1
NEWSYM yrevadder,  resd 1
NEWSYM tempcach,   resd 1        ; points to cached memory
NEWSYM temptile,   resd 1        ; points to the secondary video pointer
NEWSYM bgptr,      resd 1
NEWSYM bgptrb,     resd 1
NEWSYM bgptrc,     resd 1
NEWSYM bgptrd,     resd 1
NEWSYM bgptrx1,    resd 1
NEWSYM bgptrx2,    resd 1
NEWSYM curvidoffset, resd 1
NEWSYM winon,      resd 1
NEWSYM bgofwptr,   resd 1
NEWSYM bgsubby,    resd 1
SECTION .text


NEWSYM draw8x8offset
NEWSYM draw8x8winonoffset
int 3

ALIGN32

SECTION .bss
NEWSYM offsetmodeptr, resd 1
NEWSYM offsetptra,    resd 1
NEWSYM offsetptrb,    resd 1
NEWSYM prevtempcache, resd 1
NEWSYM prevoffsetdat, resd 1
NEWSYM offsetenab,    resd 1
NEWSYM offsettilel,   resd 1
NEWSYM offsetrevval,  resd 1
NEWSYM posyscroll,    resd 1
NEWSYM offsetmcol,    resd 1
NEWSYM offsetmshl,    resd 1
NEWSYM offsetmptr,    resd 1
NEWSYM offsetmtst,    resd 1
NEWSYM offsetmclr,    resd 1
NEWSYM offsetcedi,    resd 1
SECTION .text

NEWSYM proc16x16
NEWSYM draw16x16
int 3

SECTION .bss
.yadd      resw 1
.yflipadd  resw 1
SECTION .text

NEWSYM draw16x16winon
int 3

SECTION .bss
NEWSYM temp,       resb 1
NEWSYM bshifter,   resb 1
NEWSYM a16x16xinc, resb 1
NEWSYM a16x16yinc, resb 1
SECTION .text
