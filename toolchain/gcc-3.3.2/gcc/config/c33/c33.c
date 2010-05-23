/* Subroutines for insn-output.c for EPSON C33 series
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.
   Contributed by Jeff Law (law@cygnus.com).

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/***********************/
/* #include            */
/***********************/
#include "config.h"
#include "system.h"
#include "tree.h"
#include "rtl.h"
#include "regs.h"
#include "hard-reg-set.h"
#include "real.h"
#include "insn-config.h"
#include "conditions.h"
#include "output.h"
#include "insn-attr.h"
#include "flags.h"
#include "recog.h"
#include "expr.h"
#include "function.h"
#include "toplev.h"
#include "ggc.h"
#include "integrate.h"
#include "tm_p.h"
#include "target.h"
#include "target-def.h"

#ifndef streq
#define streq(a,b) (strcmp (a, b) == 0)
#endif

/***********************/
/* #define             */
/***********************/
#define GDA_REGNUM	15
#define ZDA_REGNUM	14
#define TDA_REGNUM	13
#define SDA_REGNUM	12


// ADD K.Watanabe V1.8 >>>>>>>
/***********************/
/*     prototype       */
/***********************/
void override_options ();
rtx function_arg ( CUMULATIVE_ARGS *cum, enum machine_mode mode, tree type, int named );
static void const_double_split ( rtx x, HOST_WIDE_INT *p_high, HOST_WIDE_INT *p_low );
static int const_costs_int ( HOST_WIDE_INT value );
int const_costs ( rtx r, enum rtx_code c );
void print_operand ( FILE *file, rtx x, int code );
void print_operand_address ( FILE *file, rtx addr );
void final_prescan_insn ( rtx insn, rtx *operand, int num_operands );
char * output_move_single ( rtx *operands, int unsignedp );
char *output_move_double ( rtx *operands );
char *output_btst ( rtx *operands );
char *output_bclr ( rtx *operands );
char *output_bset ( rtx *operands );
int call_address_operand ( rtx op, enum machine_mode mode );
int power_of_two_operand ( rtx op, enum machine_mode mode );
int not_power_of_two_operand ( rtx op, enum machine_mode mode );
int general_operand_post_inc ( rtx op, enum machine_mode mode );
int compute_register_save_size ( long *p_reg_saved );
int compute_frame_size ( int size, long *p_reg_saved );
static int check_call_being ();
static int check_mlt_being ();
void expand_prologue ( FILE *file );
void expand_epilogue ( FILE *file );
void notice_update_cc ( rtx exp, rtx insn );
c33_data_area c33_get_data_area ( tree decl );
static void c33_set_data_area ( tree decl, c33_data_area data_area );
void c33_encode_data_area ( tree decl );
int c33_interrupt_function_p (void);
void c33_select_section ( tree decl, int reloc );
int nshift_operator ( rtx x, enum machine_mode mode );
int expand_a_shift ( enum machine_mode mode, int code, rtx operands[] );
char *emit_a_shift ( enum rtx_code code, rtx *operands );
void print_options ( FILE *out );
void declare_object ( FILE *stream, char *name, 
                       char *init_string, char *final_string, int size );
void c33_output_aligned_bss ( FILE * file, tree decl, char * name, int size, int align );
void c33_output_common ( FILE * file, tree decl, char * name, int size, int align );
void c33_output_local ( FILE * file, tree decl, char * name, int size, int align );
void asm_file_start ( FILE *file );
static void block_move_loop ( rtx dest_reg, rtx src_reg, 
                                              int bytes, int align, rtx orig_src );
static void block_move_call ( rtx dest_reg, rtx src_reg, rtx bytes_rtx );
void expand_block_move ( rtx operands[] );
char * output_block_move ( rtx insn, rtx operands[], int num_regs );
int c33_adjust_insn_length ( rtx insn, int length );
static tree c33_handle_interrupt_attribute ( tree *node, tree name, 
            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED, bool *no_add_attrs );
static tree c33_handle_section_attribute ( tree *node, tree name, 
            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED, bool *no_add_attrs );
static tree c33_handle_data_area_attribute ( tree *node, tree name, 
            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED, bool *no_add_attrs );
static void c33_encode_section_info ( tree decl, int first );
static void c33_asm_out_constructor ( rtx symbol, int priority ATTRIBUTE_UNUSED );
static void c33_asm_out_destructor ( rtx symbol, int priority ATTRIBUTE_UNUSED );
static void c33_unique_section ( tree decl, int reloc );
int c33_output_addr_const_extra ( FILE * file, rtx x );
static const char *c33_strip_name_encoding ( const char *str );
// ADD K.Watanabe V1.8 <<<<<<<


/************************/
/* C33: Global variable */
/************************/
/* ADD K.Watanabe V1.7 >>>>>>> */
const struct attribute_spec c33_attribute_table[];

#undef TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR  c33_asm_out_constructor

#undef TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR   c33_asm_out_destructor

#undef TARGET_ATTRIBUTE_TABLE
#define TARGET_ATTRIBUTE_TABLE c33_attribute_table

#undef  TARGET_ASM_FUNCTION_PROLOGUE
#define TARGET_ASM_FUNCTION_PROLOGUE expand_prologue

#undef  TARGET_ASM_FUNCTION_EPILOGUE
#define TARGET_ASM_FUNCTION_EPILOGUE expand_epilogue

#undef  TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION  c33_select_section

#undef TARGET_ASM_SELECT_RTX_SECTION
#define TARGET_ASM_SELECT_RTX_SECTION const_section

#undef TARGET_ENCODE_SECTION_INFO
#define TARGET_ENCODE_SECTION_INFO c33_encode_section_info

#undef TARGET_ASM_UNIQUE_SECTION
#define TARGET_ASM_UNIQUE_SECTION	c33_unique_section

// ADD K.Watanabe V1.8 >>>>>>>
#undef TARGET_STRIP_NAME_ENCODING
#define TARGET_STRIP_NAME_ENCODING c33_strip_name_encoding
// ADD K.Watanabe V1.8 <<<<<<<


struct gcc_target targetm = TARGET_INITIALIZER;
/* ADD K.Watanabe V1.7 <<<<<<< */

/* Information about the various small memory areas.  */
/* C33: The default maximum value of the gda size is 16 byte. */

struct small_memory_info small_memory[ (int)SMALL_MEMORY_max ] =
{
  /* name	value		max		physical max */
  { "tda",	(char *)0,	0,		8192 },
  { "sda",	(char *)0,	0,		8192 },
  { "zda",	(char *)0,	0,		8192 },
  /* >>>>> change iruma m.takeishi '03.09.22
      C33: Change the default from "-mgda=4" to "=0".
  { "gda",	(char *)0,	4,		8192 },
  */
  { "gda",	(char *)0,	0,		8192 },
};

/* C33: Change the variable for the interrupt function. */
/* True if we don't need to check any more if the current
   function is an interrupt handler */
static int c33_interrupt_cache_p = FALSE;

/* Whether current function is an interrupt handler.  */
static int c33_interrupt_p = FALSE;

#ifdef LEAF_REGISTERS
/* C33: The array which maps the normal reigster number to the interrupt leaf function. */
char leaf_reg_remap[] =
  { -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1
};

#endif /*  */

/* Files to separate the text and the data output, so that all of the data
   can be emitted before the text, which will mean that the assembler will
   generate smaller code, based on the global pointer.  */
FILE *asm_out_data_file;
FILE *asm_out_text_file;

/* Strings to hold which cpu and instruction set architecture to use. */
char *c33_cpu_string;		/* for -mcpu=<xxx> */

/* C33: Set the default GP number. */
/* >>>>> change iruma m.takeishi '03.10.08 */
/* 
int gp_max = 4;
*/
int gp_max = 1;
/* <<<<< change iruma m.takeishi '03.10.08 */
char *gp_no;			/* for -mdp=<xxx> */

/* C33: specification for gdp */
int gdp = GDA_REGNUM;		/* C33: the register number for gdp */
char *gdp_string;		/* for -mgdp=xxx */

struct gdp_select
{
    char *name;
    int	number;
};

struct gdp_select select_table[] =
  {
    /* name	number */
    { "dp",	GDA_REGNUM },
    { "zdp",	ZDA_REGNUM },
    { "tdp",	TDA_REGNUM },
    { "sdp",	SDA_REGNUM },
    { "r15",	GDA_REGNUM },
    { "r14",	ZDA_REGNUM },
    { "r13",	TDA_REGNUM },
    { "r12",	SDA_REGNUM },
    { NULL,	0  }
  };

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */
void
override_options ()
{
  	int i;
	
  /* parse -mdp=n switch */
  if (gp_no != NULL)
    {
      if (!isdigit (*gp_no))
	{
	  error ("dp=%s is not numeric.", gp_no);
	}

      else
	{
          i = read_integral_parameter (gp_no, gp_no - 5, 4);
	  if (i > 6)
	    error ("dp=%s is too large.", gp_no);
	  else if (i < 1)
	    error ("dp=%s is too small.", gp_no);
	  else
	    gp_max = i;
	}
    }
    
    /* ADD K.Watanabe V1.7 >>>>>>> */
    /* C33: "-mdp" in not supported.
	        So display warning if the value is set except 1. */
	
    if( gp_max != 1 ){
	    warning ( "-mdp=%d -- It does not support formally. Use it as AS IS.",gp_max );
	}	    
    /* ADD K.Watanabe V1.7 <<<<<<< */
        
  /* parse -mgdp=xxx switch */
  if (gdp_string != NULL)
    {
      struct gdp_select *sel;

      gdp = 0;
      for (sel = &select_table[0]; sel->name != NULL; sel++) 
	{
	  if (streq (gdp_string, sel->name)) {
	    gdp = sel->number;
	    break;
	  }
	}
    }
    
  if (gdp == 0)
    {
      error ("gdp=%s is invalid.", gdp_string);
      gdp = GDA_REGNUM;
    }

  if (gdp < 16 - gp_max)
    {
      error ("-mdp=%s does not support -mgdp=%s.", gp_no, gdp_string);
      gdp = GDA_REGNUM;
    }

    /* ADD K.Watanabe V1.7 >>>>>>> */
    /* C33: "-mgdp" in not supported.
	        So display warning if the value is set except 15. ( Its meaing is %r15 ). */
    if( gdp != 15 ){
	    warning ( "-mgdp=r%d -- It does not support formally. Use it as AS IS.",gdp );
	}	    
    /* ADD K.Watanabe V1.7 <<<<<<< */
    
  if (c33_cpu_string == (char *) 0)
    {
      if ((TARGET_C33ADV) && (TARGET_C33PE))
	error ("-mc33adv conflicts with -mc33pe.");
      else if (TARGET_C33ADV)
        c33_cpu_string = C33_CPU_STRING_DEFAULT_ADVANCED;
      else if (TARGET_C33PE)
        c33_cpu_string = C33_CPU_STRING_DEFAULT_C33PE;
      else 
        c33_cpu_string = C33_CPU_STRING_DEFAULT;
    }

  /* Parse -mgda=nnn switches */
  for (i = (int)SMALL_MEMORY_GDA; i < (int)SMALL_MEMORY_max; i++)
    {
      if (small_memory[i].value)
	{
	  if (!isdigit (*small_memory[i].value))
	    error ("%s=%s is not numeric.",
		   small_memory[i].name,
		   small_memory[i].value);
	  else
	    {
	      small_memory[i].max = read_integral_parameter (small_memory[i].value, small_memory[i].value - 6, 4);
	      if (small_memory[i].max > small_memory[i].physical_max)
		error ("%s=%s is too large.",
		   small_memory[i].name,
		   small_memory[i].value);
	    }
	}
    }

	/* ADD K.Watanabe V1.7 >>>>>>> */
    /* C33: "-mgda" in not supported.
	        So display warning if the value is set except 0. */
	if( small_memory [(int) SMALL_MEMORY_GDA].max != 0 ){
		warning ( "-mgda=%d -- It does not support formally. Use it as AS IS.",small_memory [(int) SMALL_MEMORY_GDA].max );
	}
	/* ADD K.Watanabe V1.7 <<<<<<< */

  /* check -me[zts]da and -mdp=n switch */
  if (TARGET_EXT_ZDA)
    {
      if (gp_max < 2){
	  	error ("-mdp=%s does not support -mezda.", gp_no);
	  	target_flags &= ~MASK_EXT_ZDA;
	  }
	  
      /* ADD K.Watanabe V1.7 >>>>>>> */
      /* C33: "-mezda" in not supported.
	        So display warning if "-mezda" is set. */
      warning ( "-mezda -- It does not support formally. Use it as AS IS." );   
      /* ADD K.Watanabe V1.7 <<<<<<< */
	}

  if (TARGET_EXT_TDA)
    {
      if (gp_max < 3){
		  error ("-mdp=%s does not support -metda.", gp_no);
		  target_flags &= ~MASK_EXT_TDA;
		}
		
      /* ADD K.Watanabe V1.7 >>>>>>> */
      /* C33: "-metda" in not supported.
	        So display warning if "-metda" is set. */
      warning ( "-metda -- It does not support formally. Use it as AS IS." );   
      /* ADD K.Watanabe V1.7 <<<<<<< */
	
    }
  
  if (TARGET_EXT_SDA)
    {
      if (gp_max < 4){
		  error ("-mdp=%s does not support -mesda.", gp_no);
		  target_flags &= ~MASK_EXT_SDA;
		}
		
      /* ADD K.Watanabe V1.7 >>>>>>> */
      /* C33: "-mesda" in not supported.
	        So display warning if "-mesda" is set. */
      warning ( "-mesda -- It does not support formally. Use it as AS IS." );   
      /* ADD K.Watanabe V1.7 <<<<<<< */
   }
   
   	/* ADD K.Watanabe V1.7 >>>>>>> */
   	if( !TARGET_C33ADV ){
	   if( flag_pic && TARGET_EXT_32 ){
	   		error ("-fPIC conflicts with -medda32.");
	   }
	}	   
   	/* ADD K.Watanabe V1.7 <<<<<<< */
}

/* Return an RTX to represent where a value with mode MODE will be returned
   from a function.  If the result is 0, the argument is pushed. */
   
/* C33: English above is incorrect.
        This function returns RTX which represents where arguments 
        with mode MODE will be set.
        If the result is 0, the argument is pushed.                */
rtx
function_arg (cum, mode, type, named)
     CUMULATIVE_ARGS *cum;
     enum machine_mode mode;
     tree type;
     int named;

{
  rtx result = 0;
  int size, align;

/* C33: 1. The argument is pushed if if is variable argument. */
  if (!named)
    return NULL_RTX;

/* C33: 2. Decide the size which corresponds to the mode. */
  if (mode == BLKmode)
    {
      return 0;		/* C33: Structure is passed through the stack. */
    }
  else
    size = GET_MODE_SIZE (mode);

/* C33: 3. Decide the accumlation arguments size after adjusting alignment. */
  if (type)
    align = TYPE_ALIGN (type) / BITS_PER_UNIT;

  else
    align = size;
  cum->nbytes = (cum->nbytes + align - 1) & ~(align - 1);

/* C33: 4. The argument is pushed if the accumlation arguments is beyond 4 bytes. */
  if (cum->nbytes > 4 * UNITS_PER_WORD)
    return 0;

  if (type == NULL_TREE && cum->nbytes + size > 4 * UNITS_PER_WORD)
    return 0;

/* C33: 5. The argument is passed through the stack, if the mode is 'DFmode' and
           already used argument registers is 3 or more. */
  if (mode == DFmode)
    {

      /* C33: Do argument registers be used 3 or more? */
      if (cum->nbytes >= ((4 - 1) * UNITS_PER_WORD))
	return (rtx) 0;		/* C33: passed through the stack */
    }

/* C33: 6. Decide the argumtent register No. according to the accumlation argument size. */
  switch (cum->nbytes / UNITS_PER_WORD)
    {
    case 0:
      result = gen_rtx (REG, mode, 6);
      break;
    case 1:
      result = gen_rtx (REG, mode, 7);
      break;
    case 2:
      result = gen_rtx (REG, mode, 8);
      break;
    case 3:
      result = gen_rtx (REG, mode, 9);
      break;
    default:
      result = 0;
    }

/* C33: 7. Return the result. */
  return result;

/* C33: 8. Terminating process. */
}


/* Return the high and low words of a CONST_DOUBLE */
static void
const_double_split (x, p_high, p_low)
     rtx x;
     HOST_WIDE_INT *p_high;
     HOST_WIDE_INT *p_low;

