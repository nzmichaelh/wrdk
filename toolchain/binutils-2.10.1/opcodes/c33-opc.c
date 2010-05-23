/* Assemble V850 instructions.
   Copyright (C) 1996 Free Software Foundation, Inc.

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

#include "sysdep.h"
#include "opcode/c33.h"
#include <stdio.h>
#include "opintl.h"

#if 0   /* c33 */
/* regular opcode */
#define OP(x)       ((x & 0x3f) << 5)
#define OP_MASK     OP (0x3f)
#endif  /* c33 */

/* c33 addition */
#define OP_CLASS0_1(x)  ((x & 0x3ff) << 6)
#define OP_CLASS0_2(x)  ((x & 0xff) << 8)
#define OP_CLASS1(x)    ((x & 0xff) << 8)   
#define OP_CLASS2(x)    ((x & 0x3f) << 10)
#define OP_CLASS3(x)    ((x & 0x3f) << 10)
#define OP_CLASS4_1(x)  ((x & 0x3f) << 10)
#define OP_CLASS4_2(x)  OP_CLASS1(x)            /* shift/rotate,div */
#define OP_CLASS5(x)    OP_CLASS1(x)
#define OP_CLASS6(x)    ((x & 0x7) << 13)
#define OP_CLASS7(x)    ((x & 0x3f) << 10)

#define OP_CLASS0_1_MASK    OP_CLASS0_1(0x3ff)
#define OP_CLASS0_2_MASK    OP_CLASS0_2(0xff)
#define OP_CLASS1_MASK      OP_CLASS1(0xff)
#define OP_CLASS2_MASK      OP_CLASS2(0x3f)
#define OP_CLASS3_MASK      OP_CLASS3(0x3f)
#define OP_CLASS4_1_MASK    OP_CLASS4_1(0xff)
#define OP_CLASS4_2_MASK    OP_CLASS4_2(0xff)
#define OP_CLASS5_MASK      OP_CLASS5(0xff)
#define OP_CLASS6_MASK      OP_CLASS6(0x7)
#define OP_CLASS7_MASK      OP_CLASS7(0x3f)


#if 0
/* The functions used to insert and extract complicated operands.  */

/* Note: There is a conspiracy between these functions and
   v850_insert_operand() in gas/config/tc-v850.c.  Error messages
   containing the string 'out of range' will be ignored unless a
   specific command line option is given to GAS.  */

static const char * not_valid    = N_ ("displacement value is not in range and is not aligned");
static const char * out_of_range = N_ ("displacement value is out of range");
static const char * not_aligned  = N_ ("displacement value is not aligned");

static const char * immediate_out_of_range = N_ ("immediate value is out of range");

static unsigned long
insert_d5_4 (insn, value, errmsg)
     unsigned long insn;
     long          value;
     const char ** errmsg;
{
  if (value > 0x1f || value < 0)
    {
      if (value & 1)
    * errmsg = _(not_valid);
      else
    *errmsg = _(out_of_range);
    }
  else if (value & 1)
    * errmsg = _(not_aligned);

  value >>= 1;

  return (insn | (value & 0x0f));
}

static unsigned long
extract_d5_4 (insn, invalid)
     unsigned long insn;
     int *         invalid;
{
  unsigned long ret = (insn & 0x0f);

  return ret << 1;
}
#endif


/* Warning: code in gas/config/tc-v850.c examines the contents of this array.
   If you change any of the values here, be sure to look for side effects in
   that code. */
