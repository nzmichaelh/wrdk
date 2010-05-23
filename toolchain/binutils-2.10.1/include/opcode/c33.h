/* c33.h -- Header file for EPSON C33 opcode table
   Copyright 1996 Free Software Foundation, Inc.
   Written by J.T. Conklin, Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef V850_H
#define V850_H

/* The opcode table is an array of struct c33_opcode.  */

struct c33_opcode
{
  /* The opcode name.  */
  const char *name;

  /* The opcode itself.  Those bits which will be filled in with
     operands are zeroes.  */
  unsigned long opcode;

  /* The opcode mask.  This is used by the disassembler.  This is a
     mask containing ones indicating those bits which must match the
     opcode field, and zeroes indicating those bits which need not
     match (and are presumably filled in by operands).  */
  unsigned long mask;

  /* An array of operand codes.  Each code is an index into the
     operand table.  They appear in the order which the operands must
     appear in assembly code, and are terminated by a zero.  */
  unsigned char operands[8];

  /* Which (if any) operand is a memory operand.  */
  unsigned int memop;

	/* special flag */
	/* 0: special process none */
	/* 1: [sp+imm32]  byte (1byte) */
	/* 2: [sp+imm32]  half word (2byte) */
	/* 4: [sp+imm32]  word(4byte) */
	/* 5:shift/lotate */
	unsigned int specialFlag;
};

#if 0	/* c33 */
/* Values for the processors field in the c33_opcode structure.  */
#define PROCESSOR_V850		(1 << 0)		/* Just the V850.  */
#define PROCESSOR_ALL		-1			/* Any processor.  */
#define PROCESSOR_V850E		(1 << 1)		/* Just the V850E. */
#define PROCESSOR_NOT_V850	(~ PROCESSOR_V850)	/* Any processor except the V850.  */
#define PROCESSOR_V850EA	(1 << 2)		/* Just the V850EA. */
#endif	/* c33 */

/* The table itself is sorted by major opcode number, and is otherwise
   in the order in which the disassembler should consider
   instructions.  */
extern const struct c33_opcode c33_opcodes[];

/* add tazaki 2001.11.07 */
extern int	g_iAdvance;
extern const struct c33_opcode c33_advance_opcodes[];
/* add tazaki 2001.09.19 */
extern const struct c33_opcode c33_ext_opcodes[];
/* add tazaki 2001.09.19 */
extern const int c33_num_opcodes;

/* add T.Tazaki 2003/11/18 >>> */
extern int g_iPE;
extern const struct c33_opcode c33_pe_opcodes[];
/* add T.Tazaki 2003/11/18 <<< */

/* add T.Tazaki 2004/07/30 >>> */
extern int g_iMedda32;
extern const struct c33_opcode c33_opcodes32[];
extern const struct c33_opcode c33_advance_opcodes32[];
extern const struct c33_opcode c33_pe_opcodes32[];
/* add T.Tazaki 2004/07/30 <<< */



/* The operands table is an array of struct c33_operand.  */

struct c33_operand
{
  /* The number of bits in the operand.  */
  /* If this value is -1 then the operand's bits are in a discontinous distribution in the instruction. */
  int bits;

  /* (bits >= 0):  How far the operand is left shifted in the instruction.  */
  /* (bits == -1): Bit mask of the bits in the operand.  */
  int shift;

  /* Insertion function.  This is used by the assembler.  To insert an
     operand value into an instruction, check this field.

     If it is NULL, execute
         i |= (op & ((1 << o->bits) - 1)) << o->shift;
     (i is the instruction which we are filling in, o is a pointer to
     this structure, and op is the opcode value; this assumes twos
     complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction and the operand value.  It will return the new value
     of the instruction.  If the ERRMSG argument is not NULL, then if
     the operand value is illegal, *ERRMSG will be set to a warning
     string (the operand will be inserted in any case).  If the
     operand value is legal, *ERRMSG will be unchanged (most operands
     can accept any value).  */
  unsigned long (* insert) PARAMS ((unsigned long instruction, long op,
				   const char ** errmsg));

  /* Extraction function.  This is used by the disassembler.  To
     extract this operand type from an instruction, check this field.

     If it is NULL, compute
         op = o->bits == -1 ? ((i) & o->shift) : ((i) >> o->shift) & ((1 << o->bits) - 1);
	 if (o->flags & V850_OPERAND_SIGNED)
	     op = (op << (32 - o->bits)) >> (32 - o->bits);
     (i is the instruction, o is a pointer to this structure, and op
     is the result; this assumes twos complement arithmetic).

     If this field is not NULL, then simply call it with the
     instruction value.  It will return the value of the operand.  If
     the INVALID argument is not NULL, *INVALID will be set to
     non-zero if this operand type can not actually be extracted from
     this operand (i.e., the instruction does not match).  If the
     operand is valid, *INVALID will not be changed.  */
  unsigned long (* extract) PARAMS ((unsigned long instruction, int * invalid));