{
  if (GET_CODE (x) == CONST_DOUBLE)
    {
      long t[2];
      REAL_VALUE_TYPE rv;
      switch (GET_MODE (x))
	{
	case DFmode:
	  REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	  REAL_VALUE_TO_TARGET_DOUBLE (rv, t);
	  *p_high = t[1];	/* since v850 is little endian */
	  *p_low = t[0];	/* high is second word */
	  return;
	case SFmode:
	  REAL_VALUE_FROM_CONST_DOUBLE (rv, x);
	  REAL_VALUE_TO_TARGET_SINGLE (rv, *p_high);
	  *p_low = 0;
	  return;
	case VOIDmode:
	case DImode:
	  *p_high = CONST_DOUBLE_HIGH (x);
	  *p_low = CONST_DOUBLE_LOW (x);
	  return;
	}
    }
  fatal_insn ("const_double_split got a bad insn:", x);
}


/* Return the cost of the rtx R with code CODE.  */
/* C33: ld.w %rd, sign6 */
static int
const_costs_int (value)
     HOST_WIDE_INT value;

{
  if (CONST_OK_FOR_SIGNED6 (value))	/* 6 bit signed */
    return 1;

  else if (CONST_OK_FOR_SIGNED19 (value))	/* 19 bit signed */
    return 2;

  else
    return 3;
}

int
const_costs (r, c)
     rtx r;
     enum rtx_code c;

{
  //HOST_WIDE_INT high, low;
  switch (c)
    {
    case CONST_INT:
      return const_costs_int (INTVAL (r));
    case CONST_DOUBLE:
      return 20;		/* C33: Big value. */
    case SYMBOL_REF:
    case LABEL_REF:
    case CONST:
    default:
      return 3;			/* C33: sign32 */
    }
}


/* Print operand X using operand code CODE to assembly language output file
   FILE.  */
/********************************************************************************************
Format		: void print_operand (file, x, code)
Input		: FILE *file -- pointer for the output file
              rtx x
              int code -- kind of opecode
Output		: None
Return		: None
Explanation	: Add necessary information to opecode.
*********************************************************************************************/
void print_operand (file, x, code)
     FILE *file;
     rtx x;
     int code;
{
  HOST_WIDE_INT high, low;
  HOST_WIDE_INT value;
  
  switch (code)
    {
    case 'b':			 /* C33: branch */
    case 'B':			 /* C33: branch -- inverse condition */
	      switch (code == 'B' ? reverse_condition (GET_CODE (x)) : GET_CODE (x))
		{
			case NE:
			  fprintf (file, "ne");
			  break;
			case EQ:
			  fprintf (file, "eq");
			  break;
			case GE:
			  fprintf (file, "ge");
			  break;
			case GT:
			  fprintf (file, "gt");
			  break;
			case LE:
			  fprintf (file, "le");
			  break;
			case LT:
			  fprintf (file, "lt");
			  break;
			case GEU:
			  fprintf (file, "uge");
			  break;
			case GTU:
			  fprintf (file, "ugt");
			  break;
			case LEU:
			  fprintf (file, "ule");
			  break;
			case LTU:
			  fprintf (file, "ult");
			  break;
			default:
			  abort ();
		}
      	break;
      	
    case 'M':			/* C33: bit operaion (btst, bset) */
      fprintf (file, "%d", exact_log2 (0xff & INTVAL (x)));
      break;
    case 'm':			/* C33: bit operaion (bclr) */
      fprintf (file, "%d", exact_log2 (0xff & ~(INTVAL (x))));
      break;
      
    case 'W':			/* C33: print the instruction suffix -- sign extention */
	      /* C33: Is the mode dst or src? */
	      switch (GET_MODE (x))
		{
			default:
			  abort ();
			case QImode:
			  fputs (".b", file);
			  break;
			case HImode:
			  fputs (".h", file);
			  break;
			case SImode:
			  fputs (".w", file);
			  break;
			case SFmode:
			  fputs (".w", file);
			  break;
		}
	      break;
	      
    case 'w':			/* C33: print the instruction suffix -- zero extention */

	      /* C33: Is the mode dst or src? */
	      switch (GET_MODE (x))
		{
			default:
			  abort ();
			case QImode:
			  fputs (".ub", file);
			  break;
			case HImode:
			  fputs (".uh", file);
			  break;
			case SImode:
			  fputs (".w", file);
			  break;
			case SFmode:
			  fputs (".w", file);
			  break;
		}
	    break;
	      
    case 'p':			/* SYMBOL_REF/LABEL_REF/CONST */
    					/* C33: The offset process in the case that symbols are reffered. */
        {
          char* name;

		  if ((GET_CODE (x) == SYMBOL_REF) || (GET_CODE (x) == LABEL_REF))
		    name = XSTR (x, 0);
		  else if (GET_CODE (x) == CONST)
		    name = XSTR (XEXP (XEXP (x, 0), 0), 0);
		  else
		    abort ();

		  if (ZDA_NAME_P (name))
		    {
		      if (TARGET_EXT_ZDA)
		        {
		          fprintf (file, "ext zoff_hi(");
		          print_operand_address (file, x);
		          fprintf (file, ")\n\t");
		        }
		      fprintf (file, "ext zoff_lo(");
		      print_operand_address (file, x);
		      fprintf (file, ")");
		    }
		  else if (TDA_NAME_P (name))
		    {
		      if (TARGET_EXT_TDA)
		        {
		          fprintf (file, "ext toff_hi(");
		          print_operand_address (file, x);
		          fprintf (file, ")\n\t");
		        }
		      fprintf (file, "ext toff_lo(");
		      print_operand_address (file, x);
		      fprintf (file, ")");
		    }
		  else if (SDA_NAME_P (name))
		    {
		      if (TARGET_EXT_SDA)
		        {
		          fprintf (file, "ext soff_hi(");
		          print_operand_address (file, x);
		          fprintf (file, ")\n\t");
		        }
		      fprintf (file, "ext soff_lo(");
		      print_operand_address (file, x);
		      fprintf (file, ")");
		    }
		  else if (GDA_NAME_P (name))
		    {
		      fprintf (file, "ext goff_lo(");

		      print_operand_address (file, x);
		      fprintf (file, ")");
		    }
		  else
		    {
				if(TARGET_C33ADV){
			    	fprintf (file, "ext dpoff_h(");
			        print_operand_address (file, x);
		    	    fprintf (file, ")\n\text dpoff_m(");
		    	    
			       	print_operand_address (file, x);		/* ADD K.Watanabe V1.4 */
			       	fprintf (file, ")");					/* ADD K.Watanabe V1.4 */
	           	} else {
           		/* CHG K.Watanabe V1.7 >>>>>>> */
           		/************************************************************/
           		/* C33: In the case that symbol address is reffered.        */
           		/* ( no -medda32 )											*/
       			/* ext		doff_hi()										*/
       			/* ext		doff_lo()										*/
       			/* add		%rn,%r15										*/
       			/*															*/
				/* ( -medda32 )												*/
				/*  xld.w	%rn,xxxx										*/	
           		/************************************************************/
           			if( TARGET_EXT_32 ){
           				;
           			} else {
				       fprintf (file, "ext doff_hi(");
				       print_operand_address (file, x);
				       fprintf (file, ")\n\text doff_lo(");
				       	print_operand_address (file, x);		/* ADD K.Watanabe V1.7 */
				       	fprintf (file, ")");					/* ADD K.Watanabe V1.7 */
					}				       
	           	}
	            /* CHG K.Watanabe V1.7 <<<<<<< */
	           	
		       	// print_operand_address (file, x);		/* DEL K.Watanabe V1.4 */
		       	// fprintf (file, ")");					/* DEL K.Watanabe V1.4 */
		    }
        }
        break;

    case 'P':		/* C33: The offset process in the case that the memory is accessed. */
      {
		rtx xx;
	        char* name;

		xx = XEXP(x, 0);	/* C33: "x" means "MEM", so "xx" shall be "CONST" or "SYMBOL_REF". */
		if (GET_CODE (xx) == SYMBOL_REF)
		  name = XSTR (xx, 0);
		else if (GET_CODE (xx) == CONST)
		  name = XSTR (XEXP (XEXP (xx, 0), 0), 0);
		else
		  abort ();

		if (ZDA_NAME_P (name))
		  {
		    if (TARGET_EXT_ZDA)
		      {
		        fprintf (file, "ext zoff_hi(");
		        print_operand_address (file, xx);
		        fprintf (file, ")\n\t");
		      }
		    fprintf (file, "ext zoff_lo(");
		    print_operand_address (file, xx);
		    fprintf (file, ")");
		  }
		else if (TDA_NAME_P (name))
		  {
		    if (TARGET_EXT_TDA)
		      {
		        fprintf (file, "ext toff_hi(");
		        print_operand_address (file, xx);
		        fprintf (file, ")\n\t");
		      }
		    fprintf (file, "ext toff_lo(");
		    print_operand_address (file, xx);
		    fprintf (file, ")");
		  }
		else if (SDA_NAME_P (name))
		  {
		    if (TARGET_EXT_SDA)
		      {
		        fprintf (file, "ext soff_hi(");
		        print_operand_address (file, xx);
		        fprintf (file, ")\n\t");
		      }
		    fprintf (file, "ext soff_lo(");
		    print_operand_address (file, xx);
		    fprintf (file, ")");
		  }
		else if (GDA_NAME_P (name))
		  {
		    fprintf (file, "ext goff_lo(");

		    print_operand_address (file, xx);
		    fprintf (file, ")");
		  }
		else
		  {
			if(TARGET_C33ADV){
		        fprintf (file, "ext dpoff_h(");
		        print_operand_address (file, xx);
		        fprintf (file, ")\n\text dpoff_m(");
		        
			    print_operand_address (file, xx);		/* ADD K.Watanabe V1.4 */
			    fprintf (file, ")");					/* ADD K.Watanabe V1.4 */
			    
			/* CHG K.Watanabe V1.7 >>>>>>> */
			} else {
           		/************************************************************/
       			/* ext		doff_hi( symbol )								*/
       			/* ext		doff_lo( symbol )								*/
       			/*		↓              									*/
				/*  xld.w	%rn,symbol										*/
           		/************************************************************/
	           	if (TARGET_EXT_32){
				    ;									/* C33:  It can't come here. */
	           	} else {
				    fprintf (file, "ext doff_hi(");
				    print_operand_address (file, xx);
				    fprintf (file, ")\n\text doff_lo(");
				        
					print_operand_address (file, xx);		
					fprintf (file, ")");					
				}				   
	        }
			/* CHG K.Watanabe V1.7 <<<<<<< */
	        
		    //print_operand_address (file, xx);		/* DEL K.Watanabe V1.4 */
		    //fprintf (file, ")");					/* DEL K.Watanabe V1.4 */
		  }
      }
      break;
      
    case 'q':					/* C33:  Second operand in the case that symbol is refferd. */
      {
        char* name;

        if ((GET_CODE (x) == SYMBOL_REF) || (GET_CODE (x) == LABEL_REF))
          name = XSTR (x, 0);
        else if (GET_CODE (x) == CONST)
          name = XSTR (XEXP (XEXP (x, 0), 0), 0);
        else
          abort ();

        if (ZDA_NAME_P (name))
          fprintf (file, "%%r14");
        else if (TDA_NAME_P (name))
          fprintf (file, "%%r13");
        else if (SDA_NAME_P (name))
          fprintf (file, "%%r12");
        else if (GDA_NAME_P (name))
          fprintf (file, "%s", reg_names[gdp]);
        else
		  if(TARGET_C33ADV)
            {
              fprintf (file, "dpoff_l(");
              print_operand_address (file, x);
              fprintf (file, ")");
            }
          else
            {
           		/* CHG K.Watanabe V1.7 >>>>>>> */
           		/************************************************************/
           		/* C33: In the case that symbol address is referred.        */
           		/* ( no -medda32 )											*/
       			/* ext		doff_hi()										*/
       			/* ext		doff_lo()										*/
       			/* add		%rn,%r15										*/
       			/*															*/
				/* ( -medda32 )												*/
				/*  xld.w	%rn,xxxx										*/	
           		/************************************************************/
           		if( TARGET_EXT_32 ){
			        print_operand_address (file, x);
			    } else {    
             		fprintf (file, "%%r15");
             	}	
           		/* CHG K.Watanabe V1.7 <<<<<<< */             	
            }
      }
      break;
    case 'Q':					/* C33: The register which saves the memory access address. */
    							/* C33: [ %rn ] */
      {
        rtx xx;
        char* name;

        xx = XEXP(x, 0);	    /* C33: "x" means "MEM", so "xx" shall be "CONST" or "SYMBOL_REF". */
        if (GET_CODE (xx) == CONST)
          name = XSTR (XEXP (XEXP (xx, 0), 0), 0);
        else if (GET_CODE (xx) == SYMBOL_REF)
          name = XSTR (xx, 0);
        else
          abort ();

        if (ZDA_NAME_P (name))
          fprintf (file, "%%r14");
        else if (TDA_NAME_P (name))
          fprintf (file, "%%r13");
        else if (SDA_NAME_P (name))
          fprintf (file, "%%r12");
        else if (GDA_NAME_P (name))
          fprintf (file, "%s", reg_names[gdp]);
        else
		  if(TARGET_C33ADV)
            {
              fprintf (file, "%%dp+dpoff_l(");
              print_operand_address (file, xx);
              fprintf (file, ")");
            }
          else 
            {
           		/* CHG K.Watanabe V1.7 >>>>>>> */
           		/************************************************************/
       			/* ext		doff_hi( symbol )								*/
       			/* ext		doff_lo( symbol )								*/
       			/*		↓              									*/	
				/*  xld.w	%rn,symbol										*/
           		/************************************************************/
            	if( TARGET_EXT_32 ){
            		;							/* C33: It can't come here. */
            	} else {
           		/* CHG K.Watanabe V1.7 <<<<<<< */         	
            		fprintf (file, "%%r15");
            	}	
            }
      }
      break;
    case 'R':			/* 2nd word of a double.  */
      switch (GET_CODE (x))
	{
	case REG:
	  fputs (reg_names[REGNO (x) + 1], file);
	  break;
	case MEM:
/* CHG K.Watanabe V1.7 >>>>>>> */	
/*	
	  print_operand_address (file,
				 XEXP (adj_offsettable_operand (x, 4), 0));
*/
	  print_operand_address (file,
				 XEXP (adjust_address (x, SImode, 4), 0));
/* CHG K.Watanabe V1.7 <<<<<<< */

	  break;
	default:
	  break;
	}
      break;
    case 'F':			/* high word of CONST_DOUBLE */
      if (GET_CODE (x) == CONST_INT)
	fprintf (file, "%d", (INTVAL (x) >= 0) ? 0 : -1);

      else if (GET_CODE (x) == CONST_DOUBLE)

	{
	  const_double_split (x, &high, &low);
	  //fprintf (file, "0x%lx", (long) high);
	  fprintf (file, "0x%x", high);
	}

      else
	abort ();
      break;
    case 'f':			/* C33: hex notation float for debug */
      {
	REAL_VALUE_TYPE type;	
	char s[30];	
	/* CHG K.Watanabe V1.7 >>>>>>> */
	#if 0
	REAL_VALUE_FROM_CONST_DOUBLE (type, x);
	REAL_VALUE_TO_DECIMAL (type, "%.20e", s); 
	#endif
	
	real_to_decimal (s, CONST_DOUBLE_REAL_VALUE (x), sizeof (s), 0, 1);
	/* CHG K.Watanabe V1.7 <<<<<<< */	
	fprintf (file, s);
      }
      break;
    case 'v':			/* C33: hex notation for debug */
      fprintf (file, "0x%x", INTVAL (x));
      break;
    case '#':

      /* C33: ".d" is not output, if there is not the delay slot instruction. */
      if (dbr_sequence_length () != 0)
	fputs (".d", file);
      break;
    default:
      switch (GET_CODE (x))
	{
	case MEM:
	  if (GET_CODE (XEXP (x, 0)) == CONST_INT)
	    output_address (gen_rtx
			    (PLUS, SImode, gen_rtx (REG, SImode, 0),
			     XEXP (x, 0)));

	  else
	    output_address (XEXP (x, 0));
	  break;
	case REG:
	  /* %rd / %rs */
	  /* C33: It comes here in the case that the insturction is both load and save. */
	  fputs (reg_names[REGNO (x)], file);
	  break;

	case CONST_INT:
	case SYMBOL_REF:
	case CONST:
	case LABEL_REF:
	case CODE_LABEL:
	  print_operand_address (file, x);
	  break;
	default:
	  abort ();
	}
      break;
    }
}

