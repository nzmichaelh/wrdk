/* Definitions of target machine for GNU compiler. EPSON C33 series
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

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Delete the following line, because it is specified to include 
        in the order of dbxelf.h / elfos.h / svr4.h / c33.h
        in gcc/config.gcc. ( reference to v850 )                       */

/* #include "svr4.h"	*//* Automatically does #undef CPP_PREDEFINES */
/* DEL K.Watanabe V1.7 <<<<<<< */

/* ADD K.Watanabe V1.7 >>>>>>> */
/**************************************************************/
/* C33: Definition is necessary to correspond to C+           */
/**************************************************************/
/* Don't assume anything about the header files.  */
#define NO_IMPLICIT_EXTERN_C
/* ADD K.Watanabe V1.7 <<<<<<< */


/* ADD K.Watanabe V1.7 >>>>>>> */
/*******************************************************************************/
/* C33: Definition is necessary to re-define in c33.h, 
        this definition was described in svr4.h in v2.95.2.                    */
/*******************************************************************************/

/* This is how to allocate empty space in some section.  The .zero
   pseudo-op is used for this on most c33 assemblers.  */
#undef  SKIP_ASM_OP   
#define SKIP_ASM_OP	".zero"

#undef ASM_OUTPUT_SKIP
#define ASM_OUTPUT_SKIP(FILE,SIZE) \
  fprintf (FILE, "\t%s\t%u\n", SKIP_ASM_OP, (SIZE))

#undef  READONLY_DATA_SECTION
#define READONLY_DATA_SECTION() const_section ()

#undef ALIGN_ASM_OP
#define ALIGN_ASM_OP ".align"


// CHG K.Watanabe V1.8 >>>>>>>
#if 0
#undef  ASM_OUTPUT_EXTERNAL_LIBCALL
#define ASM_OUTPUT_EXTERNAL_LIBCALL(FILE, FUN)				\
  ASM_GLOBALIZE_LABEL (FILE, XSTR (FUN, 0))
#endif

#undef  ASM_OUTPUT_EXTERNAL_LIBCALL
#define ASM_OUTPUT_EXTERNAL_LIBCALL(FILE, FUN)				\
    do { fputs ("\t.global ", FILE); assemble_name (FILE, XSTR (FUN, 0)); fputs ("\n", FILE);} while (0)
// CHG K.Watanabe V1.8 <<<<<<<


// DEL K.Watanabe V1.8 >>>>>>>
#if 0
#define ASM_GLOBALIZE_LABEL(FILE, NAME)	\
  do { fputs ("\t.global ", FILE); assemble_name (FILE, NAME); fputs ("\n", FILE);} while (0)
#endif
// DEL K.Watanabe V1.8 <<<<<<<


/* Define the strings used for the special c33 .type and .size directives.
   These strings generally do not vary from one system running c33 to
   another, but if a given system (e.g. m88k running svr) needs to use
   different pseudo-op names for these, they may be overridden in the
   file which includes this one.  */

#undef  TYPE_ASM_OP
#define TYPE_ASM_OP	".type"

#undef  SIZE_ASM_OP
#define SIZE_ASM_OP	".size"

/* C33: Define the following definitions in order not to create _main() function,
        though they are unused sections.                                          */
#undef INIT_SECTION_ASM_OP
#define INIT_SECTION_ASM_OP	".section\t.init"
#undef FINI_SECTION_ASM_OP
#define FINI_SECTION_ASM_OP	".section\t.fini"


/* ADD K.Watanabe V1.7 >>>>>>> */
/* C33: Set #undef because the linker did not correspond to weak symbol
        at the time of releasing of GNU33 V1.7.                          */
#undef ASM_WEAKEN_LABEL
/* ADD K.Watanabe V1.7 <<<<<<< */


/* These macros generate the special .type and .size directives which
   are used to set the corresponding fields of the linker symbol table
   entries in an ELF object file under c33.  These macros also output
   the starting labels for the relevant functions/objects.  */
   

/* C33: If the following four definitions is used, a warning occurrs.
        Probably the linker is too old.                                       
   ASM_DECLARE_FUNCTION_NAME -- definition of v2.95.2                         
   ASM_DECLARE_OBJECT_NAME   -- definition of v2.95.2                         
   ASM_FINISH_DECLARE_OBJECT -- definition of v2.95.2                         
   ASM_DECLARE_FUNCTION_SIZE -- definition of of v3.3.2 in rs6000/linux64.h   */

/* Write the extra assembler code needed to declare a function properly.
   Some c33 assemblers need to also have something extra said about the
   function's return value.  We allow for that here.  */

#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)			\
  do {									\
    fprintf (FILE, "\t%s\t ", TYPE_ASM_OP);				\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "function");			\
    putc ('\n', FILE);							\
    ASM_DECLARE_RESULT (FILE, DECL_RESULT (DECL));			\
    ASM_OUTPUT_LABEL(FILE, NAME);					\
  } while (0)

/* Write the extra assembler code needed to declare an object properly.  */

#define ASM_DECLARE_OBJECT_NAME(FILE, NAME, DECL)			\
  do {									\
    fprintf (FILE, "\t%s\t ", TYPE_ASM_OP);				\
    assemble_name (FILE, NAME);						\
    putc (',', FILE);							\
    fprintf (FILE, TYPE_OPERAND_FMT, "object");				\
    putc ('\n', FILE);							\
    size_directive_output = 0;						\
    if (!flag_inhibit_size_directive && DECL_SIZE (DECL))		\
      {									\
	size_directive_output = 1;					\
	fprintf (FILE, "\t%s\t ", SIZE_ASM_OP);				\
	assemble_name (FILE, NAME);					\
	putc (',', FILE);						\
	fprintf (FILE, HOST_WIDE_INT_PRINT_DEC,				\
		 int_size_in_bytes (TREE_TYPE (DECL)));			\
	fputc ('\n', FILE);						\
      }									\
    ASM_OUTPUT_LABEL(FILE, NAME);					\
  } while (0)

/* Output the size directive for a decl in rest_of_decl_compilation
   in the case where we did not do so before the initializer.
   Once we find the error_mark_node, we know that the value of
   size_directive_output was set
   by ASM_DECLARE_OBJECT_NAME when it was run for the same decl.  */

#define ASM_FINISH_DECLARE_OBJECT(FILE, DECL, TOP_LEVEL, AT_END)	 \
do {									 \
     char *name = XSTR (XEXP (DECL_RTL (DECL), 0), 0);			 \
     if (!flag_inhibit_size_directive && DECL_SIZE (DECL)		 \
         && ! AT_END && TOP_LEVEL					 \
	 && DECL_INITIAL (DECL) == error_mark_node			 \
	 && !size_directive_output)					 \
       {								 \
	 size_directive_output = 1;					 \
	 fprintf (FILE, "\t%s\t ", SIZE_ASM_OP);			 \
	 assemble_name (FILE, name);					 \
	 putc (',', FILE);						 \
	 fprintf (FILE, HOST_WIDE_INT_PRINT_DEC,			 \
		  int_size_in_bytes (TREE_TYPE (DECL))); 		 \
	fputc ('\n', FILE);						 \
       }								 \
   } while (0)

/* This is how to declare the size of a function.  */
#define	ASM_DECLARE_FUNCTION_SIZE(FILE, FNAME, DECL)			\
  do									\
    {									\
      if (!flag_inhibit_size_directive)					\
	{								\
	  fputs ("\t.size\t.", (FILE));					\
	  assemble_name ((FILE), (FNAME));				\
	  fputs (",.-.", (FILE));					\
	  assemble_name ((FILE), (FNAME));				\
	  putc ('\n', (FILE));						\
	}								\
    }									\
  while (0)
  
/* ADD K.Watanabe V1.7 <<<<<<< */


#define GCC33_VERSION "20070709R"

/*************/
/* A: Driver */
/*************/
#undef ASM_SPEC
#define ASM_SPEC "%{mc33adv:-mc33adv} %{mc33pe:-mc33pe} %{medda32:-medda32} %{mc33401:-mc33401} %{mc33401_2:-mc33401_2}"

#define CPP_SPEC "\
                  %{mc33adv:-D__c33adv} \
                  %{mc33pe:-D__c33pe} \
                  %{mc33401:-D__c33401} \
                  %{mc33401_2:-D__c33401_2} \
                  %{!mc33401:%{!mc33401_2:%{!mc33adv:%{!mc33pe:-D__c33std}}}} \
                 "

#undef LIB_SPEC
#undef ENDFILE_SPEC 
#undef LINK_SPEC
#undef STARTFILE_SPEC 
#define LIB_SPEC ""
#define STARTFILE_SPEC "" 

/**********************/
/* B: Run-time Target */
/**********************/

/* C33: If the following line is not defined, a compile error occurrs.  */
/* Names to predefine in the preprocessor for this target machine.  */
#define CPP_PREDEFINES "-D__c33"

/* Run-time compilation parameters selecting different hardware subsets.  */

extern int target_flags;

/* Target flags bits, see below for an explanation of the bits.  */
#define MASK_LONG_CALLS		0x00000001
#define MASK_MEMCPY		0x00000002	/* call memcpy instead of inline code*/
#define MASK_DEBUG		0x40000000

#define MASK_C33                0x00000010
#define MASK_C33ADV             0x00000020
#define MASK_C33PE              0x00000040

#define MASK_C33401             0x00000000
#define MASK_C33401_2           0x00000000 

#define MASK_ADDRESSES		0x00000100

#define MASK_EXT_ZDA		0x00001000
#define MASK_EXT_TDA		0x00002000
#define MASK_EXT_SDA		0x00004000
#define MASK_EXT_32			0x00008000					/* ADD K.Watanabe V1.4 */

#ifndef MASK_DEFAULT
#define MASK_DEFAULT            MASK_C33
#endif

#define TARGET_C33    		(target_flags & MASK_C33)
#define TARGET_C33ADV   	(target_flags & MASK_C33ADV)
#define TARGET_C33PE    	(target_flags & MASK_C33PE)


/* Macros used in the machine description to test the flags.  */

/* C33: Distinguish function call with one ext from function call with two ext. */
#define TARGET_LONG_CALLS	(target_flags & MASK_LONG_CALLS)

/* call memcpy instead of inline code */
#define TARGET_MEMCPY		(target_flags & MASK_MEMCPY)

/* General debug flag */
#define TARGET_DEBUG		(target_flags & MASK_DEBUG)

/* Dump recorded insn lengths into the output file.  This helps debug the
   md file.  */