const struct c33_operand c33_operands[] =
{
#define UNUSED  0
  { 0, 0, NULL, NULL, 0, 0 }, 

#define SP      1
  { 0, 0, NULL, NULL, C33_OPERAND_SP, 0}, 

#define RD      2                               
  { 4, 0, NULL, NULL, C33_OPERAND_REG, 0}, 

#define RS      3                               
  { 4, 0, NULL, NULL, C33_OPERAND_REG, 0 },

#define RS2     4                               
  { 4, 4, NULL, NULL, C33_OPERAND_REG, 0},

#define SD      5
  { 4, 0, NULL, NULL, C33_OPERAND_SREG, 0}, 

#define SS      6
  { 4, 4, NULL, NULL, C33_OPERAND_SREG, 0}, 

#define RB0     7
  { 4, 0, NULL, NULL, C33_OPERAND_REG, 0},

#define RB      8
  { 4, 4, NULL, NULL, C33_OPERAND_RB, 0},

#define MEM_IMM13   9                                   /* sld,sbit */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM, 13},

#define MEM_IMM26   10                                  /* xld,xbit */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM, 26},

#define REGINC  11                          
  { 4, 4, NULL, NULL, C33_OPERAND_REGINC, 0},

#define SIGN6   12                          
  { 6, 4, NULL, NULL, C33_OPERAND_SIGNED, 6}, 

#define SIGN8   13                          
  { 8, 0, NULL, NULL, C33_OPERAND_SIGNED, 8}, 

#define SIGN32  14                          
  { 6, 4, NULL, NULL, C33_OPERAND_SIGNED, 32}, 

#define IMM2    15                      
  { 2, 0, NULL, NULL, C33_OPERAND_IMM, 2}, 

#define IMM3    16                              
  { 3, 0, NULL, NULL, C33_OPERAND_IMM, 3}, 

#define IMM4    17                              
  { 4, 4, NULL, NULL, C33_OPERAND_IMM, 4}, 

#define IMM5    18                          
  { 4, 4, NULL, NULL, C33_OPERAND_IMM, 5}, 

#define IMM6    19
  { 6, 4, NULL, NULL, C33_OPERAND_IMM, 6}, 

#define IMM10   20
  { 10, 0, NULL, NULL, C33_OPERAND_IMM, 10}, 

#define IMM32   21
  { 6, 4, NULL, NULL, C33_OPERAND_IMM, 32}, 

#define SPIMM6  22                              
  { 6, 4, NULL, NULL, C33_OPERAND_SPMEM, 6}, 

#define SPIMM32 23                                  /* xld,sld,xbit,sbit */
  { 6, 4, NULL, NULL, C33_OPERAND_SPMEM, 32}, 

#define IMM13_LABEL 24                          
//  { 13, 0, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_LABEL, 13},    /* ext */ 
  { 13, 0, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_LABEL, 13},    /* ext */ 	/* change 2005/04/07 T.Tazaki */

#define SIGN32_LABELIMM32   25
  { 8, 0, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_LABEL | C33_OPERAND_PC, 32}, /* jp,call */

#define SIGN32_SYMBOLIMM32  26
  { 6, 4, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_SYMBOL, 32}, /* xld.w*/

#define SIGN6_SYMBOLIMM6    27
  { 6, 4, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_SYMBOL, 6}, /* ld.w */

#define SYMBOLIMM19 28
  { 6, 4, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_SYMBOL, 19}, /* sld.w */

#define SIGN8_LABELIMM8 29
  { 8, 0, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_LABEL | C33_OPERAND_PC, 8}, /* jp,call */

#define SIGN32_LABELIMM22   30
  { 8, 0, NULL, NULL, C33_OPERAND_SIGNED | C33_OPERAND_LABEL | C33_OPERAND_PC, 22}, /* scall,sjp,sjrxx */

#define RD01    31
  { 4, 0, NULL, NULL, C33_OPERAND_REG | C33_OPERAND_01, 0},     /* Advanced macro class0  */

#define RS01    32
  { 4, 0, NULL, NULL, C33_OPERAND_REG | C33_OPERAND_01, 0 },    /* Advanced macro class0  */

#define SS01    33
  { 4, 0, NULL, NULL, C33_OPERAND_SREG | C33_OPERAND_01, 0},    /* Advanced macro class0 */

#define SD01    34
  { 4, 0, NULL, NULL, C33_OPERAND_SREG | C33_OPERAND_01, 0},    /* Advanced macro class0 */

#define RB01    35
  { 4, 0, NULL, NULL, C33_OPERAND_REG | C33_OPERAND_01, 0},     /* Advanced macro class0  */

#define IMM4_01 36                              
  { 4, 0, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_01, 5},     /* Advanced macro class0  */

#define IMM5_2  37
  { 4, 0, NULL, NULL, C33_OPERAND_IMM, 5},                      /* Advanced macro : loop  */

#define IMM6_OP3    38
  { 6, 0, NULL, NULL, C33_OPERAND_IMM, 6}, 

#define IMM5_OP3_01 39
  { 5, 0, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_OP3_01, 5}, /* Advanced macro : psrset  */

#define IMM5_OP3_10 40
  { 5, 0, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_OP3_10, 5}, /* Advanced macro : psrclr  */

#define DPIMM6  41
  { 6, 4, NULL, NULL, C33_OPERAND_DPMEM, 6},                    /* Advanced macro : ld.x [%dp+imm6] */

#define DPIMM32 42
  { 6, 4, NULL, NULL, C33_OPERAND_DPMEM, 32},                   /* Advanced macro : xld.x [%dp+imm32] */

#define DP_SYMBOL19 43
  { 6, 4, NULL, NULL, C33_OPERAND_MEM | C33_OPERAND_DP_SYMBOL, 19}, /* Advanced macro : ald.w [symbol+imm]  */

#define DP_SYMBOL32 44
  { 6, 4, NULL, NULL, C33_OPERAND_MEM | C33_OPERAND_DP_SYMBOL, 32}, /* Advanced macro : xld.w [symbol+imm]  */

#define DP      45
  { 4, 0, NULL, NULL, C33_OPERAND_DP | C33_OPERAND_01, 0},          /* Advanced macro : add %rd,%dp */

#define DP_OFF_SYMBOL6  46
  { 6, 4, NULL, NULL, C33_OPERAND_MEM | C33_OPERAND_DPSYMBOL6, 6},  /* Advanced macro : ld.x [%dp+dpoff_l(symbol)] */

#define DP_OFF_SYMBOL6_2    47
  { 6, 4, NULL, NULL, C33_OPERAND_MEM | C33_OPERAND_DPSYMBOL6_2, 6},    /* Advanced macro : ld.x [%dp+dpoff_l(symbol)] */

#define COND    48
  { 4, 4, NULL, NULL, C33_OPERAND_COND, 6},                             /* Advanced macro : ext condition */

#define OP_SHIFT    49
  { 2, 2, NULL, NULL, C33_OPERAND_OP_SHIFT, 0},                         /* Advanced macro : ext condition */

#define IMM5_LABEL  50
  { 4, 4, NULL, NULL, C33_OPERAND_IMM | C33_OPERAND_LABEL, 5},         /* Advanced macro : loop */

#define SD_LD   51
  { 4, 0, NULL, NULL, C33_OPERAND_LD_SREG, 0},                         /* Advanced macro,PE : ld.w %sd */

#define SS02    52
  { 4, 0, NULL, NULL, C33_OPERAND_PUSHS_SREG | C33_OPERAND_01, 0},  /* Advanced macro,PE : pushs */

#define SD02    53
  { 4, 0, NULL, NULL, C33_OPERAND_PUSHS_SREG | C33_OPERAND_01, 0},  /* Advanced macro,PE : pops */

/* add T.Tazaki 2004/07/23 >>> */
#define XLDB_RD_MEM_IMM26   54                                      /* xld.b %rd,[symbol+imm32] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDB_RD, 26},
#define XLDB_WR_MEM_IMM26   55                                      /* xld.b [symbol+imm32],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDB_WR, 26},
#define XLDH_RD_MEM_IMM26   56                                      /* xld.h %rd,[symbol+imm32] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDH_RD, 26},
#define XLDH_WR_MEM_IMM26   57                                      /* xld.h [symbol+imm32],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDH_WR, 26},
#define XLDW_RD_MEM_IMM26   58                                      /* xld.w %rd,[symbol+imm32] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDW_RD, 26},
#define XLDW_WR_MEM_IMM26   59                                      /* xld.w [symbol+imm32],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDW_WR, 26},
#define XLDUB_RD_MEM_IMM26  60                                      /* xld.ub %rd,[symbol+imm32] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDUB_RD, 26},
#define XLDUH_RD_MEM_IMM26  61                                      /* xld.uh %rd,[symbol+imm32] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDUH_RD, 26},
#define XBTST_MEM_IMM26     62                                      /* xbtst [symbol+imm32],imm3 */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XBTST, 26},
#define XBCLR_MEM_IMM26     63                                      /* xbclr [symbol+imm32],imm3 */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XBCLR, 26},
#define XBSET_MEM_IMM26     64                                      /* xbset [symbol+imm32],imm3 */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XBSET, 26},
#define XBNOT_MEM_IMM26     65                                      /* xbnot [symbol+imm32],imm3 */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XBNOT, 26},

#define ALDB_RD_MEM_IMM19   66                                      /* ald.b %rd,[symbol+imm19] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDB_RD, 19},
#define ALDB_WR_MEM_IMM19   67                                      /* ald.b [symbol+imm19],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDB_WR, 19},
#define ALDH_RD_MEM_IMM19   68                                      /* ald.h %rd,[symbol+imm19] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDH_RD, 19},
#define ALDH_WR_MEM_IMM19   69                                      /* ald.h [symbol+imm19],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDH_WR, 19},
#define ALDW_RD_MEM_IMM19   70                                      /* ald.w %rd,[symbol+imm19] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDW_RD, 19},
#define ALDW_WR_MEM_IMM19   71                                      /* ald.w [symbol+imm19],%rs */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDW_WR, 19},
#define ALDUB_RD_MEM_IMM19  72                                      /* ald.ub %rd,[symbol+imm19] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDUB_RD, 19},
#define ALDUH_RD_MEM_IMM19  73                                      /* ald.uh %rd,[symbol+imm19] */
  { 4, 4, NULL, NULL, C33_OPERAND_MEM | C33_XLDUH_RD, 19},
#define RB_IMM26   			74
  { 4, 4, NULL, NULL, C33_OPERAND_RB | C33_OPERAND_26, 0},     		/* xld.x [%rb+imm26] */

/* add T.Tazaki 2004/07/23 <<< */

};