/* Output assembly language output for the address ADDR to FILE.  */
void
print_operand_address (file, addr)
     FILE *file;
     rtx addr;

{
  switch (GET_CODE (addr))
    {
    case REG:
      print_operand (file, addr, 0);
      break;
    case POST_INC:
      print_operand (file, XEXP (addr, 0), 0);
      break;
    case PLUS:
      if (GET_CODE (XEXP (addr, 0)) == CONST_INT)
	{
	  print_operand (file, XEXP (addr, 1), 0);
	  if (INTVAL (XEXP (addr, 0)) >= 0)
	    fprintf (file, "+");
	  print_operand (file, XEXP (addr, 0), 0);
	}

      else
	{
	  print_operand (file, XEXP (addr, 0), 0);
	  if (INTVAL (XEXP (addr, 1)) >= 0)
	    fprintf (file, "+");
	  print_operand (file, XEXP (addr, 1), 0);
	}
      break;
    default:
      output_addr_const (file, addr);
      break;
    }
}


/* Output all insn addresses and their sizes into the assembly language
   output file.  This is helpful for debugging whether the length attributes
   in the md file are correct.  This is not meant to be a user selectable
   option.  */

void
final_prescan_insn (insn, operand, num_operands)
     rtx insn, *operand;
     int num_operands;
{
  /* This holds the last insn address.  */
  static int last_insn_address = 0;

  int uid = INSN_UID (insn);

  if (TARGET_ADDRESSES)
    {
    /* CHG K.Watanabe V1.7 >>>>>>> */
/*
      fprintf (asm_out_file, "; 0x%x %d\n", insn_addresses[uid],
               insn_addresses[uid] - last_insn_address);
      last_insn_address = insn_addresses[uid];
*/
      fprintf (asm_out_file, "; 0x%x %d\n", INSN_ADDRESSES(uid),
               INSN_ADDRESSES(uid) - last_insn_address);
      last_insn_address = INSN_ADDRESSES(uid);
      /* CHG K.Watanabe V1.7 <<<<<<< */
    }
}

/* Return appropriate code to load up a 1, 2, or 4 integer/floating
   point value.  */
   
/*****************************************************************************************************
Format		: char * output_move_single (operands, unsignedp)
Input		: rtx *operands -- pointer for operand
              int unsignedp -- true in the case of zero extention
Output		: None
Return		: assember string output
Explanation	: ・Output "ld.x" instruction.
              ・There is not the instruction of memory saving and memory loading in one instruction
                in the case of "-medda32" && "!adv" && "default data area".
*****************************************************************************************************/   
char * output_move_single (operands, unsignedp)
     rtx *operands;
     int unsignedp;		/* C33: true in the case of zero extention */

{
  	rtx dst = operands[0];
	rtx src = operands[1];
	rtx xx;	
	char* name;
	
  	if (REG_P (dst)){
      	if (REG_P (src)) {						/* C33: register to register */
	  		if (unsignedp == TRUE){
		    	return "ld%w1\t%0,%1";			/* C33: zero extention */
	    	} else {
	      		return "ld%W1\t%0,%1";			/* C33: sign extention */
	    	}
		} else if (GET_CODE (src) == CONST_INT) {	/* C33: immediate to register */
		  	return "xld.w\t%0,%1\t;%v1";
		} else if (GET_CODE (src) == CONST_DOUBLE && GET_MODE (src) == SFmode) {
		  	return "xld.w\t%0,%F1\t;%f1";		/* C33: immediate to register */
		} else if (GET_CODE (src) == MEM) {		/* C33: memory to register */
		  	if (GET_CODE (XEXP (src, 0)) == POST_INC){
		      	if (unsignedp == TRUE){
			  		return "ld%w1\t%0,[%1]+";	/* C33: zero extention */
				} else {
			  		return "ld%W1\t%0,[%1]+";	/* C33: sign extention */
				}
	    	} else if (GET_CODE (XEXP (src, 0)) == REG){
	      		if (unsignedp == TRUE){
		  			return "ld%w1\t%0,[%1]";	/* C33: zero extention */
				} else {
		  			return "ld%W1\t%0,[%1]";	/* C33: sign extention */
				}
	    	} else if (GET_CODE (XEXP (src, 0)) == PLUS){
	      		if (unsignedp == TRUE) {
		  			return "xld%w1\t%0,[%1]";	/* C33: zero extention */
				} else {
		  			return "xld%W1\t%0,[%1]";	/* C33: sign extention */
				}
	    	} else {
	      		if (unsignedp == TRUE) {
		  			return "%P1\n\tld%w1\t%0,[%Q1]";	/* C33: zero extention */
				} else {
		  			return "%P1\n\tld%W1\t%0,[%Q1]";	/* C33: sign extention */
				}
			}	
		} else if (GET_CODE (src) == LABEL_REF			/* C33: symbol to register */
	       	|| GET_CODE (src) == SYMBOL_REF || GET_CODE (src) == CONST) {

	  		/* C33: 2 instructions at addition in the case that indirect of "GP" can be used. */
			if (TARGET_C33ADV){
        		/* char *name; */

            	if ((GET_CODE (src) == SYMBOL_REF) || (GET_CODE (src) == LABEL_REF))
            		name = XSTR (src, 0);
            	else 
                	name = XSTR (XEXP (XEXP (src, 0), 0), 0);

            	if (ENCODED_NAME_P (name))
            		return "%p1\n\tadd\t%0,%q1";						/* C33: s/t/z/g data area */
            	else 
            		return "ld.w\t%0,%%dp\n\t%p1\n\tadd\t%0,%q1";		/* C33: default data area */
       		} 
       		/* CHG K.Watanabe V1.7 >>>>>>> */
       		else 
       		{					
       			if ((GET_CODE (src) == SYMBOL_REF) || (GET_CODE (src) == LABEL_REF)){
            		name = XSTR (src, 0);
            	} else {
                	name = XSTR (XEXP (XEXP (src, 0), 0), 0);
				}
				
           		/************************************************************/
           		/* C33: In the case of symbol address is referred.			*/
           		/* ( no -medda32 )											*/
       			/* ext		doff_hi()										*/
       			/* ext		doff_lo()										*/
       			/* add		%rn,%r15										*/
       			/*															*/
				/* ( -medda32 )												*/
				/*  xld.w	%rn,xxxx										*/	
           		/************************************************************/
           		
	    		/* return "%p1\n\tadd\t%0,%q1";	*/
	    		if ( ( ENCODED_NAME_P (name) )){						/* C33: s/t/z/g data area */
		        	return "%p1\n\tadd\t%0,%q1";						
		        } else {												/* C33: default data area */
			        if ( TARGET_EXT_32 ){
			        	return "%p1xld.w\t%0,%q1";			        	/* C33: "-medda32" option */
			        }else{
			        	return "%p1\n\tadd\t%0,%q1";					/* C33: Not "-medda32" option */
			        }
				}			        
	    	}									
	    	/* CHG K.Watanabe V1.7 <<<<<<< */	
		}
    } else if (GET_CODE (dst) == MEM) {									/* C33: register to memory */
      	if (REG_P (src)) {
	  		if (GET_CODE (XEXP (dst, 0)) == POST_INC){
		  		return "ld%W1\t[%0]+,%1";			/* C33: sign extention */
	    	} else if (GET_CODE (XEXP (dst, 0)) == REG) {
		  		return "ld%W1\t[%0],%1";			/* C33: sign extention */
	    	} else if (GET_CODE (XEXP (dst, 0)) == PLUS){
		  		return "xld%W1\t[%0],%1";			/* C33: sign extention */
	    	} else {
		  		return "%P0\n\tld%W1\t[%Q0],%1";	/* C33: sign extention */
	    	}
		}
    }
  	fatal_insn ("output_move_single:", gen_rtx (SET, VOIDmode, dst, src));
  	return "";
}

/* Return appropriate code to load up an 8 byte integer or
   floating point value */
char *
output_move_double (operands)
     rtx *operands;

{
  enum machine_mode mode = GET_MODE (operands[0]);
  rtx dst = operands[0];
  rtx src = operands[1];

  /* C33: register to register */
  if (register_operand (dst, mode) && register_operand (src, mode))

    {
      if (REGNO (src) + 1 == REGNO (dst))
	return "ld.w\t%R0,%R1\n\tld.w\t%0,%1";

      else
	return "ld.w\t%0,%1\n\tld.w\t%R0,%R1";
    }

  /* C33: immediate to register */
  if (GET_CODE (src) == CONST_INT)	/* C33: Double type or long long type in the case that 
                                            only sign is set in high order register. */
    {
      HOST_WIDE_INT high_low[2];
      int i;
      rtx xop[10];
#if 1
      high_low[0] = INTVAL (src) & ((1ULL << BITS_PER_WORD) - 1); // fix for 64 bit
      //high_low[1] = INTVAL (src) >> BITS_PER_WORD; // might this be more correct than the line below
      high_low[1] = ((INTVAL (src) & (1ULL << (BITS_PER_WORD - 1))) == 0) ? 0 : -1;
#else
      high_low[0] = INTVAL (src);
      high_low[1] = (INTVAL (src) >= 0) ? 0 : -1;
#endif
      for (i = 0; i < 2; i++)

	{
	  xop[0] = gen_rtx (REG, SImode, REGNO (dst) + i);
	  xop[1] = GEN_INT (high_low[i]);
	  
	  // CHG K.Watanabe V1.8 >>>>>>>
	  #if 0
	  output_asm_insn (output_move_single (xop), xop);
	  #endif
	  
	  output_asm_insn (output_move_single (xop,1), xop);
	  // CHG K.Watanabe V1.8 <<<<<<<
	}
      return "";
    }

  else if (GET_CODE (src) == CONST_DOUBLE)

    {
      split_double (src, operands + 2, operands + 3);
      if (GET_MODE (src) == DFmode)

	{
	  return "xld.w\t%0,%v2\t;double %f1\n\txld.w\t%R0,%v3";
	}

      else

	{
	  return "xld.w\t%0,%2\t;long long\n\txld.w\t%R0,%3";
	}
    }

  /* C33: memory to register */
  if (GET_CODE (src) == MEM)

    {
      int ptrreg = -1;
      int dreg = REGNO (dst);
      rtx inside = XEXP (src, 0);
      if (GET_CODE (inside) == REG)
	ptrreg = REGNO (inside);

      else if (GET_CODE (inside) == SUBREG)
      /* CHG K.Watanabe V1.7 >>>>>>> */
      /*
	    ptrreg = REGNO (SUBREG_REG (inside)) + SUBREG_WORD (inside);
	  */  
	    ptrreg = subreg_regno (inside);
      /* CHG K.Watanabe V1.7 <<<<<<< */
      else if (GET_CODE (inside) == PLUS)
	ptrreg = REGNO (XEXP (inside, 0));
      if (dreg == ptrreg)
	return "xld.w\t%R0,[%R1]\t;output_move_double\n\txld.w\t%0,[%1]";

      else
	return "xld.w\t%0,[%1]\t;output_move_double\n\txld.w\t%R0,[%R1]";
    }

  /* C33: register to memory */
  if (GET_CODE (dst) == MEM)
    return "xld.w\t[%0],%1\t;output_move_double\n\txld.w\t[%R0],%R1";
  fatal_insn ("output_move_double:", gen_rtx (SET, VOIDmode, dst, src));
  return "";
}

char *
output_btst (operands)
     rtx *operands;

{
  rtx dst = operands[0];
  rtx src = operands[1];

  if (GET_CODE (src) == MEM)
    {
      if (GET_CODE (XEXP (src, 0)) == REG)
	{
	  return "btst\t[%1],%M4";
	}
      else if (GET_CODE (XEXP (src, 0)) == PLUS)
	{
	  return "xbtst\t[%1],%M4";
	}
      else
	{
	  return "%P1\n\tbtst\t[%Q1],%M4";
	}
    }
  fatal_insn ("output_btst:", gen_rtx (SET, VOIDmode, dst, src));
  return "";
}


char *
output_bclr (operands)
     rtx *operands;

{
  rtx dst = operands[0];
  rtx src = operands[1];

  if (GET_CODE (src) == MEM)
    {
      if (GET_CODE (XEXP (src, 0)) == REG)
	{
	  return "bclr\t[%1],%m4";
	}
      else if (GET_CODE (XEXP (src, 0)) == PLUS)
	{
	  return "xbclr\t[%1],%m4";
	}
      else
	{
	  return "%P1\n\tbclr\t[%Q1],%m4";
	}
    }
  fatal_insn ("output_bclr:", gen_rtx (SET, VOIDmode, dst, src));
  return "";
}


char *
output_bset (operands)
     rtx *operands;

{
  rtx dst = operands[0];
  rtx src = operands[1];

  if (GET_CODE (src) == MEM)
    {
      if (GET_CODE (XEXP (src, 0)) == REG)
	{
	  return "bset\t[%1],%M4";
	}
      else if (GET_CODE (XEXP (src, 0)) == PLUS)
	{
	  return "xbset\t[%1],%M4";
	}
      else
	{
	  return "%P1\n\tbset\t[%Q1],%M4";
	}
    }
  fatal_insn ("output_bset:", gen_rtx (SET, VOIDmode, dst, src));
  return "";
}

/* Return true if OP is a valid call operand.  */
int
call_address_operand (op, mode)
     rtx op;
     enum machine_mode mode;

{
  return (GET_CODE (op) == SYMBOL_REF || GET_CODE (op) == REG);
}

/* C33: for bit operation */
int
power_of_two_operand (op, mode)
     rtx op;
     enum machine_mode mode;

{
  if (GET_CODE (op) != CONST_INT)
    return 0;
  if (exact_log2 (INTVAL (op)) == -1)
    return 0;
  return 1;
}

int
not_power_of_two_operand (op, mode)
     rtx op;
     enum machine_mode mode;

{
  unsigned int mask;
  if (mode == QImode)
    mask = 0xff;

  else if (mode == HImode)
    mask = 0xffff;

  else if (mode == SImode)
    mask = 0xffffffff;

  else
    return 0;
  if (GET_CODE (op) != CONST_INT)
    return 0;
  if (exact_log2 (~INTVAL (op) & mask) == -1)
    return 0;
  return 1;
}


/* Return true is OP is a valid operand for an integer move instruction. */
int
general_operand_post_inc (op, mode)
     rtx op;
     enum machine_mode mode;

{
  if (GET_CODE (op) == MEM && GET_CODE (XEXP (op, 0)) == POST_INC)
    return 1;
  return general_operand (op, mode);
}


int
compute_register_save_size (p_reg_saved)
     long *p_reg_saved;		/* C33: The information that which register is saved. */

{
  int size;
  int i;

  int interrupt_handler = c33_interrupt_function_p ();

  long reg_saved = 0;
  
  /* >>>>> add iruma m.takeishi '03.10.03 */
  long size_add = 0;
  /* <<<<< add iruma m.takeishi '03.10.03 */



/* C33: 1. Allocate the area for PC. */
  size = 4;

/* C33: How "ahr,alr" shall be done at the time of interrupt? */

/* C33: 2. Count the registers which shall be saved. */
  /* Count space for the register saves.  */
  if (interrupt_handler)
    {				/* C33: in the case of interrupt */
      /* C33: for general purpose registers ( except %r15 ) */
      for (i = 0; i < 15; i++)
	{
	  if (regs_ever_live[i] != 0)
	    {
	      /* >>>>> change iruma m.takeishi '03.10.06 */
	      /* C33: The calculation method of bytes of registers shall be correspond to "expand_prologue".
	              It is calculated from the maximum register No.                                         */
	      /*
	      size += 4;
	      */
	      size_add = (i +1)* 4;
	      /* >>>>> change iruma m.takeishi '03.10.06 */
	      reg_saved |= 1L << i;
	    }

	  else
	    {

	      /* EMPTY */
	    }
	}

      /* >>>>> add iruma m.takeishi '03.10.03 */
      size += size_add;
      /* <<<<< add iruma m.takeishi '03.10.03 */
    }

  else
    {				/* C33: normal function call */

      /* C33: for general purpose registers ( except %r15 ) */
      for (i = 0; i < 15; i++)
	{
	  /* C33: IF   Is this saved register under use? */
	  if (regs_ever_live[i] && (!call_used_regs[i]))
	    {

	      /* YES */
	      /* C33: Allocation of the area and registration of the register No. */
	      /* >>>>> change iruma m.takeishi '03.10.03 */
          /* C33: The calculation method of bytes of registers shall be correspond to "expand_prologue".
	              It is calculated from the maximum register No.                                         */
	      /*
	      size += 4;
	      */
	      size_add = (i +1)* 4;
	      /* <<<<< change iruma m.takeishi '03.10.03 */
	      reg_saved |= 1L << i;
	    }

	  else
	    {

	      /* NO */

	      /* EMPTY */
	    }
	}
      
      /* >>>>> add iruma m.takeishi '03.10.03 */
      size += size_add;
      /* <<<<< add iruma m.takeishi '03.10.03 */
    }

/* C33: 3. Return the saved register No. */
  if (p_reg_saved)
    *p_reg_saved = reg_saved;

/* C33: 4. Return the saved area size. ( Containing PC size ) */
  return size;

/* C33: 5. Terminating process. */
}


