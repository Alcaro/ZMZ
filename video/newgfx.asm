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

EXTSYM BG116x16t,BG1SXl,BG1SYl,BG216x16t,BG2SXl,BG2SYl,BG316x16t,BG3PRI,BG3SXl
EXTSYM BG3SYl,BG416x16t,BG4SXl,BG4SYl,BGFB,BGMA,BGMS1,BGOPT1,BGOPT2,BGOPT3
EXTSYM BGOPT4,BGPT1,BGPT1X,BGPT1Y,BGPT2,BGPT2X,BGPT2Y,BGPT3,BGPT3X,BGPT3Y,BGPT4
EXTSYM BGPT4X,BGPT4Y,StartDrawNewGfx16b,bg1objptr,bg1ptr,bg1ptrx,bg1ptry
EXTSYM bg1scrolx,bg1scroly,bg2objptr,bg2ptr,bg2ptrx,bg2ptry,bg2scrolx,bg2scroly
EXTSYM bg3highst,bg3objptr,bg3ptr,bg3ptrx,bg3ptry,bg3scrolx,bg3scroly,bg4objptr
EXTSYM bg4ptr,bg4ptrx,bg4ptry,bg4scrolx,bg4scroly,bgmode,bgtxad,cachesingle2bng
EXTSYM cachesingle8bng,cbitmode,cfield,colormodedef,csprbit,curmosaicsz
EXTSYM curvidoffset,curypos,forceblnk,interlval,intrlng,mode7A,m7starty
EXTSYM mode7C,mode7X0,mode7ab,mode7cd,mode7set,mode7st,mode7xy,mosaicon,mosaicsz
EXTSYM mosenng,mosszng,ngceax,ngcedi,ngpalcon2b,ngpalcon8b,ngptrdat,prdata
EXTSYM prdatb,prdatc,res640,resolutn,scrndis,scrnon,spritetablea,sprleftpr
EXTSYM sprlefttot,sprpriodata,sprtbng,sprtlng,t16x161,t16x162,t16x163,t16x164
EXTSYM tltype2b,tltype8b,vcache2b,vcache8b,vidbuffer,vidmemch2,ngptrdat2
EXTSYM vidmemch8,vram,vrama,winon,xtravbuf,ng16bbgval,ng16bprval,ofshvaladd
EXTSYM bgwinchange,res480,drawtileng2b,drawtileng4b,drawtileng8b,drawmode7win
EXTSYM drawtileng16x162b,drawtileng16x164b,drawtileng16x168b
EXTSYM osm2dis,drawlineng2b,drawlineng4b,drawlineng8b,processmode7hires
EXTSYM drawlineng16x162b,drawlineng16x164b,drawlineng16x168b,winboundary
EXTSYM winbg1enval,winbg2enval,winbg3enval,winbg4enval,winbgobjenval
EXTSYM winlogicaval,disableeffects,winenabs,scanlines,winl1,winbg1en,winobjen
EXTSYM winlogica,winenabm,bgallchange,bg1change,bg2change,bg3change,bg4change
EXTSYM hiresstuff,drawlineng16x84b,drawlineng16x82b,drawlinengom4b,WindowRedraw
EXTSYM winlogicb,ngwinptr,objwlrpos,objwen,objclineptr,CSprWinPtr
EXTSYM ofsmtptrs,ofsmcptr2,drawmode7ngextbg,drawmode7ngextbg2

%include "video/vidmacro.mac"
%include "video/newgfx2.mac"
%include "video/newgfx.mac"

SECTION .text

NEWSYM newengine8b
NEWSYM BuildWindow2
NEWSYM BuildWindow
int 3

SECTION .data
NEWSYM firstdrawn, db 0

NEWSYM bgusedng
         dd 01010101h,00010101h,00000101h,00000101h,00000101h,00000101h
         dd 00000001h,00000001h

SECTION .bss
NEWSYM bgcmsung, resd 1
NEWSYM modeused, resd 2
NEWSYM reslbyl,  resd 1
NEWSYM sprprdrn, resd 1
NEWSYM csprival, resd 1
NEWSYM pesimpng2, resd 1
NEWSYM cfieldad, resd 1
NEWSYM ignor512, resd 1
NEWSYM ofsmcptr, resd 1
NEWSYM ofsmtptr, resd 1
NEWSYM ofsmmptr, resd 1
NEWSYM ofsmcyps, resd 1
NEWSYM ofsmady,  resd 1
NEWSYM ofsmadx,  resd 1
NEWSYM mosoldtab, resd 15