#define TARGET_ADDRESSES	(target_flags & MASK_ADDRESSES)

#define TARGET_EXT_ZDA		(target_flags & MASK_EXT_ZDA)
#define TARGET_EXT_TDA		(target_flags & MASK_EXT_TDA)
#define TARGET_EXT_SDA		(target_flags & MASK_EXT_SDA)
#define TARGET_EXT_32		(target_flags & MASK_EXT_32)		/* ADD K.Watanabe V1.4 */

/* Macro to define tables used to set the flags.
   This is a list in braces of pairs in braces,
   each pair being { "NAME", VALUE }
   where VALUE is the bits to set or minus the bits to clear.
   An empty string NAME is used to identify the default VALUE.  */

#define TARGET_SWITCHES	{						\
   { "long-calls",		MASK_LONG_CALLS }, 			\
   { "no-long-calls",		-MASK_LONG_CALLS },			\
   { "memcpy",			MASK_MEMCPY },				\
   { "no-memcpy",		-MASK_MEMCPY },				\
   { "debug",			MASK_DEBUG },				\
   { "c33",			MASK_C33 },				\
   { "c33adv",			MASK_C33ADV },				\
   { "c33401",			MASK_C33401 },				\
   { "c33401_2",		MASK_C33401_2 },				\
   { "c33pe",			MASK_C33PE },				\
   { "addresses",	       	MASK_ADDRESSES },			\
   { "ezda",		       	MASK_EXT_ZDA },				\
   { "etda",		       	MASK_EXT_TDA },				\
   { "esda",		       	MASK_EXT_SDA },				\
   { "edda32",	       		MASK_EXT_32 },				\
   EXTRA_SWITCHES							\
   { "",			TARGET_DEFAULT}}

#ifndef EXTRA_SWITCHES
#define EXTRA_SWITCHES
#endif

#ifndef TARGET_DEFAULT
#define TARGET_DEFAULT 		MASK_DEFAULT | MASK_MEMCPY
#endif


/* Print subsidiary information on the compiler version in use.  */

#ifndef TARGET_VERSION
/* >>>>> change iruma m.takeishi '03.09.22 */
/*
#define TARGET_VERSION fprintf (stderr, " (EPSON C33)");
*/

/* c33 xgcc, cpp, cc1 version */
/* if version is upped, change below version. */
#define C33_TARGET_VERSION " (EPSON C33, rev 1.8 '07.07.09)"

#define TARGET_VERSION fprintf (stderr, C33_TARGET_VERSION);
/* <<<<< change iruma m.takeishi '03.09.22 */
#endif

/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
#ifndef TARGET_VERSION_INTERNAL
#define TARGET_VERSION_INTERNAL(STREAM)					\
  fprintf (STREAM, " --- gcc33v2 %s", GCC33_VERSION)
#endif
#endif

#ifndef TARGET_VERSION_INTERNAL
#define TARGET_VERSION_INTERNAL(STREAM)					\
  fprintf (STREAM, " --- gcc33 %s", GCC33_VERSION)
#endif
/* CHG K.Watanabe V1.7 <<<<<<< */

/* Information about the various small memory areas.  */
struct small_memory_info {
  char *name;
  char *value;
  long max;
  long physical_max;
};

enum small_memory_type {
  /* gp1 data area, using R12 as base register */
  SMALL_MEMORY_SDA = 0,
  /* gp2 data area, using R13 as base register */
  SMALL_MEMORY_TDA,
  /* gp3 data area, using R14 as base register */
  SMALL_MEMORY_ZDA,
  /* gp4 data area, using R15 as base register */
  SMALL_MEMORY_GDA,
  SMALL_MEMORY_max
};

extern struct small_memory_info small_memory[(int)SMALL_MEMORY_max];


extern char *c33_cpu_string;
extern char *gp_no;
extern int  gp_max;
extern char *gdp_string;

/* This macro is similar to `TARGET_SWITCHES' but defines names of
   command options that have values.  Its definition is an
   initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   fixed part of the option name, and the address of a variable.  The
   variable, type `char *', is set to the variable part of the given
   option if the fixed part matches.  The actual option name is made
   by appending `-m' to the specified name.

   Here is an example which defines `-mshort-data-NUMBER'.  If the
   given option is `-mshort-data-512', the variable `m88k_short_data'
   will be set to the string `"512"'.

          extern char *m88k_short_data;
          #define TARGET_OPTIONS \
           { { "short-data-", &m88k_short_data } } */

#define TARGET_OPTIONS							\
{									\
  { "cpu=",	&c33_cpu_string, "" },					\
  { "cpu-",	&c33_cpu_string, "" },					\
  { "dp=",	&gp_no, "" },						\
  { "dp-",	&gp_no, "" },						\
  { "gda=",	&small_memory[ (int)SMALL_MEMORY_GDA ].value, 		\
      "Set the max size of data eligible for the GDA area"  },		\
  { "gda-",	&small_memory[ (int)SMALL_MEMORY_GDA ].value, "" },	\
  { "gdp=",	&gdp_string, "" },					\
  { "gdp-",	&gdp_string, "" },					\
}

#define C33_CPU_STRING_DEFAULT	        "S1C332xx"
#define C33_CPU_STRING_DEFAULT_ADVANCED	"S1C334xx"
#define C33_CPU_STRING_DEFAULT_C33PE	"S1C336xx"

/* Sometimes certain combinations of command options do not make
   sense on a particular target machine.  You can define a macro
   `OVERRIDE_OPTIONS' to take account of this.  This macro, if
   defined, is executed once just after all the command options have
   been parsed.

   Don't use this macro to turn on various extra optimizations for
   `-O'.  That is what `OPTIMIZATION_OPTIONS' is for.  */
#define OVERRIDE_OPTIONS override_options ()


/* Show we can debug even without a frame pointer.  */
#define CAN_DEBUG_WITHOUT_FP


/* Some machines may desire to change what optimizations are
   performed for various optimization levels.   This macro, if
   defined, is executed once just after the optimization level is
   determined and before the remainder of the command options have
   been parsed.  Values set in this macro are used as the default
   values for the other command line options.

   LEVEL is the optimization level specified; 2 if `-O2' is
   specified, 1 if `-O' is specified, and 0 if neither is specified.

   You should not use this macro to change options that are not
   machine-specific.  These should uniformly selected by the same
   optimization level on all supported machines.  Use this macro to
   enable machine-specific optimizations.

   *Do not examine `write_symbols' in this macro!* The debugging
   options are not supposed to alter the generated code. */

/* C33: "flag_omit_frame_pointer = 1" is default when you specify the optimize. */
#define OPTIMIZATION_OPTIONS(LEVEL, SIZE)				\
{									\
  if (LEVEL >= 0)							\
    flag_omit_frame_pointer = 1;					\
									\
  /* C33: Disable the following flgas in C33, though elabled in -O2 and over. */	\
  flag_schedule_insns = 0;						\
  flag_schedule_insns_after_reload = 0;					\
                                                                        \
  /* C33: "-fno-common" shall be dafault in C33. GNU-GCC-050 2002/6/4 watanabe */     \
  flag_no_common = 1;                                                   \
}

/* >>>>> add iruma m.takeishi '03.09.22 */
/* C33:  Define the following line in order to work EPSON-code in gcc core files. */
#define EPSON 1
/* <<<<< add iruma m.takeishi '03.09.22 */




/*********************/
/* C: Storage Layout */
/*********************/

/* Target machine storage layout */

/* Define this if most significant bit is lowest numbered
   in instructions that operate on numbered bit-fields.
   This is not true on the EPSON C33.  */
#define BITS_BIG_ENDIAN 0
/* C33: C33 is little endian. */

/* Define this if most significant byte of a word is the lowest numbered.  */
/* This is not true on the EPSON C33.  */
#define BYTES_BIG_ENDIAN 0

/* Define this if most significant word of a multiword number is lowest
   numbered.
   This is not true on the EPSON C33.  */
#define WORDS_BIG_ENDIAN 0

/* Number of bits in an addressable storage unit */
#define BITS_PER_UNIT 8

/* Width in bits of a "word", which is the contents of a machine register.
   Note that this is not necessarily the width of data type `int';
   if using 16-bit ints on a 68000, this would still be 32.
   But on a machine with 16-bit registers, this would be 16.  */
#define BITS_PER_WORD		32

/* Width of a word, in units (bytes).  */
#define UNITS_PER_WORD		4

/* Width in bits of a pointer.
   See also the macro `Pmode' defined below.  */
#define POINTER_SIZE 		32

/* Define this macro if it is advisable to hold scalars in registers
   in a wider mode than that declared by the program.  In such cases,
   the value is constrained to be within the bounds of the declared
   type, but kept valid in the wider mode.  The signedness of the
   extension may differ from that of the type.

   Some simple experiments have shown that leaving UNSIGNEDP alone
   generates the best overall code.  */

#define PROMOTE_MODE(MODE,UNSIGNEDP,TYPE)  \
  if (GET_MODE_CLASS (MODE) == MODE_INT \
      && GET_MODE_SIZE (MODE) < 4)      \
    { (MODE) = SImode; }

/* Allocation boundary (in *bits*) for storing arguments in argument list.  */
#define PARM_BOUNDARY		32

/* The stack goes in 32 bit lumps.  */
#define STACK_BOUNDARY 		32

/* Allocation boundary (in *bits*) for the code of a function.
   16 is the minimum boundary; 32 would give better performance.  */
#define FUNCTION_BOUNDARY	16

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT	32

/* No structure field wants to be aligned rounder than this.  */
#define BIGGEST_FIELD_ALIGNMENT 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY	32

/* C33: The alignment of ".data" section shall be 4 byte boundary. */
/* If defined, a C expression to compute the alignment for a static
   variable.  TYPE is the data type, and ALIGN is the alignment that
   the object would ordinarily have.  The value of this macro is used
   instead of that alignment to align the object.

   If this macro is not defined, then ALIGN is used.

   One use of this macro is to increase alignment of medium-size
   data to make it all fit in fewer cache lines.  Another is to
   cause character arrays to be word-aligned so that `strcpy' calls
   that copy constants to character arrays can be done inline.  */

#undef DATA_ALIGNMENT
#define DATA_ALIGNMENT(TYPE, ALIGN)					\
  ((((ALIGN) < BITS_PER_WORD)						\
    && (TREE_CODE (TYPE) == ARRAY_TYPE					\
	|| TREE_CODE (TYPE) == UNION_TYPE				\
	|| TREE_CODE (TYPE) == RECORD_TYPE)) ? BITS_PER_WORD : (ALIGN))