int
compute_frame_size (size, p_reg_saved)
     int size;
     long *p_reg_saved;

{
//  extern int current_function_outgoing_args_size;		/* DEL K.Watanabe V1.7 */
  return (size + compute_register_save_size (p_reg_saved)
	  + current_function_outgoing_args_size);}


static int
check_call_being ()
{
  /* C33: Changed because of containing "call.d" */
  /* Return nonzero if this function has no function calls.  */
  return !leaf_function_p ();
}


/* C33: 2000.07.19  New create.
        The following function returns the number of "MLT" instructions in the function. */
static int
check_mlt_being ()
{
  int num = 0;
  rtx insn;
  for (insn = get_insns (); insn; insn = NEXT_INSN (insn))
    {
      /* C33: Reference from try_combine() in combine.c ( 1558th line ) */
      if (GET_CODE (insn) == INSN
	  && GET_CODE (PATTERN (insn)) == SET
	  && GET_CODE (SET_SRC (PATTERN (insn))) == MULT)
	{
	  num++;
	}
    }
  return num;
}


/* C33: 2000.07.19.         -- All corrected.  expand_prologue() & expand_epilogue()
        2000.10.26 watanabe -- The regsieter assignment in the interrutp leaf function is changed from %r0.
                               Stack frame supports 32bit value.
                               Support the inturrupt function.
        2001.11.14 watanabe -- "gp" number is variable from 1 to 6.
                               
    if( Theire is function call. ){

	【 interrupt function ( There is function call. ) 】
	pushn/popn %r9
	alr ⇔ %r0
	ahr ⇔ %r1		pushn/popn %r1 付加
	SP scratch：%r0
    }else{

	【 interrupt function ( There is not function call. ) 】
	if( reg_saved == 0 ){

	    the process of (a): Not using registers. "run_alr_ahr == 0" in implicity.

	}else{

	    if( run_alr_ahr == 1 ){

		the process of (b): 
		if( There is the unused registers. ){

		    the process of (b-1):
			pushn/popn %r(i+2)
			alr ⇔ %r(i+1)
			ahr ⇔ %r(i+2)
			SP scratch：%r0
		}else{

		    the process of (b-2):
			pushn/popn %ri
			alr ⇔ %r0
			ahr ⇔ %r1		ADD pushn/popn %r1 
			SP scratch：%r0

		}

	    }else{

		the process of (c):
		    pushn/popn %ri
		    SP scratch：%r0

	    }
	}
    }
*/

/****************************************************************************************************************************
Format		: void expand_prologue ( FILE *file)
Input		: pointer for the output file
Output		: None
Return		: None
Explanation	: ・Do prologue process at the time of calling function.
			  ・"expand_prlogue()" and "expand_epilogue()" must be a pair.
			  ・The structure of the stack frame.
			  						argument														|
			  						return address													|
			  																						|
			  						☆Jump															|
			  																						|
			  						register protection 											|
			  						( in the case of the normal function  --  %r0 - %r3				|
			  						  in the case of the interrupt function -- all used registers )	|
			  						local variable													↓	SP
****************************************************************************************************************************/
void expand_prologue (file)
     FILE *file;

{
  char *fnname;
  unsigned int i;
  unsigned int size;		/* C33: for local variable ( for frame ) */
  long reg_saved = 0;

  int interrupt_handler = c33_interrupt_function_p ();

/* C33: Note: This is considered to support for varialbe arguments. */

/* C33: 1. Decide the size of local variables and arguments. */
  size = get_frame_size () + current_function_outgoing_args_size;

/* C33: 2. Decide the saved register No. */
  compute_register_save_size (&reg_saved);

/* C33: 4. Decide the maximun register No. which is saved. */

  /* C33: Is there the saved register? */
  if (reg_saved != 0)
    {
      /* C33: Yes, there is the saved register.
              Decide the maximun register No. which is saved.
              Check to all general purpose registers.           */
      for (i = 31; i >= 0; i--)
	{
	  /* C33: Is the register saved? */
	  if (((1L << i) & reg_saved) != 0)
	    {
	      /* C33: Yes, we have finded the maximum register No. */
	      break;
	    }
	}
    }

/* C33: 5. Move "%sp". */
  /* C33: Is the function interrupt? */
  if (interrupt_handler)
    {
      /* C33: Yes, it is interrupt function */

      /* C33: Is there function call? */
      if (check_call_being ())
	{
	  /* C33: Yes, there is function call.
	          Output "push" instruction.
	          Save all except "gp".           */
	  fprintf (file, "\tpushn\t%%r%d\n", 15 - gp_max);/* pushn   %rX     */

	  /* C33: Save "alr, ahr" to the stack. */
	  fprintf (file, "\tld.w\t%%r0,%%alr\n");	/* ld.w    %r0, %alr */
	  fprintf (file, "\tld.w\t%%r1,%%ahr\n");	/* ld.w    %r1, %ahr */
	  fprintf (file, "\tpushn\t%%r1\n");		/* pushn   %r1       */
	}
      else
	{
	  /* C33: No, there is not function call. */
	  /* C33: Registers are used in the order of %r0,%r1,%r2,%r3,%r4,%r5,%r6,%r7,%r8,%r9
	          in the case that there is not function call in the interrupt function.      */

	  /* C33: Are registers used? */
	  if (reg_saved == 0)
	    {
          /* C33: Registers are not used. */
          /* C33: "push" instruction is unused. */

	      /* C33: Is the movement of %sp which uses scrath registers necessary? */
	      if (size > 0xffc)
		{
		  /* C33: Yes, it is necessary. */
		  fprintf (file, "\tpushn\t%%r0\n");	/* pushn   %r0       */
		}
	    }
	  else
	    {
	      /* C33: Registers are used. */
	      /* C33: Is "ahr / alr" saved? */
	      if (check_mlt_being ())
		{
		  /* C33: Yes, "ahr / alr" is saved. */
		  
		  /* C33: Is there general purpose register for saving "ahr,alr"? */
		  if (i < 14 - gp_max)
		    {
			  /* C33: Yes, there is. */
		      /* C33: Output "pushn" instruction. */
		      fprintf (file, "\tpushn\t%%r%d\n", i + 2);	/* pushn    %r(i+2)         */
		      fprintf (file, "\tld.w\t%%r%d,%%alr\n", i + 1);	/* ld.w     %r(i+1), %alr   */
		      fprintf (file, "\tld.w\t%%r%d,%%ahr\n", i + 2);	/* ld.w     %r(i+2), %ahr   */
		    }
		  else
		    {
			  /* C33: No, there is not. ( So, save to the stack. )  */
		      /* C33: Output "pushn" instruction. */
		      fprintf (file, "\tpushn\t%%r%d\n", i);		/* pushn    %r?             */
		      fprintf (file, "\tld.w\t%%r0,%%alr\n");		/* ld.w     %r0, %alr       */
		      fprintf (file, "\tld.w\t%%r1,%%ahr\n");		/* ld.w     %r1, %ahr       */
		      fprintf (file, "\tpushn\t%%r1\n");		/* pushn    %r1             */
		    }
		}
	      else
		{
		  /* C33: No, "ahr / alr" is not saved. */
		  /* C33: Output "pushn" instruction. */
		  fprintf (file, "\tpushn\t%%r%d\n", i);		/* pushn    %r? */
		}
	    }			/* C33: endif( reg_saved == 0) */
	}				/* C33: endif( function call ) */

      /* C33: Is the movement of %sp necessary? */
      if (size > 0xffc)
	{
	  /* C33: Yes, it is necessary. */
	  fprintf (file, "\tld.w\t%%r0,%%sp\t;sub %%sp,%d\n", size);	/* ld.w     %r0, %sp        */
	  fprintf (file, "\txsub\t%%r0,%d\t;\n", size);			/* xsub     %r0, imm32      */
	  fprintf (file, "\tld.w\t%%sp,%%r0\t;\n");			/* ld.w     %sp, %r0        */
	}
      else if (size != 0)
	{
	  /* C33: Yes, it is necessary. */
	  fprintf (file, "\tsub\t%%sp,%d\t;%d\n", (size >> 2), size);	/* sub      %sp, imm12      */
	}
      else
	{
  	  /* C33: No, it is not necessary. */
	  /* EMPTY */
	}
    }
  else
    {
      /* C33: No, the function is normal. */

      /* C33: Is there the saved registers? */
      if (reg_saved != 0)
	{
	  /* C33: Yes, there is. */

	  /* C33: Output "pushn" instrcution. */
	  fprintf (file, "\tpushn\t%%r%d\n", i);			/* pushn    %r?             */
	}
      else
	{
	  /* C33: No, there is not. */
	  /* EMPTY */
	}
      /* C33: Use %r4 for the scratch register which is used for stack movement.
              At this point, it is no problem that the register for storing returned values is destroyed. */
      
      /* C33: Is the movement of %sp necessary? */
      if (size > 0xffc)
	{
	  /* C33: Yes, it is necessary by using the scrath registers. */
	  fprintf (file, "\tld.w\t%%r4,%%sp\t;sub %%sp,%d\n", size);	/* ld.w     %r4, %sp   */
	  fprintf (file, "\txsub\t%%r4,%d\t;\n", size);					/* xsub     %r4, imm32 */
	  fprintf (file, "\tld.w\t%%sp,%%r4\t;\n");						/* ld.w     %sp, %r4   */
	}
      else if (size != 0)
	{
	  /* C33: Yes, it is necessary. */
	  fprintf (file, "\tsub\t%%sp,%d\t;%d\n", (size >> 2), size);	/* sub      %sp, imm12 */
	}
      else
	{
	  /* C33: No, it is not necessary. */
	  /* EMPTY */
	}
    }

	/* ADD K.Watanabe V1.7 >>>>>>> */
  	/* Set up frame pointer if it is necessary.  */
  	if (frame_pointer_needed){
	    fprintf ( file, "\tld.w	%%r0,%%sp\n" );
	}    	
	/* ADD K.Watanabe V1.7 <<<<<<< */
	    
/* 6. Return. */
  return;

/* C33: Terminating process. */
}


/*************************************************************************************************************
Format		: void expand_epilogue (file)
Input		: pointer for the output file
Output		: None
Return		: None
Explanation	: ・Do epilogue process at the time of calling function.
			  ・"expand_prlogue()" and "expand_epilogue()" must be a pair.
*************************************************************************************************************/
void expand_epilogue (file)
     FILE *file;

{
  unsigned int i;
  unsigned int size;		/* C33: for local variable ( for frame ) */
  long reg_saved = 0;

  int interrupt_handler = c33_interrupt_function_p ();

/* C33: Note: This is considered to support for varialbe arguments. */

/* C33: 1. Decide the size of local variables and arguments. */
  size = get_frame_size () + current_function_outgoing_args_size;

/* C33: 2. Decide the resotred register No. */
  compute_register_save_size (&reg_saved);

/* C33: 3. Decide the maximun register No. which is restored. */
     
  /* C33: If the resut is "i==0", there is not the restored register 
          in the case of interrupt function.
          In the case of normal function, there is not the restored register or it means %r0. */
             
  /* C33: Is there the saved register? */
  if (reg_saved != 0)
    {
      /* C33: Yes, there is. */

      /* C33: Check to all general purpose registers. */
      for (i = 31; i >= 0; i--)
	{
  	  /* C33: Is the register restored? */
	  if (((1L << i) & reg_saved) != 0)
	    {
	      /* C33: Yes, We have finded the maximum register No. */
	      break;
	    }
	}
    }

/* C33: 4. Move "%sp". */
/* C33: 5. Registers are saved. */
/* C33: 6. Output return instruction. */

  /* C33: Is the function interrupt? */
  if (interrupt_handler)
    {
      /* C33: Yes, the interrupt function */

      /* C33: Is the movement of %sp necessary? */
      if (size > 0xffc)
	{
	  /* C33: Yes, it is necessary by using the scrath registers. */
	  fprintf (file, "\tld.w\t%%r0,%%sp\t;add %%sp,%d\n", size);	/* ld.w     %r0, %sp        */
	  fprintf (file, "\txadd\t%%r0,%d\t;\n", size);			/* xadd     %r0, imm32      */
	  fprintf (file, "\tld.w\t%%sp,%%r0\t;\n");			/* ld.w     %sp, %r0        */
	}
      else if (size != 0)
	{
	  /* C33: Yes, it is necessary. */
	  fprintf (file, "\tadd\t%%sp,%d\t;%d\n", (size >> 2), size);	/* add      %sp, imm12      */
	}
      else
	{
	  /* C33: No, it is not necessary. */
	  /* EMPTY */
	}

      /* C33: Is there function call? */
      if (check_call_being ())
	{
	  /* C33: Yes, there is function call. */

	  /* C33: Restore "alr,ahr" from the stack. */
	  fprintf (file, "\tpopn\t%%r1\n");						/* popn     %r1       */
	  fprintf (file, "\tld.w\t%%alr,%%r0\n");				/* ld.w     %alr, %r0 */
	  fprintf (file, "\tld.w\t%%ahr,%%r1\n");				/* ld.w     %ahr, %r1 */

	  /* C33: Output "popn" instruction. */
	  /* C33: Restore all except "gp". */
	  fprintf (file, "\tpopn\t%%r%d\n", 15 - gp_max);		/* popn     %rX       */
	}
      else
	{
	  /* C33: No, there is not function call. */
	  /* C33: Registers are used in the order of %r0,%r1,%r2,%r3,%r4,%r5,%r6,%r7,%r8,%r9
	          in the case that there is not function call in the interrupt function.      */

	  /* C33: Are registers used? */
	  if (reg_saved == 0)
	    {
	      /* C33: Registers is not used. */

	      /* C33: Does %sp move by using the scrath registers? */
	      if (size > 0xffc)
		{
		  /* C33: Yes, it does. */
		  fprintf (file, "\tpopn\t%%r0\n");			/* popn     %r0             */
		}
	      else
		{
		  /* C33: No, it does not. */
		  /* EMPTY */
		}
	      /* "pop" instruction is unused. */
	    }
	  else
	    {
	      /* C33: Registers is used. */

	      /* C33: Is "ahr alr" saved? */
	      if (check_mlt_being ())
		{
		  /* C33: Yes, "ahr alr" is saved. */

		  /* C33: Is there general purpose register for saving "ahr,alr"? */
		  if (i < 14 - gp_max)
		    {
			  /* C33: Yes, there is. */
			
		      /* C33: Restore "alr,ahr" from the register. */
		      fprintf (file, "\tld.w\t%%alr,%%r%d\n", i + 1);	/* ld.w     %alr, %r(i+1)   */
		      fprintf (file, "\tld.w\t%%ahr,%%r%d\n", i + 2);	/* ld.w     %ahr, %r(i+2)   */

		      /* C33: Output "popn" instruction. */
		      fprintf (file, "\tpopn\t%%r%d\n", i + 2);		/* popn     %r(i+2)         */
		    }
		  else
		    {
			  /* C33: No, there is not. ( So, save to the stack. )  */
			  
		      /* C33: Restore "alr,ahr" from the stack. */
		      fprintf (file, "\tpopn\t%%r1\n");			/* popn     %r1             */
		      fprintf (file, "\tld.w\t%%alr,%%r0\n");		/* ld.w     %alr, %r0       */
		      fprintf (file, "\tld.w\t%%ahr,%%r1\n");		/* ld.w     %ahr, %r1       */

		      /* C33: Output "popn" instruction. */
		      fprintf (file, "\tpopn\t%%r%d\n", i);		/* popn     %r?             */
		    }
		}
	      else
		{
		  /* C33: No, "ahr alr" is not saved. */
		  /* C33: Output "pop" instruction. */
		  fprintf (file, "\tpopn\t%%r%d\n", i);			/* popn     %r?             */
		}
	    }			/* endif( reg_saved == 0) */
	}			/* endif( Is there function call? ) */

      /* C33: Output "reti" instruction. */
      fprintf (file, "\treti\n");
    }
  else
    {
      /* C33: No, the normal function. */
      /* C33: Use %r6 for the scratch register which is used for stack movement.
              At this point, it is no problem that the register for storing returned values is destroyed. */

      /* C33: Is the movement of %sp necessary? */
      if (size > 0xffc)
	{
	  /* C33: Yes, it is necessary by using the scrath registers. */
	  fprintf (file, "\tld.w\t%%r6,%%sp\t;add %%sp,%d\n", size);	/* ld.w     %r6, %sp        */
	  fprintf (file, "\txadd\t%%r6,%d\t;\n", size);			/* xadd     %r6, imm32      */
	  fprintf (file, "\tld.w\t%%sp,%%r6\t;\n");			/* ld.w     %sp, %r6        */
	}
      else if (size != 0)
	{
	  /* C33: Yes, it is necessary. */
	  fprintf (file, "\tadd\t%%sp,%d\t;%d\n", (size >> 2), size);	/* add      %sp, imm12      */
	}
      else
	{
	  /* C33: No, it is not necessary. */
	  /* EMPTY */
	}

      /* C33: Are there any registers to restore? */
      if (reg_saved != 0)
	{
	  /* C33: Yes, there are registers to restore. */

	  /* C33: Restore the registers. */
	  fprintf (file, "\tpopn\t%%r%d\n", i);
	}
      else
	{
	  /* C33: No, there are not registers to restore. */
	  /* EMPTY */
	}

      /* C33: Output "ret" instruction. */
      fprintf (file, "\tret\n");
    }

/* C33: 8. Initialize for the commom variable. */
  c33_interrupt_cache_p = FALSE;	/* C33: Interrupt judgement. */
  c33_interrupt_p = FALSE;			/* C33: Interrupt judgement. */

/* C33: 9. Return. */
  return;

/* C33: 10. Terminating process. */
}


