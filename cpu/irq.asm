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

EXTSYM flagnz,flago,flagc,SfxSCMR,curnmi,execloop,initaddrl,nmiv,snesmap2
EXTSYM snesmmap,stackand,stackor,xe,xirqb,xpb,xpc,xs,irqon,irqv,irqv8
EXTSYM execloopdeb,nmiv8,membank0w8


SECTION .text
NEWSYM switchtonmi
NEWSYM NMIemulmode
NEWSYM switchtovirq
NEWSYM switchtovirqret
NEWSYM IRQemulmode
NEWSYM switchtovirqdeb
NEWSYM IRQemulmodedeb
NEWSYM switchtonmideb
NEWSYM NMIemulmodedeb
int 3