/* The opcode table.    < STANDARD MACRO >

   The format of the opcode table is:

   NAME     OPCODE          MASK               { OPERANDS }    MEMOP

   NAME is the name of the instruction.
   OPCODE is the instruction opcode.
   MASK is the opcode mask; this is used to tell the disassembler
     which bits in the actual opcode must match OPCODE.
   OPERANDS is the list of operands.
   MEMOP specifies which operand (if any) is a memory operand.
   PROCESSORS specifies which CPU(s) support the opcode.
   
   The disassembler reads the table in order and prints the first
   instruction which matches, so this table is sorted to put more
   specific instructions before more general instructions.  It is also
   sorted by major opcode.

   The table is also sorted by name.  This is used by the assembler.
   When parsing an instruction the assembler finds the first occurance
   of the name of the instruciton in this table and then attempts to
   match the instruction's arguments with description of the operands
   associated with the entry it has just found in this table.  If the
   match fails the assembler looks at the next entry in this table.
   If that entry has the same name as the previous entry, then it
   tries to match the instruction against that entry and so on.  This
   is how the assembler copes with multiple, different formats of the
   same instruction.  */

/* It is necessary to put in order and describe the same command. */
/* It is meaningful in the order of the row of a command.
    REGINC is bad if there is nothing before RB. */
/* The operand in which a symbol is possible will not become, 
if it gets used after the operand for which a symbol is improper. */
/*************************************************************************************************/
/*  NO use default data area mode 																 */
/*************************************************************************************************/
const struct c33_opcode c33_opcodes32[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB_IMM26},      0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDB_RD_MEM_IMM26},     0,  0},		/* change 2004/07/23 T.Tazaki */
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB_IMM26,RS},      0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDB_WR_MEM_IMM26,RS},     0,  0},		/* change 2004/07/23 T.Tazaki */

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB_IMM26},      0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.ub", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDUB_RD_MEM_IMM26},     0,  0},	/* change 2004/07/23 T.Tazaki */

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDH_RD_MEM_IMM26},     0,  0},		/* change 2004/07/23 T.Tazaki */
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB_IMM26,RS},      0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDH_WR_MEM_IMM26,RS},     0,  0},		/* change 2004/07/23 T.Tazaki */

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB_IMM26},      0,  0},				/* change 2004/07/23 T.Tazaki */
{ "xld.uh", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDUH_RD_MEM_IMM26},     0,  0},	/* change 2004/07/23 T.Tazaki */