/* C33: → conditions.h final.c */
/* Update the condition code from the insn.  */
void
notice_update_cc (exp, insn)
     rtx exp;
     rtx insn;			
{
  if (GET_CODE (exp) == SET)
    {

      /* Jumps do not alter the cc's.  */
      if (SET_DEST (exp) == pc_rtx)
	return;

      /* Moving register or memory into a register:
         it doesn't alter the cc's, but it might invalidate
         the RTX's which we remember the cc's came from.
         (Note that moving a constant 0 or 1 MAY set the cc's).  */
      if (REG_P (SET_DEST (exp))
	  && (REG_P (SET_SRC (exp)) || GET_CODE (SET_SRC (exp)) == MEM
	      || GET_RTX_CLASS (GET_CODE (SET_SRC (exp))) == '<'))
	{
	  if (cc_status.value1
	      && reg_overlap_mentioned_p (SET_DEST (exp),
					  cc_status.
					  value1)) cc_status.value1 = 0;
	  if (cc_status.value2
	      && reg_overlap_mentioned_p (SET_DEST (exp), cc_status.value2))
	    cc_status.value2 = 0;
	  return;
	}

      /* Moving register into memory doesn't alter the cc's.
         It may invalidate the RTX's which we remember the cc's came from.  */
      if (GET_CODE (SET_DEST (exp)) == MEM
	  && (REG_P (SET_SRC (exp))
	      || GET_RTX_CLASS (GET_CODE (SET_SRC (exp))) == '<'))

	{
	  if (cc_status.value1
	      && reg_overlap_mentioned_p (SET_DEST (exp),
					  cc_status.
					  value1)) cc_status.value1 = 0;
	  if (cc_status.value2
	      && reg_overlap_mentioned_p (SET_DEST (exp),
					  cc_status.
					  value2)) cc_status.value2 = 0;
	  return;
	}

      /* Function calls clobber the cc's.  */
      else if (GET_CODE (SET_SRC (exp)) == CALL)
	{
	  CC_STATUS_INIT;
	  return;
	}

      /* Tests and compares set the cc's in predictable ways.  */
      else if (SET_DEST (exp) == cc0_rtx)
	{
	  CC_STATUS_INIT;
	  cc_status.value1 = SET_SRC (exp);
	  return;
	}

      /* Certain instructions effect the condition codes. */
      else if (GET_MODE (SET_SRC (exp)) == SImode
	       || GET_MODE (SET_SRC (exp)) == HImode
	       || GET_MODE (SET_SRC (exp)) == QImode)
	switch (GET_CODE (SET_SRC (exp)))
	  {
	  case ASHIFTRT:
	  case LSHIFTRT:
	  case ASHIFT:

	    /* Shifts on the 386 don't set the condition codes if the
	       shift count is zero. */
	    if (GET_CODE (XEXP (SET_SRC (exp), 1)) != CONST_INT)
	      {
		CC_STATUS_INIT;
		break;
	      }

	    /* We assume that the CONST_INT is non-zero (this rtx would
	       have been deleted if it were zero. */
	  case PLUS:
	  case MINUS:
	  case NEG:
	  case AND:
	  case IOR:
	  case XOR:
	    cc_status.flags = CC_NO_OVERFLOW;
	    cc_status.value1 = SET_SRC (exp);
	    cc_status.value2 = SET_DEST (exp);
	    break;
	  default:
	    CC_STATUS_INIT;
	  }

      else
	{
	  CC_STATUS_INIT;
	}
    }

  else if (GET_CODE (exp) == PARALLEL
	   && GET_CODE (XVECEXP (exp, 0, 0)) == SET)
    {
      rtx p = XVECEXP (exp, 0, 0);
      if ((GET_MODE (SET_SRC (p)) == SImode
	   || GET_MODE (SET_SRC (p)) == HImode
	   || GET_MODE (SET_SRC (p)) == QImode)
	  && (GET_CODE (SET_SRC (p)) == ASHIFTRT
	      || GET_CODE (SET_SRC (p)) == LSHIFTRT
	      || GET_CODE (SET_SRC (p)) == ASHIFT))

	{
	  cc_status.flags = CC_NO_OVERFLOW;
	  cc_status.value1 = SET_SRC (p);
	  cc_status.value2 = SET_DEST (p);
	}
    }

  else
    {
      CC_STATUS_INIT;
    }
}


/* Retrieve the data area that has been chosen for the given decl.  */
c33_data_area
c33_get_data_area (decl)
     tree decl;
{
  /* CHG K.Watanabe V1.7 >>>>>>> */
/*  if (lookup_attribute ("sda", DECL_MACHINE_ATTRIBUTES (decl)) != NULL_TREE) */
  if (lookup_attribute ("sda", DECL_ATTRIBUTES (decl)) != NULL_TREE)  
  /* CHG K.Watanabe V1.7 <<<<<<< */
    {
      if (gp_max < 4)
        return DATA_AREA_NORMAL;
      else
        return DATA_AREA_SDA;
    }

  /* CHG K.Watanabe V1.7 >>>>>>> */
/*  if (lookup_attribute ("tda", DECL_MACHINE_ATTRIBUTES (decl)) != NULL_TREE) */
  if (lookup_attribute ("tda", DECL_ATTRIBUTES (decl)) != NULL_TREE)  
  /* CHG K.Watanabe V1.7 <<<<<<< */  
    {
      if (gp_max < 3)
        return DATA_AREA_NORMAL;
      else
        return DATA_AREA_TDA;
    }

  /* CHG K.Watanabe V1.7 >>>>>>> */  
/*  if (lookup_attribute ("zda", DECL_MACHINE_ATTRIBUTES (decl)) != NULL_TREE) */
  if (lookup_attribute ("zda", DECL_ATTRIBUTES (decl)) != NULL_TREE)  
  /* CHG K.Watanabe V1.7 <<<<<<< */    
    {
      if (gp_max < 2)
        return DATA_AREA_NORMAL;
      else
        return DATA_AREA_ZDA;
    }

  /* CHG K.Watanabe V1.7 >>>>>>> */ 
/*  if (lookup_attribute ("gda", DECL_MACHINE_ATTRIBUTES (decl)) != NULL_TREE) */
  if (lookup_attribute ("gda", DECL_ATTRIBUTES (decl)) != NULL_TREE)  
  /* CHG K.Watanabe V1.7 <<<<<<< */   
    return DATA_AREA_GDA;

  return DATA_AREA_NORMAL;
}


/* Store the indicated data area in the decl's attributes.  */
static void
c33_set_data_area (decl, data_area)
     tree decl;
     c33_data_area data_area;
{
  tree name;
  
  switch (data_area)
    {
    case DATA_AREA_SDA: name = get_identifier ("sda"); break;
    case DATA_AREA_TDA: name = get_identifier ("tda"); break;
    case DATA_AREA_ZDA: name = get_identifier ("zda"); break;
    case DATA_AREA_GDA: name = get_identifier ("gda"); break;
    default:
      return;
    }

/* CHG K.Watanabe V1.7 >>>>>>> */
/*
  DECL_MACHINE_ATTRIBUTES (decl) = tree_cons
    (name, NULL, DECL_MACHINE_ATTRIBUTES (decl));
*/    
  DECL_ATTRIBUTES (decl) = tree_cons
    (name, NULL, DECL_ATTRIBUTES (decl));
/* CHG K.Watanabe V1.7 <<<<<<< */
}



/* Return nonzero if ATTR is a valid attribute for DECL.
   ATTRIBUTES are any existing attributes and ARGS are the arguments
   supplied with ATTR.

   Supported attributes:

   interrupt_handler or interrupt: output a prologue and epilogue suitable
   for an interrupt handler.  */

/* Return nonzero if ATTR is a valid attribute for DECL.
   ARGS are the arguments supplied with ATTR.  */
/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. */

#if 0
int
c33_valid_machine_decl_attribute (decl, attr, args)
     tree decl;
     tree attr;
     tree args;
{
  c33_data_area data_area;
  c33_data_area area;
  
  if (args != NULL_TREE)
    return 0;

  if (is_attribute_p ("interrupt_handler", attr)
      || is_attribute_p ("interrupt", attr))
    return TREE_CODE (decl) == FUNCTION_DECL;

  /* Implement data area attribute.  */
  if (is_attribute_p ("sda", attr))
    {
      if (gp_max < 4)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'sda'.");
          return 1;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'sda'.");
          return 1;
        }
      else
        data_area = DATA_AREA_SDA;
    }
  else if (is_attribute_p ("tda", attr))
    {
      if (gp_max < 3)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'tda'.");
          return 1;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'tda'.");
          return 1;
        }
      else
        data_area = DATA_AREA_TDA;
    }
  else if (is_attribute_p ("zda", attr))
    {
      if (gp_max < 2)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'zda'.");
          return 1;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'zda'.");
          return 1;
        }
      else
        data_area = DATA_AREA_ZDA;
    }
  else
    return 0;
  
  switch (TREE_CODE (decl))
    {
    case VAR_DECL:
      if (current_function_decl != NULL_TREE)
	error_with_decl (decl, "\
a data area attribute cannot be specified for local variables");
      
      /* Drop through.  */

    case FUNCTION_DECL:
      area = c33_get_data_area (decl);
      if (area != DATA_AREA_NORMAL && data_area != area)
	error_with_decl (decl, "\
data area of '%s' conflicts with previous declaration");
      
      return 1;
      
    default:
      break;
    }
  
  return 0;
}
#endif
/* DEL K.Watanabe V1.7 <<<<<<< */


extern struct obstack * saveable_obstack;

void
c33_encode_data_area (decl)
     tree decl;
{
  char * str = XSTR (XEXP (DECL_RTL (decl), 0), 0);
  int    len = strlen (str);
  char * newstr;

  /* Map explict sections into the appropriate attribute */
  if (c33_get_data_area (decl) == DATA_AREA_NORMAL)
    {
      if (DECL_SECTION_NAME (decl))
	{
	  char * name = TREE_STRING_POINTER (DECL_SECTION_NAME (decl));
	  
	  if (streq (name, ".zdata") || streq (name, ".zbss"))
	    c33_set_data_area (decl, DATA_AREA_ZDA);

	  else if (streq (name, ".sdata") || streq (name, ".sbss"))
	    c33_set_data_area (decl, DATA_AREA_SDA);

	  else if (streq (name, ".tdata") || streq (name, ".tbss"))
	    c33_set_data_area (decl, DATA_AREA_TDA);

	  else if (streq (name, ".gdata") || streq (name, ".gbss"))
	    c33_set_data_area (decl, DATA_AREA_GDA);
	}

      /* If no attribute, support -mgda=n */
      else
	{

	  int size = int_size_in_bytes (TREE_TYPE (decl));
	  if (size <= 0)
	    ;

	  else if (size <= small_memory [(int) SMALL_MEMORY_GDA].max)
	    {
#if 0
	      if ((TREE_CODE (decl) == VAR_DECL
		    && TREE_READONLY (decl) && !TREE_SIDE_EFFECTS (decl))
	    	    && (!DECL_INITIAL (decl)
	    		|| TREE_CONSTANT (DECL_INITIAL (decl))))
	        /* C33: Do not change the section if it is "const". */
		;
	      else
	        /* C33: It is "DATA_AREA_GDA" if it is not "const". */
	        c33_set_data_area (decl, DATA_AREA_GDA);
#else
	      if ((TREE_CODE (decl) == VAR_DECL
		    && TREE_READONLY (decl) && !TREE_SIDE_EFFECTS (decl)))
		{
		  if (!DECL_INITIAL (decl))
		    {
		      ;
	            }
                  else if ((DECL_INITIAL (decl) == error_mark_node)
			   || (TREE_CONSTANT (DECL_INITIAL (decl))))
                    {
                      ;
                    }
                  else
                    {
                  /* C33: It is "DATA_AREA_GDA" if it is not "const". */
	              c33_set_data_area (decl, DATA_AREA_GDA);
                    }
                }
	      else
	        /* C33: It is "DATA_AREA_GDA" if it is not "const". */
	        c33_set_data_area (decl, DATA_AREA_GDA);
#endif
	    }
	}
      
      if (c33_get_data_area (decl) == DATA_AREA_NORMAL)
	return;
    }

  /* CHG K.Watanabe V1.7 >>>>>>> */
/* newstr = obstack_alloc (saveable_obstack, len + 2); */
  newstr = alloca (len + 2);
  /* CHG K.Watanabe V1.7 <<<<<<< */  

  strcpy (newstr + 1, str);

  switch (c33_get_data_area (decl))
    {
    case DATA_AREA_ZDA: *newstr = ZDA_NAME_FLAG_CHAR; break;
    case DATA_AREA_TDA: *newstr = TDA_NAME_FLAG_CHAR; break;
    case DATA_AREA_SDA: *newstr = SDA_NAME_FLAG_CHAR; break;
    case DATA_AREA_GDA: *newstr = GDA_NAME_FLAG_CHAR; break;
    default: abort ();
    }

  /* CHG K.Watanabe V1.7 >>>>>>> */  
/*  XSTR (XEXP (DECL_RTL (decl), 0), 0) = newstr; */
  XSTR (XEXP (DECL_RTL (decl), 0), 0) = ggc_alloc_string (newstr, len + 2);
  /* CHG K.Watanabe V1.7 <<<<<<< */    
}



/* Return nonzero if FUNC is an interrupt function as specified
   by the "interrupt" attribute.  */
int
c33_interrupt_function_p (void)
{
  tree a;
  int ret = 0;
  tree func = current_function_decl;	/* 001026 watanabe */
  if (c33_interrupt_cache_p)
    return c33_interrupt_p;
  if (TREE_CODE (func) != FUNCTION_DECL)
    return 0;
    
/* CHG K.Watanabe V1.7 >>>>>>> */    
/*  a = lookup_attribute ("interrupt_handler", DECL_MACHINE_ATTRIBUTES (func)); */
  a = lookup_attribute ("interrupt_handler", DECL_ATTRIBUTES (func));  
/* CHG K.Watanabe V1.7 <<<<<<< */  
  if (a != NULL_TREE)
    ret = 1;

  else
    {
/* CHG K.Watanabe V1.7 >>>>>>> */     
/*      a = lookup_attribute ("interrupt", DECL_MACHINE_ATTRIBUTES (func)); */
      a = lookup_attribute ("interrupt", DECL_ATTRIBUTES (func));
/* CHG K.Watanabe V1.7 <<<<<<< */       
      ret = a != NULL_TREE;
    }

  /* Its not safe to trust global variables until after function inlining has
     been done.  */
  if (reload_completed | reload_in_progress)
    c33_interrupt_p = ret;
  return ret;
}


