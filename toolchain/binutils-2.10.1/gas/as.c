/* as.c - GAS main program.
   Copyright (C) 1987, 1990, 91, 92, 93, 94, 95, 96, 97, 98, 99, 2000, 2001
   Free Software Foundation, Inc.

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

/*
 * Main program for AS; a 32-bit assembler of GNU.
 * Understands command arguments.
 * Has a few routines that don't fit in other modules because they
 * are shared.
 *
 *
 *          bugs
 *
 * : initialisers
 *  Since no-one else says they will support them in future: I
 * don't support them now.
 *
 */

#include "ansidecl.h"

#define C33_AS_REVISION "2.15 <2008/05/22>"

/* >>>>> DELETED D.Fujimoto 2007/10/15 e_machine set in elf.c */
//#define EM_SE_C33      107      /* S1C33 Family of Seiko Epson processor add T.Tazaki 2003/05/14*/
/* <<<<< DELETED D.Fujimoto 2007/10/15 e_machine set in elf.c */

#define COMMON

#include "as.h"
#include "subsegs.h"
#include "output-file.h"
#include "sb.h"
#include "macro.h"
#include "ext_remove.h"				// add D.Fujimoto 2007/02/28

#ifdef HAVE_ITBL_CPU
#include "itbl-ops.h"
#else
#define itbl_parse(itbl_file) 1
#define itbl_init()
#endif

#ifdef HAVE_SBRK
#ifdef NEED_DECLARATION_SBRK
extern PTR sbrk ();
#endif
#endif

static void show_usage PARAMS ((FILE *));
static void parse_args PARAMS ((int *, char ***));
static void dump_statistics PARAMS ((void));
static void perform_an_assembly_pass PARAMS ((int argc, char **argv));
static int macro_expr PARAMS ((const char *, int, sb *, int *));

int listing;            /* true if a listing is wanted */

/* add T.Tazaki 2002.04.26 >>> */
int g_listing = 0;
/* add T.Tazaki 2002.04.26 <<< */

static char *listing_filename = NULL;   /* Name of listing file.  */

/* Type of debugging to generate.  */

enum debug_info_type debug_type = DEBUG_NONE;

/* Maximum level of macro nesting.  */

int max_macro_nest = 100;

char *myname;           /* argv[0] */
#ifdef BFD_ASSEMBLER
segT reg_section, expr_section;
segT text_section, data_section, bss_section;
#endif

/* The default obstack chunk size.  If we set this to zero, the
   obstack code will use whatever will fit in a 4096 byte block.  */
int chunksize = 0;

/* To monitor memory allocation more effectively, make this non-zero.
   Then the chunk sizes for gas and bfd will be reduced.  */
int debug_memory = 0;

/* We build a list of defsyms as we read the options, and then define
   them after we have initialized everything.  */

struct defsym_list
{
  struct defsym_list *next;
  char *name;
  valueT value;
};

static struct defsym_list *defsyms;

/* Keep a record of the itbl files we read in. */

struct itbl_file_list
{
  struct itbl_file_list *next;
  char *name;
};

static struct itbl_file_list *itbl_files;

#ifdef USE_EMULATIONS
#define EMULATION_ENVIRON "AS_EMULATION"

extern struct emulation mipsbelf, mipslelf, mipself;
extern struct emulation mipsbecoff, mipslecoff, mipsecoff;
extern struct emulation i386coff, i386elf, i386aout;

static struct emulation *const emulations[] = { EMULATIONS };
static const int n_emulations = sizeof (emulations) / sizeof (emulations[0]);

static void select_emulation_mode PARAMS ((int, char **));

static void
select_emulation_mode (argc, argv)
     int argc;
     char **argv;
{
  int i;
  char *p, *em = 0;

  for (i = 1; i < argc; i++)
    if (!strncmp ("--em", argv[i], 4))
      break;

  if (i == argc)
    goto do_default;

  p = strchr (argv[i], '=');
  if (p)
    p++;
  else
    p = argv[i+1];

  if (!p || !*p)
    as_fatal (_("missing emulation mode name"));
  em = p;

 do_default:
  if (em == 0)
    em = getenv (EMULATION_ENVIRON);
  if (em == 0)
    em = DEFAULT_EMULATION;

  if (em)
    {
      for (i = 0; i < n_emulations; i++)
    if (!strcmp (emulations[i]->name, em))
      break;
      if (i == n_emulations)
    as_fatal (_("unrecognized emulation name `%s'"), em);
      this_emulation = emulations[i];
    }
  else
    this_emulation = emulations[0];

  this_emulation->init ();
}

const char *
default_emul_bfd_name ()
{
  abort ();
  return NULL;
}

void
common_emul_init ()
{
  this_format = this_emulation->format;

  if (this_emulation->leading_underscore == 2)
    this_emulation->leading_underscore = this_format->dfl_leading_underscore;

  if (this_emulation->default_endian != 2)
    target_big_endian = this_emulation->default_endian;

  if (this_emulation->fake_label_name == 0)
    {
      if (this_emulation->leading_underscore)
    this_emulation->fake_label_name = "L0\001";
      else
    /* What other parameters should we test?  */
    this_emulation->fake_label_name = ".L0\001";
    }
}
#endif

