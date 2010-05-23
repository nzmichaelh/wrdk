/* Disassemble V850 instructions.
   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

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


#include <stdio.h>

#include "sysdep.h"
#include "opcode/c33.h" 
#include "dis-asm.h"
#include "opintl.h"

static const char *const c33_reg_names[] =
{ "%r0", "%r1", "%r2", "%r3", "%r4", "%r5", "%r6", "%r7", 
  "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15", 
};

static const char *const c33_sreg_names[] =		/* add "idir,dbg__base" T.Tazaki 2003/11/18 */
{ "%psr","%sp", "%alr", "%ahr", "%lco", "%lsa", "%lea", "%sor", "%ttbr", "%dp","%idir","%dbbr","","%usp", "%ssp", "%pc" };

/* add tazaki 2001.09.12 >>>>> */

/******* type definiton ******/
typedef char    INT8;
typedef short   INT16;
typedef long    INT32;
typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef unsigned long   UINT32;
typedef unsigned long   ADDR;

#define NO_ERR      0
#define ERR         2
#define ERR2        3
#define QUIT		100 /* add D.Fujimoto 2007/10/17 */

#define BIT0    0x01
#define BIT1    0x02
#define BIT2    0x04
#define BIT3    0x08
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80
#define BIT8    0x100
#define BIT9    0x200
#define BIT10   0x400
#define BIT11   0x800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000
#define BIT16   0x10000
#define BIT17   0x20000
#define BIT18   0x40000
#define BIT19   0x80000
#define BIT20   0x100000
#define BIT21   0x200000
#define BIT22   0x400000
#define BIT23   0x800000
#define BIT24   0x1000000
#define BIT25   0x2000000
#define BIT26   0x4000000
#define BIT27   0x8000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

#define YES                 1
#define NO                  0

#define  CLASS_MASK  0xE000
#define  CLASS0      0x0000
#define  CLASS1      0x2000
#define  CLASS2      0x4000
#define  CLASS3      0x6000
#define  CLASS5      0xA000
#define  CLASS6      0xC000
#define  CLASS7      0xE000 /* add tazaki 2001.09.19 */
#define  CLASS6_DATA 0x1fff
#define  EXT1        1
#define  EXT2        2
#define  EXT3        3
#define  CALC1       10
#define  CALC2       20
#define  CALC3_BYTE  31
#define  CALC3_HALF  32
#define  CALC3_WORD  33
#define  CALC4       40
#define  CALC5       50

#define  MASK1_0     0x3
#define  MASK7_0     0xff
#define  MASK9_4     0x3f0
#define  MASK9_5     0x3e0
#define  MASK9_6     0x3c0
#define  MASK12_0    0x1fff
#define  MASK12_3    0x1ff8
#define  MASK31_6    0xffffffc0
#define  MASK31_9    0xfffffe00
#define  MASK31_19   0xfff80000
#define  MASK31_22   0xffc00000

/* #define  DEBUG33     1 */

/* >>>>> D.Fujimoto 2007/10/04 string buffer size for objdump output */
#define DUMP_OUTPUT_BUF_SIZE 512
#define SYMBOL_NAME_BUF_SIZE	256
/* <<<<< D.Fujimoto 2007/10/04 string buffer size for objdump output */