/* C33: The following function is the quotation from mips.c. */
/* Choose the section to use for DECL.  RELOC is true if its value contains
   any relocatable expression.

   Some of the logic used here needs to be replicated in
   ENCODE_SECTION_INFO in mips.h so that references to these symbols
   are done correctly.  Specifically, at least all symbols assigned
   here to rom (.text and/or .rodata) must not be referenced via
   ENCODE_SECTION_INFO with %gprel, as the rom might be too far away.

   If you need to make a change here, you probably should check
   ENCODE_SECTION_INFO to see if it needs a similar change.  */
void
c33_select_section (decl, reloc)
     tree decl;
     int reloc;
{
  /* For embedded applications, always put an object in read-only data
     if possible, in order to reduce RAM usage.  */

  if ((TREE_CODE (decl) == VAR_DECL
        && TREE_READONLY (decl) && !TREE_SIDE_EFFECTS (decl)
        && DECL_INITIAL (decl)
        && (DECL_INITIAL (decl) == error_mark_node
	    || TREE_CONSTANT (DECL_INITIAL (decl))))
      && ! (flag_pic && reloc))
    {
      switch (c33_get_data_area (decl))
        {
        case DATA_AREA_ZDA:	rozdata_section (); break;
        case DATA_AREA_TDA:	rotdata_section (); break;
        case DATA_AREA_SDA:	rosdata_section (); break;
        case DATA_AREA_GDA:	rogdata_section (); break;
        default:	        READONLY_DATA_SECTION ();  break;
        }
    }

       /* Deal with calls from output_constant_def_contents.  */
  else if ((TREE_CODE (decl) != VAR_DECL
        && (TREE_CODE (decl) != STRING_CST
	    || !flag_writable_strings))
      && ! (flag_pic && reloc))
    READONLY_DATA_SECTION ();
  else
    {
      switch (c33_get_data_area (decl))
        {
        case DATA_AREA_ZDA:	zdata_section (); break;
        case DATA_AREA_TDA:	tdata_section (); break;
        case DATA_AREA_SDA:	sdata_section (); break;
        case DATA_AREA_GDA:	gdata_section (); break;
        default:	        data_section ();  break;
        }
    }
}


/* Shifts.  */
int
nshift_operator (x, mode)
     rtx x;
     enum machine_mode mode;

{
  switch (GET_CODE (x))
    {
    case ASHIFTRT:
    case LSHIFTRT:
    case ASHIFT:
    case ROTATE:
    case ROTATERT:
      return 1;
    default:
      return 0;
    }
}


/* Called from the .md file to emit code to do shifts.
   Returns a boolean indicating success
   (currently this is always TRUE).  */
int
expand_a_shift (mode, code, operands)
     enum machine_mode mode;
     int code;
     rtx operands[];

{
  emit_move_insn (operands[0], operands[1]);

  /* need a loop to get all the bits we want  - we generate the
     code at emit time, but need to allocate a scratch reg now  */
  if ((TARGET_C33ADV || TARGET_C33PE))
    emit_insn (gen_rtx (SET, VOIDmode, operands[0],
                        gen_rtx (code, mode, operands[0],
                                 operands[2])));
  else
    emit_insn (gen_rtx
  	       (PARALLEL, VOIDmode,
	        gen_rtvec (2,
			   gen_rtx (SET, VOIDmode, operands[0],
				    gen_rtx (code, mode, operands[0],
					     operands[2])),
			   gen_rtx (CLOBBER, VOIDmode,
				    gen_rtx (SCRATCH, QImode, 0)))));
  return 1;
}


/* Emit the assembler code for doing shifts.  */
/********************************************************************************************
Format		: char* emit_a_shift (code, operands)
Input		: code -- rtx of code
              operands -- rtx of operand
Output		: None
Return		: pointer for the output string
Explanation	: Output shift instruction called from "insn_outfun()".
*********************************************************************************************/
char *
emit_a_shift (code, operands)
     enum rtx_code code;
     rtx *operands;

{
  char *shift_type;
  rtx xoperands[2];
  static char mask_tab[] = {0x1, 0x3, 0x7, 0xf, 0x1f};

  switch (code)
    {
    case ASHIFTRT:
      shift_type = "sra";
      break;
    case LSHIFTRT:
      shift_type = "srl";
      break;
    case ASHIFT:
      shift_type = "sll";
      break;
    case ROTATE:
      shift_type = "rl";
      break;
    case ROTATERT:
      shift_type = "rr";
      break;
    }

  if (GET_CODE (operands[2]) == CONST_INT)
    {
      int n = INTVAL (operands[2]) & 0x1f;

      if ((code == LSHIFTRT) && (n > 27))
        {
          /* rl + and */
          xoperands[0] = operands[0];
          xoperands[1] = GEN_INT (32 - n);
          output_asm_insn ("rl\t%0,%1", xoperands);
          xoperands[1] = GEN_INT (mask_tab[31 - n]);
          output_asm_insn ("and\t%0,%1", xoperands);
        }
      else if ((code == ASHIFT) && (n > 27))
        {
          /* and + rr */
          xoperands[0] = operands[0];
          xoperands[1] = GEN_INT (mask_tab[31 - n]);
          output_asm_insn ("and\t%0,%1", xoperands);
          xoperands[1] = GEN_INT (32 - n);
          output_asm_insn ("rr\t%0,%1", xoperands);
        }
      else
        {
          /* C33: Inverse the direction if the value of rotate is 16 or more. */
          if ((code == ROTATE) && (n > 16))
            {
               n = 32 - n;
               shift_type = "rr";
            }
          else if ((code == ROTATERT) && (n > 16))
            {
               n = 32 - n;
               shift_type = "rl";
            }

          while (n > 8)
            {
              n -= 8;
              fprintf (asm_out_file, "\t%s", shift_type);
              output_asm_insn ("%0,8", operands);
            }
          xoperands[0] = operands[0];
          xoperands[1] = GEN_INT (n % 9);
          fprintf (asm_out_file, "\t%s", shift_type);
          output_asm_insn ("%0,%1", xoperands);
        }
    }
  else
    {				/* register */
      output_asm_insn ("ld.w\t%3,%2", operands);
      output_asm_insn ("and\t%3,0x1f", operands);
      output_asm_insn ("cmp\t%3,0x08", operands);
      fprintf (asm_out_file, "\tjrle\t4\n");
      fprintf (asm_out_file, "\t%s", shift_type);
      output_asm_insn ("%0,8", operands);
      fprintf (asm_out_file, "\tjp.d\t-3\n");
      output_asm_insn ("sub\t%3,8", operands);
      fprintf (asm_out_file, "\t%s", shift_type);
      output_asm_insn ("%0,%3", operands);
    }
  return "";
}


/* Print the options used in the assembly file.  */
static struct
{
  char *name;
  int value;
}
target_switches[] = TARGET_SWITCHES;

void
print_options (out)
     FILE *out;

{
  /* CHG K.Watanabe V1.7 >>>>>>> */    
  /*
  extern char *language_string;
  extern char *version_string;
  */
  char *language_string = "GNU C";
  extern const char version_string[];
  /* CHG K.Watanabe V1.7 <<<<<<< */  
  extern char **save_argv;
  int line_len;
  int len;
  int j;
  char **p;
  int mask = TARGET_DEFAULT;

  /* Allow assembly language comparisons with -mdebug eliminating the
     compiler version number and switch lists.  */
  fprintf (out, "\n;  %s %s", language_string, version_string);

#ifdef TARGET_VERSION_INTERNAL
  TARGET_VERSION_INTERNAL (out);

#endif /*  */
#ifdef __GNUC__
  fprintf (out, " compiled by GNU C\n\n");

#else /*  */
  fprintf (out, " compiled by CC\n\n");

#endif /*  */
  fprintf (out, ";  Cc1 defaults:");
  line_len = 32767;
  for (j = 0; j < sizeof target_switches / sizeof target_switches[0]; j++)
    {
      if (target_switches[j].name[0] != '\0'
	  && target_switches[j].value > 0
	  && (target_switches[j].value & mask) == target_switches[j].value)
	{
	  mask &= ~target_switches[j].value;
	  len = strlen (target_switches[j].name) + 1;
	  if (len + line_len > 79)
	    {
	      line_len = 2;
	      fputs ("\n; ", out);
	    }
	  fprintf (out, " -m%s", target_switches[j].name);
	  line_len += len;
	}
    }
  fprintf (out, "\n\n;  Cc1 arguments (-cpu = %s):", c33_cpu_string);
  line_len = 32767;
  for (p = &save_argv[1]; *p != (char *) 0; p++)
    {
      char *arg = *p;
      if (*arg == '-')
	{
	  len = strlen (arg) + 1;
	  if (len + line_len > 79)
	    {
	      line_len = 2;
	      fputs ("\n; ", out);
	    }
	  fprintf (out, " %s", *p);
	  line_len += len;
	}
    }
  fputs ("\n\n", out);
}


/* Emit either a label, .comm, or .lcomm directive, and mark
   that the symbol is used, so that we don't emit an .extern
   for it in asm_file_end.  */
void
declare_object (stream, name, init_string, final_string, size)
     FILE *stream;
     char *name;
     char *init_string;
     char *final_string;
     int size;

{
  fputs (init_string, stream);	/* "", "\t.comm\t", or "\t.lcomm\t" */
  assemble_name (stream, name);
  fprintf (stream, final_string, size);	/* ":\n", ",%u\n", ",%u\n" */
}


extern tree last_assemble_variable_decl;
extern int size_directive_output;

/* A version of asm_output_aligned_bss() that copes with the special
   data areas of the c33. */
/* Called via the macro ASM_OUTPUT_ALIGNED_BSS */
void
c33_output_aligned_bss (file, decl, name, size, align)
     FILE * file;
     tree decl;
     char * name;
     int size;
     int align;
{
  /* DEL K.Watanabe V1.7 >>>>>>> */
  /* ASM_GLOBALIZE_LABEL (file, name); */
  /* DEL K.Watanabe V1.7 <<<<<<< */
   
  switch (c33_get_data_area (decl))
    {
    case DATA_AREA_ZDA:
      zbss_section ();
      break;

    case DATA_AREA_SDA:
      sbss_section ();
      break;

    case DATA_AREA_TDA:
      tbss_section ();    /* C33: Though v850 has only "tdata", c33 has both. */
      break;
      
    case DATA_AREA_GDA:
      gbss_section ();
      break;	/* GNU-GCC-045 2002/3/6 watanabe */
      
    default:
      bss_section ();
      break;
    }
  
  ASM_OUTPUT_ALIGN (file, floor_log2 (align / BITS_PER_UNIT));
#ifdef ASM_DECLARE_OBJECT_NAME
  last_assemble_variable_decl = decl;
  ASM_DECLARE_OBJECT_NAME (file, name, decl);
#else
  /* Standard thing is just output label for the object.  */
  ASM_OUTPUT_LABEL (file, name);
#endif /* ASM_DECLARE_OBJECT_NAME */
  ASM_OUTPUT_SKIP (file, size ? size : 1);
}


/* Called via the macro ASM_OUTPUT_ALIGNED_DECL_COMMON */
void
c33_output_common (file, decl, name, size, align)
     FILE * file;
     tree decl;
     char * name;
     int size;
     int align;
{
  if (decl == NULL_TREE)
    {
      fprintf (file, "\t%s\t", COMMON_ASM_OP);
    }
  else
    {
      switch (c33_get_data_area (decl))
	{
	case DATA_AREA_ZDA:
	  fprintf (file, "\t%s\t", ZCOMMON_ASM_OP);
	  break;

	case DATA_AREA_SDA:
	  fprintf (file, "\t%s\t", SCOMMON_ASM_OP);
	  break;

	case DATA_AREA_TDA:
	  fprintf (file, "\t%s\t", TCOMMON_ASM_OP);
	  break;
      
	case DATA_AREA_GDA:
	  fprintf (file, "\t%s\t", GCOMMON_ASM_OP);
	  break;
      
	default:
	  fprintf (file, "\t%s\t", COMMON_ASM_OP);
	  break;
	}
    }
  
  assemble_name (file, name);
  fprintf (file, ",%u,%u\n", size, align / BITS_PER_UNIT);
}


/* Called via the macro ASM_OUTPUT_ALIGNED_DECL_LOCAL */
void
c33_output_local (file, decl, name, size, align)
     FILE * file;
     tree decl;
     char * name;
     int size;
     int align;
{
  fprintf (file, "\t%s\t", LOCAL_ASM_OP);
  assemble_name (file, name);
  fprintf (file, "\n");
  ASM_OUTPUT_ALIGNED_DECL_COMMON (file, decl, name, size, align);
}


/* Output assembly code for the start of the file.  */
void
asm_file_start (file)
     FILE *file;
{
  output_file_directive (file, main_input_filename);
  
  print_options (file);
}


/* Write a loop to move a constant number of bytes.
   Generate load/stores as follows:

   do {
     temp1 = src[0];
     temp2 = src[1];
     ...
     temp<last> = src[MAX_MOVE_REGS-1];
     dest[0] = temp1;
     dest[1] = temp2;
     ...
     dest[MAX_MOVE_REGS-1] = temp<last>;
     src += MAX_MOVE_REGS;
     dest += MAX_MOVE_REGS;
   } while (src != final);

   This way, no NOP's are needed, and only MAX_MOVE_REGS+3 temp
   registers are needed.

   Aligned moves move MAX_MOVE_REGS*4 bytes every (2*MAX_MOVE_REGS)+3
   cycles, unaligned moves move MAX_MOVE_REGS*4 bytes every
   (4*MAX_MOVE_REGS)+3 cycles, assuming no cache misses.  */

#define MAX_MOVE_REGS 2
#define MAX_MOVE_BYTES (MAX_MOVE_REGS * UNITS_PER_WORD)
static void
block_move_loop (dest_reg, src_reg, bytes, align, orig_src)
     rtx dest_reg;		/* register holding destination address */
     rtx src_reg;		/* register holding source address */
     int bytes;			/* # bytes to move */
     int align;			/* alignment */
     rtx orig_src;		/* original source for making a reg note */

{
  rtx dest_mem = gen_rtx (MEM, BLKmode, dest_reg);
  rtx src_mem = gen_rtx (MEM, BLKmode, src_reg);
  rtx align_rtx = GEN_INT (align);
  rtx label;
  rtx final_src;
  rtx bytes_rtx;
  int leftover;

  /* C33: If transmission byte is less than 16 byte. */
  if (bytes < 2 * MAX_MOVE_BYTES)

    /* C33: Stop processing. */
    abort ();
  leftover = bytes % MAX_MOVE_BYTES;
  bytes -= leftover;

  /* C33: Create the label rtx. */
  label = gen_label_rtx ();

  /* C33: Create the register rtx which saves the transmission end address. */
  final_src = gen_reg_rtx (Pmode);
  bytes_rtx = GEN_INT (bytes);

  /* C33: Emit the rtx whick decides the transmission end address. */
  emit_insn (gen_addsi3 (final_src, src_reg, bytes_rtx));

  /* C33: Emit the label rtx. */
  emit_label (label);
  bytes_rtx = GEN_INT (MAX_MOVE_BYTES);

  /* C33: Emit the rtx of transmission instruction array. */
  emit_insn (gen_movstrsi_internal (dest_reg, src_reg, bytes_rtx, align_rtx));
  /* C33: Emit the rtx of "cmp" instruction. */
  emit_insn (gen_cmpsi (src_reg, final_src));

  /* C33: Emit the rtx of "jump" instruction. */
  emit_jump_insn (gen_bne (label));
  if (leftover)

    /* C33: Emit the rtx of transmission instruction array. */
    emit_insn (gen_movstrsi_internal
	       (dest_reg, src_reg, GEN_INT (leftover), align_rtx));
}


/* Use a library function to move some bytes.  */
static void
block_move_call (dest_reg, src_reg, bytes_rtx)
     rtx dest_reg;
     rtx src_reg;
     rtx bytes_rtx;

