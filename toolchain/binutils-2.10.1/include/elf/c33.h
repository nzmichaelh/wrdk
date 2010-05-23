/* C33 ELF support for BFD.
   Copyright (C) 1997,2001 Free Software Foundation, Inc.
   Created by Michael Meissner, Cygnus Support <meissner@cygnus.com>

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file holds definitions specific to the MIPS ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

#ifndef _ELF_C33_H
#define _ELF_C33_H

/* Processor specific flags for the ELF header e_flags field.  */
#if 0	/* c33 */
/* これらはbinutils\readelf.cで使用している */
/*	get_machine_flags()でELFヘッダのe_flagsによって、アーキテクチャの文字列を */
/*	設定している。ｃ３３の場合はどうしたらいいか？	*/

/* Four bit C33 architecture field.  */
#define EF_V850_ARCH		0xf0000000

/* v850 code.  */
#define E_V850_ARCH		0x00000000

/* v850e code.  */
#define E_V850E_ARCH		0x10000000

/* v850ea code.  */
#define E_V850EA_ARCH		0x20000000
#endif	/* c33 */


/* Flags for the st_other field */
#define V850_OTHER_SDA		0x01	/* symbol had SDA relocations */
#define V850_OTHER_ZDA		0x02	/* symbol had ZDA relocations */
#define V850_OTHER_TDA		0x04	/* symbol had TDA relocations */
#define V850_OTHER_TDA_BYTE	0x08	/* symbol had TDA byte relocations */
#define V850_OTHER_ERROR	0x80	/* symbol had an error reported */

/* C33 relocations */
#include "elf/reloc-macros.h"

START_RELOC_NUMBERS (c33_reloc_type)
     RELOC_NUMBER (R_C33_NONE, 0)
     RELOC_NUMBER (R_C33_32, 1)
     RELOC_NUMBER (R_C33_16, 2)
     RELOC_NUMBER (R_C33_8,  3)
     RELOC_NUMBER (R_C33_AH, 4)
     RELOC_NUMBER (R_C33_AL, 5)
     RELOC_NUMBER (R_C33_RH, 6)
     RELOC_NUMBER (R_C33_RM, 7)
     RELOC_NUMBER (R_C33_RL, 8)
     RELOC_NUMBER (R_C33_H,  9)
     RELOC_NUMBER (R_C33_M,  10)
     RELOC_NUMBER (R_C33_L,  11)
     RELOC_NUMBER (R_C33_DH, 12)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_DL, 13)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_GL, 14)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_SH, 15)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_SL, 16)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_TH, 17)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_TL, 18)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_ZH, 19)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_ZL, 20)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_DPH,21)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_DPM,22)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_DPL,23)	/* add tazaki 2002.01.11 */
     RELOC_NUMBER (R_C33_LOOP,24)	/* add tazaki 2002.03.05 */
     RELOC_NUMBER (R_C33_JP, 25)	/* add tazaki 2002.04.22 */
     RELOC_NUMBER (R_C33_S_RH, 26)	/* add tazaki 2002.05.02 */
     RELOC_NUMBER (R_C33_S_RM, 27)	/* add tazaki 2002.05.02 */
     RELOC_NUMBER (R_C33_S_RL, 28)	/* add tazaki 2002.05.02 */
     RELOC_NUMBER (R_C33_PUSHN_R0,29)	/* add tazaki 2004/08/19 */
     RELOC_NUMBER (R_C33_PUSHN_R1,30)	/* add tazaki 2004/08/19 */
     RELOC_NUMBER (R_C33_PUSH_R1,31)	/* add tazaki 2004/08/19 */

     EMPTY_RELOC (R_C33_max)
END_RELOC_NUMBERS


/* Processor specific section indices.  These sections do not actually
   exist.  Symbols with a st_shndx field corresponding to one of these
   values have a special meaning.  */

/* Small data area common symbol.  */
#define SHN_C33_COMM	0xff00
#define SHN_C33_GCOMM	0xff01
#define SHN_C33_SCOMM	0xff02
#define SHN_C33_TCOMM	0xff03
#define SHN_C33_ZCOMM	0xff04
#define SHN_C33_GBSS	0xff05
#define SHN_C33_SBSS	0xff06
#define SHN_C33_TBSS	0xff07
#define SHN_C33_ZBSS	0xff08


/* Processor specific section types.  */

/* Section contains the .scommon data.  */
#define SHT_C33_COMM	0x70000000
#define SHT_C33_GCOMM	0x70000001
#define SHT_C33_SCOMM	0x70000002
#define SHT_C33_TCOMM	0x70000003
#define SHT_C33_ZCOMM	0x70000004
#define SHT_C33_GBSS	0x70000005
#define SHT_C33_SBSS	0x70000006
#define SHT_C33_TBSS	0x70000007
#define SHT_C33_ZBSS	0x70000008



#endif /* _ELF_C33_H */