/* >>>>> D.Fujimoto 2007/10/04 declare prototypes */
static UINT8
fnCrGetExtType(UINT32 ulCode,int *iExtType, int *iCalcType);
static UINT32
fnCrCalcImmVal(int nExtInst, int iCalcType, UINT32 ulExt1, UINT32 ulExt2, UINT32 ulCode);
static UINT8
fnCrGetXinst(UINT32 ulCode, UINT32 ulImmVal,int iExtType, ADDR tdAddr, INT8 *szXinst, int iArraySize, INT8 *szTmp, int iExtCnt, struct disassemble_info *info);
static void
vfnDisasm (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass0 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass1 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass2 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass3 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass4 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass5 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass6 (unsigned short uwCode, char *pszBuf);
static void
vfnDisasmClass7 (unsigned short uwCode, char *pszBuf);
/* <<<<< D.Fujimoto 2007/10/04 declare prototypes */

static int  g_iExtCnt = 0;
static int  g_ExtImm[ 2 ];

/* <<<<< add tazaki 2001.09.18 */
/* Advanced Macro Opecode name class0 - bit5,4 = 0,1 */
extern const char *c33_adv_opcodes[];

/* Advanced Macro Opecode name class5 - bit12_8 = 111:11 */
extern const char *c33_adv_class5_opcodes[];
/* >>>>> add tazaki 2001.09.18 */

/* <<<<< add tazaki 2002.02.29 */

int     g_iShiftFlag =0;
const   char    szOpShift[3][4] = { "sra","sla","sll" };
const   char    szCond[10][4]   = { "gt","ge","lt","le","ugt","uge","ult","ule","eq","ne" };

/* >>>>> ADDED D.Fujimoto 2007/10/03 for displaying symbol */
#include <stdarg.h>
static char dummy_buffer[4096], *dummy_p;
static int
dummy_sprintf (FILE *f, const char *format, ...)
{
        char *buf;
        va_list args;
        size_t n;

        va_start (args, format);

        vasprintf (&buf, format, args);

        va_end (args);

        n = strlen (buf);
        if (dummy_p + n + 1 < dummy_buffer + sizeof(dummy_buffer)) {
                strcpy(dummy_p, buf);
                dummy_p += n;
        } else {
                n = 0;
        }

        free (buf);

        return n;
}

static char *find_symbolname_for_address(bfd_vma vma, struct disassemble_info *info)
{
        char *name;
        fprintf_ftype saved_fprintf_func;

        if (!info->symbol_at_address_func(vma, info)) return NULL;

        dummy_p = dummy_buffer;

        saved_fprintf_func = info->fprintf_func;
        info->fprintf_func = (fprintf_ftype)dummy_sprintf;

        info->print_address_func(vma, info);

        info->fprintf_func = saved_fprintf_func;

        name = strrchr(dummy_buffer, '>');
        if (!name) return NULL;
        *name = '\0';
        name = strchr(dummy_buffer, '<');
        if (!name) return NULL;
        return name + 1;
}
/* <<<<< ADDED D.Fujimoto 2007/10/03 */

static int
disassemble (memaddr, info, insn)
     bfd_vma memaddr;
     struct disassemble_info *info;
     unsigned long insn;
{
/*  struct c33_opcode *          op = (struct c33_opcode *)c33_opcodes; */
  struct c33_opcode *          op = (struct c33_opcode *)c33_advance_opcodes; /* tazaki 2001.12.05 */

  const struct c33_operand *   operand;
  int                           match = 0;
  int               bytes_read;

/* add tazaki 2001.07.31 >>>>> */
    ADDR    tdAddr;            /* address of target instruction */
    INT8    szExtString[DUMP_OUTPUT_BUF_SIZE];  /* extended instruntion string   */	// MOD D.Fujimoto 2007/10/04 string buffer size
    int     nExtInst;          /* number of ext-instrcution */
    int     iCalcType;         /* immediate value calculation type */
    int     iExtType;          /* extension type */
    int     i;
    UINT32  ulImm;             /* extended immediate value */
    UINT32  ulImm1;            /* immediate value of first ext_instruction */
    UINT32  ulImm2;            /* immediate value of second ext_instruction */
    UINT32  ulCode;            /* target instruction code */
    INT8    szMessage[DUMP_OUTPUT_BUF_SIZE];    /* array of mnemonic or warning message */	// MOD D.Fujimoto 2007/10/04 string buffer size
    INT8    szTmp[DUMP_OUTPUT_BUF_SIZE]; 	// MOD D.Fujimoto 2007/10/04 string buffer size
    UINT8   ucErrFlag = NO_ERR;/* error flag */
    INT8    szDisp[DUMP_OUTPUT_BUF_SIZE];        /* for Get string length compute */	// MOD D.Fujimoto 2007/10/04 string buffer size
    INT8    szBuf[DUMP_OUTPUT_BUF_SIZE];         /* for Get string length compute work buffer */	// MOD D.Fujimoto 2007/10/04 string buffer size
    INT8    szName[DUMP_OUTPUT_BUF_SIZE];	// MOD D.Fujimoto 2007/10/04 string buffer size
    unsigned long   insn_wk;
/* <<<<< add tazaki 2001.07.31 */

  
    bytes_read = 2;
    insn &= 0xffff;

    szDisp[0] = 0;  /* add tazaki 2001.08.03 */

    g_iShiftFlag =0;


    if ((insn & CLASS_MASK) == CLASS1) {     /* class1 */

        g_iShiftFlag = 1;

        /* Convert Opecode */
        insn_wk = insn & 0xff00;

        switch( insn_wk ){
        case 0x2300: insn_wk = 0x8800;  break;  /* srl -->class 4 : srl */
        case 0x2700: insn_wk = 0x8c00;  break;  /* sll -->class 4 : sll */
        case 0x2b00: insn_wk = 0x9000;  break;  /* sra -->class 4 : sra */
        case 0x2f00: insn_wk = 0x9400;  break;  /* sla -->class 4 : sla */
        case 0x3300: insn_wk = 0x9800;  break;  /* rr  -->class 4 : rr  */
        case 0x3700: insn_wk = 0x9c00;  break;  /* rl  -->class 4 : rl  */
        default:
            g_iShiftFlag = 0;
            break;
        }
        insn = insn_wk | ( insn & 0x00ff );
    }

  /* Find the opcode.  */
    while (op->name)
    {
        
        if ((op->mask & insn) == op->opcode)
        {
            const unsigned char * opindex_ptr;
            unsigned int          opnum;
            unsigned int          memop;
            unsigned short        uwOp1;
            unsigned short        uwOp2;
            int                   i;
            int                   iLength;
            

            strcpy( szName, op->name );


            /* Advanced Macro bit5,4 = 0,1 H add tazaki 2001.09.12 >>>  */
            /* ONLY class0 */
            if ((insn & CLASS_MASK) == CLASS0) {     /* class0 */
                uwOp1 = insn & 0x1e00;
                uwOp1 >>= 9;
                if( uwOp1 < 4 ){
                    if( insn & 0x0010 ){    /* bit5,4 = 0,1 ? */
                        i = 0;
                        while( c33_adv_opcodes[ i ] != 0 ){
                            if( !strcmp( c33_adv_opcodes[ i ], szName ) ){
                                break;
                            }
                            ++i;
                        }
                        if( c33_adv_opcodes[ i ] == 0 ){    /* No found ? */
                            op++;
                            continue;                       /* skip --> next search */
                        }
                    }
                }
            }
            /* <<<  add tazaki 2001.09.12  */

            /* Advanced Macro bit12-8=111:11 add tazaki 2001.09.18 >>>  */
            /* ONLY class5 */
            if ((insn & 0xff00) == 0xbf00) {     /* class5 111:11 */
                if( (insn & 0x00c0) == 0x0000 ){    /* do.c imm6 ? */
                    if( strcmp( c33_adv_class5_opcodes[ 0 ], szName )){
                        op++;
                        continue;
                    }
                }
                if( (insn & 0x00c0) == 0x0040 ){    /* psrset imm5 ? */
                    if( strcmp( c33_adv_class5_opcodes[ 1 ], szName )){
                        op++;
                        continue;
                    }
                }
                if( (insn & 0x00c0) == 0x0080 ){    /* psrclr imm5 ? */
                    if( strcmp( c33_adv_class5_opcodes[ 2 ], szName )){
                        op++;
                        continue;
                    }
                }

            }
            /* <<<  add tazaki 2001.09.12  */

            /* add tazaki 2001.08.03 >>>>> ‚s‚`‚a”‚ÌŒvŽZ */

            match = 1;

            (*info->fprintf_func) (info->stream, "  %s", szName);
            sprintf( szBuf,"  %s", szName );
            strcat( szDisp,szBuf );

            iLength = 8 - strlen( szName );
            for( i = 0; i < iLength; ++i ){
                (*info->fprintf_func) (info->stream, " ");
                strcat( szDisp," " );
            }
            /* <<<<< add tazaki 2001.08.03 */
            

            memop = op->memop;
            /* Now print the operands.

             MEMOP is the operand number at which a memory
             address specification starts, or zero if this
             instruction has no memory addresses.

             A memory address is always two arguments.

             This information allows us to determine when to
             insert commas into the output stream as well as
             when to insert disp[reg] expressions onto the
             output stream.  */
      
            for (opindex_ptr = op->operands, opnum = 1;
                                    *opindex_ptr != 0;
                                    opindex_ptr++, opnum++)
            {
                long    value;
                long    value2;
                long    value3;
                int     flag;
                int       status;
                bfd_byte    buffer[ 4 ];
                int     i_op_match;
                
                operand = &c33_operands[*opindex_ptr];
                
                if (operand->extract)
                    value = (operand->extract) (insn, 0);
                else
                {
                    value = (insn >> operand->shift) & ((1 << operand->bits) - 1);
                }

                if (opnum > 1) {
                    info->fprintf_func (info->stream, ",");
                    sprintf( szBuf,"," );   /* add tazaki 2001.08.03 */
                    strcat( szDisp,szBuf ); /* add tazaki 2001.08.03 */
                }
                /* extract the flags, ignorng ones which do not effect disassembly output. */
                flag = operand->flags;

                i_op_match = 0;
                /* >>> add tazaki 2002.02.29 */

                if( !strcmp( szName,"ext" ) )
                {
                    if ((insn & 0xff00) == 0x3b00) {     /* class1 001:11011 */
                        if( (insn & 0x00f0) == 0x0000 ){    
                            /* ext OP,imm2  */
                            value = insn & 0x000c;
                            value >>= 2;
                            value2 = insn & 0x0003;
                            if( value > 0 ){
                                info->fprintf_func (info->stream, "%s,0x%x", szOpShift[ value - 1],value2 );
                                sprintf( szBuf,"%s,0x%x", szOpShift[ value - 1],value2 );
                                strcat( szDisp,szBuf ); 
                                i_op_match = 1;
                            }
                        }
                        else
                        {
                            /* ext cond  */
                            value = insn & 0x00f0;
                            value >>= 4;
/* >>>>> MODIFIED D.Fujimoto 2008/05/22 disassemble hangs when processing with -D */
//                            if( value >= 4 ){
                            if( value >= 4 && value - 4 < 10 ){	// depends on szCond index
/* <<<<< MODIFIED D.Fujimoto 2008/05/22 disassemble hangs when processing with -D */
                                info->fprintf_func (info->stream, "%s", szCond[ value - 4] );
                                sprintf( szBuf,"%s", szCond[ value - 4] );
                                strcat( szDisp,szBuf ); 
                                i_op_match = 1;
                            }
                        }
                        
                    }
                    if ((insn & 0xff00) == 0x3f00) {     /* class1 001:11111 */
                        if( (insn & 0x000f) != 0x0000 ){    
                            /* ext %rs,OP,imm2 */
                            value = insn & 0x00f0;
                            value >>= 4;
                            value2 = insn & 0x000c;
                            value2 >>= 2;
                            value3 = insn & 0x0003;
                            if( value2 >= 1 ){
                                info->fprintf_func (info->stream, "%s,%s,0x%x", 
                                                c33_reg_names[value],
                                                szOpShift[ value2 - 1 ],
                                                value3
                                                );
                                sprintf( szBuf, "%s,%s,0x%x", 
                                                c33_reg_names[value],
                                                szOpShift[ value2 - 1 ],
                                                value3
                                                );
                                strcat( szDisp,szBuf ); 
                                i_op_match = 1;
                            }
                        }
                        else
                        {
                            /* ext %rs */
                            value = insn & 0x00f0;
                            value >>= 4;
                            info->fprintf_func (info->stream, "%s", c33_reg_names[value] );
                            sprintf( szBuf,"%s", c33_reg_names[value] );
                            strcat( szDisp,szBuf ); 
                            i_op_match = 1;
                        }
                    }
                }

                if( i_op_match == 0 )
                {
                    if( flag == C33_OPERAND_LD_SREG ){
                        flag = C33_OPERAND_SREG;
                    }
                    if( flag == (C33_OPERAND_PUSHS_SREG | C33_OPERAND_01) ){
                        flag = C33_OPERAND_SREG;
                    }
                    if( flag == ( C33_OPERAND_DP | C33_OPERAND_01 ) ){
                        flag &= ( C33_OPERAND_01 ^ 0xffff );
                    }
                    
                    if( flag != C33_OPERAND_DPMEM && flag != C33_OPERAND_DP ) {
                        flag &= - flag; 
                    }
                    
                    switch (flag)
                    {
                    
                    case C33_OPERAND_MEM    :
                        info->fprintf_func (info->stream, "[0x%x]", value);
                        sprintf( szBuf,"[0x%x]", value );
                        strcat( szDisp,szBuf ); 
                        break;

                    case C33_OPERAND_SPMEM  :
                        info->fprintf_func (info->stream, "[%%sp+0x%x]", value); /* [%sp+imm6] */
                        sprintf( szBuf,"[%%sp+0x%x]", value );  
                        strcat( szDisp,szBuf );
                        break;

                    case C33_OPERAND_DPMEM  :
                        info->fprintf_func (info->stream, "[%%dp+0x%x]", value); /* [%dp+imm6] */
                        sprintf( szBuf,"[%%dp+0x%x]", value );
                        strcat( szDisp,szBuf );
                        break;

                    case C33_OPERAND_REG:  
                        info->fprintf_func (info->stream, "%s", c33_reg_names[value]);
                        sprintf( szBuf,"%s", c33_reg_names[value] );
                        strcat( szDisp,szBuf );                     
                        break;
                        
                    case C33_OPERAND_SREG:
                        info->fprintf_func (info->stream, "%s", c33_sreg_names[value]);
                        sprintf( szBuf,"%s", c33_sreg_names[value] );
                        strcat( szDisp,szBuf );                     
                        break;


                    case C33_OPERAND_REGINC :
                        info->fprintf_func (info->stream, "[%s]+", c33_reg_names[value]);
                        sprintf( szBuf,"[%s]+", c33_reg_names[value] ); 
                        strcat( szDisp,szBuf );                        
                        break;

                    case C33_OPERAND_RB :
                        info->fprintf_func (info->stream, "[%s]", c33_reg_names[value]);
                        sprintf( szBuf,"[%s]", c33_reg_names[value] );
                        strcat( szDisp,szBuf );                        
                        break;

                    case C33_OPERAND_SP :
                        info->fprintf_func (info->stream, "%%sp");
                        sprintf( szBuf,"%%sp" );                
                        strcat( szDisp,szBuf );                 
                        break;
                        
                    case C33_OPERAND_DP :
                        info->fprintf_func (info->stream, "%%dp");
                        sprintf( szBuf,"%%dp" );            
                        strcat( szDisp,szBuf );             
                        break;
                        
                    case C33_OPERAND_IMM    :
                        if( g_iShiftFlag == 1 ){
                            value += 16;        /* shift : imm + 16 */
                        }
                    case C33_OPERAND_SIGNED :
                    default:
                        info->fprintf_func (info->stream, "0x%x", value);
                        sprintf( szBuf,"0x%x", value );         
                        strcat( szDisp,szBuf );                 
                        break;

                    }
                }
                else
                {
                    break;      // exit operand loop
                }
            }

            /* All done. */
            break;              /* exit while loop */
        }
        op++;
    }

    /* not found opcode ? */
    if (!match)
    {
      info->fprintf_func (info->stream, " .short\t0x%04x", insn);
      g_iExtCnt = 0;
      return bytes_read;
    }


/* add tazaki 2001.07.31 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /*==========================================*/
    /* Extended Instruction Display             */
    /*==========================================*/
    /* insn : 16 bit    */

    if( ( insn & CLASS_MASK ) == CLASS6 ){  /* ext ? */
        if( g_iExtCnt == 2 ){
            g_ExtImm[ 1 ] = insn & CLASS6_DATA;
        }else{
            g_ExtImm[g_iExtCnt] = insn & CLASS6_DATA;
        }
        if( g_iExtCnt < 2 ){
            ++g_iExtCnt;        /* increment ext counter */
        }
        return bytes_read;
    }
    
    ulCode = insn;
    ulImm1 = ulImm2 = 0;
    if( g_iExtCnt == 2 ){
        ulImm1 = g_ExtImm[ 0 ];
        ulImm2 = g_ExtImm[ 1 ];
    }else if( g_iExtCnt == 1 ){
        ulImm2 = g_ExtImm[ 0 ];
    }

    nExtInst = g_iExtCnt;

    tdAddr = memaddr;           /* offset address set */

/* >>>>> DELETED D.Fujimoto 2007/10/03 iExtCnt == 0 will also use these routines */
//    if( g_iExtCnt > 0 ){
/* <<<<< DELETED D.Fujimoto 2007/10/03 iExtCnt == 0 will also use these routines */

        /* analize instruction code */ /* extention type and calculation type */
        ucErrFlag = fnCrGetExtType(ulCode, &iExtType, &iCalcType);

        if (ucErrFlag == NO_ERR) {                 /* the target instruction is extendable */
            /* calculate extended immediate value */
            ulImm = fnCrCalcImmVal(nExtInst, iCalcType, ulImm1, ulImm2, ulCode);
            
            /* make extended instruction */
/* >>>>> MODIFIED D.Fujimoto 2007/10/03 added info as param */
//            ucErrFlag = fnCrGetXinst(ulCode, ulImm, iExtType, tdAddr, szMessage, 256, szTmp);
            ucErrFlag = fnCrGetXinst(ulCode, ulImm, iExtType, tdAddr, szMessage, DUMP_OUTPUT_BUF_SIZE, szTmp, g_iExtCnt, info);
/* <<<<< MODIFIED D.Fujimoto 2007/10/03 added info as param */
            
            if (ucErrFlag == NO_ERR) {               /* not disassemble error */
                if( strlen( szMessage ) > 0 ){
                    strcpy( (INT8 *)szExtString, szMessage );
                    szExtString[ strlen(szExtString)-1 ] = 0;

                    for( i = 0; i < ( 26 - strlen( szDisp ) ); ++i ){
                        info->fprintf_func (info->stream, " ");
                    }
                    info->fprintf_func (info->stream, "%s", szExtString);
                }
/* >>>>> ADDED D.Fujimoto 2007/10/10 display plain mnemonic */
            } else if (ucErrFlag == QUIT) {
				/* get mnemonic code */
				vfnDisasm ((unsigned short)ulCode, szTmp);
				for( i = 0; i < ( 26 - strlen( szDisp ) ); ++i ){
					info->fprintf_func (info->stream, " ");
				}
				info->fprintf_func (info->stream, "%s", szTmp);
/* <<<<< ADDED D.Fujimoto 2007/10/10 display plain mnemonic */
			}
/* >>>>> MODIFIED D.Fujimoto 2007/10/04 display when ERR and ERR2 */
//        }else if (ucErrFlag == ERR) {
		  } else {
/* <<<<< MODIFIED D.Fujimoto 2007/10/04 display when ERR and ERR2 */
            /* get mnemonic code */
            vfnDisasm ((unsigned short)ulCode, szTmp);
            for( i = 0; i < ( 26 - strlen( szDisp ) ); ++i ){
                info->fprintf_func (info->stream, " ");
            }
            info->fprintf_func (info->stream, "%s", szTmp);
        }

/* >>>>> DELETED D.Fujimoto 2007/10/03 iExtCnt == 0 will also use these routines */
//    }else{
//        /* get mnemonic code */
//        vfnDisasm ((unsigned short)ulCode, szTmp);
//        for( i = 0; i < ( 26 - strlen( szDisp ) ); ++i ){
//            info->fprintf_func (info->stream, " ");
//        }
//        info->fprintf_func (info->stream, "%s", szTmp);
//
//    }
/* <<<<< DELETED D.Fujimoto 2007/10/03 iExtCnt == 0 will also use these routines */

	g_iExtCnt = 0;  /* clear ext inst counter */
    
    return bytes_read;
}

static UINT8
fnCrGetExtType(ulCode,iExtType,iCalcType)

    /***************************************************************
     *
     *      --- Get the extention type and
     *                  the immediate calculation type ---
     *
     *      Check the extention type (set to the parameter2) and
     *      immediate value calculation type (set to the parameter3).
     *
     *      RETURN: NO_ERR: the target instruction is extendable
     *              ERR   : the target instruction cannot be extended
     *
     ***************************************************************/

    UINT32  ulCode;            /* target instruction */
    int    *iExtType;          /* extention type, 1-3 */
    int    *iCalcType;         /* immediate value calculation type, 1-5 */

{
    UINT32  ulOp1;             /* op1 */
    UINT32  ulOp2;             /* op2 */
    UINT8   ucErrFlag = ERR;   /* error flag */
    
    /* get the extention type and the immediate calculation type, according to the class */
    switch (ulCode & CLASS_MASK) {               /* switch by class type */
      case CLASS0:
        if ((ulCode & 0xfff0 )== 0x0350) {    /* add %rd,%dp ? */
            ucErrFlag = ERR2;               /* not show 3 operand. add tazaki 2001.12.21 */
        }else{
            ulOp1 = (ulCode >> 0x9) & 0xf;           /* bit12:9 */
            if ((ulOp1 & 0xc) != 0x0) {              /* not OP1 = 00XX, illeagal */
              ucErrFlag = NO_ERR;
              *iExtType = EXT1;
              *iCalcType = CALC1;
            }
        }
        break;
      
      case CLASS1:
        if (((ulCode & BIT8) == 0x0) && ((ulCode & BIT9) == 0x0)) {     /* load/store */
          ucErrFlag = NO_ERR;
          *iExtType = EXT2;
          *iCalcType = CALC2;
        }
        else{
            if (((ulCode & BIT8) == 0x0) && ((ulCode & BIT9) != 0x0)) {/* numeric/logical/compare */
                if ( ((ulCode & BIT10) != 0x0) && ((ulCode & BIT11) != 0x0) && ((ulCode & BIT12) != 0x0)) { /* not ? */
                    ulOp1 = (ulCode >> 0xa) & 0x7;                               
                    if ((ulOp1 != 0x3) && (ulOp1 != 0x7)) {                      
                        ucErrFlag = NO_ERR;
                        *iExtType = EXT3;
                        *iCalcType = CALC2;
                    }
                }else{
                    ucErrFlag = ERR2;   /* not show 3 operand. add tazaki 2001.11.06 */
                }
            }
        }
        break;
    
      case CLASS2:
        ucErrFlag = NO_ERR;
        *iExtType = EXT1;
        
        ulOp1 = (ulCode >> 0xa) & 0x7;                                  /* bit12:10 */
        if (((ulOp1 & 0x6) == 0x0) || (ulOp1 == 0x5)) {                 /* byte type */
          *iCalcType = CALC3_BYTE;
        }
        else if (((ulOp1 & 0x6) == 0x2) || (ulOp1 == 0x6)) {            /* half word type */
          *iCalcType = CALC3_HALF;
        }
        else {                                                          /* word type */
          *iCalcType = CALC3_WORD;
        }
        break;
      
      case CLASS3:
        ucErrFlag = NO_ERR;
        *iExtType = EXT1;
        
        ulOp1 = (ulCode >> 0xa) & 0x7;           /* bit12:10 */
        if ((ulOp1 & 0x6) == 0x0) {              /* OP1 = 00x, imm6 type */
          *iCalcType = CALC4;
        }
        else {                                   /* sign6 type */
          *iCalcType = CALC5;
        }
        break;
      
      case CLASS5:
        ulOp1 = (ulCode >> 0xa) & 0x7;           /* bit12:10 */
        ulOp2 = (ulCode >> 0x8) & 0x3;           /* bit9:8 */
        if ((ulOp1 == 0x2) || (ulOp1 == 0x3) || (ulOp1 == 0x4) || (ulOp1 == 0x5)) {
          if ((ulOp2 == 0x0) && ((ulCode & BIT3) == 0x0)) {
            ucErrFlag = NO_ERR;
            *iExtType = EXT2;
            *iCalcType = CALC2;
          }
        }
        break;

    /* >>> add tazaki 2001.09.19 */

      case CLASS7:
        ucErrFlag = NO_ERR;
        *iExtType = EXT1;
        
        ulOp1 = (ulCode >> 0xa) & 0x7;                                  /* bit12:10 */
        if (((ulOp1 & 0x6) == 0x0) || (ulOp1 == 0x5)) {                 /* byte type */
          *iCalcType = CALC3_BYTE;
        }
        else if (((ulOp1 & 0x6) == 0x2) || (ulOp1 == 0x6)) {            /* half word type */
          *iCalcType = CALC3_HALF;
        }
        else {                                                          /* word type */
          *iCalcType = CALC3_WORD;
        }
        break;
    
    /* >>> add tazaki 2001.09.19 */
    
    }
    
    return ucErrFlag;
}



static UINT32
fnCrCalcImmVal(nExtInst,iCalcType,ulExt1,ulExt2,ulCode)

    /***************************************************************
     *
     *      --- Calculate the extended immediate value ---
     *
     *      Calculate the extended immediate value according to the
     *      extention type and the immediate value calculation type.
     *
     *      RETURN: extended immediate value
     *
     ***************************************************************/

    int     nExtInst;          /* extention type, 1-3 */
    int     iCalcType;         /* immediate value calculation type, 1-5 */
    UINT32  ulExt1;            /* immediate value of ext1 */
    UINT32  ulExt2;            /* immediate value of ext2 */
    UINT32  ulCode;            /* target instruction code */

{
    UINT32  ulImm32;           /* extended immediate value */

    /* clear not ext-instruction code */
    if (nExtInst == 0) {
      ulExt1 = 0x0;
      ulExt2 = 0x0;
    }
    else if (nExtInst == 1) {
      ulExt1 = 0x0;
    }
    
    /* calculate immediate value according to iCalcType */
    switch (iCalcType) {
      case CALC1:                      /* ext1[bit12:3] + ext2[bit12:0] + IR[bit7:0]"0" */
        ulImm32 = ((ulExt1 & MASK12_3) << 19)
                     + ((ulExt2 & MASK12_0) << 9) + ((ulCode & MASK7_0) << 1);
        
        /* sign extention */
        if (nExtInst == 0) {
          if ((ulCode & BIT7) != 0x0) {
            ulImm32 |= MASK31_9;
          }
        }
        else if (nExtInst == 1) {
          if ((ulExt2 & BIT12) != 0x0) {
            ulImm32 |= MASK31_22;
          }
        }
        break;
      
      case CALC2:                      /* ext1[bit12:0] + ext2[bit12:0] */
        ulImm32 = ((ulExt1 & MASK12_0) << 13) + (ulExt2 & MASK12_0);
        break;
      
      case CALC3_BYTE:
      case CALC4:                      /* ext1[bit12:0] + ext2[bit12:0] + IR[bit9:4] */
        ulImm32 = ((ulExt1 & MASK12_0) << 19)
                     + ((ulExt2 & MASK12_0) << 6) + ((ulCode & MASK9_4) >> 4);
        break;
        
      case CALC3_HALF:
        if (nExtInst == 0) {           /* IR[bit9:4]"0" */
          ulImm32 = (ulCode & MASK9_4) >> 3;
          ulImm32 &= ~BIT0;
        }
        else {                         /* ext1[bit12:0] + ext2[bit12:0] + IR[bit9:5]"0" */
          ulImm32 =((ulExt1 & MASK12_0) << 19)
                     + ((ulExt2 & MASK12_0) << 6) + ((ulCode & MASK9_5) >> 4);
          ulImm32 &= ~BIT0;
        }
        break;
        
      case CALC3_WORD:
        if (nExtInst == 0) {           /* IR[bit9:4]"00" */
          ulImm32 = (ulCode & MASK9_4) >> 2;
          ulImm32 &= ~MASK1_0;
        }
        else {                         /* ext1[bit12:0] + ext2[bit12:0] + IR[bit9:6]"00" */
          ulImm32 =((ulExt1 & MASK12_0) << 19)
                     + ((ulExt2 & MASK12_0) << 6) + ((ulCode & MASK9_6) >> 4);
          ulImm32 &= ~MASK1_0;
        }
        break;
        
      case CALC5:                      /* ext1[bit12:0] + ext2[bit12:0] + IR[bit9:4] */
        ulImm32 = ((ulExt1 & MASK12_0) << 19)
                     + ((ulExt2 & MASK12_0) << 6) + ((ulCode & MASK9_4) >> 4);
        
        /* sign extention */
        if (nExtInst == 0) {
          if ((ulCode & BIT9) != 0x0) {
            ulImm32 |= MASK31_6;
          }
        }
        else if (nExtInst == 1) {
          if ((ulExt2 & BIT12) != 0x0) {
            ulImm32 |= MASK31_19;
          }
        }
        
        break;
        
      default:                         /* this may not be occured */
        ulImm32 = 0;
        break;
    } /* end of switch */
    
    return ulImm32;
}

/* >>>>> MODIFIED D.Fujimoto 2007/10/03 added params for displaying symbols */
//static UINT8
//fnCrGetXinst(ulCode,ulImmVal,iExtType,tdAddr,szXinst,iArraySize,szTmp)
static UINT8
fnCrGetXinst(ulCode,ulImmVal,iExtType,tdAddr,szXinst,iArraySize,szTmp, iExtCnt, info)
/* <<<<< MODIFIED D.Fujimoto 2007/10/03 added params for displaying symbols */

    /***************************************************************
     *
     *      --- Make output string ---
     *
     *      Get extended mnemonic code from instruction code.
     *      Replace extended immediate value with original immediate
     *      value or add extended immediate value according to the
     *      extention type.
     *      If instruction is one of branch instruction, destination
     *      address is added to the extended mnemonic code.
     *
     *      RETURN: NO_ERR  success
     *              ERR     cannot get correct mnemonic
     *
     ***************************************************************/

    UINT32  ulCode;            /* target instruction code */
    UINT32  ulImmVal;          /* extended immediate value */
    int     iExtType;          /* extention type, 1-3 */
    ADDR    tdAddr;            /* address of target instruction */
    INT8   *szXinst;           /* extended mnemonic code */
    int     iArraySize;        /* size of szXinst */
    INT8   *szTmp;           /* extended mnemonic code */
/* >>>>> ADDED D.Fujimoto 2007/10/03 added params for displaying symbols */
    int     iExtCnt;         /* size of szXinst */
    struct disassemble_info *info;
/* <<<<< ADDED D.Fujimoto 2007/10/03 added params for displaying symbols */
{
/*  INT8    szTmp[256];   */   /* temporal array */
/* >>>>> MOD D.Fujimoto 2007/10/03 increased buffer for long symbol names */
//    INT8    szImm32[32];       /* immediate value */
    INT8    szImm32[DUMP_OUTPUT_BUF_SIZE];       /* immediate value */
/* <<<<< MOD D.Fujimoto 2007/10/03 increased buffer for long symbol names */
    INT8   *pchCutText;        /* pointer to the character to be replaced */
    INT8   *pchNextToCutText;  /* pointer next to the character to be replaced */
    INT8   *pchMnemonic;       /* pointer to the extended mnemonic code */
    INT8   *pchCopyChar;       /* pointer to the original memonic code */
    UINT32  ulOp1;             /* OP code to check if branch instruction or not */
    UINT8   ucErrFlag = ERR;   /* error flag */
    int     iIsBranch = NO;    /* branch instruction check flag */
/* >>>>> ADDED D.Fujimoto 2007/10/03 added params for displaying symbols */
	char 	*name;				/* symbol name for display */
	char nameBuf[SYMBOL_NAME_BUF_SIZE];	// length should be under szImm32
	int iAbbrevStrLength;
	UINT32 sign8Val = 0;
	enum eOpcode {add, sub, cmp, ld, and, or, xor, not};
	enum eOpcode opc3 = ld;		// class3 opecode
/* <<<<< ADDED D.Fujimoto 2007/10/03 added params for displaying symbols */

/* >>>>> MOD D.Fujimoto 2007/10/03 increased buffer for long symbol names */
//  if( iArraySize > 256){
    if( iArraySize > DUMP_OUTPUT_BUF_SIZE){
        return ucErrFlag;
    }
/* <<<<< MOD D.Fujimoto 2007/10/03 increased buffer for long symbol names */
    
    /* get mnemonic code */
    vfnDisasm ((unsigned short)ulCode, szTmp);
    
    /* set the pointer, according to the extention type */
    switch(iExtType) {
      case EXT1: /* immediate value ---> extended immediate value */
        pchCutText = szTmp;
        
        /* search immediate value */
        while((pchCutText = strchr(pchCutText, '0')) != NULL) {
          if (strncmp(pchCutText, "0x", 2) == 0) {        /* immediate value starts with "0x" */
            pchNextToCutText = pchCutText + 2;
            while (isxdigit(*pchNextToCutText) != 0) {    /* search the end of immediate value */
              ++pchNextToCutText;
            }
            
            ucErrFlag = NO_ERR;
            break;
          }
          else {
            ++pchCutText;                                 /* feed for next search */
          }
        }
        break;
        
      case EXT2: /* add displace ment */
/* >>>>> MODIFIED D.Fujimoto 2007/10/10 display plain mnemonic */
//        pchNextToCutText = strchr(szTmp, ']');
//        if (pchNextToCutText != NULL) {
//          pchCutText = pchNextToCutText;
//          ucErrFlag = NO_ERR;
//        }

		if (iExtCnt != 0) {
	        pchNextToCutText = strchr(szTmp, ']');
	        if (pchNextToCutText != NULL) {
	          pchCutText = pchNextToCutText;
	          ucErrFlag = NO_ERR;
	        }
		} else {
			// do not add displacement, disasm shown in disassemble()
			ucErrFlag = QUIT;	
		}
/* <<<<< MODIFIED D.Fujimoto 2007/10/10 display plain mnemonic */

        break;
        
      case EXT3: /* three opeland */
        pchNextToCutText = strchr(szTmp, '\0');
        if (pchNextToCutText != NULL) {
          pchCutText = pchNextToCutText;
          ucErrFlag = NO_ERR;
        }
        break;
        
      default:
        break;
    }
    
    /* get extended mnemonic code */
    if (ucErrFlag == NO_ERR) {
/* >>>>> MODIFIED D.Fujimoto 2007/10/10 do not add 'x' when iExtCnt is 0 */
//      pchMnemonic = szXinst;
//      *pchMnemonic = 'x';
//      ++pchMnemonic;

		pchMnemonic = szXinst;
		if (iExtCnt != 0) {
			*pchMnemonic = 'x';
			++pchMnemonic;

      
	      /* special case */
	      if (strncmp(szTmp, "or", 2) == 0) {        /* or ---> xoor */
	        *pchMnemonic = 'o';
	        ++pchMnemonic;
	      }
		}
/* <<<<< MODIFIED D.Fujimoto 2007/10/10 do not add 'x' when iExtCnt is 0 */
      
      for (pchCopyChar = szTmp; pchCopyChar < pchCutText;) {
        *pchMnemonic = *pchCopyChar;
        ++pchMnemonic;
        ++pchCopyChar;
      }
      
      switch (iExtType) {
        case EXT1: /* immediate value ---> extended immediate value */
            ;                                    /* null */
          break;
        
        case EXT2: /* add displace ment */
          *pchMnemonic = '+';
          ++pchMnemonic;
          break;
        
        case EXT3: /* three opeland */
          *pchMnemonic = ',';
          ++pchMnemonic;
          break;
        
      }
      
      /* change immediate value */
      /* set branch instruction flag, in case of branch instruction */
      if ((ulCode & CLASS_MASK) == CLASS0) {     /* class0 */
        ulOp1 = (ulCode >> 0x9) & 0xf;           /* bit12:9 */
        if ((ulOp1 & 0xc) != 0x0) {              /* branch inst is not OP1 = 00XX, illeagal */
          iIsBranch = YES;
/* >>>>> ADDED D.Fujimoto 2007/10/04 display sign8 val for branches when ext count is 0 */
		  sign8Val = ulCode & 0xff;
/* <<<<< ADDED D.Fujimoto 2007/10/04 display sign8 val for branches when ext count is 0 */
        }
      }
      
      if (iIsBranch == YES) {
        tdAddr += ulImmVal;                      /* destination address */
/* >>>>> MODIFIED D.Fujimoto 2007/10/04 display sign8 val for branches when ext count is 0 */
//	        sprintf(szImm32, "0x%-8lx (0x%08lX)", ulImmVal, tdAddr);
		if (iExtCnt == 0) {
			// sign8
	        sprintf(szImm32, "0x%-8lx (0x%08lX)", sign8Val, tdAddr);
		} else {
	        sprintf(szImm32, "0x%-8lx (0x%08lX)", ulImmVal, tdAddr);
		}
/* <<<<< MODIFIED D.Fujimoto 2007/10/04 display sign8 val for branches when ext count is 0 */
        strcpy(pchMnemonic, szImm32);

/* >>>>> ADDED D.Fujimoto 2007/10/03 */
		// show symbol for branch
		name = find_symbolname_for_address((bfd_vma) tdAddr, info);
		if (name) {
			// avoid buffer overflow for szImm32
			iAbbrevStrLength = strlen("...");
			if (strlen(name) > SYMBOL_NAME_BUF_SIZE ) {								// length of " <>", "..." and '\0'
				strncpy(nameBuf, name, SYMBOL_NAME_BUF_SIZE);						// copy string
				strncpy(nameBuf + (SYMBOL_NAME_BUF_SIZE - iAbbrevStrLength - 1), "...", iAbbrevStrLength);	// replace 
				nameBuf[SYMBOL_NAME_BUF_SIZE - 1] = '\0';
			} else {
				strcpy(nameBuf, name);
			}
			sprintf(szImm32, " <%s>", nameBuf);
            strcat(pchMnemonic, szImm32);
		}

        strcat(pchMnemonic, pchNextToCutText);  /* maybe none */

/* <<<<< ADDED D.Fujimoto 2007/10/03 */

      } else {	// not branch
/* >>>>> ADDED D.Fujimoto 2007/10/09 */
		// determine class3 opecode
		if ((ulCode & CLASS_MASK) == CLASS3) {
			switch ((ulCode & 0xfc00)) {
				case 0x6000:
					opc3 = add;
					break;
				case 0x6400:
					opc3 = sub;
					break;
				case 0x6800:
					opc3 = cmp;
					break;
				case 0x6c00:
					opc3 = ld;
					break;
				case 0x7000:
					opc3 = and;
					break;
				case 0x7400:
					opc3 = or;
					break;
				case 0x7800:
					opc3 = xor;
					break;
				case 0x7c00:
					opc3 = not;
					break;
			}
		} else if (iExtCnt == 0) {
			// recalculate imm6 value
			if ((ulCode & CLASS_MASK) == CLASS2) {
				// ld.* %rd, [%sp+imm6] 
				// ld.* [%sp+imm6], %rs 
				ulImmVal = (ulCode & MASK9_4) >> 4;
			} else if ((ulCode & CLASS_MASK) == CLASS7) {
				// ld.* %rd, [%dp+imm6]
				// ld.* [%dp+imm6], %rs
				ulImmVal = (ulCode & MASK9_4) >> 4;
			}
		}

/* <<<<< ADDED D.Fujimoto 2007/10/09 */

        sprintf(szImm32, "0x%lx", ulImmVal);
        strcpy(pchMnemonic, szImm32);            /* change immediate value */
        strcat(pchMnemonic, pchNextToCutText);

/* >>>>> ADDED D.Fujimoto 2007/10/09 */
		// skip insns that do not take label operands
		if ((ulCode & CLASS_MASK) == CLASS3 && opc3 == ld) {
			// show symbol for normal insn
			name = find_symbolname_for_address((bfd_vma) ulImmVal, info);
			if (name) {
				// avoid buffer overflow for szImm32
				iAbbrevStrLength = strlen("...");
				if (strlen(name) > SYMBOL_NAME_BUF_SIZE ) {								// length of " <>", "..." and '\0'
					strncpy(nameBuf, name, SYMBOL_NAME_BUF_SIZE);						// copy string
					strncpy(nameBuf + (SYMBOL_NAME_BUF_SIZE - iAbbrevStrLength - 1), "...", iAbbrevStrLength);	// replace 
					nameBuf[SYMBOL_NAME_BUF_SIZE - 1] = '\0';
				} else {
					strcpy(nameBuf, name);
				}
				sprintf(szImm32, " <%s>", nameBuf);
				strcat(pchMnemonic, szImm32);
			}
		}
/* <<<<< ADDED D.Fujimoto 2007/10/09 */

      }
      strcat(pchMnemonic, "\n");
    }
    
    return ucErrFlag;
}

/************************************************
 *  vfnDisasm : execute dis-assemble    *
 ************************************************/

static void
vfnDisasm (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;

{
    unsigned short  uwClass;

    /*@ extract class */
    uwClass = uwCode & 0xe000;
    uwClass >>= 13;

    switch (uwClass) {      /* class */

    case 0:         /* class 0 */
    vfnDisasmClass0 (uwCode, pszBuf);
    break;

    case 1:         /* class 1 */
    vfnDisasmClass1 (uwCode, pszBuf);
    break;

    case 2:         /* class 2 */
    vfnDisasmClass2 (uwCode, pszBuf);
    break;

    case 3:         /* class 3 */
    vfnDisasmClass3 (uwCode, pszBuf);
    break;

    case 4:         /* class 4 */
    vfnDisasmClass4 (uwCode, pszBuf);
    break;

    case 5:         /* class 5 */
    vfnDisasmClass5 (uwCode, pszBuf);
    break;

    case 6:         /* class 6 */
    vfnDisasmClass6 (uwCode, pszBuf);
    break;

    case 7:         /* class 7 */
    vfnDisasmClass7 (uwCode, pszBuf);
    break;
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass0 : execute dis-assemble class0       *
 ****************************************************************/

static void
vfnDisasmClass0 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;

{
    unsigned short  uwOp1, uwDelay, uwOp2;
    unsigned short  uwBit0_3, uwBit4_5, uwBit0_7;
    char            szTmpBuf[10];
    char           *spSpecialReg[16];
    int             i;
    int             iLoop;

    /*@ initialize */   
    spSpecialReg[0]  = "%psr";
    spSpecialReg[1]  = "%sp";
    spSpecialReg[2]  = "%alr";
    spSpecialReg[3]  = "%ahr";
    spSpecialReg[4]  = "%lco";
    spSpecialReg[5]  = "%lsa";
    spSpecialReg[6]  = "%lea";
    spSpecialReg[7]  = "%sor";
    spSpecialReg[8]  = "%ttbr";
    spSpecialReg[9]  = "%dp";
    spSpecialReg[10] = "%idir";		/* "" -->"%idir"      T.Tazaki 20003/11/18 */
    spSpecialReg[11] = "%dbbr"; 	/* "" -->"%dbbr"      T.Tazaki 20003/11/18 */
    spSpecialReg[12] = "";
    spSpecialReg[13] = "%usp";
    spSpecialReg[14] = "%ssp";
    spSpecialReg[15] = "%pc";

    /*@ extract op1 */
    uwOp1 = uwCode & 0x1e00;
    uwOp1 >>= 9;

    /*@ extract "d" */
    uwDelay = uwCode & 0x0100;
    uwDelay >>= 8;

    if (uwOp1 < 4) {
        /*@ Yes op1 : 0000 - 0011 */

        /*@ extract op2 from code */
        uwOp2 = uwCode & 0x00c0;
        uwOp2 >>= 6;
        /*@ extract bit4_5 from code */
        uwBit4_5 = uwCode & 0x0030;
        uwBit4_5 >>= 4;
        /*@ extract bit0_3 from code */
        uwBit0_3 = uwCode & 0x000f;

        /*@ check if code is OK */
        if (uwBit4_5 == 0) {
            switch (uwOp1 << 2 | uwOp2) {   /* instruction */

            case 0:     /* 0000:00 nop */

                /*@ check if code is OK */
                if ((uwDelay != 0) || (uwBit0_3 != 0)) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "nop");
                }
                break;

            case 1:     /* 0000:01 slp */

                /*@ check if code is OK */
                if ((uwDelay != 0) || (uwBit0_3 != 0)) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "slp");
                }
                break;

            case 2:     /* 0000:10 halt */

                /*@ check if code is OK */
                if ((uwDelay != 0) || (uwBit0_3 != 0)) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "halt");
                }
                break;

            case 3:     /* 0000:11 reserved */

                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;

            case 4:     /* 0001:00 pushn rs */

                /*@ check if code is OK */
                if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "pushn    %%r%d", uwBit0_3);
                }
                break;

            case 5:     /* 0001:01 popn %rd */

                /*@ check if code is OK */
                if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "popn     %%r%d", uwBit0_3);
                }
                break;

            case 6:     /* 0001:10 reserved */
                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;
     
            case 7:     /* 0001:11 jpr %rb */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "jpr.d    %%r%d", uwBit0_3);
                }
                else {
                    (void) sprintf (pszBuf, "jpr      %%r%d", uwBit0_3);
                }
                break;

            case 8:     /* 0010:00 brk */

                /*@ check if code is OK */
                if (uwBit0_3 == 0) {
                    if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                    }
                    else {
                    (void) sprintf (pszBuf, "brk");
                    }
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 9:     /* 0010:01 retd */

                /*@ check if code is OK */
                if (uwBit0_3 == 0) {
                    /*@ check if code is OK */
                    if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                    }
                    else {
                    (void) sprintf (pszBuf, "retd");
                    }
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 10:        /* 0010:10 int imm2 */

                /*@ check if code is OK */
                if ((uwBit0_3 & 0xc) == 0) {
                    /*@ check if code is OK */
                    if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                    }
                    else {
                    (void) sprintf (pszBuf, "int      0x%x", uwBit0_3);
                    }
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 11:        /* 0010:11 reti */

                /*@ check if code is OK */
                if (uwBit0_3 == 0) {
                    /*@ check if code is OK */
                    if (uwDelay != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                    }
                    else {
                    (void) sprintf (pszBuf, "reti");
                    }
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 12:        /* 0011:00 call rb */

                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "call.d   %%r%d", uwBit0_3);
                }
                else {
                    (void) sprintf (pszBuf, "call     %%r%d", uwBit0_3);
                }
                break;

            case 13:        /* 0011:01 ret */

                /*@ check if code is OK */
                if (uwBit0_3 == 0) {
                    /*@ check if code is OK */
                    if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "ret.d");
                    }
                    else {
                    (void) sprintf (pszBuf, "ret");
                    }
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 14:        /* 0011:10 jp rb */

                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "jp.d     %%r%d", uwBit0_3);
                }
                else {
                    (void) sprintf (pszBuf, "jp       %%r%d", uwBit0_3);
                }
                break;

            case 15:        /* 0011:11 reserved */

                if (uwDelay == 0) {
                    (void) sprintf (pszBuf, "retm");    /* 2002/10/01 */
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;
            }
        }
        else if (uwBit4_5 == 1) 
        {
            switch (uwOp1 << 2 | uwOp2) {   /* instruction */

            case 0:     /* 0000:00 push %rs , d=1 mac.w %rs*/

                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "mac.w    %%r%d", uwBit0_3);
                }
                else {
                    (void) sprintf (pszBuf, "push     %%r%d", uwBit0_3);
                }
                break;
            
            case 1:     /* 0000:01 pop %rd , d=1 mac.hw %rs*/

                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "mac.hw   %%r%d", uwBit0_3);
                }
                else {
                    (void) sprintf (pszBuf, "pop      %%r%d", uwBit0_3);
                }
                break;
            
            case 2:     /* 0000:10 pushs %ss , d=1 macclr */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "macclr");
                }else{
//                  if (((uwBit0_3 >= 0) && (uwBit0_3 <= 9)) || ((uwBit0_3 >= 13) && (uwBit0_3 <= 15))){	/* change T.Tazaki 2003/11/18 */
                    if (((uwBit0_3 >= 0) && (uwBit0_3 <= 11)) || ((uwBit0_3 >= 13) && (uwBit0_3 <= 15))){
                        (void) sprintf (pszBuf, "pushs    %s", spSpecialReg[uwBit0_3]);
                    }
                    else {
                        (void) sprintf (pszBuf, "***");
                    }
                }
                break;
            
            case 3:     /* 0000:11 pops %sd , d=1 ld.cf */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "ld.cf");
                }else{
//                  if (((uwBit0_3 >= 0) && (uwBit0_3 <= 9)) || ((uwBit0_3 >= 13) && (uwBit0_3 <= 15))){	/* change T.Tazaki 2003/11/18 */
                    if (((uwBit0_3 >= 0) && (uwBit0_3 <= 11)) || ((uwBit0_3 >= 13) && (uwBit0_3 <= 15))){
                        (void) sprintf (pszBuf, "pops     %s", spSpecialReg[uwBit0_3]);
                    }
                    else {
                        (void) sprintf (pszBuf, "***");
                    }
                }
                break;
            
            case 4:     /* 0001:00 divu.w %rb */
                (void) sprintf (pszBuf, "divu.w   %%r%d", uwBit0_3);
                break;

            case 5:     /* 0001:01 div.w %rb , d=1 add &rd,%dp */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "add      %%r%d,%%dp", uwBit0_3);
                }else {
                    (void) sprintf (pszBuf, "div.w    %%r%d", uwBit0_3);
                }
                break;

            case 6:    /* 0001:10 repeat %rb */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "***");
                }else {
                    (void) sprintf (pszBuf, "repeat   %%r%d", uwBit0_3);
                }
                break;

            case 7:    /* 0001:11 repeat imm4 */
                if (uwDelay != 0) {
                    (void) sprintf (pszBuf, "***");
                }else {
                    (void) sprintf (pszBuf, "repeat   0x%x", uwBit0_3);
                }
                break;

            default:
                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;
            }
        }else{
            /*@ error */
            (void) sprintf (pszBuf, "***");
        }
    }
    else {
        /*@ No  op1 : 0100 - 1111 */

        /*@ extract bit0_7 from code */
        uwBit0_7 = uwCode & 0x00ff;

        switch (uwOp1) {    /* instruction */

        case 4:     /* 0100 jrgt sign9 */
            (void) sprintf (pszBuf, "jrgt");
            break;

        case 5:     /* 0101 jrge sign9 */

            (void) sprintf (pszBuf, "jrge");
            break;

        case 6:     /* 0110 jrlt sign9 */

            (void) sprintf (pszBuf, "jrlt");
            break;

        case 7:     /* 0111 jrle sign9 */

            (void) sprintf (pszBuf, "jrle");
            break;

        case 8:     /* 1000 jrugt sign9 */

            (void) sprintf (pszBuf, "jrugt");
            break;

        case 9:     /* 1001 jruge sign9 */

            (void) sprintf (pszBuf, "jruge");
            break;

        case 10:        /* 1010 jrult sign9 */

            (void) sprintf (pszBuf, "jrult");
            break;

        case 11:        /* 1011 jrule sign9 */

            (void) sprintf (pszBuf, "jrule");
            break;

        case 12:        /* 1100 jreq sign9 */

            (void) sprintf (pszBuf, "jreq");
            break;

        case 13:        /* 1101 jrne sign9 */

            (void) sprintf (pszBuf, "jrne");
            break;

        case 14:        /* 1110 call sign9 */

            (void) sprintf (pszBuf, "call");
            break;

        case 15:        /* 1111 jp sign9 */

            (void) sprintf (pszBuf, "jp");
            break;
        }
        /*@ check if "d" bit exist */
        if (uwDelay != 0) {
            /*@ add ".d" to mnemonic */
            (void) strcat (pszBuf, ".d");
        }
        iLoop = 9 - strlen( pszBuf );
        for( i = 0; i < iLoop; ++i ){
            (void) strcat (pszBuf, " ");
        }

        /*@ format mnemonic */
        /*    pszBuf[8] = 0;    */

        /*@ convert immediate value to string */
        (void) sprintf (&szTmpBuf[0], "0x%x", uwBit0_7);

        /*@ concatenate mnemonic and string */
        (void) strcat (pszBuf, &szTmpBuf[0]);
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass1 : execute dis-assemble class1       *
 ****************************************************************/


static void
vfnDisasmClass1 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1, uwOp2;
    unsigned short  uwBit0_3, uwBit4_7, uwBit0_1, uwBit2_3;

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract op2 from code */
    uwOp2 = uwCode & 0x0300;    /* uwCode[9:8] */
    uwOp2 >>= 8;
    /*@ extract bit4_7 from code */
    uwBit4_7 = uwCode & 0x00f0;
    uwBit4_7 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;

    uwBit2_3 = uwBit0_3 & 0x000c;
    uwBit2_3 >>= 2;
    uwBit0_1 = uwBit0_3 & 0x0003;

    switch (uwOp2) {        /* op2 */

    case 0:         /* 00 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:00 ld.b rd,[rb] */

                (void) sprintf (pszBuf, "ld.b     %%r%d,[%%r%d]", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:00 ld.ub rd,[rb] */

                (void) sprintf (pszBuf, "ld.ub    %%r%d,[%%r%d]", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:00 ld.h rd,[rb] */

                (void) sprintf (pszBuf, "ld.h     %%r%d,[%%r%d]", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:00 ld.uh rd,[rb] */

                (void) sprintf (pszBuf, "ld.uh    %%r%d,[%%r%d]", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:00 ld.w rd,[rb] */

                (void) sprintf (pszBuf, "ld.w     %%r%d,[%%r%d]", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:00 ld.b [rb],rs */

                (void) sprintf (pszBuf, "ld.b     [%%r%d],%%r%d", uwBit4_7, uwBit0_3);
                break;

            case 6:     /* 110:00 ld.h [rb],rs */

                (void) sprintf (pszBuf, "ld.h     [%%r%d],%%r%d", uwBit4_7, uwBit0_3);
                break;

            case 7:     /* 111:00 ld.w [rb],rs */

                (void) sprintf (pszBuf, "ld.w     [%%r%d],%%r%d", uwBit4_7, uwBit0_3);
                break;
            }

            break;

    case 1:         /* 01 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:01 ld.b  rd,[rb]+ */

                (void) sprintf (pszBuf, "ld.b     %%r%d,[%%r%d]+", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:01 ld.ub rd,[rb]+ */

                (void) sprintf (pszBuf, "ld.ub    %%r%d,[%%r%d]+", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:01 ld.h  rd,[rb]+ */

                (void) sprintf (pszBuf, "ld.h     %%r%d,[%%r%d]+", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:01 ld.uh rd,[rb]+ */

                (void) sprintf (pszBuf, "ld.uh    %%r%d,[%%r%d]+", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:01 ld.w  rd,[rb]+ */

                (void) sprintf (pszBuf, "ld.w     %%r%d,[%%r%d]+", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:01 ld.b  [rb]+,rs */

                (void) sprintf (pszBuf, "ld.b     [%%r%d]+,%%r%d", uwBit4_7, uwBit0_3);
                break;

            case 6:     /* 110:01 ld.h  [rb]+,rs */

                (void) sprintf (pszBuf, "ld.h     [%%r%d]+,%%r%d", uwBit4_7, uwBit0_3);
                break;

            case 7:     /* 111:01 ld.w  [rb]+,rs */

                (void) sprintf (pszBuf, "ld.w     [%%r%d]+,%%r%d", uwBit4_7, uwBit0_3);
                break;
            }

            break;

    case 2:         /* 10 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:10 add   rd,rs */

                (void) sprintf (pszBuf, "add      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:10 sub   rd,rs */

                (void) sprintf (pszBuf, "sub      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:10 cmp   rd,rs */

                (void) sprintf (pszBuf, "cmp      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:10 ld.w  rd,rs */

                (void) sprintf (pszBuf, "ld.w     %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:10 and   rd,rs */

                (void) sprintf (pszBuf, "and      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:10 or    rd,rs */

                (void) sprintf (pszBuf, "or       %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 6:     /* 110:10 xor   rd,rs */

                (void) sprintf (pszBuf, "xor      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 7:     /* 111:10 not   rd,rs */

                (void) sprintf (pszBuf, "not      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;
            }

            break;

    case 3:         /* 11 */
    
            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:11 srl   rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "srl      %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:11 sll   rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "sll      %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:11 sra   rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "sra      %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:11 sla  rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "sla      %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:11 rr   rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "rr       %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:11 rl   rd,imm4 */
            
                /* class4-->class1 convertCadd 16 */
                uwBit4_7 += 16;

                (void) sprintf (pszBuf, "rl       %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 6:

                if( uwBit4_7 == 0 ) {
                    
                    /* ext  OP,imm2 */
                    if( uwBit2_3 > 0 ) {
                        (void) sprintf (pszBuf, "ext      %s,0x%x", szOpShift[ uwBit2_3 - 1 ], uwBit0_1);
                    }
                }else{
                    /* ext Cond */
                    if( uwBit4_7 >= 4 && uwBit4_7 <= 0x0d ) {
                        (void) sprintf (pszBuf, "ext      %s",szCond[ uwBit4_7 - 4 ] );
                    }
                }
                break;

            case 7:

                if( uwBit0_3 == 0 ) {
                    
                    /* ext  %rs */
                    (void) sprintf (pszBuf, "ext      %%r%d", uwBit4_7);
                }else{
                    /* ext %rs,OP,imm2 */
                    if( uwBit2_3 > 0 ) {
                        (void) sprintf (pszBuf, "ext      %%r%d,%s,0x%x", 
                                            uwBit4_7,szOpShift[ uwBit2_3 - 1 ], uwBit0_1);
                    }
                }

                break;
            }

            break;
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass2 : execute dis-assemble class2       *
 ****************************************************************/

static void
vfnDisasmClass2 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1;
    unsigned short  uwBit0_3, uwBit4_9;

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract bit4_9 from code */
    uwBit4_9 = uwCode & 0x03f0;
    uwBit4_9 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;

    switch (uwOp1) {        /* op1 */

    case 0:         /* 000 ld.b rd,[sp+imm6] */

    (void) sprintf (pszBuf, "ld.b     %%r%d,[%%sp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 1:         /* 001 ld.ub rd,[sp+imm6] */

    (void) sprintf (pszBuf, "ld.ub    %%r%d,[%%sp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 2:         /* 010 ld.h rd,[sp+imm7] */

    (void) sprintf (pszBuf, "ld.h     %%r%d,[%%sp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 3:         /* 011 ld.uh rd,[sp+imm7] */

    (void) sprintf (pszBuf, "ld.uh    %%r%d,[%%sp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 4:         /* 100 ld.w rd,[sp+imm8] */

    (void) sprintf (pszBuf, "ld.w     %%r%d,[%%sp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 5:         /* 101 ld.b [sp+imm6],rs */

    (void) sprintf (pszBuf, "ld.b     [%%sp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;

    case 6:         /* 110 ld.h [sp+imm7],rs */

    (void) sprintf (pszBuf, "ld.h     [%%sp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;

    case 7:         /* 111 ld.w [sp+imm8],rs */

    (void) sprintf (pszBuf, "ld.w     [%%sp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass3 : execute dis-assemble class3       *
 ****************************************************************/


static void
vfnDisasmClass3 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1;
    unsigned short  uwBit0_3, uwBit4_9;
    unsigned long   ulSign32;

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract bit4_9 from code */
    uwBit4_9 = uwCode & 0x03f0;
    uwBit4_9 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;

    switch (uwOp1) {        /* op1 */

    case 0:         /* 000 add rd,imm6 */

    (void) sprintf (pszBuf, "add      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    break;

    case 1:         /* 001 sub rd,imm6 */

    (void) sprintf (pszBuf, "sub      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    break;

    case 2:         /* 010 cmp rd,sign6 */

/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){      
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "cmp      %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xcmp     %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "cmp      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */
/*      (void) sprintf (pszBuf, "cmp\tr%d,0x%x", uwBit0_3, uwBit4_9);   del tazaki 2001.10.10 */
    break;

    case 3:         /* 011 ld.w rd,sign6 */

/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "ld.w     %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xld.w    %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "ld.w     %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */
/*     (void) sprintf (pszBuf, "ld.w\tr%d,0x%x", uwBit0_3, uwBit4_9); del tazaki 2001.10.10 */
    break;

    case 4:         /* 100 and rd,sign6 */
/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "and      %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xand     %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "and      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */

/*    (void) sprintf (pszBuf, "and\tr%d,0x%x", uwBit0_3, uwBit4_9); del tazaki 2001.10.10 */
    break;

    case 5:         /* 101 or rd,sign6 */

/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "or       %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xoor     %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "or       %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */
/*    (void) sprintf (pszBuf, "or\tr%d,0x%x", uwBit0_3, uwBit4_9); del tazaki 2001.10.10 */
    break;

    case 6:         /* 110 xor rd,sign6 */

/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "xor      %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xxor     %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "xor      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */
/*    (void) sprintf (pszBuf, "xor\tr%d,0x%x", uwBit0_3, uwBit4_9);  del tazaki 2001.10.10 */
    break;

    case 7:         /* 111 not rd,sign6 */

/* >>>>> add tazaki 2001.10.10 */
    if( uwBit4_9 & 0x20 ){
        if( g_iExtCnt > 0 ){
            (void) sprintf (pszBuf, "not      %%r%d,0x%x", uwBit0_3, uwBit4_9);
        }else{
            ulSign32 = 0xffffffc0 + uwBit4_9;   /* Mark extension */
            (void) sprintf (pszBuf, "xnot     %%r%d,0x%x", uwBit0_3, ulSign32);
        }
    }else{
        (void) sprintf (pszBuf, "not      %%r%d,0x%x", uwBit0_3, uwBit4_9);
    }
/* <<<<< add tazaki 2001.10.10 */
/*    (void) sprintf (pszBuf, "not\tr%d,0x%x", uwBit0_3, uwBit4_9);  del tazaki 2001.10.10 */
    break;
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass4 : execute dis-assemble class4       *
 ****************************************************************/

static void
vfnDisasmClass4 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1, uwOp2;
    unsigned short  uwBit0_3, uwBit4_7, uwBit0_9;

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract op2 from code */
    uwOp2 = uwCode & 0x0300;
    uwOp2 >>= 8;
    /*@ extract bit0_9 from code */
    uwBit0_9 = uwCode & 0x03ff;
    /*@ extract bit4_7 from code */
    uwBit4_7 = uwCode & 0x00f0;
    uwBit4_7 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;

    if (uwOp1 == 0) {
    /* 000 add sp,imm12 */
    (void) sprintf (pszBuf, "add      %%sp,0x%x", uwBit0_9);
    }
    else if (uwOp1 == 1) {
    /* 001 sub sp,imm12 */
    (void) sprintf (pszBuf, "sub      %%sp,0x%x", uwBit0_9);
    }
    else {
    /*@ op1 : 010 - 111 */

    switch (uwOp2) {    /* op2 */

    case 0:     /* op2 = 00 */

        if( g_iShiftFlag == 1 ){
            uwBit4_7 += 16;
        }

        switch (uwOp1) {    /* op1 */

        case 2:     /* 010 srl rd,imm4 */

        (void) sprintf (pszBuf, "srl      %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;

        case 3:     /* 011 sll rd,imm4 */

        (void) sprintf (pszBuf, "sll      %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;

        case 4:     /* 100 sra rd,imm4 */

        (void) sprintf (pszBuf, "sra      %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;

        case 5:     /* 101 sla rd,imm4 */

        (void) sprintf (pszBuf, "sla      %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;

        case 6:     /* 110 rr rd,imm4 */

        (void) sprintf (pszBuf, "rr       %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;

        case 7:     /* 111 rl rd,imm4 */

        (void) sprintf (pszBuf, "rl       %%r%d,0x%x", uwBit0_3, uwBit4_7);
        break;
        }

        break;

    case 1:     /* op2 = 01 */

        switch (uwOp1) {    /* op1 */

        case 2:     /* 010 srl rd,rs */

        (void) sprintf (pszBuf, "srl      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 3:     /* 011 sll rd,rs */

        (void) sprintf (pszBuf, "sll      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 4:     /* 100 sra rd,rs */

        (void) sprintf (pszBuf, "sra      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 5:     /* 101 sla rd,rs */

        (void) sprintf (pszBuf, "sla      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 6:     /* 110 rr rd,rs */

        (void) sprintf (pszBuf, "rr       %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 7:     /* 111 rl rd,rs */

        (void) sprintf (pszBuf, "rl       %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;
        }

        break;

    case 2:     /* op2 = 10 */

        switch (uwOp1) {    /* op1 */

        case 2:     /* scan0 rd,rs */

        (void) sprintf (pszBuf, "scan0    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 3:     /* scan1 rd,rs */

        (void) sprintf (pszBuf, "scan1    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 4:     /* swap rd,rs */

        (void) sprintf (pszBuf, "swap     %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 5:     /* mirror rd,rs */

        (void) sprintf (pszBuf, "mirror   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 6:     /* swaph rd,rs */

        (void) sprintf (pszBuf, "swaph    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        case 7:     /* sat.b rd,rs */

        (void) sprintf (pszBuf, "sat.b    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;

        /*@ error */
        (void) sprintf (pszBuf, "***");
        break;
        }

        break;

    case 3:     /* op2 = 11 */

        switch (uwOp1) {    /* op1 */

        case 2:     /* div0s rs */

        (void) sprintf (pszBuf, "div0s    %%r%d", uwBit4_7);
        break;

        case 3:     /* div0u rs */

        (void) sprintf (pszBuf, "div0u    %%r%d", uwBit4_7);
        break;

        case 4:     /* div1 rs */

        (void) sprintf (pszBuf, "div1     %%r%d", uwBit4_7);
        break;

        case 5:     /* div2s rs */

        (void) sprintf (pszBuf, "div2s    %%r%d", uwBit4_7);
        break;

        case 6:     /* div3s */

        (void) sprintf (pszBuf, "div3s");
        break;

        case 7:     /* sat.ub rd,rs */

        (void) sprintf (pszBuf, "sat.ub   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
        break;
        }

        break;
    }
    }

    return;
}

/****************************************************************
 *  vfnDisasmClass5 : execute dis-assemble class5       *
 ****************************************************************/

static void
vfnDisasmClass5 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1, uwOp2, uwOp3;
    unsigned short  uwBit0_3, uwBit4_7, uwBit0_5;
    char           *spSpecialReg[16];

    /*@ initialize */   
    spSpecialReg[0]  = "%psr";
    spSpecialReg[1]  = "%sp";
    spSpecialReg[2]  = "%alr";
    spSpecialReg[3]  = "%ahr";
    spSpecialReg[4]  = "%lco";
    spSpecialReg[5]  = "%lsa";
    spSpecialReg[6]  = "%lea";
    spSpecialReg[7]  = "%sor";
    spSpecialReg[8]  = "%ttbr";
    spSpecialReg[9]  = "%dp";
    spSpecialReg[10] = "%idir";		/* "" -->"%idir"      T.Tazaki 20003/11/18 */
    spSpecialReg[11] = "%dbbr";		/* "" -->"%dbbr"      T.Tazaki 20003/11/18 */
    spSpecialReg[12] = "";
    spSpecialReg[13] = "%usp";
    spSpecialReg[14] = "%ssp";
    spSpecialReg[15] = "%pc";

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract op2 from code */
    uwOp2 = uwCode & 0x0300;
    uwOp2 >>= 8;
    /*@ extract op3 from code */
    uwOp3 = uwCode & 0x00c0;
    uwOp3 >>= 6;
    /*@ extract bit4_7 from code */
    uwBit4_7 = uwCode & 0x00f0;
    uwBit4_7 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;
    /*@ extract bit0_5 from code */
    uwBit0_5 = uwCode & 0x003f;

    switch (uwOp2) {        /* op2 */

    case 0:         /* 00 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:00 ld.w %sd,rs */

                /*@ check if code is OK */
                if (((uwBit0_3 >= 0) && (uwBit0_3 <= 9)) || ((uwBit0_3 >= 13) && (uwBit0_3 <= 15))){
                    (void) sprintf (pszBuf, "ld.w     %s,%%r%d", spSpecialReg[uwBit0_3], uwBit4_7);
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 1:     /* 001:00 ld.w rd,%ss */

                /*@ check if code is OK */
//              if (((uwBit4_7 >= 0) && (uwBit4_7 <= 9)) || ((uwBit4_7 >= 13) && (uwBit4_7 <= 15))){	/* change T.Tazaki 2003/11/18 */
                if (((uwBit4_7 >= 0) && (uwBit4_7 <= 11)) || ((uwBit4_7 >= 13) && (uwBit4_7 <= 15))){
                    (void) sprintf (pszBuf, "ld.w     %%r%d,%s", uwBit0_3, spSpecialReg[uwBit4_7]);
                }
                else {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                break;

            case 2:     /* 010:00 btst [rb],imm3 */

                /*@ check if code is OK */
                if ((uwBit0_3 & 0x8) != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "btst     [%%r%d],0x%d", uwBit4_7, uwBit0_3);
                }
                break;

            case 3:     /* 011:00 bclr [rb],imm3 */

                /*@ check if code is OK */
                if ((uwBit0_3 & 0x8) != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "bclr     [%%r%d],0x%d", uwBit4_7, uwBit0_3);
                }
                break;

            case 4:     /* 100:00 bset [rb],imm3 */

                /*@ check if code is OK */
                if ((uwBit0_3 & 0x8) != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "bset     [%%r%d],0x%d", uwBit4_7, uwBit0_3);
                }
                break;

            case 5:     /* 101:00 bnot [rb],imm3 */

                /*@ check if code is OK */
                if ((uwBit0_3 & 0x8) != 0) {
                    /*@ error */
                    (void) sprintf (pszBuf, "***");
                }
                else {
                    (void) sprintf (pszBuf, "bnot     [%%r%d],0x%d", uwBit4_7, uwBit0_3);
                }
                break;

            case 6:     /* 110:00 adc  rd,rs */

                (void) sprintf (pszBuf, "adc      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 7:     /* 111:00 sbc  rd,rs */

                (void) sprintf (pszBuf, "sbc      %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;
            }

            break;

    case 1:         /* 01 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:01 ld.b  rd,rs */

                (void) sprintf (pszBuf, "ld.b     %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:01 ld.ub  rd,rs */

                (void) sprintf (pszBuf, "ld.ub    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:01 ld.h  rd,rs */

                (void) sprintf (pszBuf, "ld.h     %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:01 ld.uh rd,rs */

                (void) sprintf (pszBuf, "ld.uh    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:01 ld.c rd,imm4 */

                (void) sprintf (pszBuf, "ld.c     %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:01 ld.c imm4,rs */

                (void) sprintf (pszBuf, "ld.c     0x%x,%%r%d", uwBit4_7, uwBit0_3);
                break;

            case 6:     /* 110:01 loop rc,ra */

                (void) sprintf (pszBuf, "loop     %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 7:     /* 111:01 sat.w rd,rs */

                (void) sprintf (pszBuf, "sat.w    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            default:
                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;
            }

            break;

    case 2:         /* 10 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:10 mlt.h  rd,rs */

                (void) sprintf (pszBuf, "mlt.h    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:10 mltu.h  rd,rs */

                (void) sprintf (pszBuf, "mltu.h   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:10 mlt.w  rd,rs */

                (void) sprintf (pszBuf, "mlt.w    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 3:     /* 011:10 mltu.w rd,rs */

                (void) sprintf (pszBuf, "mltu.w   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:10 mac rs */

                (void) sprintf (pszBuf, "mac      %%r%d", uwBit4_7);
                break;

            case 5:     /* 101:10 sat.h rd,rs */

                (void) sprintf (pszBuf, "sat.h    %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 6:     /* 110:10 loop rc,imm4 */

                (void) sprintf (pszBuf, "loop     %%r%d,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 7:     /* 111:10 sat.uw rd,rs */

                (void) sprintf (pszBuf, "sat.uw   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            default:
            
                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;
            }

            break;

    case 3:         /* 11 */

            switch (uwOp1) {    /* op1 */

            case 0:     /* 000:11 mlt.hw  rd,rs */

                (void) sprintf (pszBuf, "mlt.hw   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 1:     /* 001:11 mac1.h  rd,rs */

                (void) sprintf (pszBuf, "mac1.h   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 2:     /* 010:11 mac1.hw  rd,rs */

                (void) sprintf (pszBuf, "mac1.hw  %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 4:     /* 100:11 mac1.w  rd,rs */

                (void) sprintf (pszBuf, "mac1.w   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 5:     /* 101:11 sat.uh  rd,rs */

                (void) sprintf (pszBuf, "sat.uh   %%r%d,%%r%d", uwBit0_3, uwBit4_7);
                break;

            case 6:     /* 110:11 loop  imm4,imm4 */

                (void) sprintf (pszBuf, "loop     0x%x,0x%x", uwBit0_3, uwBit4_7);
                break;

            case 7:     /* 111:11 */

                switch( uwOp3 ) {   /* op3 */

                case 0: /* 111:11:00    do.c imm6 */
                        (void) sprintf (pszBuf, "do.c     0x%x", uwBit0_5);
                        break;
                case 1: /* 111:11:01    psrset imm5 */
                        (void) sprintf (pszBuf, "psrset   0x%x", uwBit0_5);
                        break;
                case 2: /* 111:11:10    psrclr imm5 */
                        (void) sprintf (pszBuf, "psrclr   0x%x", uwBit0_5);
                        break;
                default:
                        /*@ error */
                        (void) sprintf (pszBuf, "***");
                        break;
                }
                break;

            default:
                /*@ error */
                (void) sprintf (pszBuf, "***");
                break;
            }
            
            break;
    }
}

/****************************************************************
 *  vfnDisasmClass6 : execute dis-assemble class6       *
 ****************************************************************/

static void
vfnDisasmClass6 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwBit0_12;

    /*@ extract bit0_12 from code */
    uwBit0_12 = uwCode & 0x1fff;

    (void) sprintf (pszBuf, "ext      0x%x", uwBit0_12);

    return;
}

/****************************************************************
 *  vfnDisasmClass7 : execute dis-assemble class7       *
 ****************************************************************/

static void
vfnDisasmClass7 (uwCode, pszBuf)
     unsigned short  uwCode;    /* 16bit code */
     char           *pszBuf;


{
    unsigned short  uwOp1;
    unsigned short  uwBit0_3, uwBit4_9;

    /*@ extract op1 from code */
    uwOp1 = uwCode & 0x1c00;
    uwOp1 >>= 10;
    /*@ extract bit4_9 from code */
    uwBit4_9 = uwCode & 0x03f0;
    uwBit4_9 >>= 4;
    /*@ extract bit0_3 from code */
    uwBit0_3 = uwCode & 0x000f;

    switch (uwOp1) {        /* op1 */

    case 0:         /* 000 ld.b rd,[dp+imm6] */

    (void) sprintf (pszBuf, "ld.b     %%r%d,[%%dp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 1:         /* 001 ld.ub rd,[dp+imm6] */

    (void) sprintf (pszBuf, "ld.ub    %%r%d,[%%dp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 2:         /* 010 ld.h rd,[dp+imm6] */

    (void) sprintf (pszBuf, "ld.h     %%r%d,[%%dp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 3:         /* 011 ld.uh rd,[dp+imm6] */

    (void) sprintf (pszBuf, "ld.uh    %%r%d,[%%dp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 4:         /* 100 ld.w rd,[dp+imm6] */

    (void) sprintf (pszBuf, "ld.w     %%r%d,[%%dp+0x%x]", uwBit0_3, uwBit4_9);
    break;

    case 5:         /* 101 ld.b [dp+imm6],rs */

    (void) sprintf (pszBuf, "ld.b     [%%dp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;

    case 6:         /* 110 ld.h [dp+imm6],rs */

    (void) sprintf (pszBuf, "ld.h     [%%dp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;

    case 7:         /* 111 ld.w [dp+imm6],rs */

    (void) sprintf (pszBuf, "ld.w     [%%dp+0x%x],%%r%d", uwBit4_9, uwBit0_3);
    break;
    }

    return;
}

/* <<<<<<<<<<<<<<< add tazaki 2001.07.31 */


int 
print_insn_c33 (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info * info;
{
    int           status;
    bfd_byte      buffer[ 4 ];
    unsigned long insn = 0;

    /* First figure out how big the opcode is.  */

    status = info->read_memory_func (memaddr, buffer, 2, info);
    if (status == 0)
    {
        insn = bfd_getl16 (buffer);
    }

    if (status != 0)
    {
        info->memory_error_func (status, memaddr, info);
        return -1;
    }

    /* Make sure we tell our caller how many bytes we consumed.  */
    return disassemble (memaddr, info, insn);
}