{

  /* C33: If it is not "VOIDmode", and not "Pmode". */
  /* We want to pass the size as Pmode, which will normally be SImode
     but will be DImode if we are using 64 bit longs and pointers.  */
  if (GET_MODE (bytes_rtx) != VOIDmode && GET_MODE (bytes_rtx) != Pmode)

    /* C33: Yes, it is not "VOIDmode", and not "Pmode". */

    /* C33: Change the mode to Pmode. */
    bytes_rtx = convert_to_mode (Pmode, bytes_rtx, TRUE);

#ifdef TARGET_MEM_FUNCTIONS
  /* C33: Emit the rtx for calling memcpy library. */
  emit_library_call (gen_rtx (SYMBOL_REF, Pmode, "memcpy"), 0, VOIDmode, 3,
		     dest_reg, Pmode, src_reg, Pmode, bytes_rtx, Pmode);

#else /*  */
  /* C33: Emit the rtx for calling bcopy library. */
  emit_library_call (gen_rtx (SYMBOL_REF, Pmode, "bcopy"), 0, VOIDmode, 3,
		     src_reg, Pmode, dest_reg, Pmode, bytes_rtx, Pmode);

#endif /*  */

  /* C33: Terminating process. */
}


/* Expand string/block move operations.

   operands[0] is the pointer to the destination.
   operands[1] is the pointer to the source.
   operands[2] is the number of bytes to move.
   operands[3] is the alignment.  */
void
expand_block_move (operands)
     rtx operands[];

{
  rtx bytes_rtx = operands[2];
  rtx align_rtx = operands[3];
  int constp = (GET_CODE (bytes_rtx) == CONST_INT);
  int bytes = (constp ? INTVAL (bytes_rtx) : 0);
  int align = INTVAL (align_rtx);
  rtx orig_src = operands[1];
  rtx src_reg;
  rtx dest_reg;

  /* C33: If the transmission byte is less than 0. */
  if (constp && bytes <= 0)

    /* C33: Terminating process. */
    return;

  /* C33: If alignment is more than word. */
  if (align > UNITS_PER_WORD)

    /* C33: Fix to the word unit. */
    align = UNITS_PER_WORD;

  /* C33: Set the pair of scratch registers. */
  /* Move the address into scratch registers.  */
  dest_reg = copy_addr_to_reg (XEXP (operands[0], 0));
  src_reg = copy_addr_to_reg (XEXP (orig_src, 0));

  /* C33:
     IF   Optimize is enabled.
          There is The transmission data.
          The number of transmission is 2 or less. */
  if (constp && optimize && bytes != 0 && (bytes / align) <= 2)

    /* C33: Yes, the number of transmission is 2 or less. */

    /* C33: Emit the rtx of transmission instruction array. */
    emit_insn (gen_movstrsi_internal
	       (dest_reg, src_reg, bytes_rtx, align_rtx));
  else
    /* C33, No, the number of transmission is 3 or more. */

    /* IF
          There is "-mno-memcpy" option.( It means default ) */
  if (TARGET_MEMCPY)

    /* YES memcpy */

    /* C33: Emit the rtx for calling memcopy library. */
    block_move_call (dest_reg, src_reg, bytes_rtx);


  /* C33: There is "-mno-memcpy" option.
          ELSE IF     the transmission byte is 16 or less */
  else if (constp && bytes <= 2 * MAX_MOVE_BYTES)
    /* C33: Yes, the transmission byte is 16 or less. */

    /* C33: Emit the rtx of transmission instruction array. */
    emit_insn (gen_movstrsi_internal
	       (dest_reg, src_reg, bytes_rtx, align_rtx));
  
  /* C33: ELSE IF
          The alignment is word and the optimize is enabled. */
  else if (constp && align >= UNITS_PER_WORD && optimize)

    /* YES */

    /* C33: Create the rtl sequence of the block transmission loop. */
    block_move_loop (dest_reg, src_reg, bytes, align, orig_src);

  /* C33: ELSE IF
          The alignment is not word and the optimize is enabled. */
  else if (constp && optimize)

    {

      /* If the alignment is not word aligned, generate a test at
         runtime, to see whether things wound up aligned, and we
         can use the faster lw/sw instead ulw/usw.  */
      rtx temp = gen_reg_rtx (Pmode);
      rtx aligned_label = gen_label_rtx ();
      rtx join_label = gen_label_rtx ();
      int leftover = bytes % MAX_MOVE_BYTES;
      bytes -= leftover;

      /* C33: Create the instructions which are address calculation and
              jump destination calculation for transmitting by loop. */
      emit_insn (gen_iorsi3 (temp, src_reg, dest_reg));
      emit_insn (gen_andsi3 (temp, temp, GEN_INT (UNITS_PER_WORD - 1)));
      emit_insn (gen_cmpsi (temp, const0_rtx));
      emit_jump_insn (gen_beq (aligned_label));

      /* C33: Create the transmission loop which transmits 1byte at a time. */
      /* Unaligned loop.  */
      block_move_loop (dest_reg, src_reg, bytes, 1, orig_src);
      emit_jump_insn (gen_jump (join_label));
      emit_barrier ();

      /* C33: Create the transmission loop by word unit. */
      /* Aligned loop.  */
      emit_label (aligned_label);
      block_move_loop (dest_reg, src_reg, bytes, UNITS_PER_WORD, orig_src);
      emit_label (join_label);

      /* C33: Create the transmission instruction which can transmit 
                                               less than the loop tranmit by word unit. */
      /* Bytes at the end of the loop.  */
      if (leftover)

	{
	  emit_insn (gen_movstrsi_internal
		     (dest_reg, src_reg, GEN_INT (leftover),
		      GEN_INT (align)));}
    }

  else
    /* C33: Yes, the specification of byte number is not constants 
            or does not have the optimization. */
    
    /* C33: Emit the rtx for calling memcopy library. */
    block_move_call (dest_reg, src_reg, bytes_rtx);
}


/********************************************************************************************
Format		: char* output_block_move (insn, operands, num_regs)
Input		: 
Output		: None
Return		: 
Explanation	: Output load instruction called from "insn_outfun()".
*********************************************************************************************/
char *
output_block_move (insn, operands, num_regs)
     rtx insn;
     rtx operands[];
     int num_regs;		/* C33: The number of registers which can be used for transmission. */
{
  rtx dest_reg = operands[0];
  rtx src_reg = operands[1];
  int bytes = INTVAL (operands[2]);
  int align = INTVAL (operands[3]);
  int num = 0;
  int last_operand = num_regs + 4;
  int safe_regs = 4;
  int i;
  rtx xoperands[10];
  struct
  {
    char *load;			/* load insn without nop */
    char *store;		/* store insn */
    enum machine_mode mode;	/* mode to use on (MEM) */
  }
  load_store[4];

  /* Detect a bug in GCC, where it can give us a register
     the same as one of the addressing registers and reduce
     the number of registers available.  */
  for (i = 4;
       i < last_operand
       && safe_regs < (sizeof (xoperands) / sizeof (xoperands[0])); i++)

    {
      if (!reg_mentioned_p (operands[i], operands[0])
	  && !reg_mentioned_p (operands[i], operands[1]))
	xoperands[safe_regs++] = operands[i];
    }
  if (safe_regs < last_operand)

    {
      xoperands[0] = operands[0];
      xoperands[1] = operands[1];
      xoperands[2] = operands[2];
      xoperands[3] = operands[3];
      return output_block_move (insn, xoperands, safe_regs - 4);
    }

  /* If we are given global or static addresses, and we would be
     emitting a few instructions, try to save time by using a
     temporary register for the pointer.  */

  /* C33:  IF   The number of registers which can be used for transmission
                is 3 or more, and the numer of transmission is 3 or more.   */
  if (num_regs > 2 && (bytes > 2 * align))

    {
      /* C33: IF  The address of memory from which to transmit is the label or the constants. */
      if (CONSTANT_P (src_reg))
	{

	  /* C33: Get the "template" register. */
	  src_reg = operands[3 + num_regs--];

	  /* C33: Output the instruction which loads the address of memory 
	          from which to transmit to the "template" register.      */
	  xoperands[1] = operands[1];
	  xoperands[0] = src_reg;
	  output_asm_insn ("xld.w\t%0,%1", xoperands);
	}

      /* C33: IF  The address of memory to which to transmit is the label or the symbols or the constants. */
      if (CONSTANT_P (dest_reg))
	{

	  /* C33: Get the "template" register. */
	  dest_reg = operands[3 + num_regs--];

	  /* C33: Output the instruction which loads the address of memory 
	          from which to trasmit to the "template" register.          */
	  xoperands[1] = operands[0];
	  xoperands[0] = dest_reg;
	  output_asm_insn ("xld.w\t%0,%1", xoperands);
	}
    }

  /* C33:  IF   The number of registers which can be used for transmission is 4 or more. */
  if (num_regs > (sizeof (load_store) / sizeof (load_store[0])))

    /* C33: The number of registers which can be used shall be 4 or less. */
    num_regs = (sizeof (load_store) / sizeof (load_store[0]));

  /* C33: WHILE       Until all transmitted data is output. */
  while (bytes > 0)

    {

      /* C33: IF    The transmission bytes and alignment are 4byte or more. */
      if (bytes >= 4 && align >= 4)

	{

	  /* C33: Transmit by word unit. */
	  load_store[num].load = "ld.w\t%0,[%1]+";
	  load_store[num].store = "ld.w\t[%1]+,%0";
	  load_store[num].mode = SImode;
	  bytes -= 4;
	}

      /* C33:  ELSE IF    The transmission bytes and alignment are 2byte or more. */
      else if (bytes >= 2 && align >= 2)

	{
	  /* C33: Transmit by half unit. */
	  load_store[num].load = "ld.h\t%0,[%1]+";
	  load_store[num].store = "ld.h\t[%1]+,%0";
	  load_store[num].mode = HImode;
	  bytes -= 2;
	}

      /* C33: ELSE  transmit except by word or by half. */
      else

	{

	  /* byte転送 */
	  load_store[num].load = "ld.b\t%0,[%1]+";
	  load_store[num].store = "ld.b\t[%1]+,%0";
	  load_store[num].mode = QImode;
	  bytes--;
	}

      /* Emit load/stores now if we have run out of registers or are
         at the end of the move.  */

      /* C33: IF     All registers for transmission have been used, 
                     or all instructions which transmit all data have been output.        */
      if (++num == num_regs || bytes == 0)

	{

	  /* C33: Output the load instruction. */
	  for (i = 0; i < num; i++)

	    {
	      if (!operands[i + 4])
		abort ();

	      /* C33: IF   The mode of "temp" register which saves temporarily
	                   the content of memory to transmit is different from the mode of data to transmit. */
	      if (GET_MODE (operands[i + 4]) != load_store[i].mode)

		/* C33: Change the mode of "temp" register which saves temporarily the data. */
		operands[i + 4] =
		  gen_rtx (REG, load_store[i].mode, REGNO (operands[i + 4]));

	      /* C33: Output the instruction which loads the content of memory 
	              from which to transmit to "temp" register.                */
	      xoperands[0] = operands[i + 4];
	      xoperands[1] = gen_rtx (MEM, load_store[i].mode, src_reg);
	      output_asm_insn (load_store[i].load, xoperands);
	    }

	  /* C33: Output the store instruction. */
	  for (i = 0; i < num; i++)

	    {
	      /* C33: Output the instruction which loads the content of "temp" register 
	              to the memory to which to transmit.                               */
	      xoperands[0] = operands[i + 4];
	      xoperands[1] = gen_rtx (MEM, load_store[i].mode, dest_reg);
	      output_asm_insn (load_store[i].store, xoperands);
	    }

	  /* C33: Set 0 to the register number which is used for transmission. */
	  num = 0;		/* reset load_store */
	}
    }

  /* C33: Terminating process. */
  return "";
}


/* Given INSN and its current length LENGTH, return the adjustment
   (in bytes) to correctly compute INSN's length.

   We use this to get the lengths of various memory references correct.  */
/*****************************************************************************************************
Format		: int c33_adjust_insn_length (insn, length)
Input		: rtx insn   -- INSN checked.
              int length -- Unused.
Output		: None
Return		: The instruction lengths corrected.( by byte unit )
Explanation	: ・Called from "shorten_branches()".
              ・Correct the instruction lengths.
              ・It is not necessary for adjusting here, because "m -> r" and "r -> m" 
                are not created in the case of "-medda32" && "defaut data area".
*****************************************************************************************************/
// CHG K.Watanabe V1.8 >>>>>>>
#if 0
c33_adjust_insn_length (insn, length)
#endif

