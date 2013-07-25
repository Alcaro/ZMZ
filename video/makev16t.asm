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

SECTION .text

NEWSYM makedualwincol
NEWSYM procmode716tsub
NEWSYM procmode716tsubextbg
NEWSYM procmode716tsubextbgb
NEWSYM procmode716tsubextbg2
NEWSYM procmode716tmain
NEWSYM procmode716tmainextbg
NEWSYM procmode716tmainextbgb
NEWSYM procmode716tmainextbg2
NEWSYM procspritessub16t
NEWSYM procspritesmain16t
NEWSYM drawbackgrndsub16t
NEWSYM drawbackgrndmain16t
NEWSYM procspritessub16tfix
NEWSYM procspritesmain16tfix
NEWSYM drawbackgrndsub16tfix
NEWSYM drawbackgrndmain16tfix
int 3

ALIGN32

SECTION .bss
NEWSYM transpbuf, resb 576+16+288*2        ; Transparent buffer
SECTION .text

NEWSYM drawline16t
NEWSYM NextDrawLine16bt
NEWSYM priority216t
NEWSYM Priority2NextDrawLine16bt
NEWSYM processmode716t
NEWSYM processmode716t2
int 3

SECTION .bss
NEWSYM prevrgbcol, resd 1
NEWSYM prevrgbpal, resd 1
SECTION .text

NEWSYM clearback16bts
int 3

SECTION .bss
mmxtempdat resd 2
SECTION .text

NEWSYM clearback16bts0b
NEWSYM clearback16bts0
NEWSYM dowindowback16b
NEWSYM dowindowback16brev
NEWSYM clearback16bdual
NEWSYM clearback16bdualrev
NEWSYM clearback16bdualb2
NEWSYM clearback16bdualrev2
int 3

SECTION .bss
NEWSYM DoTransp, resb 1
SECTION .text

NEWSYM clearback16t
NEWSYM clearback16ts
NEWSYM drawsprites16bt
NEWSYM drawsprites16btwinon
NEWSYM drawsprites16btprio
NEWSYM drawsprites16btpriow
NEWSYM drawsprites16t
NEWSYM drawsprites16twinon
NEWSYM drawspritesfulladdwinon
NEWSYM drawsprites16tprio
NEWSYM drawsprites16tpriow
NEWSYM drawspritesfulladdprio
NEWSYM drawspritesfulladdpriow
NEWSYM drawspritesfullsubprio
NEWSYM drawspritesfullsubpriow
NEWSYM draw8x816bt
NEWSYM draw8x816btwinon
NEWSYM draw8x816t
NEWSYM draw8x8fulladd
NEWSYM draw8x816ts
int 3

ALIGN32
SECTION .bss
NEWSYM coadder16, resd 1
SECTION .text

NEWSYM draw8x816twinon
NEWSYM draw8x8fulladdwinon
NEWSYM draw8x816tswinon
NEWSYM draw16x816t
NEWSYM draw16x816tb
NEWSYM draw16x816twinon
NEWSYM draw16x816twinonb
NEWSYM draw16x816tbfa
NEWSYM draw16x816twinonfa
NEWSYM draw16x816twinonbfa
NEWSYM draw16x816tbs
NEWSYM draw16x816twinons
NEWSYM draw16x816twinonbs
NEWSYM draw8x816toffset
NEWSYM draw8x8fulladdoffset
NEWSYM draw8x816tsoffset
NEWSYM draw8x816twinonoffset
NEWSYM draw8x8fulladdwinonoffset
NEWSYM draw8x816tswinonoffset
NEWSYM draw16x1616bt
NEWSYM draw16x1616btwinon
NEWSYM draw16x1616t
int 3

SECTION .bss
NEWSYM yadd,   resw 1
NEWSYM yflipadd,  resw 1
SECTION .text

NEWSYM draw16x16fulladd
NEWSYM draw16x1616ts
NEWSYM draw16x1616twinon
NEWSYM draw16x16fulladdwinon
NEWSYM draw16x1616tswinon
int 3
