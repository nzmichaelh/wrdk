/* tc-c33.h -- Header file for tc-c33.c.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

   This file is part of GAS, the GNU Assembler.

   GAS is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GAS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GAS; see the file COPYING.  If not, write to the Free
   Software Foundation, 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA. */

#define TC_C33

#include <elf/c33.h>

#define TARGET_BYTES_BIG_ENDIAN 0

#ifndef BFD_ASSEMBLER
 #error C33 support requires BFD_ASSEMBLER
#endif

/* The target BFD architecture.  */
#define TARGET_ARCH 		bfd_arch_c33

/* The target BFD format.  */
#define TARGET_FORMAT 		"elf32-c33"

#define MD_APPLY_FIX3

#define md_operand(x)

#define obj_fix_adjustable(fixP) c33_fix_adjustable(fixP)
#define TC_FORCE_RELOCATION(fixp) c33_force_relocation(fixp)
extern int c33_force_relocation PARAMS ((struct fix *));

/* Permit temporary numeric labels.  */
#define LOCAL_LABELS_FB 1

#define DIFF_EXPR_OK		/* foo-. gets turned into PC relative relocs */

/* We don't need to handle .word strangely.  */
#define WORKING_DOT_WORD

#define md_number_to_chars number_to_chars_littleendian
     
/* We need to handle lo(), hi(), etc etc in .hword, .word, etc
   directives, so we have to parse "cons" expressions ourselves.  */
#define TC_PARSE_CONS_EXPRESSION(EXP, NBYTES) parse_cons_expression_c33 (EXP)
#define TC_CONS_FIX_NEW cons_fix_new_c33
//extern const struct relax_type md_relax_table[];
#define TC_GENERIC_RELAX_TABLE md_relax_table

/* This section must be in the small data area (pointed to by GP).  */
#define SHF_C33_GPREL		0x10000000

/* tazaki 2001.12.03 */
#define ELF_TC_SPECIAL_SECTIONS \
  { ".comm",	SHT_C33_COMM,	 	SHF_ALLOC + SHF_WRITE + SHF_C33_GPREL	}, \
  { ".gcomm",	SHT_C33_GCOMM,	 	SHF_ALLOC + SHF_WRITE + SHF_C33_GPREL	}, \
  { ".scomm",	SHT_C33_SCOMM, 		SHF_ALLOC + SHF_WRITE + SHF_C33_GPREL	}, \
  { ".tcomm",	SHT_C33_TCOMM,		SHF_ALLOC + SHF_WRITE + SHF_C33_GPREL	}, \
  { ".zcomm",	SHT_C33_ZCOMM, 		SHF_ALLOC + SHF_WRITE + SHF_C33_GPREL	}, \


#define MD_PCREL_FROM_SECTION(fixP,section) c33_pcrel_from_section (fixP, section)
extern long c33_pcrel_from_section ();

#define LEX_PCT  LEX_BEGIN_NAME	/* %‚ð—LŒø‚É‚·‚é */