int c33_adjust_insn_length (insn, length)
// CHG K.Watanabe V1.8 <<<<<<<
     rtx insn;
     int length;
{
  rtx pat;
  pat = PATTERN (insn);

  /* Ajust length for symbol/label->reg. */
  if (GET_CODE (pat) == SET
      && (GET_CODE (SET_DEST (pat)) != PC)
      && (GET_CODE (SET_SRC (pat)) == LABEL_REF
          || GET_CODE (SET_SRC (pat)) == SYMBOL_REF
          || GET_CODE (SET_SRC (pat)) == CONST))
    {
      /* This insn might need a length adjustment.  */
      rtx addr;
      char *name;

	  /***************************************************/
	  /* C33: Get the symobl address.                    */
	  /***************************************************/
      if (GET_CODE (SET_SRC (pat)) == SYMBOL_REF
          || GET_CODE (SET_SRC (pat)) == LABEL_REF)
        name = XSTR (SET_SRC (pat), 0);
      else
        name = XSTR (XEXP (XEXP (SET_SRC (pat), 0), 0), 0);

      if (ENCODED_NAME_P (name))
			if ((TARGET_EXT_ZDA && ZDA_NAME_P (name))
	    		|| (TARGET_EXT_TDA && TDA_NAME_P (name))
	    		|| (TARGET_EXT_SDA && SDA_NAME_P (name)))
	  			return 0;		/* [stz] data area extended */
			else
          		return -2;		/* [stzg] data area */
      else
        	if (TARGET_C33ADV)
          		return 2;		/* default data area */
        	else
          		return 0; 
    }

  /* Adjust length for reg->mem and mem->reg copies.  */
  if (GET_CODE (pat) == SET
      && (GET_CODE (SET_SRC (pat)) == MEM
	  || GET_CODE (SET_DEST (pat)) == MEM))
    {
      /* This insn might need a length adjustment.  */
      rtx addr;
      int mode;
      char *name;

      if (GET_CODE (SET_SRC (pat)) == MEM)
	{
	  addr = XEXP (SET_SRC (pat), 0);
	  mode = GET_MODE (SET_SRC (pat));
	}
      else
	{
	  addr = XEXP (SET_DEST (pat), 0);
	  mode = GET_MODE (SET_DEST (pat));
	}

      switch (mode)
        {
	case DFmode:
	case DImode:
	  if (GET_CODE (addr) == REG)
	    return -6;
	  else if (GET_CODE (addr) == PLUS)
	    return 0;
	  else
	    {
	      printf("c33_adjust_insn_length: MEM\n");// debug
	      return 0;
	    }
	  break;

	default:
	  switch (GET_CODE (addr))
            {
	    case REG:
	      return -4;
	      break;

	    case PLUS:
              if (GET_CODE (XEXP (addr, 0)) == REG
	          && REGNO (XEXP (addr, 0)) == STACK_POINTER_REGNUM
	          && GET_CODE (XEXP (addr, 1)) == CONST_INT)
	        {
	          switch (mode)
                    {
	            case QImode:
	              if (CONST_OK_FOR_IMM6 (INTVAL (XEXP (addr, 1))))
	                return -4;
	              if (CONST_OK_FOR_IMM19 (INTVAL (XEXP (addr, 1))))
	                return -2;
	              break;

	            case HImode:
	              if (CONST_OK_FOR_IMM7 (INTVAL (XEXP (addr, 1))))
	                return -4;
	              if (CONST_OK_FOR_IMM19 (INTVAL (XEXP (addr, 1))))
	                return -2;
	              break;

	            case SImode:
	            default:
	              if (CONST_OK_FOR_IMM8 (INTVAL (XEXP (addr, 1))))
	                return -4;
	              if (CONST_OK_FOR_IMM19 (INTVAL (XEXP (addr, 1))))
	                return -2;
	              break;
	            }
	        }
	      else
	        {
	          if (CONST_OK_FOR_IMM13 (INTVAL (XEXP (addr, 1))))
	            return -2;
	        }
	      break;

	  /****************************************************************/
	  /* C33: Reference the symbol.( loading memory / saving memory ) */
	  /****************************************************************/
	    case SYMBOL_REF:
	      	name = XSTR (addr, 0);

	      	/* C33: It is -2 except in the case of "dafault data area". */
	      	if (ENCODED_NAME_P (name))
	        	if ((TARGET_EXT_ZDA && ZDA_NAME_P (name))
	            	|| (TARGET_EXT_TDA && TDA_NAME_P (name))
	            	|| (TARGET_EXT_SDA && SDA_NAME_P (name)))
	          		return 0;		/* [stz] data area extended */
	        	else
                  	return -2;		/* [stzg] data area */
	      	else
	      		if (TARGET_C33ADV)
	          		return 2;		/* default data area */
	        	else
	          		return 0;
	      	break;

	    case CONST:
	    	name = XSTR (XEXP (XEXP (addr, 0), 0), 0);

	      	/* It is -2 except in the case of "dafault data area". */
	      	if (ENCODED_NAME_P (name))
	      		if ((TARGET_EXT_ZDA && ZDA_NAME_P (name))
	            	|| (TARGET_EXT_TDA && TDA_NAME_P (name))
	            	|| (TARGET_EXT_SDA && SDA_NAME_P (name)))
	          		return 0;		/* [stz] data area extended */
	        	else
                  	return -2;		/* [stzg] data area */
	      	else
	        	if (TARGET_C33ADV)
	          		return 2;		/* default data area */
	        	else
	          		return 0;
	      	break;
	    	}
        }
        
    /* ADD K.Watanabe V1.4 >>>>>>> */    
    } else {
    	/*******************************************************************************************/
    	/*   ①SIGN_EXTEND( MEM ) -> REG 														   */
    	/*     C33: In the case that the argument is the global variable of "char" / "short" type. */
    	/*   ②ZERO_EXTEND( MEM ) -> REG 														   */
    	/*     C33: In the case that the argument is the global variable of "unsigned char" /      */
    	/*          "unsigned short" type.                                                         */
    	/*******************************************************************************************/
    	rtx addr;
      	int mode;
      	char *name;
		int i_chk_flg;
		
		i_chk_flg = 0; 
		if( GET_CODE (pat) == SET ){
			if( GET_CODE (SET_DEST (pat)) == REG ){
				if( ( GET_CODE (SET_SRC (pat)) == SIGN_EXTEND ) || ( GET_CODE (SET_SRC (pat)) == ZERO_EXTEND ) ){
					if( GET_CODE( SUBREG_REG( SET_SRC (pat) ) ) == MEM ){
						i_chk_flg = 1;
						addr = XEXP( SUBREG_REG( SET_SRC (pat) ),0 );
						mode = GET_MODE ( SUBREG_REG( SET_SRC (pat) ) );							
					}
				}
			}			
		}	
		
		/* SIGN_EXTEND( MEM ) -> REG */
		/* ZERO_EXTEND( MEM ) -> REG */
		if( i_chk_flg == 1 ){
	    	switch (mode){
				case DFmode:
				case DImode:
			  		break;
			  		
				default:
			  		switch (GET_CODE (addr)){
			    		case REG:
			    		case PLUS:
			      			break;

				  		/****************************************************************/
				   	    /* C33: Reference the symbol.( loading memory / saving memory ) */
				  		/****************************************************************/
				    	case SYMBOL_REF:
				      		name = XSTR (addr, 0);
				      		
			      			/* It is -2 except in the case of "dafault data area". */
				      		if ( ( ENCODED_NAME_P (name) ) ){
				      			if( (TARGET_EXT_ZDA && ZDA_NAME_P (name))
					            		|| (TARGET_EXT_TDA && TDA_NAME_P (name))
					            		|| (TARGET_EXT_SDA && SDA_NAME_P (name)) ){
					          		return 0;		/* [stz] data area extended */
					        	} else {
				                  	return -2;		/* [stzg] data area */
				                }  	
				      		} 		          		
				      		break;
				      		
				    	case CONST:
				    		name = XSTR (XEXP (XEXP (addr, 0), 0), 0);

							/* It is -2 except in the case of "dafault data area". */
				      		if ( ( ENCODED_NAME_P (name) ) ){
				      			if( (TARGET_EXT_ZDA && ZDA_NAME_P (name))
					            		|| (TARGET_EXT_TDA && TDA_NAME_P (name))
					            		|| (TARGET_EXT_SDA && SDA_NAME_P (name)) ){
					          		return 0;		/* [stz] data area extended */
					        	} else {
				                  	return -2;		/* [stzg] data area */
				                }  	
				      		} 
				      		break;
				    }	
		    }
		}		    
    }	
   	/* ADD K.Watanabe V1.4 <<<<<<< */
   	
   	/* ADD K.Watanabe V1.7 >>>>>>> */
   	/* C33: Add the process because the adjustment of instruction lengths was not described
   	        in the case of outputting multiple shift instrcutions.                          */
   	        
	if( ( GET_CODE (pat) == PARALLEL )
		&& ( GET_CODE (XVECEXP (pat, 0, 0)) == SET ) ){
		
      	rtx p = XVECEXP (pat, 0, 0);
  	   	enum rtx_code code;
      	code = GET_CODE (SET_SRC (p));
		if( ( code == ASHIFTRT )
	      		|| ( code == LSHIFTRT )
	      		|| ( code == ASHIFT )
	      		|| ( code == ROTATE )
	      		|| ( code == ROTATERT ) ) {
			if( GET_CODE( XEXP (SET_SRC (p), 1) ) == CONST_INT ){
		      	int n = INTVAL( (XEXP (SET_SRC (p), 1)) ) & 0x1f;
				int i_ret = 0;
				
				if ((code == LSHIFTRT) && (n > 27)){
          			;
        		} else if ((code == ASHIFT) && (n > 27)){
          			;
        		} else {
          			/* C33: Inverse the rotate in the case of 16 or more. */
          			if ((code == ROTATE) && (n > 16)){
               			n = 32 - n;
            		} else if ((code == ROTATERT) && (n > 16)) {
              			n = 32 - n;
            		}
			        while (n > 8){
			        	n -= 8;
					    i_ret += 2;
			        }
			    }		        
			    return i_ret;
		    }
		}
    }
	/* ADD K.Watanabe V1.7 <<<<<<< */
   	   	
  	return 0;
}


/* ADD K.Watanabe V1.7 >>>>>>> */
const struct attribute_spec c33_attribute_table[] =
{
  /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler } */
  { "interrupt_handler", 0, 0, true,  false, false, c33_handle_interrupt_attribute },
  { "section",           0, 0, true,  false, false, c33_handle_section_attribute },  
  { "sda",               0, 0, true,  false, false, c33_handle_data_area_attribute },
  { "tda",               0, 0, true,  false, false, c33_handle_data_area_attribute },
  { "zda",               0, 0, true,  false, false, c33_handle_data_area_attribute },  
  { NULL,                0, 0, false, false, false, NULL }
};


/* Handle an "interrupt" attribute; arguments as in            */
/*  struct attribute_spec.handler.                             */
/* C33: Quoted from v850.                                      */
static tree
c33_handle_interrupt_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name;
     tree args ATTRIBUTE_UNUSED;
     int flags ATTRIBUTE_UNUSED;
     bool *no_add_attrs;
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning ("`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}


static tree
c33_handle_section_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name;
     tree args ATTRIBUTE_UNUSED;
     int flags ATTRIBUTE_UNUSED;
     bool *no_add_attrs;
{
  if (TREE_CODE (*node) != FUNCTION_DECL)
    {
      warning ("`%s' attribute only applies to functions",
	       IDENTIFIER_POINTER (name));
      *no_add_attrs = true;
    }

  return NULL_TREE;
}


/* Handle a "sda", "tda" or "zda" attribute; arguments as in   */
/*  struct attribute_spec.handler.                             */
/* C33: Quoted from v850.
        The following function is correspond to that which is removed "interrupt_handler"
        from c33_valid_machine_decl_attribute() in v2.95.2.                                */
static tree
c33_handle_data_area_attribute (node, name, args, flags, no_add_attrs)
     tree *node;
     tree name;
     tree args ATTRIBUTE_UNUSED;
     int flags ATTRIBUTE_UNUSED;
     bool *no_add_attrs;
{
  c33_data_area data_area;
  c33_data_area area;
  tree decl = *node;
  
  /* Implement data area attribute.  */
  if (is_attribute_p ("sda", name))
    {
      if (gp_max < 4)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'sda'.");
          return NULL_TREE;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'sda'.");
          return NULL_TREE;
        }
      else
        data_area = DATA_AREA_SDA;
    }
  else if (is_attribute_p ("tda", name))
    {
      if (gp_max < 3)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'tda'.");
          return NULL_TREE;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'tda'.");
          return NULL_TREE;
        }
      else
        data_area = DATA_AREA_TDA;
    }
  else if (is_attribute_p ("zda", name))
    {
      if (gp_max < 2)
        {
          error_with_decl (decl, "data area of '%s' cannot be specified for 'zda'.");
          return NULL_TREE;
        }
      else if (TREE_CODE (decl) == FUNCTION_DECL)
        {
          error_with_decl (decl, "'%s' cannot be specified for 'zda'.");
          return NULL_TREE;
        }
      else
        data_area = DATA_AREA_ZDA;
    }
  else
    abort ();
  
  switch (TREE_CODE (decl))
    {
    case VAR_DECL:
      if (current_function_decl != NULL_TREE)
	error_with_decl (decl, "\
a data area attribute cannot be specified for local variables");
      
      /* Drop through.  */

    case FUNCTION_DECL:
      area = c33_get_data_area (decl);
      if (area != DATA_AREA_NORMAL && data_area != area)
	error_with_decl (decl, "\
data area of '%s' conflicts with previous declaration");
      
      break;
      
    default:
      break;
    }
  
  return NULL_TREE;
}


static void
c33_encode_section_info (decl, first)
     tree decl;
     int first;
{
  if (first && TREE_CODE (decl) == VAR_DECL
      && (TREE_STATIC (decl) || DECL_EXTERNAL (decl)))
    c33_encode_data_area (decl);
}


static void
c33_asm_out_constructor (symbol, priority)
     rtx symbol;
     int priority ATTRIBUTE_UNUSED;
{
    ctors_section ();							
    fprintf (asm_out_file, "\t%s\t ", ".long");	
    assemble_name (asm_out_file, XSTR (symbol, 0));				
    fprintf (asm_out_file, "\n");					
}


static void
c33_asm_out_destructor (symbol, priority)
     rtx symbol;
     int priority ATTRIBUTE_UNUSED;
{
    dtors_section ();                   		
    fprintf (asm_out_file, "\t%s\t ", ".long");		
    assemble_name (asm_out_file, XSTR (symbol, 0));              	
    fprintf (asm_out_file, "\n");						
}

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
/*****************************************************************************************************
Format		: static void c33_unique_section (tree decl,int reloc)
Input		: tree decl
              int reloc
Output		: None
Return		: None
Explanation	: ・Output ".gnu.linkonce." section.
              ・Created from mips_unique_section() in mips.c.
              ・Do not output ".gnu.linkonce." section of "S/T/Z/G" data area 
                because "S/T/Z/G" data area is not supported no more.
                The following function is described as comment for preparation in the case that 
                "S/T/Z/G" data area is supported.
                If "S/T/Z/G" data area is supported in the future, the description is needed 
                in the "*.lds" file for ".gnu.linkonce." section of "S/T/Z/G" data area.
*****************************************************************************************************/
static void
c33_unique_section (decl, reloc)
     tree decl;
     int reloc;
{
  int len;							
  char *name, *string, *prefix;					
  static const char *const cp_prefixes[11][2] = {
    { ".text.", ".gnu.linkonce.t." },
    { ".rozdata.", ".gnu.linkonce.zr." },
    { ".rotdata.", ".gnu.linkonce.tr." },
    { ".rosdata.", ".gnu.linkonce.sr." },
    { ".rodata.", ".gnu.linkonce.r." },
    { ".rodata.", ".gnu.linkonce.r." },
    { ".zdata.", ".gnu.linkonce.zd." },
    { ".tdata.", ".gnu.linkonce.td." },
    { ".sdata.", ".gnu.linkonce.sd." },
    { ".gdata.", ".gnu.linkonce.gd." },
    { ".data.", ".gnu.linkonce.d." }
  };
  int i_sec;
  			
  	i_sec = -1;							
	name = IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (decl));	
	name = (* targetm.strip_name_encoding) (name);		
					
    if (TREE_CODE (decl) == FUNCTION_DECL){
		i_sec = 0;	
	} else if (DECL_INITIAL (decl) == 0
           || DECL_INITIAL (decl) == error_mark_node) {
		switch (c33_get_data_area (decl)){
	    	case DATA_AREA_ZDA:	
				// i_sec = 6;			// C33: Not supported.
	    		break;
	        case DATA_AREA_TDA:	
				// i_sec = 7;			// C33: Not supported.
	        	break;
	        case DATA_AREA_SDA:	
				// i_sec = 8;			// C33: Not supported.
	        	break;
	        case DATA_AREA_GDA:	
				// i_sec = 9;			// C33: Not supported.
	        	break;
	        default:
				i_sec = 10;
	        	break;	
        }		
    } else if (decl_readonly_section (decl, reloc)) {
		switch (c33_get_data_area (decl)){
	    	case DATA_AREA_ZDA:	
				// i_sec = 1;			// C33: Not supported.
	    		break;
	        case DATA_AREA_TDA:	
				// i_sec = 2;			// C33: Not supported.
	        	break;
	        case DATA_AREA_SDA:	
				// i_sec = 3;			// C33: Not supported.
	        	break;
	        case DATA_AREA_GDA:	
				// i_sec = 4;			// C33: Not supported.
	        	break;
	        default:
				i_sec = 5;
	        	break;	
        }
    } else {						
		switch (c33_get_data_area (decl)){
	    	case DATA_AREA_ZDA:	
				// i_sec = 6;			// C33: Not supported.
	    		break;
	        case DATA_AREA_TDA:	
				// i_sec = 7;			// C33: Not supported.
	        	break;
	        case DATA_AREA_SDA:	
				// i_sec = 8;			// C33: Not supported.
	        	break;
	        case DATA_AREA_GDA:	
				// i_sec = 9;			// C33: Not supported.
	        	break;
	        default:
				i_sec = 10;
	        	break;	
        }				
    }							
	    
	if( i_sec != -1 ){
	    prefix = cp_prefixes[i_sec][DECL_ONE_ONLY (decl)];
	      	
	  	len = strlen (name) + strlen (prefix);		
	  	string = alloca (len + 1);					
	  	sprintf (string, "%s%s", prefix, name);			
										
	  	DECL_SECTION_NAME (decl) = build_string (len, string);	
	}	  	
}
#endif

/* C33: Do not output "gnu.linkonce." because the address calculation is sometimes not correct
        at the time of linking in V1.7 release.                                                */
static void
c33_unique_section (decl, reloc)
     tree decl;
     int reloc;
{
	
}
/* CHG K.Watanabe V1.7 <<<<<<< */

/* When assemble_integer is used to emit the offsets for a switch
   table it can encounter (TRUNCATE:HI (MINUS:SI (LABEL_REF:SI) (LABEL_REF:SI))).
   output_addr_const will normally barf at this, but it is OK to omit
   the truncate and just emit the difference of the two labels.  The
   .hword directive will automatically handle the truncation for us.
   Returns 1 if rtx was handled, 0 otherwise.  */
/* C33: Quoted from v850. */

int
c33_output_addr_const_extra (file, x)
     FILE * file;
     rtx x;
{
  if (GET_CODE (x) != TRUNCATE)
    return 0;

  x = XEXP (x, 0);

  /* We must also handle the case where the switch table was passed a
     constant value and so has been collapsed.  In this case the first
     label will have been deleted.  In such a case it is OK to emit
     nothing, since the table will not be used.
     (cf gcc.c-torture/compile/990801-1.c).  */
  if (GET_CODE (x) == MINUS
      && GET_CODE (XEXP (x, 0)) == LABEL_REF
      && GET_CODE (XEXP (XEXP (x, 0), 0)) == CODE_LABEL
      && INSN_DELETED_P (XEXP (XEXP (x, 0), 0)))
    return 1;

  output_addr_const (file, x);
  return 1;
}

/* ADD K.Watanabe V1.7 <<<<<<< */


// ADD K.Watanabe V1.8 >>>>>>>
static const char *
c33_strip_name_encoding (str)
     const char *str;
{
  return str + ENCODED_NAME_P (str);
}
// ADD K.Watanabe V1.8 <<<<<<<