SECTION .data
ALIGN32

NEWSYM ngwintable, times 16 dd 0EE00h
NEWSYM ngwintableb, times 16 dd 0EE00h
NEWSYM ngwintablec, times 16 dd 0EE00h
NEWSYM ngwintabled, times 16 dd 0EE00h
NEWSYM valtemp, dd 0EE00h, 0EE00h
NEWSYM ngcwinptr, dd ngwintable

SECTION .bss
NEWSYM ngwinen, resd 1
NEWSYM ngcwinmode, resd 1
NEWSYM ngcpixleft, resd 1
NEWSYM Mode7BackA, resd 1
NEWSYM Mode7BackC, resd 1
NEWSYM Mode7BackX0, resd 1
NEWSYM Mode7BackSet, resd 1
NEWSYM ngextbg, resd 1
NEWSYM cbgval, resd 1
NEWSYM ofsmval, resd 1
NEWSYM ofsmvalh, resd 1

SECTION .data
NEWSYM pwinen, dd 0FFFFh
NEWSYM pngwinen, dd 0FFFFh

SECTION .bss
NEWSYM pwinbound, resd 1
NEWSYM WinPtrAPos, resd 1
NEWSYM WinPtrBPos, resd 1

SECTION .data
NEWSYM OrLogicTable, db 0,1,1,0
NEWSYM AndLogicTable, db 0,0,1,0
NEWSYM XorLogicTable, db 0,1,0,0
NEWSYM XNorLogicTable, db 1,0,1,0

SECTION .bss
NEWSYM nglogicval, resd 1
NEWSYM pnglogicval, resd 1
NEWSYM mosjmptab, resd 15
NEWSYM Mode7HiRes, resd 1
NEWSYM pesimpng, resd 1
NEWSYM bgtxadd2, resd 1
SECTION .text

NEWSYM StartDrawNewGfx
NEWSYM drawbg1tile
NEWSYM drawbg2tile
NEWSYM drawbg3tile
NEWSYM drawbg4tile
NEWSYM drawbg1tilepr1
NEWSYM drawbg2tilepr1
NEWSYM drawbg3tilepr1
NEWSYM drawbg4tilepr1
NEWSYM drawbg1line
NEWSYM drawbg2line
NEWSYM drawbg3line
NEWSYM drawbg4line
NEWSYM domosaicng
NEWSYM mosdraw2
NEWSYM mosdraw3
NEWSYM mosdraw4
NEWSYM mosdraw5
NEWSYM mosdraw6
NEWSYM mosdraw7
NEWSYM mosdraw8
NEWSYM mosdraw9
NEWSYM mosdraw10
NEWSYM mosdraw11
NEWSYM mosdraw12
NEWSYM mosdraw13
NEWSYM mosdraw14
NEWSYM mosdraw15
NEWSYM mosdraw16
NEWSYM drawbg1linepr1
NEWSYM drawbg2linepr1
NEWSYM drawbg3linepr1
NEWSYM drawbg4linepr1
int 3


SECTION .bss
NEWSYM bgtxadd,  resd 1
NEWSYM bgcyval,  resd 1
NEWSYM bgcxval,  resd 1
NEWSYM tleftn,   resd 1
NEWSYM tleftnb,  resd 1
NEWSYM bg1totng, resd 1
NEWSYM bg2totng, resd 1
NEWSYM bg3totng, resd 1
NEWSYM bg4totng, resd 1
NEWSYM bg1drwng, resd 1
NEWSYM bg2drwng, resd 1
NEWSYM bg3drwng, resd 1
NEWSYM bg4drwng, resd 1
NEWSYM sprcurng, resd 1
NEWSYM scfbl,    resd 1
NEWSYM mode0ads, resd 1
NEWSYM mode0add, resd 1
NEWSYM taddnfy16x16, resd 1
NEWSYM taddfy16x16, resd 1
NEWSYM switch16x16, resd 1
NEWSYM yposng,     resd 1
NEWSYM flipyposng, resd 1
NEWSYM yposngom,     resd 1
NEWSYM flipyposngom, resd 1
SECTION .text

NEWSYM drawsprng
NEWSYM drawsprngm7h
int 3

SECTION .bss
NEWSYM NGNumSpr, resb 1
SECTION .text

NEWSYM drawsprngw
NEWSYM drawsprngm7w
NEWSYM makesprprtable
NEWSYM preparesprpr
int 3

SECTION .bss
NEWSYM sprclprio,  resd 1
NEWSYM sprsingle,  resd 1
SECTION .text