/* C33: The alignment of ".data" section shall be 4 byte boundary. */
/* If defined, a C expression to compute the alignment given to a
   constant that is being placed in memory.  CONSTANT is the constant
   and ALIGN is the alignment that the object would ordinarily have.
   The value of this macro is used instead of that alignment to align
   the object.

   If this macro is not defined, then ALIGN is used.

   The typical use of this macro is to increase alignment for string
   constants to be word aligned so that `strcpy' calls that copy
   constants can be done inline.  */

#define CONSTANT_ALIGNMENT(EXP, ALIGN)					\
  ((TREE_CODE (EXP) == STRING_CST  || TREE_CODE (EXP) == CONSTRUCTOR)	\
   && (ALIGN) < BITS_PER_WORD						\
	? BITS_PER_WORD							\
	: (ALIGN))

/* Define this if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/******************/
/* D: Type Layout */
/******************/

/* Define this as 1 if `char' should by default be signed; else as 0.

   On the EPSON C33, loads do sign extension, so make this default. */
#define DEFAULT_SIGNED_CHAR 1

/* Define results of standard character escape sequences.  */
#define TARGET_BELL 007
#define TARGET_BS 010
#define TARGET_TAB 011
#define TARGET_NEWLINE 012
#define TARGET_VT 013
#define TARGET_FF 014
#define TARGET_CR 015
#define TARGET_ESC 033		/* ADD K.Watanabe V1.7 */

/****************/
/* E: Registers */
/****************/

/* Standard register usage.  */

/* Number of actual hardware registers.
   The hardware registers are assigned numbers for the compiler
   from 0 to just below FIRST_PSEUDO_REGISTER.

   All registers that the compiler knows about must be given numbers,
   even those that are not normally considered general registers.  */


/* %r0-%r15, .fp, .ap, %sp */
#define FIRST_PSEUDO_REGISTER 19

/* C33: New register arrangement in C33.
	%r0-%r3		saved
	%r4-%r5 	return
	%r6-%r9		argument
	%r10-%r15	fixed/gp */

/* 1 for registers that have pervasive standard uses
   and are not available for the register allocator.  */


/* C33: Registers that the compiler can use. */
#define FIXED_REGISTERS \
  { 0, 0, 0, 0, 0, 0, 0, 0, \
    0, 0, 1, 1, 1, 1, 1, 1, \
    1, 1, 1 }

/* 1 for registers not available across function calls.
   These must include the FIXED_REGISTERS and also any
   registers that can be used without being saved.
   The latter must include the registers where values are returned
   and the register where structure-value addresses are passed.
   Aside from that, you can include as many other registers as you
   like.  */


/* C33: Registers used between function calls. */
#define CALL_USED_REGISTERS \
  { 0, 0, 0, 0, 1, 1, 1, 1, \
    1, 1, 1, 1, 1, 1, 1, 1, \
    1, 1, 1 }

/* C33: ADD scratch registers by "-mgp=n". */
#define CONDITIONAL_REGISTER_USAGE					\
{									\
  int regno;								\
  for (regno = 6 - gp_max; regno > 0; regno--) {			\
      fixed_regs[9 + regno] = 0;					\
  }									\
}

/* List the order in which to allocate registers.  Each register must be
   listed once, even those in FIXED_REGISTERS. */

/* C33: Change the priority of register assignment. 
        In order to reduce the use of "push / pop" in the normal function. */

/* C33: Assign registers in the following order in C33.
        Return gegisters, saved registers, and fixed registers.   */
/* CHG K.Watanabe V1.4 >>>>>>> */ 
#if 0
#define REG_ALLOC_ORDER							\
{									\
   4,  5,			/* return registers */			\
   6,  7,  8,  9,		/* argument registers */		\
   0,  1,  2,  3,		/* saved registers */			\
  10, 11, 12, 13,		/* fixed registers */			\
  14, 15, 16, 17, 18		/* fixed registers */			\
}
#endif

#define REG_ALLOC_ORDER							\
{									\
   4,  5,				/* return registers */			\
   6,  7,  8,  9,		/* argument registers */		\
  11, 12, 13, 14,		/* fixed registers */			\
  0,  1,  2,  3,		/* saved registers */			\
  10,													\
  15, 16, 17, 18		/* fixed registers */			\
}
/* CHG K.Watanabe V1.4 <<<<<<< */


/* Return number of consecutive hard regs needed starting at reg REGNO
   to hold something of mode MODE.

   This is ordinarily the length in words of a value of mode MODE
   but can be less for certain modes in special long registers.  */

/* C33: The number of hard registers needed to save the MODE data. */
#define HARD_REGNO_NREGS(REGNO, MODE)   \
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* Value is 1 if hard register REGNO can hold a value of machine-mode
   MODE.  */

/* C33: "OK" is necessary for in even number in order to 
        treat multi-word type such as double type or long long type.
        %sp can hold only SI-mode.                                      */

#define HARD_REGNO_MODE_OK(REGNO, MODE)                          \
   ((((REGNO) == STACK_POINTER_REGNUM)                           \
      && (GET_MODE_SIZE (MODE) == 4)) ||                         \
    (((REGNO) != STACK_POINTER_REGNUM)                           \
      && ((((REGNO) & 1) == 0) || (GET_MODE_SIZE (MODE) <= 4))))

/* Value is 1 if it is a good idea to tie two pseudo registers
   when one has mode MODE1 and one has mode MODE2.
   If HARD_REGNO_MODE_OK could produce different values for MODE1 and MODE2,
   for any hard reg, then this must be 0 for correct output.  */

/* C33: All registers are same in C33 and any MODE can be held.
        But the following line corresponds to V850. Just in case. */

#define MODES_TIEABLE_P(MODE1, MODE2) \
  (MODE1 == MODE2 || GET_MODE_SIZE (MODE1) <= 4 && GET_MODE_SIZE (MODE2) <= 4)

/* This is the order in which to allocate registers for leaf functions.  */
/* C33: Change the order of register assignment when the function is
        the interrupt function which is the leaf function.
        Because C33 can operate pushn/popn opecode only from %r0,
        so this is the avoidance measure.                             */
        

#define REG_LEAF_ALLOC_ORDER \
{  0,  1,  2,  3,  4,  5,	\
   6,  7,  8,  9, 10,		\
  11, 12, 13, 14, 15,		\
  16, 17, 18 }


// CHG K.Watanabe V1.8 >>>>>>>
#if 0
#define ORDER_REGS_FOR_LOCAL_ALLOC					\
{									\
  static int leaf[] = REG_LEAF_ALLOC_ORDER;				\
  static int nonleaf[] = REG_ALLOC_ORDER;				\
									\
  bcopy ((c33_interrupt_function_p () && leaf_function_p()) ?		\
	 leaf : nonleaf,						\
         reg_alloc_order, FIRST_PSEUDO_REGISTER * sizeof (int));	\
}
#endif

#define ORDER_REGS_FOR_LOCAL_ALLOC					\
{									\
  static int leaf[] = REG_LEAF_ALLOC_ORDER;				\
  static int nonleaf[] = REG_ALLOC_ORDER;				\
									\
  memcpy (reg_alloc_order,                                      \
          (c33_interrupt_function_p () && leaf_function_p()) ?	\
	    leaf : nonleaf,							\
          FIRST_PSEUDO_REGISTER * sizeof (int));			\
}
// CHG K.Watanabe V1.8 <<<<<<<


/***********************/
/* F: Register Classes */
/***********************/

/* Define the classes of registers for register constraints in the
   machine description.  Also define ranges of constants.

   One of the classes must always be named ALL_REGS and include all hard regs.
   If there is more than one class, another class must be named NO_REGS
   and contain no registers.

   The name GENERAL_REGS must be the name of a class (or an alias for
   another name such as ALL_REGS).  This is the class of registers
   that is allowed by "g" or "r" in a register constraint.
   Also, registers outside this class are allocated only when
   instructions express preferences for them.

   The classes must be numbered in nondecreasing order; that is,
   a larger-numbered class must never be contained completely
   in a smaller-numbered class.

   For any two classes, it is very desirable that there be another
   class that represents their union.  */