{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},      0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD,RS2},           0,  0},
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB_IMM26},      0,  0},           /* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDW_RD_MEM_IMM26},     0,  0},   /* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB_IMM26,RS},      0,  0}, 		  /* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDW_WR_MEM_IMM26,RS},     0,  0},   /* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},

{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},

{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},    0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbtst",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBTST_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},    0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbclr",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBCLR_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},    0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbset",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBSET_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},    0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbnot",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBNOT_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},

/* class 6 */

{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;

/* The opcode table.    < ADVANCED MACRO > */

const struct c33_opcode c33_advance_opcodes32[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "jpr",    OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* Adv */
{ "jpr.d",  OP_CLASS0_1(0x0f),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* Adv */
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "push",   OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "pop",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {RD01},             0,  0}, /* Adv */
{ "pushs",  OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {SS02},             0,  0}, /* Adv */
{ "pops",   OP_CLASS0_1(0x03),      OP_CLASS0_1_MASK,       {SD02},             0,  0}, /* Adv */
{ "mac.w",  OP_CLASS0_1(0x04),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "mac.hw", OP_CLASS0_1(0x05),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "macclr", OP_CLASS0_1(0x06),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},/* Adv */
{ "ld.cf",  OP_CLASS0_1(0x07),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},/* Adv */
{ "div.w",  OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "divu.w", OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "repeat", OP_CLASS0_1(0x0a),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "repeat", OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {IMM4_01},          0,  0}, /* Adv */

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retm",   OP_CLASS0_1(0x1b),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},/* Adv */ /* 2002/10/01 */
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},
{ "ld.b",   OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */

{ "xld.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  1}, /* Adv */
{ "xld.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  1}, /* Adv */
{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
//{ "xld.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
//{ "xld.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB_IMM26},      0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDB_RD_MEM_IMM26},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDB_WR_MEM_IMM26,RS},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.ub",  OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.ub",  OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */

{ "xld.ub", OP_CLASS2(0x39),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  1}, /* Adv */
{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
//{ "xld.ub", OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.ub", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDUB_RD_MEM_IMM26},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */

{ "xld.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  2}, /* Adv */
{ "xld.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  2}, /* Adv */
{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
//{ "xld.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
//{ "xld.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDH_RD_MEM_IMM26},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDH_WR_MEM_IMM26,RS},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.uh",  OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.uh",  OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */

{ "xld.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  2}, /* Adv */
{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
//{ "xld.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.uh",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,        {RD,XLDUH_RD_MEM_IMM26},   0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */

{ "ld.w",   OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */
{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},  0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD_LD,RS2},        0,  0},
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  4}, /* Adv */
{ "xld.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  4}, /* Adv */
{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},
//{ "xld.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
//{ "xld.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDW_RD_MEM_IMM26},     0,  0},	/* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDW_WR_MEM_IMM26,RS},     0,  0},	/* change 2004/07/23 T.Tazaki */


//{ "ald.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
//{ "ald.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */
//{ "ald.ub", OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
//{ "ald.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
//{ "ald.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */
//{ "ald.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
//{ "ald.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
//{ "ald.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */

{ "ald.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,ALDB_RD_MEM_IMM19},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {ALDB_WR_MEM_IMM19,RS},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.ub", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,ALDUB_RD_MEM_IMM19},      0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,ALDH_RD_MEM_IMM19},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {ALDH_WR_MEM_IMM19,RS},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.uh", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,ALDUH_RD_MEM_IMM19},      0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,ALDW_RD_MEM_IMM19},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */
{ "ald.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {ALDW_WR_MEM_IMM19,RS},       0,  0}, /* Adv */	/* change 2004/07/23 T.Tazaki */


{ "add",    OP_CLASS0_1(0x0d),      OP_CLASS0_1_MASK,       {RD01,DP},          0,  0},     /* Adv */
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS1_MASK,         {RD,DP_OFF_SYMBOL6_2},0,0},     /* Adv */
{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},/* Adv */
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},

{ "swaph",  OP_CLASS4_2(0x9a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "sat.b",  OP_CLASS4_2(0x9e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "sat.ub", OP_CLASS4_2(0x9f),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbtst",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBTST_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbclr",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBCLR_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbset",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBSET_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbnot",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBNOT_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.hw", OP_CLASS5(0xa3),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},
{ "mac1.h", OP_CLASS5(0xa7),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mac1.hw",OP_CLASS5(0xab),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mac1.w", OP_CLASS5(0xb3),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "ld.c",   OP_CLASS5(0xb1),        OP_CLASS5_MASK,         {RD,IMM5},          0,  0}, /* Adv */
{ "ld.c",   OP_CLASS5(0xb5),        OP_CLASS5_MASK,         {IMM5,RS},          0,  0}, /* Adv */
{ "sat.h",  OP_CLASS5(0xb6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "sat.uh", OP_CLASS5(0xb7),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xb9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xba),        OP_CLASS5_MASK,         {RD,IMM5_LABEL},    0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xbb),        OP_CLASS5_MASK,         {IMM5_2,IMM5_LABEL},0,  0}, /* Adv */
{ "sat.w",  OP_CLASS5(0xbd),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "sat.uw", OP_CLASS5(0xbe),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "do.c",   OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM6_OP3},         0, 0},  /* Adv */
{ "psrset", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_01},      0, 0},  /* Adv */
{ "psrclr", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_10},      0, 0},  /* Adv */

{ "ext",    OP_CLASS1(0x3f),        OP_CLASS1_MASK,         {RS2,OP_SHIFT,IMM2},0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3f),        OP_CLASS1_MASK,         {RS2},              0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3b),        OP_CLASS1_MASK,         {COND},             0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3b),        OP_CLASS1_MASK,         {OP_SHIFT,IMM2},    0,  0}, /* Adv */
{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;

const struct c33_opcode c33_ext_opcodes[] =
{
{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},
{ 0, 0, 0, {0}, 0, 0 }

} ;


/* Advanced Macro Opecode name class0 - bit5,4 = 0,1 */
const char *c33_adv_opcodes[] =
{
 "push",    
 "pop",     
 "pushs",
 "pops",
 "mac.w",
 "mac.hw",
 "macclr",
 "ld.cf",
 "div.w",
 "divu.w",
 "repeat",
 "add",
 0
};

/* Advanced Macro Opecode name class5 - bit12_8 = 111:11 */
const char *c33_adv_class5_opcodes[] =
{
 "do.c",
 "psrset",
 "psrclr",
 0
};

#if 0	/* â∫Ç÷à⁄ìÆ  T.Tazaki 2004/07/30 */
const int c33_num_opcodes =
  sizeof (c33_opcodes) / sizeof (c33_opcodes[0]);
#endif


/* The opcode table.    < PE MACRO > */

const struct c33_opcode c33_pe_opcodes32[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "jpr",    OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* PE */
{ "jpr.d",  OP_CLASS0_1(0x0f),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* PE */
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "push",   OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* PE */
{ "pop",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {RD01},             0,  0}, /* PE */
{ "pushs",  OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {SS02},             0,  0}, /* PE */
{ "pops",   OP_CLASS0_1(0x03),      OP_CLASS0_1_MASK,       {SD02},             0,  0}, /* PE */
{ "ld.cf",  OP_CLASS0_1(0x07),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},  /* PE */ /* add 2004/07/07 T.Tazaki */
//{ "div.w",  OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* PE */ /* del 2004/07/07 T.Tazaki */
//{ "divu.w", OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* PE */ /* del 2004/07/07 T.Tazaki */

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDB_RD_MEM_IMM26},     0,  0},	/* change 2004/07/23 T.Tazaki */
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.b",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDB_WR_MEM_IMM26,RS},     0,  0},	/* change 2004/07/23 T.Tazaki */

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.ub", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDUB_RD_MEM_IMM26},     0,  0},/* change 2004/07/23 T.Tazaki */

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDH_RD_MEM_IMM26},     0,  0},/* change 2004/07/23 T.Tazaki */
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.h",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDH_WR_MEM_IMM26,RS},     0,  0},/* change 2004/07/23 T.Tazaki */

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.uh", OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDUH_RD_MEM_IMM26},     0,  0},/* change 2004/07/23 T.Tazaki */

{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},      0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD_LD,RS2},        0,  0},	/* PE 	ld %sd,%rs */
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB_IMM26},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,XLDW_RD_MEM_IMM26},     0,  0},/* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB_IMM26,RS},     0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XLDW_WR_MEM_IMM26,RS},     0,  0},/* change 2004/07/23 T.Tazaki */
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},