  /* One bit syntax flags.  */
  int flags;
  
  /* 有効範囲 imm6/sign8/imm13/imm26など 
     例えば、xld.w [symbol+imm26]の有効範囲は26bit
     		sld.w [symbol+imm13]の有効範囲は13bitとなる
  */
  int range;
};

/* Elements in the table are retrieved by indexing with values from
   the operands field of the c33_opcodes table.  */

extern const struct c33_operand c33_operands[];

/* Values defined for the flags field of a struct c33_operand.  */

/* This operand names a general purpose register */
#define C33_OPERAND_REG		0x01
#define	C33_OPERAND_SREG	0x02	/* special registers */
#define	C33_OPERAND_IMM		0x04
#define C33_OPERAND_SIGNED	0x08
#define	C33_OPERAND_MEM		0x10	/* [symbol+imm32] */
#define	C33_OPERAND_SPMEM	0x20	/* [%sp+imm6] */
#define	C33_OPERAND_REGINC	0x40	/* [%rb]+ */
#define	C33_OPERAND_SP		0x80
#define	C33_OPERAND_LABEL	0x100	/* label+imm32 */
#define	C33_OPERAND_RB		0x200	/* [%rb] , [%rb+imm]*/
#define	C33_OPERAND_SYMBOL	0x400	/* symbol+imm32 */
#define	C33_OPERAND_PC		0x800	/* jp sign8(8:1) sign(0) = 0 */
/* >>> add tazaki 2002.06.19 */
#define	C33_OPERAND_OFFSET_SYMBOL	0x1000	/* ext doff_hi(symbol),doff_li(symbol), etc... */
#define C33_OPERAND_01		0x2000	/* Advanced Inst : class0 bit5,4 = ( 0,1 ) */
#define C33_OPERAND_OP3_01	0x4000	/* Advanced Inst : class5 bit7,6 = ( 0,1 ) */
#define C33_OPERAND_OP3_10	0x8000	/* Advanced Inst : class5 bit7,6 = ( 1,0 ) */
#define C33_OPERAND_DPMEM	0x0003	/* Advanced Inst : [%dp+imm6] */
#define C33_OPERAND_DP		0x0070	/* Advanced Inst : %dp        */
#define C33_OPERAND_DP_SYMBOL	0xc000	/* Advanced Inst : [symbol+imm] */
#define C33_OPERAND_DPSYMBOL6	0x3000	/* Advanced Inst : [%dp+dpoff_l(symbol)] */
#define C33_OPERAND_DPSYMBOL6_2	0x1800	/* Advanced Inst : [%dp+dpoff_l(symbol)] */
#define C33_OPERAND_COND	0x1400	/* Advanced Inst : ext cond */
#define C33_OPERAND_OP_SHIFT	0x1200	/* Advanced Inst : ext OP,imm2 | ext %rb,OP,imm2 */
#define C33_OPERAND_LD_SREG	0x10000	/* Advanced Inst : ld.w %sd,%rs */
#define	C33_OPERAND_PUSHS_SREG	0x20000	/* special registers : pushs , pops */
/* <<< add tazaki 2002.06.19 */

/* add T.Tazaki 2004/07/23 >>> */
#define	C33_XLDB_RD			0x40000		/* xld.b %rd,[symbol+imm]   */
#define	C33_XLDB_WR			0x80000		/* xld.b [symbol+imm],%rs   */
#define	C33_XLDH_RD			0x100000	/* xld.h %rd,[symbol+imm]   */
#define	C33_XLDH_WR			0x200000	/* xld.h [symbol+imm],%rs   */
#define	C33_XLDW_RD			0x400000	/* xld.w %rd,[symbol+imm]   */
#define	C33_XLDW_WR			0x800000	/* xld.w [symbol+imm],%rs   */
#define	C33_XLDUB_RD		0x1000000	/* xld.ub %rd,[symbol+imm]  */
#define	C33_XLDUH_RD		0x2000000	/* xld.uh %rd, [symbol+imm] */
#define	C33_XBTST			0x4000000	/* xbtst [symbol+imm],imm3  */
#define	C33_XBCLR			0x8000000	/* xbclr [symbol+imm],imm3  */
#define	C33_XBSET			0x10000000	/* xbset [symbol+imm],imm3  */
#define	C33_XBNOT			0x20000000	/* xbnot [symbol+imm],imm3  */
#define C33_OPERAND_26		0x40000000	/* [%rb+imm26]              */
/* add T.Tazaki 2004/07/23 <<< */
 
#endif /* C33 */