void
print_version_id ()
{
  static int printed;
  if (printed)
    return;
  printed = 1;

#ifdef BFD_ASSEMBLER
/* change T.Tazaki 2002.01.31 >>> */
/*
  fprintf (stderr, _("GNU assembler version %s (%s) using BFD version %s"),
       VERSION, TARGET_ALIAS, BFD_VERSION);
*/
  fprintf (stderr, _("GNU assembler version %s (%s) using BFD version %s(rev %s)"),
       VERSION, TARGET_ALIAS, BFD_VERSION, C33_AS_REVISION);
/* change T.Tazaki 2002.01.31 <<< */
#else
  fprintf (stderr, _("GNU assembler version %s (%s)"), VERSION, TARGET_ALIAS);
#endif
  fprintf (stderr, "\n");
}

static void
show_usage (stream)
     FILE *stream;
{
  fprintf (stream, _("Usage: %s [option...] [asmfile...]\n"), myname);

  fprintf (stream, _("\
Options:\n\
  -a[sub-option...]   turn on listings\n\
                          Sub-options [default hls]:\n\
                          c      omit false conditionals\n\
                          d      omit debugging directives\n\
                          h      include high-level source\n\
                          l      include assembly\n\
                          m      include macro expansions\n\
                          n      omit forms processing\n\
                          s      include symbols\n\
                          L      include line debug statistics (if applicable)\n\
                          =FILE  list to FILE (must be last sub-option)\n"));

  fprintf (stream, _("\
  -D                      produce assembler debugging messages\n"));
  fprintf (stream, _("\
  --defsym SYM=VAL        define symbol SYM to given value\n"));
#ifdef USE_EMULATIONS
  {
    int i;
    char *def_em;

    fprintf (stream, "\
  --em=[");
    for (i = 0; i < n_emulations-1; i++)
      fprintf (stream, "%s | ", emulations[i]->name);
    fprintf (stream, "%s]\n", emulations[i]->name);

    def_em = getenv (EMULATION_ENVIRON);
    if (!def_em) 
      def_em = DEFAULT_EMULATION;
    fprintf (stream, _("\
                          emulate output (default %s)\n"), def_em);
  }
#endif
  fprintf (stream, _("\
  -f                      skip whitespace and comment preprocessing\n"));
  fprintf (stream, _("\
  --gstabs                generate stabs debugging information\n"));
  fprintf (stream, _("\
  --gdwarf2               generate DWARF2 debugging information\n"));
  fprintf (stream, _("\
  --help                  show this message and exit\n"));
  fprintf (stream, _("\
  -I DIR                  add DIR to search list for .include directives\n"));
  fprintf (stream, _("\
  -J                      don't warn about signed overflow\n"));
  fprintf (stream, _("\
  -K                      warn when differences altered for long displacements\n"));
  fprintf (stream, _("\
  -L,--keep-locals        keep local symbols (e.g. starting with `L')\n"));
  fprintf (stream, _("\
  -M,--mri                assemble in MRI compatibility mode\n"));
  fprintf (stream, _("\
  --MD FILE               write dependency information in FILE (default none)\n"));
  fprintf (stream, _("\
  -nocpp                  ignored\n"));
  fprintf (stream, _("\
  -o OBJFILE              name the object-file output OBJFILE (default a.out)\n"));
  fprintf (stream, _("\
  -R                      fold data section into text section\n"));
  fprintf (stream, _("\
  --statistics            print various measured statistics from execution\n"));
  fprintf (stream, _("\
  --strip-local-absolute  strip local absolute symbols\n"));
  fprintf (stream, _("\
  --traditional-format    Use same format as native assembler when possible\n"));
  fprintf (stream, _("\
  --version               print assembler version number and exit\n"));
  fprintf (stream, _("\
  -W  --no-warn           suppress warnings\n"));
  fprintf (stream, _("\
  --warn                  don't suppress warnings\n"));
  fprintf (stream, _("\
  --fatal-warnings        treat warnings as errors\n"));
  fprintf (stream, _("\
  --itbl INSTTBL          extend instruction set to include instructions\n\
                          matching the specifications defined in file INSTTBL\n"));
  fprintf (stream, _("\
  -w                      ignored\n"));
  fprintf (stream, _("\
  -X                      ignored\n"));
  fprintf (stream, _("\
  -Z                      generate object file even after errors\n"));
  fprintf (stream, _("\
  --listing-lhs-width     set the width in words of the output data column of\n\
                          the listing\n"));
  fprintf (stream, _("\
  --listing-lhs-width2    set the width in words of the continuation lines\n\
                          of the output data column; ignored if smaller than\n\
                          the width of the first line\n"));
  fprintf (stream, _("\
  --listing-rhs-width     set the max width in characters of the lines from\n\
                          the source file\n"));
  fprintf (stream, _("\
  --listing-cont-lines    set the maximum number of continuation lines used\n\
                          for the output data column of the listing\n"));

/* add 2002.01.25 >>>> */

  fprintf (stream, _("\
  -mc33adv                set advanced macro assemble mode\n"));

/* add 2002.01.25 <<<< */

/* add T.Tazaki 2003/11/18 >>>> */

  fprintf (stream, _("\
  -mc33pe                 set PE macro assemble mode\n"));

/* add T.Tazaki 2003/11/18 <<<< */


// ADD D.Fujimoto usage for -mc33_ext option 2007/02/28 >>>>>>>
  fprintf (stream, _("\
  -mc33_ext               optimize ext code\n"));
// ADD D.Fujimoto usage for -mc33_ext option 2007/02/28 <<<<<<<

  md_show_usage (stream);

  fputc ('\n', stream);
  fprintf (stream, _("Report bugs to %s\n"), REPORT_BUGS_TO);
}