{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},

{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swaph",  OP_CLASS4_2(0x9a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* PE */
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
//{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbtst",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBTST_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbclr",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBCLR_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbset",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBSET_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB_IMM26,IMM3},   0,  0},			/* change T.Tazaki 2004/07/26 */
{ "xbnot",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {XBNOT_MEM_IMM26,IMM3},   0,  0},	/* change T.Tazaki 2004/07/23 */

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},
{ "ld.c",   OP_CLASS5(0xb1),        OP_CLASS5_MASK,         {RD,IMM5},          0,  0}, /* PE */	/* add T.Tazaki 2004/07/07 */
{ "ld.c",   OP_CLASS5(0xb5),        OP_CLASS5_MASK,         {IMM5,RS},          0,  0}, /* PE */	/* add T.Tazaki 2004/07/07 */
{ "do.c",   OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM6_OP3},         0, 0},  /* PE */	/* add T.Tazaki 2004/07/07 */
{ "psrset", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_01},      0, 0},  /* PE */
{ "psrclr", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_10},      0, 0},  /* PE */

/* class 6 */

{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;

/*************************************************************************************************/
/*  Use default data area mode 																	 */
/*************************************************************************************************/
const struct c33_opcode c33_opcodes[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},      0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD,RS2},           0,  0},
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},

{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM4},          0,  5},
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  6},

{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},

/* class 6 */