enum reg_class {
  NO_REGS, GENERAL_REGS, SP_REGS, BASE_REGS, ALL_REGS, LIM_REG_CLASSES
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/* Give names of register classes as strings for dump file.   */

#define REG_CLASS_NAMES \
{ "NO_REGS", "GENERAL_REGS", "SP_REGS", "BASE_REGS", "ALL_REGS", "LIM_REGS" }

/* Define which registers fit in which classes.
   This is an initializer for a vector of HARD_REG_SET
   of length N_REG_CLASSES.  */

#define REG_CLASS_CONTENTS  			\
{  0x00000000,		/* No regs      */	\
   0x0003ffff,		/* GENERAL_REGS */    	\
   0x00040000,		/* SP_REGS */    	\
   0x0007ffff,		/* BASE_REGS */    	\
   0xffffffff,		/* ALL_REGS 	*/	\
}

/* The same information, inverted:
   Return the class number of the smallest class containing
   reg number REGNO.  This could be a conditional expression
   or could index an array.  */

#define REGNO_REG_CLASS(REGNO) \
  ((REGNO) == STACK_POINTER_REGNUM ? SP_REGS : GENERAL_REGS)

/* The class value for index registers, and the one for base regs.  */

#define INDEX_REG_CLASS NO_REGS
#define BASE_REG_CLASS  BASE_REGS

/* Get reg_class from a letter such as appears in the machine description.  */
#define REG_CLASS_FROM_LETTER(C) ((C) == 'f' ? SP_REGS : NO_REGS)

/* Macros to check register numbers against specific register classes.  */

/* These assume that REGNO is a hard or pseudo reg number.
   They give nonzero only if REGNO is a hard reg of the suitable class
   or a pseudo reg currently allocated to a suitable hard reg.
   Since they use reg_renumber, they are safe only once reg_renumber
   has been allocated, which happens in local-alloc.c.  */

#define REGNO_OK_FOR_BASE_P(regno) \
  ((regno) < FIRST_PSEUDO_REGISTER || reg_renumber[regno] >= 0)

#define REGNO_OK_FOR_INDEX_P(regno) 0

/* Given an rtx X being reloaded into a reg required to be
   in class CLASS, return the class of reg to actually use.
   In general this is just CLASS; but on some machines
   in some cases it is preferable to use a more restrictive class.  */

#define PREFERRED_RELOAD_CLASS(X,CLASS)  (CLASS)

/* Return the maximum number of consecutive registers
   needed to represent mode MODE in a register of class CLASS.  */

#define CLASS_MAX_NREGS(CLASS, MODE)	\
  ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/* The letters I, J, K, L, M, N, O, P in a register constraint string
   can be used to stand for particular ranges of immediate operands.
   This macro defines what the ranges are.
   C is the letter, and VALUE is a constant value.
   Return 1 if VALUE is in the range specified by C.  */

/* C33: Characters are assigned in C33 as follows.
	I: unused
	J: unused
	K: sign6	ld,add/sub,cmp,and/or/xor
	L: sign19	ld,add/sub,cmp,and/or/xor
	M: shift immediate data
	N: shift immediate data
	O: shift immediate data
	P: unused
*/

#define CONST_OK_FOR_SIGNED6(VALUE)	((unsigned) (VALUE) + 0x20 < 0x40)
#define CONST_OK_FOR_SIGNED9(VALUE)	((unsigned) (VALUE) + 0x100 < 0x200)
#define CONST_OK_FOR_SIGNED19(VALUE)	((unsigned) (VALUE) + 0x40000 < 0x80000)

#define	CONST_OK_FOR_IMM6(VALUE)	((unsigned)(VALUE) < 0x40)
#define	CONST_OK_FOR_IMM7(VALUE)	((unsigned)(VALUE) < 0x80)
#define	CONST_OK_FOR_IMM8(VALUE)	((unsigned)(VALUE) < 0x100)
#define	CONST_OK_FOR_IMM13(VALUE)	((unsigned)(VALUE) < 0x2000)
#define	CONST_OK_FOR_IMM19(VALUE)	((unsigned)(VALUE) < 0x80000)
#define	CONST_OK_FOR_IMM26(VALUE)	((unsigned)(VALUE) < 0x4000000)

#define	CONST_OK_FOR_M(VALUE)	((unsigned)(VALUE) <= 8)
#define	CONST_OK_FOR_N(VALUE)	((unsigned)(VALUE) <= 16)
#define	CONST_OK_FOR_O(VALUE)	((unsigned)(VALUE) <= 24)
#define	CONST_OK_FOR_P(VALUE)	((unsigned)(VALUE) <= 26)	/* GNU-GCC-031 2001/12/27 watanabe */

#define CONST_OK_FOR_LETTER_P(VALUE, C)  \
  ((C) == 'I' ? 0 : \
   (C) == 'J' ? 0 : \
   (C) == 'K' ? CONST_OK_FOR_SIGNED6(VALUE) : \
   (C) == 'L' ? CONST_OK_FOR_SIGNED19(VALUE) : \
   (C) == 'M' ? CONST_OK_FOR_M(VALUE) : \
   (C) == 'N' ? CONST_OK_FOR_N(VALUE) : \
   (C) == 'O' ? CONST_OK_FOR_O(VALUE) : \
   (C) == 'P' ? CONST_OK_FOR_P(VALUE) : \
   0)

/* C33: If the following lines are not defined, the link error occurs. */
/* Similar, but for floating constants, and defining letters G and H.
   Here VALUE is the CONST_DOUBLE rtx itself.

  `G' is a zero of some form.  */

/* C33: Define character 'G' for floating-point numbers.
        'G' is '0'. Correspond to H8300.                 */

#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C) \
  ((C) == 'G' ? (VALUE) == CONST0_RTX(DFmode) \
   : 0)

/* A C expression that defines the optional machine-dependent
   constraint letters that can be used to segregate specific types of
   operands, usually memory references, for the target machine.
   Normally this macro will not be defined.  If it is required for a
   particular target machine, it should return 1 if VALUE corresponds
   to the operand type represented by the constraint letter C.  If C
   is not defined as an extra constraint, the value returned should
   be 0 regardless of VALUE.

   For example, on the ROMP, load instructions cannot have their
   output in r0 if the memory reference contains a symbolic address.
   Constraint letter `Q' is defined as representing a memory address
   that does *not* contain a symbolic address.  An alternative is
   specified with a `Q' constraint on the input and `r' on the
   output.  The next alternative specifies `m' on the input and a
   register class that does not include r0 on the output.  */

#define EXTRA_CONSTRAINT(OP, C)					\
 ((C) == 'Q'   ? 0						\
  : (C) == 'R' ? 0						\
  : (C) == 'S' ? (GET_CODE (OP) == SYMBOL_REF)			\
  : (C) == 'T' ? 0						\
  : (C) == 'U' ? 0						\
  : 0)

/************************/
/* G: Stack and Calling */
/************************/

/* G-1: Basic Stack Layout */

/* Stack layout; function entry, exit and calling.  */

/* Define this if pushing a word on the stack
   makes the stack pointer a smaller address.  */

#define STACK_GROWS_DOWNWARD

/* Define this if the nominal address of the stack frame
   is at the high-address end of the local variables;
   that is, each additional local variable allocated
   goes at a more negative offset in the frame.  */
/* C33: The frame pointer is used as plus offset in C33. */

/* #define FRAME_GROWS_DOWNWARD */
/* #define ARGS_GROWS_DOWNWARD */

/* Offset within stack frame to start allocating local variables at.
   If FRAME_GROWS_DOWNWARD, this is the offset to the END of the
   first local allocated.  Otherwise, it is the offset to the BEGINNING
   of the first local allocated.  */

/* C33: %fp and %sp shall be in common in C33.
        Allocate the space for arguments of called function.  */
        
#define STARTING_FRAME_OFFSET (current_function_outgoing_args_size)

/* Offset of first parameter from the argument pointer register value.  */
/* Is equal to the size of the saved fp + pc, even if an fp isn't
   saved since the value is used before we know.  */

#define FIRST_PARM_OFFSET(FNDECL) 0

/* G-2: Specifying How Stack Checking is Done */


/* G-3: Registers That Address the Stack Frame */

/* Specify the registers used for certain standard purposes.
   The values of these macros are register numbers.  */

/* Register to use for pushing function arguments.  */
#define STACK_POINTER_REGNUM 18

/* Base register for access to local variables of the function.  */
/* CHG K.Watanabe V1.7 >>>>>>> */
#if 0
/* C33: The following definition shall be the temporary register.
        It will be useless if it is assigned as saved register.   */

#define FRAME_POINTER_REGNUM 16  
#endif

#define FRAME_POINTER_REGNUM 0  
/* CHG K.Watanabe V1.7 <<<<<<< */


/* Base register for access to arguments of the function.  */
/* C33: Make the temporary register. */
#define ARG_POINTER_REGNUM 17

/* Register in which static-chain is passed to a function.  */
/* >>>>> change iruma m.takeishi '04.04.01 */
/* C33: The following line is corrected,
        because forcibly terminated was occurred 
        when the function was declared in the function,
        
 *  
 *  int main() {
 *      int sub () {};
 *  }
 */
/* #define STATIC_CHAIN_REGNUM 9 */

#define STATIC_CHAIN_REGNUM 9
/* <<<<< change iruma m.takeishi '04.04.01 */

/* G-4: Eliminating Frame Pointer and Arg Pointer */

/* Value should be nonzero if functions must have frame pointers.
   Zero means the frame pointer need not be set up (and parms
   may be accessed via the stack pointer) in functions that seem suitable.
   This is computed in `reload', in reload1.c.  */
#define FRAME_POINTER_REQUIRED 0

/* If defined, this macro specifies a table of register pairs used to
   eliminate unneeded registers that point into the stack frame.  If
   it is not defined, the only elimination attempted by the compiler
   is to replace references to the frame pointer with references to
   the stack pointer.

   The definition of this macro is a list of structure
   initializations, each of which specifies an original and
   replacement register.

   On some machines, the position of the argument pointer is not
   known until the compilation is completed.  In such a case, a
   separate hard register must be used for the argument pointer.
   This register can be eliminated by replacing it with either the
   frame pointer or the argument pointer, depending on whether or not
   the frame pointer has been eliminated.

   In this case, you might specify:
        #define ELIMINABLE_REGS  \
        {{ARG_POINTER_REGNUM, STACK_POINTER_REGNUM}, \
         {ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM}, \
         {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}}

   Note that the elimination of the argument pointer with the stack
   pointer is specified first since that is the preferred elimination. */

#define ELIMINABLE_REGS  \
	{{ARG_POINTER_REGNUM, STACK_POINTER_REGNUM}, \
	 {ARG_POINTER_REGNUM, FRAME_POINTER_REGNUM}, \
	 {FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM}}

/* A C expression that returns non-zero if the compiler is allowed to
   try to replace register number FROM-REG with register number
   TO-REG.  This macro need only be defined if `ELIMINABLE_REGS' is
   defined, and will usually be the constant 1, since most of the
   cases preventing register elimination are things that the compiler
   already knows about. */

#define CAN_ELIMINATE(FROM, TO) 1 

/* This macro is similar to `INITIAL_FRAME_POINTER_OFFSET'.  It
   specifies the initial difference between the specified pair of
   registers.  This macro must be defined if `ELIMINABLE_REGS' is
   defined. */

#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
{									\
  if ((FROM) == FRAME_POINTER_REGNUM && (TO) == STACK_POINTER_REGNUM)	\
    (OFFSET) = 0;							\
  else if ((FROM) == ARG_POINTER_REGNUM)				\
   (OFFSET) = compute_frame_size (get_frame_size (), (long *)0);	\
  else {								\
    abort ();								\
  }									\
}

/* G-5: Passing Function Arguments on the Stack */

/* A guess for the C33.  */
/* CHG K.Watanabe V1.7 >>>>>>> */
/* #define PROMOTE_PROTOTYPES */
#define PROMOTE_PROTOTYPES 1
/* CHG K.Watanabe V1.7 <<<<<<< */

/* Keep the stack pointer constant throughout the function.  */

/* CHG K.Watanabe V1.7 >>>>>>> */
/* #define ACCUMULATE_OUTGOING_ARGS */
#define ACCUMULATE_OUTGOING_ARGS 1
/* CHG K.Watanabe V1.7 <<<<<<< */

/* When a parameter is passed in a register, stack space is still
   allocated for it.  */
/* C33: When a parameter is passed through the register,
        stack space is not allocated for it in C33. */
#define REG_PARM_STACK_SPACE(DECL) (0)


/* Value is the number of bytes of arguments automatically
   popped when returning from a subroutine call.
   FUNDECL is the declaration node of the function (as a tree),
   FUNTYPE is the data type of the function (as a tree),
   or for a library call it is an identifier node for the subroutine name.
   SIZE is the number of bytes of arguments passed on the stack.  */

#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0

/* G-6: Passing Arguments in Registers */

/* Define where to put the arguments to a function.
   Value is zero to push the argument on the stack,
   or a hard register in which to store the argument.

   MODE is the argument's machine mode.
   TYPE is the data type of the argument (as a tree).
    This is null for libcalls where that information may
    not be available.
   CUM is a variable of type CUMULATIVE_ARGS which gives info about
    the preceding args and about the function being called.
   NAMED is nonzero if this argument is a named parameter
    (otherwise it is an extra parameter matching an ellipsis).  */

struct rtx_def *function_arg();
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
  function_arg (&CUM, MODE, TYPE, NAMED)

/* C33: Do not define the following line according to MIPS. */

/* Define a data type for recording info about an argument list
   during the scan of that argument list.  This data type should
   hold all necessary information about the function itself
   and about the args processed so far, enough to enable macros
   such as FUNCTION_ARG to determine where the next arg should go.  */

#define CUMULATIVE_ARGS struct cum_arg
struct cum_arg { int nbytes; };

/* Initialize a variable CUM of type CUMULATIVE_ARGS
   for a call to a function whose data type is FNTYPE.
   For a library call, FNTYPE is 0.  */

#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT)	\
 ((CUM).nbytes = 0)

/* Update the data in CUM to advance over an argument
   of mode MODE and data type TYPE.
   (TYPE is null for libcalls where that information may not be available.)  */

/* C33: The structure are passed through the stack.
	    Do not add in the case that the parameter is passed in DFmode
	    because registers are not used in this type.                   */
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)			\
 ((CUM).nbytes += ((MODE) == BLKmode					\
  ? 0									\
  : ((MODE) == DFmode							\
      ? (((CUM).nbytes  < ((4-1) * UNITS_PER_WORD))		 	\
          ? ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) & -UNITS_PER_WORD)	\
          : 0)								\
      : (GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) & -UNITS_PER_WORD)))


