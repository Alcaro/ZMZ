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
EXTSYM BGPT4X,BGPT4Y,bg1drwng,bg1objptr,bg1ptr,bg1ptrx,bg1ptry,bg1scrolx
EXTSYM bg1scroly,bg1totng,bg2drwng,bg2objptr,bg2ptr,bg2ptrx,bg2ptry,bg2scrolx
EXTSYM bg2scroly,bg2totng,bg3drwng,bg3highst,bg3objptr,bg3ptr,bg3ptrx,bg3ptry
EXTSYM bg3scrolx,bg3scroly,bg3totng,bg4drwng,bg4objptr,bg4ptr,bg4ptrx,bg4ptry
EXTSYM bg4scrolx,bg4scroly,bg4totng,bgcmsung,bgmode,bgtxad,bgtxadd,ngextbg
EXTSYM cachesingle2bng,cachesingle8bng,cfieldad,cgmod,cgram,coladdb,coladdg
EXTSYM coladdr,colleft16b,colormodedef,cpalval,csprbit,csprival,curmosaicsz
EXTSYM curvidoffset,curypos,firstdrawn,flipyposng,forceblnk,interlval,intrlng
EXTSYM mode0add,mode0ads,mode7A,mode7C,mode7X0,mode7ab,mode7cd,mode7set,mode7st
EXTSYM mode7xy,modeused,mosaicon,mosaicsz,mosenng,mosszng,ngceax,ngcedi
EXTSYM ngpalcon2b,ngpalcon8b,ngptrdat,pesimpng,prdata,prdatb,prdatc,prevbright
EXTSYM reslbyl,resolutn,scaddset,scaddtype,scadsng,scadtng,scfbl,scrndis,scrnon
EXTSYM spritetablea,sprleftpr,sprlefttot,sprprdrn,sprpriodata,sprtbng,sprtlng
EXTSYM t16x161,t16x162,t16x163,t16x164,taddfy16x16,taddnfy16x16,ngptrdat2
EXTSYM tleftn,tltype2b,tltype8b,vcache2b,vcache8b,vidbright,ofshvaladd
EXTSYM vidbuffer,vidmemch2,vidmemch8,vrama,winon,xtravbuf,yposng
EXTSYM vbufdptr,drawtileng2b16b,drawtileng4b16b,drawtileng8b16b,bgwinchange
EXTSYM drawtileng16x162b16b,drawtileng16x164b16b,drawtileng16x168b16b,winbg1en
EXTSYM drawlineng2b16b,drawlineng4b16b,drawlineng8b16b,BuildWindow,winenabs
EXTSYM drawlineng16x162b16b,drawlineng16x164b16b,drawlineng16x168b16b,winenabm
EXTSYM disableeffects,winl1,winbg1enval,winbg1envalm,winlogica,winlogicaval
EXTSYM winboundary,winobjen,winlogicb,nglogicval,ngwintable,winbg2enval,doveg
EXTSYM winbg3enval,winbg4enval,winbgobjenval,Mode7HiRes16b,res640,hiresstuff
EXTSYM Mode7BackA,Mode7BackC,Mode7BackX0,Mode7BackSet,drawmode7win16b,ngwinen
EXTSYM drawlineng16x84b16b,drawlineng16x82b16b,ofsmcyps,vram,ofsmcptr,ofsmady
EXTSYM ofsmadx,ofsmtptr,yposngom,flipyposngom,ofsmmptr,ofsmval,ofsmvalh,V8Mode
EXTSYM cbgval,drawlinengom4b16b,ignor512,winbg1envals,m7starty
EXTSYM FillSubScr,scanlines,drawmode7win16bd,SpecialLine,vidmemch2s,dovegrest
EXTSYM drawlinengom16x164b16b,bgallchange
EXTSYM bg1change,bg2change,bg3change,bg4change,ngwinptr,objwlrpos,objwen
EXTSYM objclineptr,CSprWinPtr,BuildWindow2,NGNumSpr,fulladdtab,MMXSupport
EXTSYM bgtxadd2,gammalevel16b,drawmode7ngextbg16b,processmode7hires16b
EXTSYM processmode7hires16bd,drawmode7ngextbg216b,osm2dis,ofsmtptrs,ofsmcptr2

%ifdef __MSDOS__
EXTSYM smallscreenon,ScreenScale
%endif

%include "video/vidmacro.mac"
%include "video/newgfx16.mac"
%include "video/newg162.mac"

SECTION .text

NEWSYM setpalallng
NEWSYM setpalette16bng
NEWSYM newengine16b
section .data
align 32
NEWSYM ngwinenval,  dd 0
NEWSYM cdrawbuffer, dd 0
NEWSYM draw16bnng,  dd 0
NEWSYM scaddsngb,   dd 0
NEWSYM scaddtngb,   dd 0
NEWSYM scaddtngbx,  dd 0
NEWSYM prevbcolng,  dd 0
NEWSYM bcolvalng,   dd 0
NEWSYM cebppos,     dd 0
NEWSYM subscreenonng, dd 0
NEWSYM cdrawmeth,   dd 0
NEWSYM cpalptrng,   dd 0
NEWSYM prevcoladdrng, dd 0
NEWSYM prevcolvalng,  dd 0
NEWSYM subscrng,      dd 0
NEWSYM ngmsdraw,      dd 0
NEWSYM CMainWinScr,   dd 0
NEWSYM CSubWinScr,    dd 0
NEWSYM Prevcoladdr,   dd 0
NEWSYM ColResult,     dd 0
NEWSYM CPalPtrng,     dd 0
NEWSYM WindowRedraw,  dd 0
NEWSYM mostranspval,  dd 0
NEWSYM mosclineval,   dd 0
NEWSYM startlinet,    dd 0
NEWSYM endlinet,      dd 0
NEWSYM palchanged,    dd 0