{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;

/* The opcode table.    < ADVANCED MACRO > */

const struct c33_opcode c33_advance_opcodes[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "jpr",    OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* Adv */
{ "jpr.d",  OP_CLASS0_1(0x0f),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* Adv */
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "push",   OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "pop",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {RD01},             0,  0}, /* Adv */
{ "pushs",  OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {SS02},             0,  0}, /* Adv */
{ "pops",   OP_CLASS0_1(0x03),      OP_CLASS0_1_MASK,       {SD02},             0,  0}, /* Adv */
{ "mac.w",  OP_CLASS0_1(0x04),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "mac.hw", OP_CLASS0_1(0x05),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* Adv */
{ "macclr", OP_CLASS0_1(0x06),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},/* Adv */
{ "ld.cf",  OP_CLASS0_1(0x07),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},/* Adv */
{ "div.w",  OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "divu.w", OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "repeat", OP_CLASS0_1(0x0a),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* Adv */
{ "repeat", OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {IMM4_01},          0,  0}, /* Adv */

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retm",   OP_CLASS0_1(0x1b),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},/* Adv */ /* 2002/10/01 */
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},
{ "ld.b",   OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.b",   OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */

{ "xld.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  1}, /* Adv */
{ "xld.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  1}, /* Adv */
{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
{ "xld.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.ub",  OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.ub",  OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */

{ "xld.ub", OP_CLASS2(0x39),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  1}, /* Adv */
{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.ub", OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.h",   OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */

{ "xld.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  2}, /* Adv */
{ "xld.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  2}, /* Adv */
{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
{ "xld.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.uh",  OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.uh",  OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */

{ "xld.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  2}, /* Adv */
{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.w",   OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DPIMM6},        0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DPIMM6,RS},        0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_OFF_SYMBOL6},0,  0}, /* Adv */
{ "ld.w",   OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_OFF_SYMBOL6,RS},0,  0}, /* Adv */
{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},  0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD_LD,RS2},        0,  0},
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DPIMM32},       0,  4}, /* Adv */
{ "xld.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DPIMM32,RS},       0,  4}, /* Adv */
{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},
{ "xld.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_SYMBOL32},   0,  0}, /* Adv */
{ "xld.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_SYMBOL32,RS},   0,  0}, /* Adv */
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ald.b",  OP_CLASS7(0x38),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
{ "ald.b",  OP_CLASS7(0x3d),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */

{ "ald.ub", OP_CLASS7(0x39),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */

{ "ald.h",  OP_CLASS7(0x3a),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
{ "ald.h",  OP_CLASS7(0x3e),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */

{ "ald.uh", OP_CLASS7(0x3b),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */

{ "ald.w",  OP_CLASS7(0x3c),        OP_CLASS7_MASK,         {RD,DP_SYMBOL19},       0,  0}, /* Adv */
{ "ald.w",  OP_CLASS7(0x3f),        OP_CLASS7_MASK,         {DP_SYMBOL19,RS},       0,  0}, /* Adv */

{ "add",    OP_CLASS0_1(0x0d),      OP_CLASS0_1_MASK,       {RD01,DP},          0,  0},     /* Adv */
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS1_MASK,         {RD,DP_OFF_SYMBOL6_2},0,0},     /* Adv */
{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},/* Adv */
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* Adv */
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},

{ "swaph",  OP_CLASS4_2(0x9a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "sat.b",  OP_CLASS4_2(0x9e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "sat.ub", OP_CLASS4_2(0x9f),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* Adv */
{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.hw", OP_CLASS5(0xa3),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},
{ "mac1.h", OP_CLASS5(0xa7),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mac1.hw",OP_CLASS5(0xab),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "mac1.w", OP_CLASS5(0xb3),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "ld.c",   OP_CLASS5(0xb1),        OP_CLASS5_MASK,         {RD,IMM5},          0,  0}, /* Adv */
{ "ld.c",   OP_CLASS5(0xb5),        OP_CLASS5_MASK,         {IMM5,RS},          0,  0}, /* Adv */
{ "sat.h",  OP_CLASS5(0xb6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "sat.uh", OP_CLASS5(0xb7),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xb9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xba),        OP_CLASS5_MASK,         {RD,IMM5_LABEL},    0,  0}, /* Adv */
{ "loop",   OP_CLASS5(0xbb),        OP_CLASS5_MASK,         {IMM5_2,IMM5_LABEL},0,  0}, /* Adv */
{ "sat.w",  OP_CLASS5(0xbd),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "sat.uw", OP_CLASS5(0xbe),        OP_CLASS5_MASK,         {RD,RS2},           0,  0}, /* Adv */
{ "do.c",   OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM6_OP3},         0, 0},  /* Adv */
{ "psrset", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_01},      0, 0},  /* Adv */
{ "psrclr", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_10},      0, 0},  /* Adv */

{ "ext",    OP_CLASS1(0x3f),        OP_CLASS1_MASK,         {RS2,OP_SHIFT,IMM2},0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3f),        OP_CLASS1_MASK,         {RS2},              0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3b),        OP_CLASS1_MASK,         {COND},             0,  0}, /* Adv */
{ "ext",    OP_CLASS1(0x3b),        OP_CLASS1_MASK,         {OP_SHIFT,IMM2},    0,  0}, /* Adv */
{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;


/* The opcode table.    < PE MACRO > */

const struct c33_opcode c33_pe_opcodes[] =
{
/* class 0 */
{ "nop",    OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "slp",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "halt",   OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "pushn",  OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RS},               0,  0},
{ "popn",   OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RD},               0,  0},
{ "jpr",    OP_CLASS0_1(0x0b),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* PE */
{ "jpr.d",  OP_CLASS0_1(0x0f),      OP_CLASS0_1_MASK,       {RB0},              0,  0}, /* PE */
{ "brk",    OP_CLASS0_1(0x10),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "retd",   OP_CLASS0_1(0x11),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "int",    OP_CLASS0_1(0x12),      OP_CLASS0_1_MASK,       {IMM2},             0,  0},
{ "reti",   OP_CLASS0_1(0x13),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "push",   OP_CLASS0_1(0x00),      OP_CLASS0_1_MASK,       {RS01},             0,  0}, /* PE */
{ "pop",    OP_CLASS0_1(0x01),      OP_CLASS0_1_MASK,       {RD01},             0,  0}, /* PE */
{ "pushs",  OP_CLASS0_1(0x02),      OP_CLASS0_1_MASK,       {SS02},             0,  0}, /* PE */
{ "pops",   OP_CLASS0_1(0x03),      OP_CLASS0_1_MASK,       {SD02},             0,  0}, /* PE */
{ "ld.cf",  OP_CLASS0_1(0x07),      OP_CLASS0_1_MASK,       {UNUSED},           0,  10},  /* PE */ /* add 2004/07/07 T.Tazaki */
//{ "div.w",  OP_CLASS0_1(0x09),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* PE */ /* del 2004/07/07 T.Tazaki */
//{ "divu.w", OP_CLASS0_1(0x08),      OP_CLASS0_1_MASK,       {RB01},             0,  0}, /* PE */ /* del 2004/07/07 T.Tazaki */

{ "call",   OP_CLASS0_1(0x18),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call",   OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "call.d", OP_CLASS0_1(0x1c),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "call.d", OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "scall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "scall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xcall",  OP_CLASS0_2(0x1c),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xcall.d",OP_CLASS0_2(0x1d),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ret",    OP_CLASS0_1(0x19),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "ret.d",  OP_CLASS0_1(0x1d),      OP_CLASS0_1_MASK,       {UNUSED},           0,  0},
{ "jp",     OP_CLASS0_1(0x1a),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp",     OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jp.d",   OP_CLASS0_1(0x1e),      OP_CLASS0_1_MASK,       {RB0},              0,  0},
{ "jp.d",   OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjp",    OP_CLASS0_2(0x1e),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjp.d",  OP_CLASS0_2(0x1f),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrgt",   OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrgt.d", OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrgt",  OP_CLASS0_2(0x08),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrgt.d",OP_CLASS0_2(0x09),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrge",   OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrge.d", OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrge",  OP_CLASS0_2(0x0A),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrge.d",OP_CLASS0_2(0x0B),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrlt",   OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrlt.d", OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrlt",  OP_CLASS0_2(0x0C),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrlt.d",OP_CLASS0_2(0x0D),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrle",   OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrle.d", OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrle",  OP_CLASS0_2(0x0E),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrle.d",OP_CLASS0_2(0x0F),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrugt",  OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrugt.d",OP_CLASS0_2(0x11),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrugt", OP_CLASS0_2(0x10),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrugt.d",OP_CLASS0_2(0x11),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jruge",  OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jruge.d",OP_CLASS0_2(0x13),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjruge", OP_CLASS0_2(0x12),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjruge.d",OP_CLASS0_2(0x13),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrult",  OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrult.d",OP_CLASS0_2(0x15),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrult", OP_CLASS0_2(0x14),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrult.d",OP_CLASS0_2(0x15),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrule",  OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrule.d",OP_CLASS0_2(0x17),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrule", OP_CLASS0_2(0x16),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrule.d",OP_CLASS0_2(0x17),     OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jreq",   OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jreq.d", OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjreq",  OP_CLASS0_2(0x18),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjreq.d",OP_CLASS0_2(0x19),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "jrne",   OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "jrne.d", OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN8_LABELIMM8},  0,  0},
{ "sjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "sjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM22},0,  0},
{ "xjrne",  OP_CLASS0_2(0x1a),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},
{ "xjrne.d",OP_CLASS0_2(0x1b),      OP_CLASS0_2_MASK,       {SIGN32_LABELIMM32},0,  0},

{ "ld.b",   OP_CLASS5(0xa1),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.b",   OP_CLASS1(0x21),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.b",   OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.b",   OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.b",   OP_CLASS1(0x35),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.b",   OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.b",   OP_CLASS1(0x34),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.b",  OP_CLASS2(0x10),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.b",  OP_CLASS1(0x20),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.b",  OP_CLASS2(0x15),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  1},
{ "xld.b",  OP_CLASS1(0x34),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.ub",  OP_CLASS5(0xa5),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.ub",  OP_CLASS1(0x25),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.ub",  OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.ub",  OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.ub", OP_CLASS2(0x11),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  1},
{ "xld.ub", OP_CLASS1(0x24),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.h",   OP_CLASS5(0xa9),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.h",   OP_CLASS1(0x29),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.h",   OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.h",   OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.h",   OP_CLASS1(0x39),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.h",   OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.h",   OP_CLASS1(0x38),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.h",  OP_CLASS2(0x12),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.h",  OP_CLASS1(0x28),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.h",  OP_CLASS2(0x16),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  2},
{ "xld.h",  OP_CLASS1(0x38),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},

{ "ld.uh",  OP_CLASS5(0xad),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "ld.uh",  OP_CLASS1(0x2d),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.uh",  OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.uh",  OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,RB},            0,  0},

{ "xld.uh", OP_CLASS2(0x13),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  2},
{ "xld.uh", OP_CLASS1(0x2c),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},

{ "ld.w",   OP_CLASS1(0x2e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "ld.w",   OP_CLASS5(0xa4),        OP_CLASS5_MASK,         {RD,SS},            0,  0},
{ "ld.w",   OP_CLASS1(0x31),        OP_CLASS1_MASK,         {RD,REGINC},        0,  0},
{ "ld.w",   OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM6},        0,  0},
{ "ld.w",   OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,RB},            0,  0},
{ "ld.w",   OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN6_SYMBOLIMM6},      0,  0},
{ "ld.w",   OP_CLASS5(0xa0),        OP_CLASS5_MASK,         {SD_LD,RS2},        0,  0},	/* PE 	ld %sd,%rs */
{ "ld.w",   OP_CLASS1(0x3d),        OP_CLASS1_MASK,         {REGINC,RS},        0,  0},
{ "ld.w",   OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM6,RS},        0,  0},
{ "ld.w",   OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {RB,RS},            0,  0},

{ "xld.w",  OP_CLASS2(0x14),        OP_CLASS2_MASK,         {RD,SPIMM32},       0,  4},
{ "xld.w",  OP_CLASS2(0x17),        OP_CLASS2_MASK,         {SPIMM32,RS},       0,  4},
{ "xld.w",  OP_CLASS1(0x30),        OP_CLASS1_MASK,         {RD,MEM_IMM26},     0,  0},
{ "xld.w",  OP_CLASS1(0x3c),        OP_CLASS1_MASK,         {MEM_IMM26,RS},     0,  0},
{ "xld.w",  OP_CLASS3(0x1b),        OP_CLASS3_MASK,         {RD,SIGN32_SYMBOLIMM32},    0,  0},

{ "add",    OP_CLASS1(0x22),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "add",    OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "add",    OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS4_1(0x20),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xadd",   OP_CLASS3(0x18),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "sub",    OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "sub",    OP_CLASS1(0x26),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "sub",    OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM6},          0,  0},
{ "xsub",   OP_CLASS4_1(0x21),      OP_CLASS4_1_MASK,       {SP,IMM10},         0,  0},
{ "xsub",   OP_CLASS3(0x19),        OP_CLASS3_MASK,         {RD,IMM32},         0,  0},

{ "cmp",    OP_CLASS1(0x2a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "cmp",    OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xcmp",   OP_CLASS3(0x1a),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

{ "and",    OP_CLASS1(0x32),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "and",    OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xand",   OP_CLASS3(0x1c),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "or",     OP_CLASS1(0x36),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "or",     OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xoor",   OP_CLASS3(0x1d),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "xor",    OP_CLASS1(0x3a),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "xor",    OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xxor",   OP_CLASS3(0x1e),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},
{ "not",    OP_CLASS1(0x3e),        OP_CLASS1_MASK,         {RD,RS2},           0,  0},
{ "not",    OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN6},         0,  0},
{ "xnot",   OP_CLASS3(0x1f),        OP_CLASS3_MASK,         {RD,SIGN32},        0,  0},

/* class 4 */

{ "srl",    OP_CLASS4_2(0x89),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "srl",    OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsrl",   OP_CLASS4_2(0x88),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sll",    OP_CLASS4_2(0x8d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sll",    OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsll",   OP_CLASS4_2(0x8C),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sra",    OP_CLASS4_2(0x91),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sra",    OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsra",   OP_CLASS4_2(0x90),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "sla",    OP_CLASS4_2(0x95),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "sla",    OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xsla",   OP_CLASS4_2(0x94),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rr",     OP_CLASS4_2(0x99),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rr",     OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xrr",    OP_CLASS4_2(0x98),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},
{ "rl",     OP_CLASS4_2(0x9d),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "rl",     OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7}, /* PE */
{ "xrl",    OP_CLASS4_2(0x9c),      OP_CLASS4_2_MASK,       {RD,IMM5},          0,  7},

{ "scan0",  OP_CLASS4_2(0x8a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "scan1",  OP_CLASS4_2(0x8e),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "swaph",  OP_CLASS4_2(0x9a),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0}, /* PE */
{ "swap",   OP_CLASS4_2(0x92),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
{ "mirror", OP_CLASS4_2(0x96),      OP_CLASS4_2_MASK,       {RD,RS2},           0,  0},
//{ "div0s",  OP_CLASS4_2(0x8b),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div0u",  OP_CLASS4_2(0x8f),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div1",   OP_CLASS4_2(0x93),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div2s",  OP_CLASS4_2(0x97),      OP_CLASS4_2_MASK,       {RS2},              0,  0},
//{ "div3s",  OP_CLASS4_2(0x9b),      OP_CLASS4_2_MASK,       {UNUSED},           0,  0},

/* class 5 */

{ "btst",   OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbtst",  OP_CLASS5(0xa8),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bclr",   OP_CLASS5(0xac),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbclr",  OP_CLASS5(0xac),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bset",   OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbset",  OP_CLASS5(0xb0),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},
{ "bnot",   OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {RB,IMM3},          0,  0},
{ "xbnot",  OP_CLASS5(0xb4),        OP_CLASS5_MASK,         {MEM_IMM26,IMM3},   0,  0},

{ "adc",    OP_CLASS5(0xb8),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "sbc",    OP_CLASS5(0xbc),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.h",  OP_CLASS5(0xa2),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.h", OP_CLASS5(0xa6),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mlt.w",  OP_CLASS5(0xaa),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mltu.w", OP_CLASS5(0xae),        OP_CLASS5_MASK,         {RD,RS2},           0,  0},
{ "mac",    OP_CLASS5(0xb2),        OP_CLASS5_MASK,         {RS2},              0,  0},
{ "ld.c",   OP_CLASS5(0xb1),        OP_CLASS5_MASK,         {RD,IMM5},          0,  0}, /* PE */	/* add T.Tazaki 2004/07/07 */
{ "ld.c",   OP_CLASS5(0xb5),        OP_CLASS5_MASK,         {IMM5,RS},          0,  0}, /* PE */	/* add T.Tazaki 2004/07/07 */
{ "do.c",   OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM6_OP3},         0, 0},  /* PE */	/* add T.Tazaki 2004/07/07 */
{ "psrset", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_01},      0, 0},  /* PE */
{ "psrclr", OP_CLASS5(0xbf),        OP_CLASS5_MASK,         {IMM5_OP3_10},      0, 0},  /* PE */

/* class 6 */

{ "ext",    OP_CLASS6(0x6),         OP_CLASS6_MASK,         {IMM13_LABEL},      0,  0},

{ 0, 0, 0, {0}, 0, 0 },

} ;

/* è„Ç©ÇÁà⁄ìÆ  T.Tazaki 2004/07/30 */
const int c33_num_opcodes =
  sizeof (c33_opcodes) / sizeof (c33_opcodes[0]);