/* 1 if N is a possible register number for function argument passing.  */

#define FUNCTION_ARG_REGNO_P(N) (N >= 6 && N <= 9)

/* G-7: How Scalar Function Values Are Returned */

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its FUNCTION_DECL;
   otherwise, FUNC is 0.   */

#define FUNCTION_VALUE(VALTYPE, FUNC) \
  gen_rtx (REG, TYPE_MODE (VALTYPE), 4)

/* C33: If the following lines are not defined, the link error occurs. */
/* Define how to find the value returned by a library function
   assuming the value has mode MODE.  */

#define LIBCALL_VALUE(MODE) \
  gen_rtx (REG, MODE, 4)

/* 1 if N is a possible register number for a function value.  */

#define FUNCTION_VALUE_REGNO_P(N) ((N) == 4)


/* G-8: How Large Values Are Returned */

/* C33: Define "DEFAULT_PCC_STRUCT_RETURN" as 0,
        or the principle of the structure / the union does not obey
        the "RETURN_IN_MEMORY".                                      */


/* Return values > 8 bytes in length in memory.  */
#define DEFAULT_PCC_STRUCT_RETURN 0

/* C33: Though define RETURN_IN_MEMORY as (TYPE_MODE (TYPE) == BLKmode) in MIPS,
        shall the structure / the union be BLKmode?
        ‚m‚n(980903)  BLKmode may be string.                                      */

#define RETURN_IN_MEMORY(TYPE)  \
  (int_size_in_bytes (TYPE) > 8 || TYPE_MODE (TYPE) == BLKmode)

/* Register in which address to store a structure value
   is passed to a function.  On the C33 it's passed as
   the first parameter.  */

#define STRUCT_VALUE 0


/* G-9: Caller-Saves Register Allocation */


/* G-10: Function Entry and Exit */

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. Define the following contents in TARGET_ASM_FUNCTION_PROLOGUE. */


/* C33: Define function prologue. */
/* #define FUNCTION_PROLOGUE(FILE, SIZE) expand_prologue(file); */
/* DEL K.Watanabe V1.7 <<<<<<< */

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */

#define EXIT_IGNORE_STACK 0

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. Define the following contents in TARGET_ASM_FUNCTION_EPILOGUE */

/* C33: Define function epilogue. */
/* #define FUNCTION_EPILOGUE(FILE, SIZE) expand_epilogue(file); */
/* DEL K.Watanabe V1.7 <<<<<<< */

/* G-11: Generating Code for Profiling */

/* C33: If the following line is not defined, the link error occurs. */
/* Output assembler code to FILE to increment profiler label # LABELNO
   for profiling a function entry.  */
#define FUNCTION_PROFILER(FILE, LABELNO) ;

/**************/
/* H: Varargs */
/**************/



/******************/
/* I: Trampolines */
/******************/

/* Length in units of the trampoline for entering a nested function.  */
/* C33: Set 0 to length, because we don't define TRAMPOLINE_TEMPLATE */
#define TRAMPOLINE_SIZE 0

/* C33: If the following line is not defined, the link error occurs. */
#define INITIALIZE_TRAMPOLINE(TRAMP, FNADDR, CXT) ;


/********************/
/* J: Library Calls */
/********************/

#define TARGET_MEM_FUNCTIONS

/***********************/
/* K: Addressing Modes */
/***********************/

/* Addressing modes, and classification of registers for them.  */

#define HAVE_POST_INCREMENT 1

/* 1 if X is an rtx for a constant that is a valid address.  */

/* ??? This seems too exclusive.  May get better code by accepting more
   possibilities here, in particular, should accept ZDA_NAME SYMBOL_REFs.  */

#define CONSTANT_ADDRESS_P(X)   \
  (GET_CODE (X) == CONST_INT	\
   && CONST_OK_FOR_IMM26 (INTVAL (X)))	/* unsigned 26bit */

/* Maximum number of registers that can appear in a valid memory address.  */

#define MAX_REGS_PER_ADDRESS 1

/* The macros REG_OK_FOR..._P assume that the arg is a REG rtx
   and check its validity for a certain class.
   We have two alternate definitions for each of them.
   The usual definition accepts all pseudo regs; the other rejects
   them unless they have been allocated suitable hard regs.
   The symbol REG_OK_STRICT causes the latter definition to be used.

   Most source files want to accept pseudo regs in the hope that
   they will get allocated to the class that the insn wants them to be in.
   Source files for reload pass need to be strict.
   After reload, it makes no difference, since pseudo regs have
   been eliminated by then.  */

#ifndef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as an index
   or if it is a pseudo reg.  */
#define REG_OK_FOR_INDEX_P(X) 0
/* Nonzero if X is a hard reg that can be used as a base reg
   or if it is a pseudo reg.  */
#define REG_OK_FOR_BASE_P(X) 1
#define REG_OK_FOR_INDEX_P_STRICT(X) 0
#define REG_OK_FOR_BASE_P_STRICT(X) REGNO_OK_FOR_BASE_P (REGNO (X))
#define STRICT 0

#else

/* Nonzero if X is a hard reg that can be used as an index.  */
#define REG_OK_FOR_INDEX_P(X) 0
/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X) REGNO_OK_FOR_BASE_P (REGNO (X))
#define STRICT 1

#endif

/* GO_IF_LEGITIMATE_ADDRESS recognizes an RTL expression
   that is a valid memory address for an instruction.
   The MODE argument is the machine mode for the MEM expression
   that wants to use this address.

   The other macros defined here are used only in GO_IF_LEGITIMATE_ADDRESS,
   except for CONSTANT_ADDRESS_P which is actually
   machine-independent.  */

/* Accept either REG or SUBREG where a register is valid.  */


//#define RTX_OK_FOR_BASE_P(X)	(REG_P (X) && REG_OK_FOR_BASE_P (X))
#define RTX_OK_FOR_BASE_P(X)						\
  ((REG_P (X) && REG_OK_FOR_BASE_P (X))					\
   || (GET_CODE (X) == SUBREG && REG_P (SUBREG_REG (X))			\
       && REG_OK_FOR_BASE_P (SUBREG_REG (X))))

/* CHG K.Watanabe V1.7 >>>>>>> */
/* C33: Reflect the option of "-medda32" in the following definition.
        When the option is "-medda32 && !adv", 
        forbid the access of symbols which use the data pointer.  */
#if 0
#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)				\
do {									\
  if (RTX_OK_FOR_BASE_P (X)) goto ADDR;					\
  if (GET_CODE (X) == SYMBOL_REF					\
      && (GET_MODE_SIZE (MODE) <= GET_MODE_SIZE (word_mode)))		\
     goto ADDR;								\
  if (GET_CODE (X) == CONST						\
      && (GET_MODE_SIZE (MODE) <= GET_MODE_SIZE (word_mode)))		\
     goto ADDR;								\
  if (GET_CODE (X) == PLUS						\
      && CONSTANT_ADDRESS_P (XEXP (X, 1))				\
      && ((MODE == QImode)						\
	  || ((MODE == HImode) && (INTVAL (XEXP(X, 1)) % 2 == 0))	\
	  || ((MODE == SImode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == DImode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == SFmode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == DFmode) && (INTVAL (XEXP(X, 1)) % 4 == 0)))	\
      && RTX_OK_FOR_BASE_P (XEXP (X, 0))) goto ADDR;			\
} while (0)
#endif

#define GO_IF_LEGITIMATE_ADDRESS(MODE, X, ADDR)				\
do {									\
  if (RTX_OK_FOR_BASE_P (X)) goto ADDR;					\
  if (GET_CODE (X) == SYMBOL_REF					\
      && (GET_MODE_SIZE (MODE) <= GET_MODE_SIZE (word_mode))		\
      && ( !TARGET_EXT_32 || TARGET_C33ADV || ENCODED_NAME_P( XSTR(X, 0) ) ) )							\
     goto ADDR;								\
  if (GET_CODE (X) == CONST						\
      && (GET_MODE_SIZE (MODE) <= GET_MODE_SIZE (word_mode))		\
      && ( !TARGET_EXT_32 || TARGET_C33ADV || ENCODED_NAME_P( XSTR(XEXP(XEXP(X, 0),0),0) ) ) )			\
     goto ADDR;								\
  if (GET_CODE (X) == PLUS						\
      && CONSTANT_ADDRESS_P (XEXP (X, 1))				\
      && ((MODE == QImode)						\
	  || ((MODE == HImode) && (INTVAL (XEXP(X, 1)) % 2 == 0))	\
	  || ((MODE == SImode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == DImode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == SFmode) && (INTVAL (XEXP(X, 1)) % 4 == 0))	\
	  || ((MODE == DFmode) && (INTVAL (XEXP(X, 1)) % 4 == 0)))	\
      && RTX_OK_FOR_BASE_P (XEXP (X, 0))) goto ADDR;			\
} while (0)
/* CHG K.Watanabe V1.7 <<<<<<< */