/*
 * Since it is easy to do here we interpret the special arg "-"
 * to mean "use stdin" and we set that argv[] pointing to "".
 * After we have munged argv[], the only things left are source file
 * name(s) and ""(s) denoting stdin. These file names are used
 * (perhaps more than once) later.
 *
 * check for new machine-dep cmdline options in
 * md_parse_option definitions in config/tc-*.c
 */

/* >>>>>>> add tazaki 2001.11.02 */
int g_iAdvance = 0;     /* 0=STANDARD Architecture Mode */
                        /* 1=Advanced Architecture Mode */
/* <<<<<<< add tazaki 2001.11.02 */

/* add T.Tazaki 2003/11/18 >>> */
int g_iPE = 0;			/* 1=PE Architecture Mode */
/* add T.Tazaki 2003/11/18 <<< */
/* add T.Tazaki 2004/07/30 >>> */
int g_iMedda32 = 0;			/* 1= No use default data area Mode */
/* add T.Tazaki 2004/07/30 <<< */


static void
parse_args (pargc, pargv)
     int *pargc;
     char ***pargv;
{
  int old_argc, new_argc;
  char **old_argv, **new_argv;

// ADD D.Fujimoto 2007/06/25 >>>>>>>
  int flag_plural_src = 0;			// specified plural source files
// ADD D.Fujimoto 2007/06/25 <<<<<<<
// ADD D.Fujimoto 2008/01/07 >>>>>>>
  int check_all_dump_flg = 0;		// flag whether to check the all objects' dump file 
  int i_len;
// ADD D.Fujimoto 2008/01/07 <<<<<<<

  /* Starting the short option string with '-' is for programs that
     expect options and other ARGV-elements in any order and that care about
     the ordering of the two.  We describe each non-option ARGV-element
     as if it were the argument of an option with character code 1.  */

  char *shortopts;
  extern CONST char *md_shortopts;
  static const char std_shortopts[] =
    {
      '-', 'J',
#ifndef WORKING_DOT_WORD
      /* -K is not meaningful if .word is not being hacked.  */
      'K',
#endif
      'L', 'M', 'R', 'W', 'Z', 'f', 'a', ':', ':', 'D', 'I', ':', 'o', ':',
#ifndef VMS
      /* -v takes an argument on VMS, so we don't make it a generic
         option.  */
      'v',
#endif
      'w', 'X',
      /* New option for extending instruction set (see also --itbl below) */
      't', ':',
      '\0'
    };
  struct option *longopts;
  extern struct option md_longopts[];
  extern size_t md_longopts_size;
  static const struct option std_longopts[] = {
#define OPTION_HELP (OPTION_STD_BASE)
    {"help", no_argument, NULL, OPTION_HELP},
    {"keep-locals", no_argument, NULL, 'L'},
    {"mri", no_argument, NULL, 'M'},
#define OPTION_NOCPP (OPTION_STD_BASE + 1)
    {"nocpp", no_argument, NULL, OPTION_NOCPP},
#define OPTION_STATISTICS (OPTION_STD_BASE + 2)
    {"statistics", no_argument, NULL, OPTION_STATISTICS},
#define OPTION_VERSION (OPTION_STD_BASE + 3)
    {"version", no_argument, NULL, OPTION_VERSION},
#define OPTION_DUMPCONFIG (OPTION_STD_BASE + 4)
    {"dump-config", no_argument, NULL, OPTION_DUMPCONFIG},
#define OPTION_VERBOSE (OPTION_STD_BASE + 5)
    {"verbose", no_argument, NULL, OPTION_VERBOSE},
#define OPTION_EMULATION (OPTION_STD_BASE + 6)
    {"emulation", required_argument, NULL, OPTION_EMULATION},
#define OPTION_DEFSYM (OPTION_STD_BASE + 7)
    {"defsym", required_argument, NULL, OPTION_DEFSYM},
#define OPTION_INSTTBL (OPTION_STD_BASE + 8)
    /* New option for extending instruction set (see also -t above).
       The "-t file" or "--itbl file" option extends the basic set of
       valid instructions by reading "file", a text file containing a
       list of instruction formats.  The additional opcodes and their
       formats are added to the built-in set of instructions, and
       mnemonics for new registers may also be defined.  */
    {"itbl", required_argument, NULL, OPTION_INSTTBL},
#define OPTION_LISTING_LHS_WIDTH (OPTION_STD_BASE + 9)
    {"listing-lhs-width", required_argument, NULL, OPTION_LISTING_LHS_WIDTH},
#define OPTION_LISTING_LHS_WIDTH2 (OPTION_STD_BASE + 10)
    {"listing-lhs-width", required_argument, NULL, OPTION_LISTING_LHS_WIDTH2},
#define OPTION_LISTING_RHS_WIDTH (OPTION_STD_BASE + 11)
    {"listing-rhs-width", required_argument, NULL, OPTION_LISTING_RHS_WIDTH},
#define OPTION_LISTING_CONT_LINES (OPTION_STD_BASE + 12)
    {"listing-cont-lines", required_argument, NULL, OPTION_LISTING_CONT_LINES},
#define OPTION_DEPFILE (OPTION_STD_BASE + 13)
    {"MD", required_argument, NULL, OPTION_DEPFILE},
#define OPTION_GSTABS (OPTION_STD_BASE + 14)
    {"gstabs", no_argument, NULL, OPTION_GSTABS},
#define OPTION_STRIP_LOCAL_ABSOLUTE (OPTION_STD_BASE + 15)
    {"strip-local-absolute", no_argument, NULL, OPTION_STRIP_LOCAL_ABSOLUTE},
#define OPTION_TRADITIONAL_FORMAT (OPTION_STD_BASE + 16)
    {"traditional-format", no_argument, NULL, OPTION_TRADITIONAL_FORMAT},
#define OPTION_GDWARF2 (OPTION_STD_BASE + 17)
    {"gdwarf2", no_argument, NULL, OPTION_GDWARF2},
    {"no-warn", no_argument, NULL, 'W'},
#define OPTION_WARN (OPTION_STD_BASE + 18)
    {"warn", no_argument, NULL, OPTION_WARN},
#define OPTION_WARN_FATAL (OPTION_STD_BASE + 19)
    {"fatal-warnings", no_argument, NULL, OPTION_WARN_FATAL},
    
/* add tazaki 2001.12.11 Avdanced Option >>>>>> */

#define OPTION_ADVANCE (OPTION_STD_BASE + 20)
    {"mc33adv", no_argument, NULL, OPTION_ADVANCE},
    
/* <<<<<<<< add tazaki 2001.12.11 */
/* add T.Tazaki 2003/11/18 PE Option >>>>>> */

#define OPTION_PE (OPTION_STD_BASE + 21)
    {"mc33pe", no_argument, NULL, OPTION_PE},
    
/* <<<<<<<< add T.Tazaki 2003/11/18 */

/* -medda32 add T.Tazaki 2004/07/30 >>> */
#define OPTION_MEDDA32 (OPTION_STD_BASE + 22)
    {"medda32", no_argument, NULL, OPTION_MEDDA32},
/* -medda32 add T.Tazaki 2004/07/30 <<< */

// ADD D.Fujimoto mc33_ext option 2007/03/01 >>>>>>>
#define OPTION_C33_EXT (OPTION_STD_BASE + 23)
    {"mc33_ext", required_argument, NULL, OPTION_C33_EXT}
// ADD D.Fujimoto mc33_ext option 2007/03/01 <<<<<<<


  };

  /* Construct the option lists from the standard list and the
     target dependent list.  */
  shortopts = concat (std_shortopts, md_shortopts, (char *) NULL);
  longopts = (struct option *) xmalloc (sizeof (std_longopts) + md_longopts_size);
  memcpy (longopts, std_longopts, sizeof (std_longopts));
  memcpy ((char *) longopts + sizeof (std_longopts),
      md_longopts, md_longopts_size);

  /* Make a local copy of the old argv.  */
  old_argc = *pargc;
  old_argv = *pargv;

  /* Initialize a new argv that contains no options.  */
  new_argv = (char **) xmalloc (sizeof (char *) * (old_argc + 1));
  new_argv[0] = old_argv[0];
  new_argc = 1;
  new_argv[new_argc] = NULL;

  while (1)
    {
      /* getopt_long_only is like getopt_long, but '-' as well as '--' can
     indicate a long option.  */
      int longind;
      int optc = getopt_long_only (old_argc, old_argv, shortopts, longopts,
                   &longind);

// ADD D.Fujimoto 2008/01/07 >>>>>>>
	  // check the all objects' dump file from command line
	  if( check_all_dump_flg == 1 ){
		  check_all_dump_flg = 0;

		  // save the dump file name
		 if( optarg == 0 ){
			  fprintf (stderr, _("Error : Cannot find the all objects\' dump file.\n") );
			  xexit (EXIT_FAILURE);
		 } else {
			  i_len = strlen( optarg );
              if( 0 == memcmp( ".dump",&(optarg[i_len-5]),5 ) ){
				  cp_All_Dump_File_Name = optarg;
				  optc = getopt_long_only (old_argc, old_argv, shortopts, longopts,&longind);
			  } else {
				  fprintf (stderr, _("Error : Cannot find the all objects\' dump file.\n") );
				  xexit (EXIT_FAILURE);
			  }
		  }
	  }
// ADD D.Fujimoto 2008/01/07 <<<<<<<


      if (optc == -1)
    break;

      switch (optc)
    {
    default:
      /* md_parse_option should return 1 if it recognizes optc,
         0 if not.  */
      if (md_parse_option (optc, optarg) != 0)
        break;
      /* `-v' isn't included in the general short_opts list, so check for
         it explicity here before deciding we've gotten a bad argument.  */
      if (optc == 'v')
        {
#ifdef VMS
          /* Telling getopt to treat -v's value as optional can result
         in it picking up a following filename argument here.  The
         VMS code in md_parse_option can return 0 in that case,
         but it has no way of pushing the filename argument back.  */
          if (optarg && *optarg)
        new_argv[new_argc++] = optarg,  new_argv[new_argc] = NULL;
          else
#else
          case 'v':
#endif
          case OPTION_VERBOSE:
        print_version_id ();
          break;
        }
      /*FALLTHRU*/

    case '?':
      exit (EXIT_FAILURE);

    case 1:         /* File name.  */
      if (!strcmp (optarg, "-"))
        optarg = "";
      new_argv[new_argc++] = optarg;
      new_argv[new_argc] = NULL;


// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
		// save the source file name
		if( cp_Current_File_Name == 0 ){
			cp_Current_File_Name = xstrdup(optarg);
		} else {
			flag_plural_src = 1;
		}
#endif
// ADD D.Fujimoto 2007/06/25 <<<<<<<

      break;

/* add tazaki 2001.11.02 Avdanced Option >>>>>> */
    case OPTION_ADVANCE:
        g_iAdvance = 1;
        break;

/* add tazaki 2001.11.02 Avdanced Option <<<<<< */

/* add T.Tazaki 2003/11/18 PE Option >>>>>> */
    case OPTION_PE:
        g_iPE = 1;
        break;

/* add T.Tazaki 2003/11/18 PE Option <<<<<< */
/* add T.Tazaki 2004/07/30 -medda32 Option >>> */
    case OPTION_MEDDA32:
        g_iMedda32 = 1;
        break;

/* add T.Tazaki 2004/07/30 -medda32 Option <<< */

// ADD D.Fujimoto check mc33_ext argument file 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
	case OPTION_C33_EXT:
		if( optarg != NULL ){
			int i_len = strlen( optarg );
			if( 0 != memcmp( ".dump",&(optarg[i_len-5]),5 ) ){
				fprintf (stderr, _("Error : Cannot find the dump file.\n"));
				xexit (EXIT_FAILURE);
			} else {
				cp_Dump_File_Name = xstrdup(optarg);
// ADD D.Fujimoto 2008/01/07 >>>>>>>
				check_all_dump_flg = 1;
// ADD D.Fujimoto 2008/01/07 <<<<<<<
			}
		} else {
			fprintf (stderr, _("Error : Cannot find the dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		break;
#endif
// ADD D.Fujimoto check mc33_ext argument file 2007/06/25 <<<<<<<

    case OPTION_HELP:
      show_usage (stdout);
      exit (EXIT_SUCCESS);

    case OPTION_NOCPP:
      break;

    case OPTION_STATISTICS:
      flag_print_statistics = 1;
      break;

    case OPTION_STRIP_LOCAL_ABSOLUTE:
      flag_strip_local_absolute = 1;
      break;

    case OPTION_TRADITIONAL_FORMAT:
      flag_traditional_format = 1;
      break;

    case OPTION_VERSION:
      /* This output is intended to follow the GNU standards document.  */
      printf (_("GNU assembler %s\n"), VERSION);
      printf (_("Copyright 2000 Free Software Foundation, Inc.\n"));
      printf (_("\
This program is free software; you may redistribute it under the terms of\n\
the GNU General Public License.  This program has absolutely no warranty.\n"));
      printf (_("This assembler was configured for a target of `%s'.\n"),
          TARGET_ALIAS);
      exit (EXIT_SUCCESS);

    case OPTION_EMULATION:
#ifdef USE_EMULATIONS
      if (strcmp (optarg, this_emulation->name))
        as_fatal (_("multiple emulation names specified"));
#else
      as_fatal (_("emulations not handled in this configuration"));
#endif
      break;

    case OPTION_DUMPCONFIG:
      fprintf (stderr, _("alias = %s\n"), TARGET_ALIAS);
      fprintf (stderr, _("canonical = %s\n"), TARGET_CANONICAL);
      fprintf (stderr, _("cpu-type = %s\n"), TARGET_CPU);
#ifdef TARGET_OBJ_FORMAT
      fprintf (stderr, _("format = %s\n"), TARGET_OBJ_FORMAT);
#endif
#ifdef TARGET_FORMAT
      fprintf (stderr, _("bfd-target = %s\n"), TARGET_FORMAT);
#endif
      exit (EXIT_SUCCESS);

    case OPTION_DEFSYM:
      {
        char *s;
        long i;
        struct defsym_list *n;

        for (s = optarg; *s != '\0' && *s != '='; s++)
          ;
        if (*s == '\0')
          as_fatal (_("bad defsym; format is --defsym name=value"));
        *s++ = '\0';
        i = strtol (s, (char **) NULL, 0);
        n = (struct defsym_list *) xmalloc (sizeof *n);
        n->next = defsyms;
        n->name = optarg;
        n->value = i;
        defsyms = n;
      }
      break;

    case OPTION_INSTTBL:
    case 't':
      {
        /* optarg is the name of the file containing the instruction 
           formats, opcodes, register names, etc. */
        struct itbl_file_list *n;

        if (optarg == NULL)
          {
        as_warn ( _("No file name following -t option\n") );
        break;
          }
        
        n = (struct itbl_file_list *) xmalloc (sizeof *n);
        n->next = itbl_files;
        n->name = optarg;
        itbl_files = n;

        /* Parse the file and add the new instructions to our internal
           table.  If multiple instruction tables are specified, the 
           information from this table gets appended onto the existing 
           internal table. */
        itbl_files->name = xstrdup (optarg);
        if (itbl_parse (itbl_files->name) != 0)
          {
        fprintf (stderr, _("Failed to read instruction table %s\n"), 
             itbl_files->name);
        exit (EXIT_SUCCESS);
          }
      }
      break;

    case OPTION_DEPFILE:
      start_dependencies (optarg);
      break;

    case OPTION_GSTABS:
      debug_type = DEBUG_STABS;
      break;
 
    case OPTION_GDWARF2:
      debug_type = DEBUG_DWARF2;
      break;

    case 'J':
      flag_signed_overflow_ok = 1;
      break;

#ifndef WORKING_DOT_WORD
    case 'K':
      flag_warn_displacement = 1;
      break;
#endif

    case 'L':
      flag_keep_locals = 1;
      break;

    case OPTION_LISTING_LHS_WIDTH:
      listing_lhs_width = atoi(optarg);
      if (listing_lhs_width_second < listing_lhs_width)
        listing_lhs_width_second = listing_lhs_width;
      break;
    case OPTION_LISTING_LHS_WIDTH2:
      {
        int tmp = atoi(optarg);
        if (tmp > listing_lhs_width)
          listing_lhs_width_second = tmp;
      }
      break;
    case OPTION_LISTING_RHS_WIDTH:
      listing_rhs_width = atoi(optarg);
      break;
    case OPTION_LISTING_CONT_LINES:
      listing_lhs_cont_lines = atoi(optarg);
      break;

    case 'M':
      flag_mri = 1;
#ifdef TC_M68K
      flag_m68k_mri = 1;
#endif
      break;

    case 'R':
      flag_readonly_data_in_text = 1;
      break;

    case 'W':
      flag_no_warnings = 1;
      break;

    case OPTION_WARN:
      flag_no_warnings = 0;
      flag_fatal_warnings = 0;
      break;

    case OPTION_WARN_FATAL:
      flag_no_warnings = 0;
      flag_fatal_warnings = 1;
      break;

    case 'Z':
      flag_always_generate_output = 1;
      break;

    case 'a':
      if (optarg)
        {
          while (*optarg)
        {
          switch (*optarg)
            {
            case 'c':
              listing |= LISTING_NOCOND;
              break;
            case 'd':
              listing |= LISTING_NODEBUG;
              break;
            case 'h':
              listing |= LISTING_HLL;
              break;
            case 'l':
              listing |= LISTING_LISTING;
              break;
            case 'm':
              listing |= LISTING_MACEXP;
              break;
            case 'n':
              listing |= LISTING_NOFORM;
              break;
            case 's':
              listing |= LISTING_SYMBOLS;
              break;
            case '=':
              listing_filename = xstrdup (optarg + 1);
              optarg += strlen (listing_filename);
              break;
            default:
              as_fatal (_("invalid listing option `%c'"), *optarg);
              break;
            }
          optarg++;
        }
        }
      if (!listing)
        listing = LISTING_DEFAULT;
        
        g_listing = 1;  /* -a option exist: @rm,@rl Warning */
                    /* add T.Tazaki 2002.04.26 */
      break;

    case 'D':
      /* DEBUG is implemented: it debugs different */
      /* things from other people's assemblers. */
      flag_debug = 1;
      break;

    case 'f':
      flag_no_comments = 1;
      break;

    case 'I':
      {         /* Include file directory */
        char *temp = xstrdup (optarg);
        add_include_dir (temp);
        break;
      }

    case 'o':
      out_file_name = xstrdup (optarg);
      break;

    case 'w':
      break;

    case 'X':
      /* -X means treat warnings as errors */
      break;
    }
    }

  free (shortopts);
  free (longopts);

  *pargc = new_argc;
  *pargv = new_argv;

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
	// process of -mc33ext option
	if( flag_plural_src == 1 ){
		fprintf (stderr, _("Error : Cannot specify plurality source files.\n"));
		xexit (EXIT_FAILURE);
	}

	if( cp_Current_File_Name != NULL ){
		if( strlen(cp_Current_File_Name) > 0 ){
			chk_is_file_inf( cp_Current_File_Name );	// check whether ".file" exists
		}
	}

	if( ( cp_Current_File_Name != NULL ) 
						&& ( cp_Dump_File_Name != NULL ) && ( out_file_name != NULL ) ){
		if( strlen(cp_Current_File_Name) > 0 ){
			xatexit( free_ext_heap_area );		// register "free_ext_heap_area()" into "xexit()"
			read_cur_file_info( cp_Current_File_Name );
			read_dump_info( cp_Dump_File_Name,out_file_name );

// ADD D.Fujimoto 2008/01/07 >>>>>>>
			// read all object dump file
			read_all_dump_info(cp_All_Dump_File_Name, out_file_name);

			// count duplicate symbols using the all object dump file
			countDuplicateSymbols(stpp_All_Dump_Inf, ul_All_Dump_Symbol_Cnt);
// ADD D.Fujimoto 2008/01/07 <<<<<<<

			// get data pointer address
			if (!g_iMedda32) {
				g_dpAddress = getDataPointerAddress(DATA_POINTER_SYMBOL);
			}

			g_c33_ext = 1;
		}
	}
#endif
// ADD D.Fujimoto 2007/06/25 <<<<<<<


}

static long start_time;

int 
main (argc, argv)
     int argc;
     char **argv;
{
  int macro_alternate;
  int macro_strip_at;
  int keep_it;
  
  /* add T.Tazaki 2003/05/14 >>> */
  FILE *fpOut;
  /* add T.Tazaki 2003/05/14 <<< */

  start_time = get_run_time ();

#if defined (HAVE_SETLOCALE) && defined (HAVE_LC_MESSAGES)
  setlocale (LC_MESSAGES, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  if (debug_memory)
    {
#ifdef BFD_ASSEMBLER
      extern long _bfd_chunksize;
      _bfd_chunksize = 64;
#endif
      chunksize = 64;
    }

#ifdef HOST_SPECIAL_INIT
  HOST_SPECIAL_INIT (argc, argv);
#endif

  myname = argv[0];
  xmalloc_set_program_name (myname);

  START_PROGRESS (myname, 0);

#ifndef OBJ_DEFAULT_OUTPUT_FILE_NAME
#define OBJ_DEFAULT_OUTPUT_FILE_NAME "a.out"
#endif

  out_file_name = OBJ_DEFAULT_OUTPUT_FILE_NAME;

  hex_init ();
#ifdef BFD_ASSEMBLER
  bfd_init ();
  bfd_set_error_program_name (myname);
#endif

#ifdef USE_EMULATIONS
  select_emulation_mode (argc, argv);
#endif

  PROGRESS (1);
  symbol_begin ();
  frag_init ();
  subsegs_begin ();
  parse_args (&argc, &argv); 
  read_begin ();
  input_scrub_begin ();
  expr_begin ();

  if (flag_print_statistics)
    xatexit (dump_statistics);

  macro_alternate = 0;
  macro_strip_at = 0;
#ifdef TC_I960
  macro_strip_at = flag_mri;
#endif
#ifdef TC_A29K
  /* For compatibility with the AMD 29K family macro assembler
     specification.  */
  macro_alternate = 1;
  macro_strip_at = 1;
#endif

  macro_init (macro_alternate, flag_mri, macro_strip_at, macro_expr);

  PROGRESS (1);

#ifdef BFD_ASSEMBLER
  output_file_create (out_file_name);
  assert (stdoutput != 0);
#endif

#ifdef tc_init_after_args
  tc_init_after_args ();
#endif

  itbl_init ();

  /* Now that we have fully initialized, and have created the output
     file, define any symbols requested by --defsym command line
     arguments.  */
  while (defsyms != NULL)
    {
      symbolS *sym;
      struct defsym_list *next;

      sym = symbol_new (defsyms->name, absolute_section, defsyms->value,
            &zero_address_frag);
      symbol_table_insert (sym);
      next = defsyms->next;
      free (defsyms);
      defsyms = next;
    }

  PROGRESS (1);

  perform_an_assembly_pass (argc, argv);    /* Assemble it. */

  cond_finish_check (-1);

#ifdef md_end
  md_end ();
#endif

  if (seen_at_least_1_file ()
      && (flag_always_generate_output || had_errors () == 0))
    keep_it = 1;
  else
    keep_it = 0;

#if defined (BFD_ASSEMBLER) || !defined (BFD)
  /* This used to be done at the start of write_object_file in
     write.c, but that caused problems when doing listings when
     keep_it was zero.  This could probably be moved above md_end, but
     I didn't want to risk the change.  */
  subsegs_finish ();
#endif

  if (keep_it)
    write_object_file ();

#ifndef NO_LISTING
  listing_print (listing_filename);
#endif

#ifndef OBJ_VMS /* does its own file handling */
#ifndef BFD_ASSEMBLER
  if (keep_it)
#endif
    output_file_close (out_file_name);

/* add T.Tazaki 2003/05/14 >>> */
    fpOut = fopen( out_file_name,"r+b" );
    if( fpOut != NULL )
    {
        /* e_machine number set */
//      fseek( fpOut, 18, SEEK_SET );
//      fputc( EM_SE_C33, fpOut );

        /* ELF header : e_flags bit31-28=CPU flag set */
        /*  flag = 0x0  : Standard macro */
        /*  flag = 'A'  : Advanced macro */
        /*  flag = 'P'  : PE macro */

        /* Advanced macro mode ? */
        if( g_iAdvance == 1 )
        {
           fseek( fpOut, 39, SEEK_SET );
           fputc( 'A', fpOut );
        }
        else if ( g_iPE == 1 )			/* add T.Tazaki 2003/11/18 */
        {
           fseek( fpOut, 39, SEEK_SET );
           fputc( 'P', fpOut );
        }
        fclose( fpOut );
    }
/* add T.Tazaki 2003/05/14 <<< */

#endif

  if (flag_fatal_warnings && had_warnings() > 0 && had_errors () == 0)
    as_bad (_("%d warnings, treating warnings as errors"), had_warnings());

  if (had_errors () > 0 && ! flag_always_generate_output)
    keep_it = 0;

  if (!keep_it)
    unlink (out_file_name);

  input_scrub_end ();

  END_PROGRESS (myname);

  /* Use xexit instead of return, because under VMS environments they
     may not place the same interpretation on the value given.  */
  if (had_errors () > 0)
    xexit (EXIT_FAILURE);

  /* Only generate dependency file if assembler was successful.  */
  print_dependencies ();

  xexit (EXIT_SUCCESS);
}

static void
dump_statistics ()
{
#ifdef HAVE_SBRK
  char *lim = (char *) sbrk (0);
#endif
  long run_time = get_run_time () - start_time;

  fprintf (stderr, _("%s: total time in assembly: %ld.%06ld\n"),
       myname, run_time / 1000000, run_time % 1000000);
#ifdef HAVE_SBRK
  fprintf (stderr, _("%s: data size %ld\n"),
       myname, (long) (lim - (char *) &environ));
#endif

  subsegs_print_statistics (stderr);
  write_print_statistics (stderr);
  symbol_print_statistics (stderr);
  read_print_statistics (stderr);

#ifdef tc_print_statistics
  tc_print_statistics (stderr);
#endif
#ifdef obj_print_statistics
  obj_print_statistics (stderr);
#endif
}


/*          perform_an_assembly_pass()
 *
 * Here to attempt 1 pass over each input file.
 * We scan argv[*] looking for filenames or exactly "" which is
 * shorthand for stdin. Any argv that is NULL is not a file-name.
 * We set need_pass_2 TRUE if, after this, we still have unresolved
 * expressions of the form (unknown value)+-(unknown value).
 *
 * Note the un*x semantics: there is only 1 logical input file, but it
 * may be a catenation of many 'physical' input files.
 */
static void 
perform_an_assembly_pass (argc, argv)
     int argc;
     char **argv;
{
  int saw_a_file = 0;
#ifdef BFD_ASSEMBLER
  flagword applicable;
#endif

  need_pass_2 = 0;

#ifndef BFD_ASSEMBLER
#ifdef MANY_SEGMENTS
  {
    unsigned int i;
    for (i = SEG_E0; i < SEG_UNKNOWN; i++)
      segment_info[i].fix_root = 0;
  }
  /* Create the three fixed ones */
  {
    segT seg;

#ifdef TE_APOLLO
    seg = subseg_new (".wtext", 0);
#else
    seg = subseg_new (".text", 0);
#endif
    assert (seg == SEG_E0);
    seg = subseg_new (".data", 0);
    assert (seg == SEG_E1);
    seg = subseg_new (".bss", 0);
    assert (seg == SEG_E2);
#ifdef TE_APOLLO
    create_target_segments ();
#endif
  }

#else /* not MANY_SEGMENTS */
  text_fix_root = NULL;
  data_fix_root = NULL;
  bss_fix_root = NULL;
#endif /* not MANY_SEGMENTS */
#else /* BFD_ASSEMBLER */
  /* Create the standard sections, and those the assembler uses
     internally.  */
  text_section = subseg_new (TEXT_SECTION_NAME, 0);
  data_section = subseg_new (DATA_SECTION_NAME, 0);
  bss_section = subseg_new (BSS_SECTION_NAME, 0);
  /* @@ FIXME -- we're setting the RELOC flag so that sections are assumed
     to have relocs, otherwise we don't find out in time. */
  applicable = bfd_applicable_section_flags (stdoutput);
  bfd_set_section_flags (stdoutput, text_section,
             applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC
                       | SEC_CODE | SEC_READONLY));
  bfd_set_section_flags (stdoutput, data_section,
             applicable & (SEC_ALLOC | SEC_LOAD | SEC_RELOC
                       | SEC_DATA));
  bfd_set_section_flags (stdoutput, bss_section, applicable & SEC_ALLOC);
  seg_info (bss_section)->bss = 1;
  subseg_new (BFD_ABS_SECTION_NAME, 0);
  subseg_new (BFD_UND_SECTION_NAME, 0);
  reg_section = subseg_new ("*GAS `reg' section*", 0);
  expr_section = subseg_new ("*GAS `expr' section*", 0);

#endif /* BFD_ASSEMBLER */

  subseg_set (text_section, 0);

  /* This may add symbol table entries, which requires having an open BFD,
     and sections already created, in BFD_ASSEMBLER mode.  */
  md_begin ();

#ifdef obj_begin
  obj_begin ();
#endif

  argv++;           /* skip argv[0] */
  argc--;           /* skip argv[0] */
  while (argc--)
    {
      if (*argv)
    {           /* Is it a file-name argument? */
      PROGRESS (1);
      saw_a_file++;
      /* argv->"" if stdin desired, else->filename */
      read_a_source_file (*argv);
    }
      argv++;           /* completed that argv */
    }
  if (!saw_a_file)
    read_a_source_file ("");
}               /* perform_an_assembly_pass() */

/* The interface between the macro code and gas expression handling.  */

static int
macro_expr (emsg, idx, in, val)
     const char *emsg;
     int idx;
     sb *in;
     int *val;
{
  char *hold;
  expressionS ex;

  sb_terminate (in);

  hold = input_line_pointer;
  input_line_pointer = in->ptr + idx;
  expression (&ex);
  idx = input_line_pointer - in->ptr;
  input_line_pointer = hold;

  if (ex.X_op != O_constant)
    as_bad ("%s", emsg);

  *val = (int) ex.X_add_number;

  return idx;
}

/* end of as.c */
