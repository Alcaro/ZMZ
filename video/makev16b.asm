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

SECTION .bss
NEWSYM tempstuff, resd 1

SECTION .text

NEWSYM procspritessub16b
NEWSYM procspritesmain16b
NEWSYM drawbackgrndsub16b
NEWSYM drawbackgrndmain16b
NEWSYM blanker16b
NEWSYM drawline16b
NEWSYM nodrawline16b
NEWSYM priority216b
NEWSYM processmode716b
NEWSYM clearback16b
NEWSYM setpalall
int 3

SECTION .bss
NEWSYM colleft16b, resb 1
SECTION .text

NEWSYM setpalette16b
NEWSYM setpalallgamma
NEWSYM setpalette16bgamma
NEWSYM drawsprites16b
NEWSYM drawsprites16bprio
NEWSYM drawspritesprio16bwinon
NEWSYM draw8x816b
int 3

SECTION .bss
NEWSYM tileleft16b, resb 1
SECTION .text

NEWSYM draw8x816bwinon
NEWSYM draw16x816
NEWSYM draw16x816b
NEWSYM draw16x816bwinon
NEWSYM draw16x816winonb
NEWSYM domosaic16b
NEWSYM domosaicwin16b
NEWSYM dowindow16b
NEWSYM draw8x816boffset
NEWSYM draw8x816bwinonoffset
NEWSYM draw16x1616b
int 3