/* Try machine-dependent ways of modifying an illegitimate address
   to be legitimate.  If we find one, return the new, valid address.
   This macro is used in only one place: `memory_address' in explow.c.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE and WIN are passed so that this macro can use
   GO_IF_LEGITIMATE_ADDRESS.

   It is always safe for this macro to do nothing.  It exists to recognize
   opportunities to optimize the output.   */

#define LEGITIMIZE_ADDRESS(X,OLDX,MODE,WIN)  {}

/* Go to LABEL if ADDR (a legitimate address expression)
   has an effect that depends on the machine mode it is used for.  */

#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL)  {}

/* Nonzero if the constant value X is a legitimate general operand.
   It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.  */
#define LEGITIMATE_CONSTANT_P(X)					\
  (GET_CODE (X) == CONST_DOUBLE						\
   || flag_pic								\
   || !(GET_CODE (X) == CONST						\
	&& GET_CODE (XEXP (X, 0)) == PLUS				\
	&& GET_CODE (XEXP (XEXP (X, 0), 0)) == SYMBOL_REF		\
	&& GET_CODE (XEXP (XEXP (X, 0), 1)) == CONST_INT		\
	&& ! CONST_OK_FOR_IMM26 (INTVAL (XEXP (XEXP (X, 0), 1)))))

/*********************/
/* L: Condition Code */
/*********************/

/* Tell final.c how to eliminate redundant test instructions.  */

/* Here we define machine-dependent flags and fields in cc_status
   (see `conditions.h').  No extra ones are needed for the vax.  */

/* Store in cc_status the expressions
   that the condition codes will describe
   after execution of an instruction whose pattern is EXP.
   Do not alter them if the instruction would not alter the cc's.  */

#define NOTICE_UPDATE_CC(EXP, INSN) notice_update_cc(EXP, INSN)


/************/
/* M: Costs */
/************/

/* A part of a C `switch' statement that describes the relative costs
   of constant RTL expressions.  It must contain `case' labels for
   expression codes `const_int', `const', `symbol_ref', `label_ref'
   and `const_double'.  Each case must ultimately reach a `return'
   statement to return the relative cost of the use of that kind of
   constant value in an expression.  The cost may depend on the
   precise value of the constant, which is available for examination
   in X, and the rtx code of the expression in which it is contained,
   found in OUTER_CODE.

   CODE is the expression code--redundant, since it can be obtained
   with `GET_CODE (X)'. */

#define CONST_COSTS(RTX,CODE,OUTER_CODE)				\
  case CONST_INT:							\
  case CONST_DOUBLE:							\
  case CONST:								\
  case SYMBOL_REF:							\
  case LABEL_REF:							\
    {									\
      int _zxy = const_costs(RTX, CODE);				\
      return (_zxy) ? COSTS_N_INSNS (_zxy) : 0;				\
    }

/* C33: According to V850 for now. */
/* A crude cut at RTX_COSTS for the V850.  */

/* Provide the costs of a rtl expression.  This is in the body of a
   switch on CODE.

   There aren't DImode MOD, DIV or MULT operations, so call them
   very expensive.  Everything else is pretty much a constant cost.  */

#define RTX_COSTS(RTX,CODE,OUTER_CODE)					\
  case ASHIFT:		/* C33 */					\
  case ASHIFTRT:	/* C33 */					\
  case LSHIFTRT:	/* C33 */					\
    {									\
      rtx stRtxOpt1 = XEXP (RTX, 1);					\
      									\
      if (GET_CODE (stRtxOpt1) == CONST_INT) {				\
									\
        if (INTVAL(stRtxOpt1) <= 8)					\
          return COSTS_N_INSNS(1);					\
									\
        if (INTVAL(stRtxOpt1) <= 16)					\
          return COSTS_N_INSNS(2);					\
									\
        if (INTVAL(stRtxOpt1) <= 24)					\
          return COSTS_N_INSNS(3);					\
									\
        return COSTS_N_INSNS(4);					\
      }									\
									\
      return COSTS_N_INSNS(8);						\
    }									\
									\
  case MOD:								\
  case DIV:								\
    return 60;								\
  case MULT:								\
    return 20;





/*#define ADDRESS_COST(ADDR) c33_address_cost (ADDR) */
/* C33: If the following line is not defined, 
        the reference of SYMBOL_REF of the memory is changed to the reference of REG.
        And the name is missed in this timing, and SIGSEGV occurs at the time of 
        outputting .comm. ( Is this a bug ? )
        reference to cse.c:find_best_add */
        
#define ADDRESS_COST(ADDR) 1	/* 000524 watanabe */


/* C33: If the following lines are not defined, the compile error occurs. */
/* Nonzero if access to memory by bytes or half words is no faster
   than accessing full words.  */
#define SLOW_BYTE_ACCESS 1

#define ADJUST_INSN_LENGTH(INSN, LENGTH) \
  LENGTH += c33_adjust_insn_length (INSN, LENGTH);


/***************/
/* N: Sections */
/***************/

/* The five different data regions on the c33.  */
typedef enum 
{
  DATA_AREA_NORMAL,
  DATA_AREA_SDA,
  DATA_AREA_TDA,
  DATA_AREA_ZDA,
  DATA_AREA_GDA
} c33_data_area;

/* A list of names for sections other than the standard two, which are
   `in_text' and `in_data'.  You need not define this macro on a
   system with no other sections (that GCC needs to use).  */
   
/* One or more functions to be defined in `varasm.c'.  These
   functions should do jobs analogous to those of `text_section' and
   `data_section', for your additional sections.  Do not define this
   macro if you do not define `EXTRA_SECTIONS'. */
#undef	EXTRA_SECTION_FUNCTIONS

/* A list of names for sections other than the standard two, which are
   `in_text' and `in_data'.  You need not define this macro on a
   system with no other sections (that GCC needs to use).  */
   
/* CHG K.Watanabe V1.7 >>>>>>> */
/* C33: Because "in_ctros, in_dtros" are already defined in varasm.c,
        delete them.                                                  */
#if 0
#undef	EXTRA_SECTIONS
#define EXTRA_SECTIONS in_tdata, in_sdata, in_zdata, in_gdata,	\
in_const, in_ctors, in_dtors,					\
in_rozdata, in_rosdata, in_rotdata, in_rogdata,			\
in_tbss, in_sbss, in_zbss, in_gbss,				\
in_tcommon, in_scommon, in_zcommon, in_gcommon
#endif

#define EXTRA_SECTIONS in_tdata, in_sdata, in_zdata, in_gdata,	\
in_const, 												\
in_rozdata, in_rosdata, in_rotdata, in_rogdata,			\
in_tbss, in_sbss, in_zbss, in_gbss,						\
in_tcommon, in_scommon, in_zcommon, in_gcommon
/* CHG K.Watanabe V1.7 <<<<<<<< */

/* This could be done a lot more cleanly using ANSI C ... */

/* CHG K.Watanabe V1.7 >>>>>>> */
/* C33: Because "in_ctros, in_dtros" are already defined in varasm.c,
        delete them.                                                  */
#if 0
#define EXTRA_SECTION_FUNCTIONS						\
CONST_SECTION_FUNCTION							\
CTORS_SECTION_FUNCTION							\
DTORS_SECTION_FUNCTION							\
									\
void									\
sdata_section ()							\
{									\
  if (in_section != in_sdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", SDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_sdata;						\
    }									\
}									\
									\
void									\
rosdata_section ()							\
{									\
  if (in_section != in_rosdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ROSDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_rosdata;						\
    }									\
}									\
									\
void									\
sbss_section ()								\
{									\
  if (in_section != in_sbss)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", SBSS_SECTION_ASM_OP);		\
      }		\
      in_section = in_sbss;						\
    }									\
}									\
									\
void									\
tdata_section ()							\
{									\
  if (in_section != in_tdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", TDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_tdata;						\
    }									\
}									\
									\
void									\
rotdata_section ()							\
{									\
  if (in_section != in_rotdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ROTDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_rotdata;						\
    }									\
}									\
									\
void									\
tbss_section ()								\
{									\
  if (in_section != in_tbss)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", TBSS_SECTION_ASM_OP);		\
      }		\
      in_section = in_tbss;						\
    }									\
}									\
									\
void									\
zdata_section ()							\
{									\
  if (in_section != in_zdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ZDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_zdata;						\
    }									\
}									\
									\
void									\
rozdata_section ()							\
{									\
  if (in_section != in_rozdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ROZDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_rozdata;						\
    }									\
}									\
									\
void									\
zbss_section ()								\
{									\
  if (in_section != in_zbss)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ZBSS_SECTION_ASM_OP);		\
      }		\
      in_section = in_zbss;						\
    }									\
}									\
									\
void									\
gdata_section ()							\
{									\
  if (in_section != in_gdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", GDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_gdata;						\
    }									\
}									\
									\
void									\
rogdata_section ()							\
{									\
  if (in_section != in_rogdata)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", ROGDATA_SECTION_ASM_OP);		\
      }		\
      in_section = in_rogdata;						\
    }									\
}									\
									\
void									\
gbss_section ()								\
{									\
  if (in_section != in_gbss)						\
    {									\
      if( i_32bit_Insn_Chk_Flg == 0 ){	\
      	fprintf (asm_out_file, "%s\n", GBSS_SECTION_ASM_OP);		\
      }		\
      in_section = in_gbss;						\
    }									\
}
#endif

#define EXTRA_SECTION_FUNCTIONS						\
void										\
const_section ()							\
{											\
  if (in_section != in_const)				\
    {										\
      fprintf (asm_out_file, "%s\n", CONST_SECTION_ASM_OP);		\
      in_section = in_const;				\
    }										\
}											\
											\
void										\
sdata_section ()							\
{											\
  if (in_section != in_sdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", SDATA_SECTION_ASM_OP);		\
      in_section = in_sdata;				\
    }										\
}											\
											\
void										\
rosdata_section ()							\
{											\
  if (in_section != in_rosdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", ROSDATA_SECTION_ASM_OP);		\
      in_section = in_rosdata;				\
    }										\
}											\
											\
