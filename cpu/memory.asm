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

EXTSYM romdata,sramb4save,curromspace,SA1Overflow
EXTSYM SFXEnable,regptra,sfxramdata,snesmmap,wramdataa
EXTSYM DSP1Write8b,regptwa,writeon,DSP1Read16b
EXTSYM Bank0datr8,Bank0datw8,Bank0datr16,Bank0datw16,xd,SA1xd
EXTSYM DSP1Read8b,DSP1Type,SA1Enable,DSP1Write16b
EXTSYM ramsize,ramsizeand,sram,sram2,ram7fa
EXTSYM SA1Status,IRAM,CurBWPtr,SA1RAMArea
EXTSYM Sdd1Mode,Sdd1Bank,Sdd1Addr,Sdd1NewAddr,memtabler8,AddrNoIncr,SDD1BankA
EXTSYM SDD1_init,SDD1_get_byte,BWShift,SA1BWPtr

;*******************************************************
; Register & Memory Access Banks (0 - 3F) / (80 - BF)
;*******************************************************
; enter : BL = bank number, CX = address location
; leave : AL = value read

SECTION .text

NEWSYM regaccessbankr8
NEWSYM regaccessbankr16
NEWSYM regaccessbankw8
NEWSYM regaccessbankw16
NEWSYM regaccessbankr8mp
int 3

section .bss
NEWSYM BWUsed2, resb 1
NEWSYM BWUsed, resb 1
section .text

section .bss
NEWSYM DPageR8, resd 1
NEWSYM DPageR16, resd 1
NEWSYM DPageW8, resd 1
NEWSYM DPageW16, resd 1
NEWSYM SA1DPageR8, resd 1
NEWSYM SA1DPageR16, resd 1
NEWSYM SA1DPageW8, resd 1
NEWSYM SA1DPageW16, resd 1
section .text

;NEWSYM UpdateDPage
;NEWSYM SA1UpdateDPage
NEWSYM membank0r8ramSA1
NEWSYM membank0r16ramSA1
NEWSYM membank0w8ramSA1
NEWSYM membank0w16ramSA1
NEWSYM membank0r8ram
NEWSYM membank0r8reg
NEWSYM membank0r8inv
NEWSYM membank0r8chip
NEWSYM membank0r8rom
NEWSYM membank0r8romram
NEWSYM membank0r16ram
NEWSYM membank0r16ramh
NEWSYM membank0r16reg
NEWSYM membank0r16inv
NEWSYM membank0r16chip
NEWSYM membank0r16rom
NEWSYM membank0r16romram
NEWSYM membank0w8ram
NEWSYM membank0w8reg
NEWSYM membank0w8inv
NEWSYM membank0w8chip
NEWSYM membank0w8rom
NEWSYM membank0w8romram
NEWSYM membank0w16ram
NEWSYM membank0w16ramh
NEWSYM membank0w16reg
NEWSYM membank0w16inv
NEWSYM membank0w16chip
NEWSYM membank0w16rom
NEWSYM membank0w16romram
NEWSYM membank0r8
NEWSYM membank0r16
NEWSYM membank0w8
NEWSYM membank0w16
NEWSYM membank0r8SA1
NEWSYM membank0r16SA1
NEWSYM membank0w8SA1
NEWSYM membank0w16SA1
NEWSYM memaccessbankr8
NEWSYM memaccessbankr16
NEWSYM memaccessbankw8
NEWSYM memaccessbankw16
NEWSYM sramaccessbankr8
NEWSYM sramaccessbankr16
NEWSYM sramaccessbankw8
NEWSYM sramaccessbankw16
NEWSYM sramaccessbankr8s
NEWSYM sramaccessbankr16s
NEWSYM sramaccessbankw8s
NEWSYM sramaccessbankw16s
NEWSYM sramaccessbankr8b
NEWSYM sramaccessbankr16b
NEWSYM sramaccessbankw8b
NEWSYM sramaccessbankw16b
NEWSYM stsramr8
NEWSYM stsramr16
NEWSYM stsramw8
NEWSYM stsramw16
NEWSYM stsramr8b
NEWSYM stsramr16b
NEWSYM stsramw8b
NEWSYM stsramw16b
NEWSYM wramaccessbankr8
NEWSYM wramaccessbankr16
NEWSYM wramaccessbankw8
NEWSYM wramaccessbankw16
NEWSYM eramaccessbankr8
NEWSYM eramaccessbankr16
NEWSYM eramaccessbankw8
NEWSYM eramaccessbankw16
NEWSYM regaccessbankr8SA1
NEWSYM regaccessbankr16SA1
NEWSYM regaccessbankw8SA1
NEWSYM regaccessbankw16SA1
NEWSYM SA1RAMaccessbankr8
NEWSYM SA1RAMaccessbankr16
NEWSYM SA1RAMaccessbankw8
NEWSYM SA1RAMaccessbankw16
NEWSYM SA1RAMaccessbankr8b
NEWSYM SA1RAMaccessbankr16b
NEWSYM SA1RAMaccessbankw8b
NEWSYM SA1RAMaccessbankw16b
int 3

SECTION .data
NEWSYM LatestBank, dd 0FFFFh
SECTION .text

; Software decompression version
NEWSYM memaccessbankr8sdd1
int 3