NEWSYM ng16bbgval, dd 0         ; bg # (mov dword[ng16bbgval],%1)
NEWSYM ng16bprval, dd 0         ; 0 = pr0, 2000h = pr1

NEWSYM mosjmptab16b, times 15 dd 0
NEWSYM mosjmptab16bt, times 15 dd 0
NEWSYM mosjmptab16btms, times 15 dd 0
NEWSYM mosjmptab16bntms, times 15 dd 0
section .text

NEWSYM StartDrawNewGfx16b
NEWSYM domosaicng16b
NEWSYM mosdraw216b
NEWSYM mosdraw316b
NEWSYM mosdraw416b
NEWSYM mosdraw516b
NEWSYM mosdraw616b
NEWSYM mosdraw716b
NEWSYM mosdraw816b
NEWSYM mosdraw916b
NEWSYM mosdraw1016b
NEWSYM mosdraw1116b
NEWSYM mosdraw1216b
NEWSYM mosdraw1316b
NEWSYM mosdraw1416b
NEWSYM mosdraw1516b
NEWSYM mosdraw1616b
NEWSYM mosdraw216bt
NEWSYM mosdraw316bt
NEWSYM mosdraw416bt
NEWSYM mosdraw516bt
NEWSYM mosdraw616bt
NEWSYM mosdraw716bt
NEWSYM mosdraw816bt
NEWSYM mosdraw916bt
NEWSYM mosdraw1016bt
NEWSYM mosdraw1116bt
NEWSYM mosdraw1216bt
NEWSYM mosdraw1316bt
NEWSYM mosdraw1416bt
NEWSYM mosdraw1516bt
NEWSYM mosdraw1616bt
NEWSYM mosdraw216btms
NEWSYM mosdraw316btms
NEWSYM mosdraw416btms
NEWSYM mosdraw516btms
NEWSYM mosdraw616btms
NEWSYM mosdraw716btms
NEWSYM mosdraw816btms
NEWSYM mosdraw916btms
NEWSYM mosdraw1016btms
NEWSYM mosdraw1116btms
NEWSYM mosdraw1216btms
NEWSYM mosdraw1316btms
NEWSYM mosdraw1416btms
NEWSYM mosdraw1516btms
NEWSYM mosdraw1616btms
NEWSYM mosdraw216bntms
NEWSYM mosdraw316bntms
NEWSYM mosdraw416bntms
NEWSYM mosdraw516bntms
NEWSYM mosdraw616bntms
NEWSYM mosdraw716bntms
NEWSYM mosdraw816bntms
NEWSYM mosdraw916bntms
NEWSYM mosdraw1016bntms
NEWSYM mosdraw1116bntms
NEWSYM mosdraw1216bntms
NEWSYM mosdraw1316bntms
NEWSYM mosdraw1416bntms
NEWSYM mosdraw1516bntms
NEWSYM mosdraw1616bntms
NEWSYM drawbg1tile16b
NEWSYM drawbg2tile16b
NEWSYM drawbg3tile16b
NEWSYM drawbg4tile16b
NEWSYM drawbg1tilepr116b
NEWSYM drawbg2tilepr116b
NEWSYM drawbg3tilepr116b
NEWSYM drawbg4tilepr116b
NEWSYM drawbg1line16b
NEWSYM drawbg2line16b
NEWSYM drawbg3line16b
NEWSYM drawbg4line16b
NEWSYM drawbg1linepr116b
NEWSYM drawbg2linepr116b
NEWSYM drawbg3linepr116b
NEWSYM drawbg4linepr116b
NEWSYM drawsprng16b
NEWSYM drawsprng16bhr
NEWSYM drawsprngw16bhr
NEWSYM drawsprngw16bthr
int 3
section .data
ALIGN32
NEWSYM UnusedBit, dd 00000000001000000000000000100000b,00000000001000000000000000100000b
NEWSYM HalfTrans, dd 11110111110111101111011111011110b,11110111110111101111011111011110b,0,0
NEWSYM UnusedBitXor, dd 11111111110111111111111111011111b,11111111110111111111111111011111b
NEWSYM ngrposng, dd 11,0
NEWSYM nggposng, dd 6,0
NEWSYM ngbposng, dd 0,0
NEWSYM HiResDone, dd 0,0
NEWSYM FullBitAnd, dd 0F800F800h,0F800F800h
NEWSYM HalfTransB, dd 00001000010000010000100001000001b,00001000010000010000100001000001b
NEWSYM HalfTransC, dd 11110111100111101111011110011110b,11110111100111101111011110011110b
NEWSYM NGNoTransp, dd 0
section .text