void										\
sbss_section ()								\
{											\
  if (in_section != in_sbss)				\
    {										\
      fprintf (asm_out_file, "%s\n", SBSS_SECTION_ASM_OP);		\
      in_section = in_sbss;					\
    }										\
}											\
											\
void										\
tdata_section ()							\
{											\
  if (in_section != in_tdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", TDATA_SECTION_ASM_OP);		\
      in_section = in_tdata;				\
    }										\
}											\
											\
void										\
rotdata_section ()							\
{											\
  if (in_section != in_rotdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", ROTDATA_SECTION_ASM_OP);		\
      in_section = in_rotdata;				\
    }										\
}											\
											\
void										\
tbss_section ()								\
{											\
  if (in_section != in_tbss)				\
    {										\
      fprintf (asm_out_file, "%s\n", TBSS_SECTION_ASM_OP);		\
      in_section = in_tbss;					\
    }										\
}											\
											\
void										\
zdata_section ()							\
{											\
  if (in_section != in_zdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", ZDATA_SECTION_ASM_OP);		\
      in_section = in_zdata;				\
    }										\
}											\
											\
void										\
rozdata_section ()							\
{											\
  if (in_section != in_rozdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", ROZDATA_SECTION_ASM_OP);		\
      in_section = in_rozdata;				\
    }										\
}											\
											\
void										\
zbss_section ()								\
{											\
  if (in_section != in_zbss)				\
    {										\
      fprintf (asm_out_file, "%s\n", ZBSS_SECTION_ASM_OP);		\
      in_section = in_zbss;					\
    }										\
}											\
											\
void										\
gdata_section ()							\
{											\
  if (in_section != in_gdata)				\
    {										\
      fprintf (asm_out_file, "%s\n", GDATA_SECTION_ASM_OP);		\
      in_section = in_gdata;				\
    }										\
}											\
											\
void										\
rogdata_section ()							\
{											\
  if (in_section != in_rogdata)				\
    {										\
     fprintf (asm_out_file, "%s\n", ROGDATA_SECTION_ASM_OP);		\
      in_section = in_rogdata;				\
    }										\
}											\
											\
void										\
gbss_section ()								\
{											\
  if (in_section != in_gbss)				\
    {										\
      fprintf (asm_out_file, "%s\n", GBSS_SECTION_ASM_OP);		\
      in_section = in_gbss;					\
    }										\
}
/* CHG K.Watanabe V1.7 <<<<<<< */

// 2001/6/5 watanabe
/* C33: Though the value of "in_section" is different between "rosdata_section"
        and "rotdata_section" in v850, it may be a bug, so we changed to call
        the sections.                                                           */
#define TEXT_SECTION_ASM_OP  "\t.section .text"
#define DATA_SECTION_ASM_OP  "\t.section .data"
#define BSS_SECTION_ASM_OP   "\t.section .bss"
#define SDATA_SECTION_ASM_OP "\t.section .sdata,\"aw\""
#define SBSS_SECTION_ASM_OP  "\t.section .sbss,\"aw\""
#define ZDATA_SECTION_ASM_OP "\t.section .zdata,\"aw\""
#define ZBSS_SECTION_ASM_OP  "\t.section .zbss,\"aw\""
#define TDATA_SECTION_ASM_OP "\t.section .tdata,\"aw\""
#define TBSS_SECTION_ASM_OP  "\t.section .tbss,\"aw\""
#define GDATA_SECTION_ASM_OP "\t.section .gdata,\"aw\""
#define GBSS_SECTION_ASM_OP  "\t.section .gbss,\"aw\""
#define ROSDATA_SECTION_ASM_OP "\t.section .rosdata,\"a\""
#define ROZDATA_SECTION_ASM_OP "\t.section .rozdata,\"a\""
#define ROTDATA_SECTION_ASM_OP "\t.section .rotdata,\"a\""
#define ROGDATA_SECTION_ASM_OP "\t.section .rogdata,\"a\""
#undef	CONST_SECTION_ASM_OP
#define CONST_SECTION_ASM_OP "\t.section .rodata"

/* ADD K.Watanabe V1.7 >>>>>>> */
#define CTORS_SECTION_ASM_OP	"\t.section .ctors,\"aw\""
#define DTORS_SECTION_ASM_OP	"\t.section .dtors,\"aw\""

/* Support a read-only data section.  */
#define READONLY_DATA_SECTION_ASM_OP	"\t.section .rodata"
/* ADD K.Watanabe V1.7 <<<<<<< */

#define SCOMMON_ASM_OP 	       ".scomm"
#define ZCOMMON_ASM_OP 	       ".zcomm"
#define TCOMMON_ASM_OP 	       ".tcomm"
#define GCOMMON_ASM_OP 	       ".gcomm"

/* A C statement or statements to switch to the appropriate section
   for output of EXP.  You can assume that EXP is either a `VAR_DECL'
   node or a constant of some sort.  RELOC indicates whether the
   initial value of EXP requires link-time relocations.  Select the
   section by calling `text_section' or one of the alternatives for
   other sections.

   Do not define this macro if you put all read-only variables and
   constants in the read-only data section (usually the text section).  */
   
/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. Define the following contents in TARGET_ASM_SELECT_SECTION. */
#if 0
#undef  SELECT_SECTION
#define SELECT_SECTION(EXP, RELOC) c33_select_section(EXP, RELOC)
#endif
/* DEL K.Watanabe V1.7 <<<<<<< */

/* A C statement or statements to switch to the appropriate section
   for output of RTX in mode MODE.  You can assume that RTX is some
   kind of constant in RTL.  The argument MODE is redundant except in
   the case of a `const_int' rtx.  Select the section by calling
   `text_section' or one of the alternatives for other sections.

   Do not define this macro if you put all constants in the read-only
   data section.  */
/* #define SELECT_RTX_SECTION(MODE, RTX) */


#define OUTPUT_ADDR_CONST_EXTRA(FILE, X, FAIL)  \
  if (! c33_output_addr_const_extra (FILE, X)) \
     goto FAIL
     
/**********/
/* O: PIC */
/**********/

/***********************/
/* P: Assembler Format */
/***********************/

/* P-1: The Overall Framework of an Assembler File */
/* Output at beginning/end of assembler file.  */
#undef ASM_FILE_START
#define ASM_FILE_START(FILE) asm_file_start(FILE)

#define ASM_COMMENT_START ";"

/* Output to assembler file text saying following lines
   may contain character constants, extra white space, comments, etc.  */

/* C33: If the following line is not defined, the compile error occurs. */
#define ASM_APP_ON ";APP\n"

/* Output to assembler file text saying following lines
   no longer contain unusual constructs.  */

/* C33: If the following line is not defined, the compile error occurs. */
#define ASM_APP_OFF ";NO_APP\n"


/* P-2: Output of Data */

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. */
#if 0
/* This is how to output an assembler line defining a `double' constant.
   It is .double or .float, depending.  */

#define ASM_OUTPUT_DOUBLE(FILE, VALUE)			\
do { char dstr[30];					\
     REAL_VALUE_TO_DECIMAL ((VALUE), "%.20e", dstr);	\
     fprintf (FILE, "\t.double %s\n", dstr);		\
   } while (0)


/* This is how to output an assembler line defining a `float' constant.  */
#define ASM_OUTPUT_FLOAT(FILE, VALUE)			\
do { char dstr[30];					\
     REAL_VALUE_TO_DECIMAL ((VALUE), "%.20e", dstr);	\
     fprintf (FILE, "\t.float %s\n", dstr);		\
   } while (0)

/* This is how to output an assembler line defining an `int' constant.  */

#define ASM_OUTPUT_INT(FILE, VALUE)		\
( fprintf (FILE, "\t.long "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

/* Likewise for `char' and `short' constants.  */

#define ASM_OUTPUT_SHORT(FILE, VALUE)		\
( fprintf (FILE, "\t.hword "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

#define ASM_OUTPUT_CHAR(FILE, VALUE)		\
( fprintf (FILE, "\t.byte "),			\
  output_addr_const (FILE, (VALUE)),		\
  fprintf (FILE, "\n"))

/* This is how to output an assembler line for a numeric constant byte.  */
/* C33: If the following line is not defined, the compile error occurs. */
#define ASM_OUTPUT_BYTE(FILE, VALUE)  \
  fprintf (FILE, "\t.byte 0x%x\n", (VALUE))

#endif	/* #if 0 */
/* DEL K.Watanabe V1.7 <<<<<<< */

/* Define the parentheses used to group arithmetic operations
   in assembler code.  */


// DEL K.Watanabe V1.8 >>>>>>>
#if 0
/* C33: If the following line is not defined, the compile error occurs. */
#define ASM_OPEN_PAREN "("
#define ASM_CLOSE_PAREN ")"
#endif
// DEL K.Watanabe V1.8 <<<<<<<


/* P-3: Output of Uninitialized Variables */

/* This says how to output the assembler to define a global
   uninitialized but not common symbol.  */

#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
  asm_output_aligned_bss ((FILE), (DECL), (NAME), (SIZE), (ALIGN))

#undef  ASM_OUTPUT_ALIGNED_BSS 
#define ASM_OUTPUT_ALIGNED_BSS(FILE, DECL, NAME, SIZE, ALIGN) \
  c33_output_aligned_bss (FILE, DECL, NAME, SIZE, ALIGN)

/* This says how to output the assembler to define a global
   uninitialized, common symbol. */
#undef  ASM_OUTPUT_ALIGNED_COMMON
#undef  ASM_OUTPUT_COMMON
#define ASM_OUTPUT_ALIGNED_DECL_COMMON(FILE, DECL, NAME, SIZE, ALIGN) \
     c33_output_common (FILE, DECL, NAME, SIZE, ALIGN)

/* This says how to output the assembler to define a local
   uninitialized symbol. */
#undef  ASM_OUTPUT_ALIGNED_LOCAL
#undef  ASM_OUTPUT_LOCAL
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(FILE, DECL, NAME, SIZE, ALIGN) \
     c33_output_local (FILE, DECL, NAME, SIZE, ALIGN)
     

/* P-4: Output and Generation of Labels */

/* This is how to output the definition of a user-level label named NAME,
   such as the label on a static function or variable NAME.  */

/* C33: If the following line is not defined, the compile error occurs. */
#define ASM_OUTPUT_LABEL(FILE, NAME)	\
  do { assemble_name (FILE, NAME); fputs (":\n", FILE); } while (0)

/* This is how to output a command to make the user-level label named NAME
   defined for reference from other files.  */

/* ADD K.Watanabe V1.7 >>>>>>> */
/* Globalizing directive for a label.  */
#define GLOBAL_ASM_OP "\t.global\t"
/* ADD K.Watanabe V1.7 <<<<<<< */

/* This is how to output a reference to a user-level label named NAME.
   `assemble_name' uses this.  */

// CHG K.Watanabe V1.8 >>>>>>>
#if 0
#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE, NAME)	          \
  do {                                            \
  char* real_name;                                \
  STRIP_NAME_ENCODING (real_name, (NAME));        \
  fprintf (FILE, "%s", real_name);                \
  } while (0)           
#endif

#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE, NAME)	          \
    asm_fprintf (FILE, "%U%s", (*targetm.strip_name_encoding) (NAME)) 
// CHG K.Watanabe V1.8 <<<<<<<


/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)	\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10),	\
  sprintf ((OUTPUT), "%s___%d", (NAME), (LABELNO)))

/* This is how we tell the assembler that two symbols have the same value.  */

#define ASM_OUTPUT_DEF(FILE,NAME1,NAME2) \
  do { assemble_name(FILE, NAME1); 	 \
       fputs(" = ", FILE);		 \
       assemble_name(FILE, NAME2);	 \
       fputc('\n', FILE); } while (0)


/* P-5: How Initialization Functions Are Handled */

/* P-6: Macros Controlling Initialization Routines */

/* P-7: Output of Assembler Instructions */

/* How to refer to registers in assembler output.
   This sequence is indexed by compiler's hard-register-number (see above).  */
#define REGISTER_NAMES							\
{  "%r0",  "%r1",  "%r2",  "%r3",  "%r4",  "%r5",  "%r6" , "%r7",	\
   "%r8",  "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15",	\
   ".fp",  ".ap", "%sp" }

#define FINAL_PRESCAN_INSN(insn, operand, nop) final_prescan_insn (insn, operand, nop)

/* Print an instruction operand X on file FILE.
   look in c33.c for details */

#define PRINT_OPERAND(FILE, X, CODE)  print_operand (FILE, X, CODE)

#define PRINT_OPERAND_PUNCT_VALID_P(CODE) \
  (((CODE) == '.') || ((CODE) == '#'))

/* Print a memory operand whose address is X, on file FILE.
   This uses a function in output-vax.c.  */

#define PRINT_OPERAND_ADDRESS(FILE, ADDR) print_operand_address (FILE, ADDR)

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)
#define ASM_OUTPUT_REG_POP(FILE,REGNO)


