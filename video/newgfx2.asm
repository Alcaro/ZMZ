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

EXTSYM ngwintable,ngwinen,ngcwinptr,ngcpixleft,ngcwinmode,cachesingle4bng
EXTSYM tleftn,ng16bprval,vrama,bg1drwng,ng16bbgval,bg1totng,ngptrdat2
EXTSYM bgtxadd,taddnfy16x16,taddfy16x16,switch16x16,curmosaicsz,domosaicng
EXTSYM vidmemch4,vidmemch2,vidmemch8,mode0add,vcache4b,vcache2b,vcache8b
EXTSYM cachesingle2bng,cachesingle8bng,ngpalcon4b,ngpalcon8b,ofshvaladd
EXTSYM ngpalcon2b,tleftnb,tltype2b,tltype4b,tltype8b,yposng,flipyposng
EXTSYM ofsmcptr,ofsmtptr,ofsmmptr,ofsmcyps,ofsmady,ofsmadx,ofsmtptrs,ofsmcptr2
EXTSYM yposngom,flipyposngom,cbgval,ofsmval,ofsmvalh,bgtxadd2

%include "video/vidmacro.mac"
%include "video/newgfx2.mac"
%include "video/newgfx.mac"
%include "video/newgfxwn.mac"


SECTION .text

NEWSYM drawtileng2b
NEWSYM drawtileng4b
NEWSYM drawtileng8b
NEWSYM drawtileng16x162b
NEWSYM drawtileng16x164b
NEWSYM drawtileng16x168b
NEWSYM drawlineng2b
NEWSYM drawlineng4b
NEWSYM drawlineng8b
NEWSYM drawlineng16x162b
NEWSYM drawlineng16x164b
NEWSYM drawlineng16x168b
NEWSYM drawlineng16x84b
NEWSYM drawlineng16x82b
NEWSYM drawlinengom4b
int 3