/* P-8: Output of Dispatch Tables */

/* C33: If the following line is not defined, the default is enabled and the error occurs.(defaults.h) */
/* This is how to output an element of a case-vector that is absolute.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE) \
  asm_fprintf (FILE, "\t.long .L%d\n", VALUE)

/* P-9: Assembler Commands for Exception Regions */

/* P-10: Assembler Commands for Alignment */

#define ASM_OUTPUT_ALIGN(FILE,LOG)	\
  if ((LOG) != 0)			\
    fprintf (FILE, "\t.align %d\n", (LOG))


/*********************/
/* Q: Debugging Info */
/*********************/

/* We don't have to worry about dbx compatibility for the c33.  */
#define DEFAULT_GDB_EXTENSIONS 1

/* Use stabs debugging info by default.  */
#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DBX_DEBUG

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Delete according to V850.
        The following line is defined in defaults.h */
#if 0
#undef DBX_REGISTER_NUMBER
#define DBX_REGISTER_NUMBER(REGNO) REGNO
#endif
/* DEL K.Watanabe V1.7 <<<<<<< */

/************************/
/* R: Cross-compilation */
/************************/


/***********/
/* S: Misc */
/***********/

/* Define this if you have defined special-purpose predicates in the
   file `MACHINE.c'.  This macro is called within an initializer of an
   array of structures.  The first field in the structure is the name
   of a predicate and the second field is an array of rtl codes.  For
   each predicate, list all rtl codes that can be in expressions
   matched by the predicate.  The list should have a trailing comma.  */

/* C33: Why is "CONST" not in "special_symbolref_operand"? */

#define PREDICATE_CODES							\
{ "call_address_operand",	{ REG, SYMBOL_REF }},			\
{ "power_of_two_operand",	{ CONST_INT }},				\
{ "not_power_of_two_operand",	{ CONST_INT }},				\
{ "general_operand_post_inc",	{ CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF,	\
			  	  LABEL_REF, SUBREG, REG, MEM}},


/* Define to use software floating point emulator for REAL_ARITHMETIC and
   decimal <-> binary conversion. */


/* C33: If the following line is not defined, the compile error occurs. */
/* Specify the machine mode that this machine uses
   for the index in the tablejump instruction.  */
#define CASE_VECTOR_MODE Pmode

#define WORD_REGISTER_OPERATIONS


/* Byte and short loads sign extend the value to a word.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND


// DEL K.Watanabe V1.8 >>>>>>>
#if 0
/* C33: If the following line is not defined, the compile error occurs. */
/* This is the kind of divide that is easiest to do in the general case.  */
#define EASY_DIV_EXPR TRUNC_DIV_EXPR
#endif
// DEL K.Watanabe V1.8 <<<<<<<<


/* >>>>> delete iruma m.takeishi '04.02.24 */
/* C33: Correction in which the change of the library specification was refrected.
        Revert to the old code.                                                    */
/* >>>>> add iruma m.takeishi '03.11.19 */
/* C33: Change the libgcc function names which are the default call. */
/* C33: /gcc/opttabs.c -- Overrideed in "init_optabs" function ( Only PE )*/
#if 0
#define INIT_TARGET_OPTABS \
        if ( TARGET_C33PE ) { \
            init_integral_libfuncs (sdiv_optab, "div_pe", '3'); \
            init_integral_libfuncs (udiv_optab, "udiv_pe", '3'); \
            init_integral_libfuncs (smod_optab, "mod_pe", '3'); \
            init_integral_libfuncs (umod_optab, "umod_pe", '3'); \
            \
            \
            init_floating_libfuncs (add_optab, "add_pe", '3'); \
            init_floating_libfuncs (sub_optab, "sub_pe", '3'); \
            /* >>>>> add iruma m.takeishi '03.12.22 */ \
            init_floating_libfuncs (smul_optab, "mul_pe", '3'); \
            /* <<<<< add iruma m.takeishi '03.12.22 */ \
            init_floating_libfuncs (flodiv_optab, "div_pe", '3'); \
            \
            floatsisf_libfunc = gen_rtx_SYMBOL_REF (Pmode, "__float_pesisf"); \
            floatsidf_libfunc = gen_rtx_SYMBOL_REF (Pmode, "__float_pesidf"); \
            extendsfdf2_libfunc = gen_rtx_SYMBOL_REF (Pmode, "__extend_pesfdf2"); \
        }
/* <<<<< add iruma m.takeishi '03.11.19 */
#endif
/* <<<<< delete iruma m.takeishi '04.02.24 */


/* C33: If the following line is not defined, the compile error occurs. */
/* Max number of bytes we can move from memory to memory
   in one reasonably fast instruction.  */
#define MOVE_MAX	4


/* C33: If the following line is not defined, the link error occurs. */
/* Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
   is done just by pretending it is already truncated.  */
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1


/* Specify the machine mode that pointers have.
   After generation of rtl, the compiler makes no further distinction
   between pointers and any other objects of this machine mode.  */
#define Pmode SImode

/* C33: If the following line is not defined, the compile error occurs. */
/* A function address in a call instruction
   is a byte address (for indexing purposes)
   so give the MEM rtx a byte's mode.  */
#define FUNCTION_MODE QImode

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused */
#if 0
/* A C expression whose value is nonzero if IDENTIFIER with arguments ARGS
   is a valid machine specific attribute for DECL.
   The attributes in ATTRIBUTES have previously been assigned to DECL.  */
#define VALID_MACHINE_DECL_ATTRIBUTE(DECL, ATTRIBUTES, IDENTIFIER, ARGS) \
c33_valid_machine_decl_attribute (DECL, IDENTIFIER, ARGS)
#endif
/* DEL K.Watanabe V1.7 <<<<<<< */


/******************/
/* C33: X: Others */
/******************/

/* Implement ZDA, TDA, SDA, and GDA */

/* DEL K.Watanabe V1.7 >>>>>>> */
/* C33: Unused. Define the following contents in "TARGET_ENCODE_SECTION_INFO". */
#if 0
#define ENCODE_SECTION_INFO(DECL)					\
do {									\
  if ((TREE_STATIC (DECL) || DECL_EXTERNAL (DECL))			\
      && TREE_CODE (DECL) == VAR_DECL)					\
    c33_encode_data_area (DECL);					\
} while (0)
#endif
/* DEL K.Watanabe V1.7 <<<<<<< */

#define ZDA_NAME_FLAG_CHAR '@'
#define TDA_NAME_FLAG_CHAR '%'
#define SDA_NAME_FLAG_CHAR '&'
#define GDA_NAME_FLAG_CHAR '$'

#define ZDA_NAME_P(NAME) (*(NAME) == ZDA_NAME_FLAG_CHAR)
#define TDA_NAME_P(NAME) (*(NAME) == TDA_NAME_FLAG_CHAR)
#define SDA_NAME_P(NAME) (*(NAME) == SDA_NAME_FLAG_CHAR)
#define GDA_NAME_P(NAME) (*(NAME) == GDA_NAME_FLAG_CHAR)

#define ENCODED_NAME_P(SYMBOL_NAME)    \
  (ZDA_NAME_P (SYMBOL_NAME)            \
   || TDA_NAME_P (SYMBOL_NAME)         \
   || SDA_NAME_P (SYMBOL_NAME)         \
   || GDA_NAME_P (SYMBOL_NAME))


// DEL K.Watanabe V1.8 >>>>>>>
#if 0
#define STRIP_NAME_ENCODING(VAR,SYMBOL_NAME) \
     (VAR) = (SYMBOL_NAME) + ENCODED_NAME_P (SYMBOL_NAME)
#endif
// DEL K.Watanabe V1.8 <<<<<<<


extern int const_costs ();
extern void print_operand ();
extern void print_operand_address ();
extern char *output_move_single ();
extern int power_of_two_operand ();
extern int not_power_of_two_operand ();
extern int compute_frame_size ();
extern void expand_prologue ();
extern void expand_epilogue ();
extern void notice_update_cc ();
extern void declare_object ();

