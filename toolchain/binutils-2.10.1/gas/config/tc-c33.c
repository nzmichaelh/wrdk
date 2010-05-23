/* tc-c33.c -- Assembler code for the EPSON EOC33
   Copyright (C) 1996, 1997, 1998, 1999 Free Software Foundation.

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
   along with GAS; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <ctype.h>
#include "as.h"
#include "subsegs.h"     
#include "opcode/c33.h"
#include "ext_remove.h"				// add D.Fujimoto 2007/06/21

#define AREA_CDA 0
#define AREA_GDA 1
#define AREA_ZDA 2
#define AREA_SDA 3
#define AREA_TDA 4
#define AREA_LCDA 5
#define AREA_LGDA 6
#define AREA_LZDA 7
#define AREA_LSDA 8
#define AREA_LTDA 9


#define O_spregister    O_md1   /* for c33 %sp */
#define O_dpregister    O_md2   /* for c33 %dp */
#define O_cond          O_md3   /* for c33 ext cond */
#define O_op_shift      O_md4   /* for c33 ext OP */

#define SGP_REG     12      /* GP register */
#define TGP_REG     13      /* GP register */
#define ZGP_REG     14      /* GP register */
#define GP_REG      15      /* GP register */

/* Temporarily holds the reloc in a cons expression.  */
static bfd_reloc_code_real_type hold_cons_reloc;



/* Structure to hold information about predefined registers.  */
struct reg_name
{
  const char * name;
  int          value;
};

/* Generic assembler global variables which must be defined by all targets. */

/* Characters which always start a comment. */
const char comment_chars[] = ";";

/* Characters which start a comment at the beginning of a line.  */
const char line_comment_chars[] = "";

/* Characters which may be used to separate multiple commands on a 
   single line.  */
const char line_separator_chars[] = "";

/* Characters which are used to indicate an exponent in a floating 
   point number.  */
const char EXP_CHARS[] = "eE";

/* Characters which mean that a number is a floating point constant, 
   as in 0d1.0.  */
const char FLT_CHARS[] = "dD";


const relax_typeS md_relax_table[] =
{
  /* Conditional branches.  */
  {0xff,     -0x100,    2, 1},
  {0x1fffff, -0x200000, 6, 0},
  /* Unconditional branches.  */
  {0xff,     -0x100,    2, 3},
  {0x1fffff, -0x200000, 4, 0},
};

/* add tazaki 2001.12.03 */
static segT comm_section = NULL;
static segT gcomm_section = NULL;
static segT scomm_section = NULL;
static segT tcomm_section = NULL;
static segT zcomm_section = NULL;
static segT gbss_section = NULL;
static segT sbss_section = NULL;
static segT tbss_section = NULL;
static segT zbss_section = NULL;
/* add tazaki 2001.12.03 */

/* add T.Tazaki 2002.04.26 >>> */
extern int g_listing;
/* add T.Tazaki 2002.04.26 <<< */

/* fixups */
#define MAX_INSN_FIXUPS (5)
struct c33_fixup
{
  expressionS              exp;
  int                      opindex;
  bfd_reloc_code_real_type reloc;
};

struct c33_fixup fixups [MAX_INSN_FIXUPS];
static int fc;



/******************************************************************************
    INPUT   int     area of symbol
    RETURN  void
    Explanation assembler false instruction(.gcomm/.scomm/.tcomm/.zcomm) Evaluation
******************************************************************************/
/* Copied from obj_elf_common() in gas/config/obj-elf.c */
static void
c33_comm (area)
     int area;
{
    char *    name;
    char      c;
    char *    p;
    int       temp;
    int       size;
    symbolS * symbolP;
    int       have_align;
     char * pfrag;

    name = input_line_pointer;
    c = get_symbol_end ();
  
    /* just after name is now '\0' */
    p = input_line_pointer;
    *p = c;
  
    /* skip space */
    SKIP_WHITESPACE ();
  
    /* Is the character after ".comm" a pause character? */
    if (*input_line_pointer != ',')
    {
        as_bad (_("Expected comma after symbol-name"));
        ignore_rest_of_line ();
        return;
    }
  
    input_line_pointer ++;      /* skip ',' */
  
    if ((temp = get_absolute_expression ()) < 0)
    {
        /* xgettext:c-format */
        as_bad (_(".COMMon length (%d.) < 0! Ignored."), temp);
        ignore_rest_of_line ();
        return;
    }
  
    size = temp;
    *p = 0;
    symbolP = symbol_find_or_make (name);
    *p = c;
  
/*  if (S_IS_DEFINED (symbolP) && ! S_IS_COMMON (symbolP))  */
    if (S_IS_DEFINED (symbolP) )
    {
        as_bad (_("Ignoring attempt to re-define symbol"));
        ignore_rest_of_line ();
        return;
    }
  
    if (S_GET_VALUE (symbolP) != 0)
    {
        if (S_GET_VALUE (symbolP) != size)
        {
            /* xgettext:c-format */
            as_warn (_("Length of .comm \"%s\" is already %ld. Not changed to %d."),
            S_GET_NAME (symbolP), (long) S_GET_VALUE (symbolP), size);
        }
    }
  
    know (symbol_get_frag (symbolP) == & zero_address_frag);
  
    if (*input_line_pointer != ',')
        have_align = 0;
    else
    {
        have_align = 1;
        input_line_pointer++;
        SKIP_WHITESPACE ();
    }
  
    if (! have_align || *input_line_pointer != '"')
    {
        if (! have_align)
            temp = 0;
        else
        {
            temp = get_absolute_expression ();
      
        if (temp < 0)
        {
            temp = 0;
            as_warn (_("Common alignment negative; 0 assumed"));
        }
    }
 

/*  if (symbol_get_obj (symbolP)->local)*/
    if (!(area == AREA_CDA || area == AREA_GDA || area == AREA_SDA || area == AREA_TDA || area == AREA_ZDA ))
    {
        /* NOT EXIST ROUTINE */
        
        segT   old_sec;
        int    old_subsec;
        int    align;
        flagword    applicable;

        old_sec = now_seg;
        old_subsec = now_subseg;
      
        applicable = bfd_applicable_section_flags (stdoutput);
          
        applicable &= SEC_ALLOC;
      
        switch (area)
        {
        case AREA_CDA:
          if (comm_section == NULL)
          {

              comm_section = subseg_new (".comm", 0);
              
              bfd_set_section_flags (stdoutput, comm_section, applicable);
              
              seg_info (comm_section)->bss = 1;
          }
          break;
      
        case AREA_GDA:
          if (gcomm_section == NULL)
          {
              gcomm_section = subseg_new (".gcomm", 0);
              
              bfd_set_section_flags (stdoutput, gcomm_section, applicable);
              
              seg_info (gcomm_section)->bss = 1;
          }
          break;
      
        case AREA_SDA:
          if (scomm_section == NULL)
          {
              scomm_section = subseg_new (".scomm", 0);
              
              bfd_set_section_flags (stdoutput, scomm_section, applicable);
              
              seg_info (scomm_section)->bss = 1;
          }
          break;
      
        case AREA_TDA:
          if (tcomm_section == NULL)
          {
              tcomm_section = subseg_new (".tcomm", 0);
              
              bfd_set_section_flags (stdoutput, tcomm_section, applicable);
              
              seg_info (tcomm_section)->bss = 1;
          }
          break;

        case AREA_ZDA:
          if (zcomm_section == NULL)
          {
              zcomm_section = subseg_new (".zcomm", 0);
              
              bfd_set_section_flags (stdoutput, zcomm_section, applicable);
              
              seg_info (zcomm_section)->bss = 1;
          }
          break;
      
        }

        if (temp)
        {
            /* convert to a power of 2 alignment */
            for (align = 0; (temp & 1) == 0; temp >>= 1, ++align)
            ;
              
            if (temp != 1)
            {
                as_bad (_("Common alignment not a power of 2"));
                ignore_rest_of_line ();
                return;
            }
        }
        else
            align = 0;
          
        switch (area)
        {
        case AREA_CDA:
            record_alignment (comm_section, align);
            obj_elf_section_change_hook();
            subseg_set (comm_section, 0);
            break;

        case AREA_GDA:
            record_alignment (gcomm_section, align);
            obj_elf_section_change_hook();
            subseg_set (gcomm_section, 0);
            break;

        case AREA_SDA:
            record_alignment (scomm_section, align);
            obj_elf_section_change_hook();
            subseg_set (scomm_section, 0);
            break;

        case AREA_TDA:
            record_alignment (tcomm_section, align);
            obj_elf_section_change_hook();
            subseg_set (tcomm_section, 0);
            break;

        case AREA_ZDA:
            record_alignment (zcomm_section, align);
            obj_elf_section_change_hook();
            subseg_set (zcomm_section, 0);
            break;

        default:
            abort();
        }
          
        if (align)
            frag_align (align, 0, 0);

        switch (area)
        {
        case AREA_CDA:
          if (S_GET_SEGMENT (symbolP) == comm_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;
          break;

        case AREA_GDA:
          if (S_GET_SEGMENT (symbolP) == gcomm_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;
          break;

        case AREA_SDA:
          if (S_GET_SEGMENT (symbolP) == scomm_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;
          break;

        case AREA_TDA:
          if (S_GET_SEGMENT (symbolP) == tcomm_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;
          break;

        case AREA_ZDA:
          if (S_GET_SEGMENT (symbolP) == zcomm_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;
          break;

        default:
          abort ();
        }
      
        symbol_set_frag (symbolP, frag_now);
        pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                (offsetT) size, (char *) 0);
        *pfrag = 0;
        S_SET_SIZE (symbolP, size);
      
        switch (area)
        {
        case AREA_CDA:
          S_SET_SEGMENT (symbolP, comm_section);
          break;
          
        case AREA_GDA:
          S_SET_SEGMENT (symbolP, gcomm_section);
          break;
          
        case AREA_SDA:
          S_SET_SEGMENT (symbolP, scomm_section);
          break;
          
        case AREA_TDA:
          S_SET_SEGMENT (symbolP, tcomm_section);
          break;
          
        case AREA_ZDA:
          S_SET_SEGMENT (symbolP, zcomm_section);
          break;
          
        default:
          abort();
        }
            
        if (symbol_get_obj (symbolP)->local)
            S_CLEAR_EXTERNAL (symbolP);
        else
            S_SET_EXTERNAL (symbolP);
        
        obj_elf_section_change_hook();
        subseg_set (old_sec, old_subsec);
    }
    else
    {
        /*==========================================================================*/
        /* Evaluation of .comm                                                      */
        /*==========================================================================*/
        segT   old_sec;
        int    old_subsec;
        int    align;
        int    i_now_align; /* add 2002.01.21 */
        i_now_align = temp; /* add 2002.01.21 */

        /* computing of alignment */
        if (temp)
        {
            /* convert to a power of 2 alignment */
            for (align = 0; (temp & 1) == 0; temp >>= 1, ++align)
            ;
              
            if (temp != 1)
            {
                as_bad (_("Common alignment not a power of 2"));
                ignore_rest_of_line ();
                return;
            }
        }
        else
            align = 0;

        old_sec = now_seg;
        old_subsec = now_subseg;

        allocate_common:
        
        /* Convert .local + .xcomm to local section. */

        if (symbol_get_obj (symbolP)->local){
            switch (area)
            {
            case AREA_CDA:
                    area = AREA_LCDA;
                    break;
            case AREA_GDA:
                    area = AREA_LGDA;
                    break;
            case AREA_SDA:
                    area = AREA_LSDA;
                    break;
            case AREA_TDA:
                    area = AREA_LTDA;
                    break;
            case AREA_ZDA:
                    area = AREA_LZDA;
                    break;
            }
        }
      
        switch (area)
        {
        case AREA_CDA:
          if (comm_section == NULL)
            {
                flagword    applicable;
                  
                applicable = bfd_applicable_section_flags (stdoutput);

                comm_section = subseg_new (".comm", 0);

                bfd_set_section_flags (stdoutput, comm_section, applicable
                   & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA
                  | SEC_HAS_CONTENTS) | SEC_IS_COMMON);
            }
            S_SET_SEGMENT (symbolP, comm_section);
            record_alignment (comm_section, align);

          break;
          
        case AREA_GDA:
          if (gcomm_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              gcomm_section = subseg_new (".gcomm", 0);
              
              bfd_set_section_flags (stdoutput, gcomm_section, applicable
                 & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA
                | SEC_HAS_CONTENTS) | SEC_IS_COMMON);
            }
            S_SET_SEGMENT (symbolP, gcomm_section);
            record_alignment (gcomm_section, align);
          break;
          
        case AREA_SDA:
          if (scomm_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              scomm_section = subseg_new (".scomm", 0);
              
              bfd_set_section_flags (stdoutput, scomm_section, applicable
                 & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA
                | SEC_HAS_CONTENTS) | SEC_IS_COMMON);
            }
            S_SET_SEGMENT (symbolP, scomm_section);
            record_alignment (scomm_section, align);
          break;
          
        case AREA_TDA:
          if (tcomm_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              tcomm_section = subseg_new (".tcomm", 0);
              
              bfd_set_section_flags (stdoutput, tcomm_section, applicable
                 & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA
                | SEC_HAS_CONTENTS) | SEC_IS_COMMON);
            }
            S_SET_SEGMENT (symbolP, tcomm_section);
            record_alignment (tcomm_section, align);
          break;
          
        case AREA_ZDA:
          if (zcomm_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              zcomm_section = subseg_new (".zcomm", 0);
              
              bfd_set_section_flags (stdoutput, zcomm_section, applicable
                 & (SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_DATA
                | SEC_HAS_CONTENTS) | SEC_IS_COMMON);
            }
            S_SET_SEGMENT (symbolP, zcomm_section);
            record_alignment (zcomm_section, align);
          break;
          
          
        case AREA_LCDA:
            /* Convert local comm to .bss section. */
            obj_elf_section_change_hook();
            subseg_set (bss_section, 0);

            if (S_GET_SEGMENT (symbolP) == bss_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;

            if (align)
                frag_align (align, 0, 0);

            symbol_set_frag (symbolP, frag_now);
            pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                    (offsetT) size, (char *) 0);
            *pfrag = 0;

            S_SET_SIZE (symbolP, size);
            S_SET_SEGMENT (symbolP, bss_section);
            record_alignment (bss_section, align);

            break;
          
        case AREA_LGDA:
          if (gbss_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              gbss_section = subseg_new (".gbss", 0);
              
              bfd_set_section_flags (stdoutput, gbss_section, applicable & SEC_ALLOC);
              
              seg_info (gbss_section)->bss = 1;
            }

            record_alignment (gbss_section, align);
            obj_elf_section_change_hook();
            subseg_set (gbss_section, 0);

            if (align)
                frag_align (align, 0, 0);

            if (S_GET_SEGMENT (symbolP) == gbss_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;

            symbol_set_frag (symbolP, frag_now);
            pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                    (offsetT) size, (char *) 0);
            *pfrag = 0;
            S_SET_SIZE (symbolP, size);
            S_SET_SEGMENT (symbolP, gbss_section);

            break;
          
        case AREA_LSDA:
          if (sbss_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              sbss_section = subseg_new (".sbss", 0);
              
              bfd_set_section_flags (stdoutput, sbss_section, applicable & SEC_ALLOC);
              
              seg_info (sbss_section)->bss = 1;
            }

            record_alignment (sbss_section, align);
            obj_elf_section_change_hook();
            subseg_set (sbss_section, 0);

            if (align)
                frag_align (align, 0, 0);

            if (S_GET_SEGMENT (symbolP) == sbss_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;

            symbol_set_frag (symbolP, frag_now);
            pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                    (offsetT) size, (char *) 0);
            *pfrag = 0;
            S_SET_SIZE (symbolP, size);
            S_SET_SEGMENT (symbolP, sbss_section);

            break;
          
        case AREA_LTDA:
          if (tbss_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              tbss_section = subseg_new (".tbss", 0);
              
              bfd_set_section_flags (stdoutput, tbss_section, applicable & SEC_ALLOC);
              
              seg_info (tbss_section)->bss = 1;
            }

            record_alignment (tbss_section, align);
            obj_elf_section_change_hook();
            subseg_set (tbss_section, 0);

            if (align)
                frag_align (align, 0, 0);

            if (S_GET_SEGMENT (symbolP) == tbss_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;

            symbol_set_frag (symbolP, frag_now);
            pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                    (offsetT) size, (char *) 0);
            *pfrag = 0;
            S_SET_SIZE (symbolP, size);
            S_SET_SEGMENT (symbolP, tbss_section);

            break;

        case AREA_LZDA:
          if (zbss_section == NULL)
            {
              flagword  applicable;
              
              applicable = bfd_applicable_section_flags (stdoutput);
              
              zbss_section = subseg_new (".zbss", 0);
              
              bfd_set_section_flags (stdoutput, zbss_section, applicable & SEC_ALLOC);
              
              seg_info (zbss_section)->bss = 1;
            }

            record_alignment (zbss_section, align);
            obj_elf_section_change_hook();
            subseg_set (zbss_section, 0);

            if (align)
                frag_align (align, 0, 0);

            if (S_GET_SEGMENT (symbolP) == zbss_section)
                symbol_get_frag (symbolP)->fr_symbol = 0;

            symbol_set_frag (symbolP, frag_now);
            pfrag = frag_var (rs_org, 1, 1, (relax_substateT) 0, symbolP,
                    (offsetT) size, (char *) 0);
            *pfrag = 0;
            S_SET_SIZE (symbolP, size);
            S_SET_SEGMENT (symbolP, zbss_section);

            break;

        default:
          abort();
        }

        if (area == AREA_LCDA || area == AREA_LGDA || area == AREA_LSDA || area == AREA_LTDA || area == AREA_LZDA) /* ローカル？ */
        {
            S_CLEAR_EXTERNAL (symbolP);
        }else{
            S_SET_VALUE (symbolP, (valueT) size);
//          S_SET_ALIGN (symbolP, temp);
            if( i_now_align )
                S_SET_ALIGN (symbolP, i_now_align);
            S_SET_EXTERNAL (symbolP);
        }

        obj_elf_section_change_hook();
        subseg_set (old_sec, old_subsec);

    }
}
  else
    {
      input_line_pointer++;
      /* @@ Some use the dot, some don't.  Can we get some consistency??  */
      if (*input_line_pointer == '.')
    input_line_pointer++;
      /* @@ Some say data, some say bss.  */
      if (strncmp (input_line_pointer, "bss\"", 4)
      && strncmp (input_line_pointer, "data\"", 5))
    {
      while (*--input_line_pointer != '"')
        ;
      input_line_pointer--;
      goto bad_common_segment;
    }
      while (*input_line_pointer++ != '"')
    ;
      goto allocate_common;
    }

  symbol_get_bfdsym (symbolP)->flags |= BSF_OBJECT;

  demand_empty_rest_of_line ();
  return;

  {
  bad_common_segment:
    p = input_line_pointer;
    while (*p && *p != '\n')
      p++;
    c = *p;
    *p = '\0';
    as_bad (_("bad .common segment %s"), input_line_pointer + 1);
    *p = c;
    input_line_pointer = p;
    ignore_rest_of_line ();
    return;
  }
}

/* The target specific pseudo-ops which we support.  */
const pseudo_typeS md_pseudo_table[] =
{
  {"comm",    c33_comm,    AREA_CDA},
  {"gcomm",   c33_comm,    AREA_GDA},
  {"scomm",   c33_comm,    AREA_SDA},
  {"tcomm",   c33_comm,    AREA_TDA},
  {"zcomm",   c33_comm,    AREA_ZDA},
  { NULL,     NULL,        0}
};

/*****************************************************************************/



/* Opcode hash table.  */
static struct hash_control *c33_hash;

/* This table is sorted. Suitable for searching by a binary search. */
static const struct reg_name pre_defined_registers[] =
{
  { "%r0",   0 },
  { "%r1",   1 },
  { "%r10", 10 },
  { "%r11", 11 },
  { "%r12", 12 },
  { "%r13", 13 },
  { "%r14", 14 },
  { "%r15", 15 },
  { "%r2",   2 },
  { "%r3",   3 },
  { "%r4",   4 },
  { "%r5",   5 },
  { "%r6",   6 },
  { "%r7",   7 },
  { "%r8",   8 },
  { "%r9",   9 },
};
#define REG_NAME_CNT    (sizeof (pre_defined_registers) / sizeof (struct reg_name))

/* standard macro spesial registers */
static const struct reg_name system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%psr",   0 },
    { "%sp",    1 },
};

/* advanced macro spesial registers */
static const struct reg_name adv_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%dbbr",  11 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%dp",    9 },        /* Adv */
    { "%idir",  10 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%lco",   4 },        /* Adv */
    { "%lea",   6 },        /* Adv */
    { "%lsa",   5 },        /* Adv */
    { "%pc",    15},
    { "%psr",   0 },
    { "%sor",   7 },        /* Adv */
    { "%sp",    1 },
    { "%ssp",   14 },       /* Adv */
    { "%ttbr",  8 },        /* Adv */
    { "%usp",   13 },       /* Adv */
};

/* ld.w %sd,%rs special registers */
static const struct reg_name adv_load_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%dbbr",  11 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%dp",    9 },        /* Adv */
    { "%idir",  10 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%lco",   4 },        /* Adv */
    { "%lea",   6 },        /* Adv */
    { "%lsa",   5 },        /* Adv */
    { "%psr",   0 },
    { "%sor",   7 },        /* Adv */
    { "%sp",    1 },
    { "%ssp",   14 },       /* Adv */
    { "%ttbr",  8 },        /* Adv */
    { "%usp",   13 },       /* Adv */
};

/* pushs , pops special registers */
static const struct reg_name adv_pushs_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%dbbr",  11 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%dp",    9 },        /* Adv */
    { "%idir",  10 },		/* Adv 	add T.Tazaki 2003/11/18 */
    { "%lco",   4 },        /* Adv */
    { "%lea",   6 },        /* Adv */
    { "%lsa",   5 },        /* Adv */
    { "%pc",    15 },
    { "%psr",   0 },
    { "%sor",   7 },        /* Adv */
    { "%sp",    1 },
    { "%ssp",   14 },       /* Adv */
    { "%ttbr",  8 },        /* Adv */
    { "%usp",   13 },       /* Adv */
};

/* PE  add T.Tazaki 2003/11/18 >>> */

/* PE macro spesial registers */
static const struct reg_name pe_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%dbbr",  11 },
    { "%idir",  10 },
    { "%pc",    15},
    { "%psr",   0 },
    { "%sp",    1 },
    { "%ttbr",  8 },
};

/* ld.w %sd,%rs special registers */
static const struct reg_name pe_load_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
    { "%psr",   0 },
    { "%sp",    1 },
    { "%ttbr",  8 },
};

/* pushs , pops special registers */
static const struct reg_name pe_pushs_system_registers[] = 
{
    { "%ahr",   3 },
    { "%alr",   2 },
};

/* PE  add T.Tazaki 2003/11/18 <<< */

#define SYSREG_NAME_CNT             (sizeof (system_registers) / sizeof (struct reg_name))
#define ADV_SYSREG_NAME_CNT         (sizeof (adv_system_registers) / sizeof (struct reg_name))
#define ADV_LOAD_SYSREG_NAME_CNT    (sizeof (adv_load_system_registers) / sizeof (struct reg_name))
#define ADV_PUSHS_SYSREG_NAME_CNT   (sizeof (adv_pushs_system_registers) / sizeof (struct reg_name))

/* PE  add T.Tazaki 2003/11/18 >>> */
#define PE_SYSREG_NAME_CNT         (sizeof (pe_system_registers) / sizeof (struct reg_name))
#define PE_LOAD_SYSREG_NAME_CNT    (sizeof (pe_load_system_registers) / sizeof (struct reg_name))
#define PE_PUSHS_SYSREG_NAME_CNT   (sizeof (pe_pushs_system_registers) / sizeof (struct reg_name))
/* PE  add T.Tazaki 2003/11/18 <<< */

/******************************************************************************
    INPUT   const struct reg_name * register name
            int                     register string size
            const char *            check register string
            boolean                 not used
    RETURN  int                     register number
    Explanation Get register number
******************************************************************************/
/* reg_name_search does a binary search of the given register table
   to see if "name" is a valid regiter name.  Returns the register
   number from the array on success, or -1 on failure. */

static int
reg_name_search (regs, regcount, name, accept_numbers)
     const struct reg_name * regs;
     int                     regcount;
     const char *            name;
     boolean                 accept_numbers;
{
  int middle, low, high;
  int cmp;
  symbolS * symbolP;
#if 0
  /* If the register name is a symbol, then evaluate it.  */
  if ((symbolP = symbol_find (name)) != NULL)
    {
      /* If the symbol is an alias for another name then use that.
     If the symbol is an alias for a number, then return the number.  */
      if (symbol_equated_p (symbolP))
    {
      name = S_GET_NAME (symbol_get_value_expression (symbolP)->X_add_symbol);
    }
      else if (accept_numbers)
    {
      int reg = S_GET_VALUE (symbolP);
      
      if (reg >= 0 && reg <= 31)
        return reg;
    }
#endif

    low = 0;
    high = regcount - 1;

    do
    {
        middle = (low + high) / 2;
        cmp = strcasecmp (name, regs[middle].name);
        if (cmp < 0)
            high = middle - 1;
        else if (cmp > 0)
            low = middle + 1;
        else
            return regs[middle].value;
    }
    while (low <= high);

    return -1;
}


/******************************************************************************
    INPUT   expressionS *   The pointer to a command code information structure object
    RETURN  boolean     TRUE    ok
                        FALSE   error
    Explanation A register operand is changed into a command code.
******************************************************************************/
/* Summary of register_name().
 *
 * in: Input_line_pointer points to 1st char of operand.
 *
 * out: A expressionS.
 *  The operand may have been a register: in this case, X_op == O_register,
 *  X_add_number is set to the register number, and truth is returned.
 *  Input_line_pointer->(next non-blank) char after operand, or is in
 *  its original state.
 */
static boolean
register_name (expressionP)
     expressionS * expressionP;
{
  int    reg_number;
  char * name;
  char * start;
  char   c;
    char *pNameEnd;
    
    /* Find the spelling of the operand */
    start = name = input_line_pointer;

    c = get_symbol_end ();

    /* Get register number */
    reg_number = reg_name_search (pre_defined_registers, REG_NAME_CNT,
                name, FALSE);

    * input_line_pointer = c;   /* put back the delimiting char */

    /* look to see if it's in the register table */
    if (reg_number >= 0) 
    {
    /* YES The right register number was acquirable. */

        /* That it is a register and a register number are saved. */
        expressionP->X_op         = O_register;
        expressionP->X_add_number = reg_number;

        /* make the rest nice */
        expressionP->X_add_symbol = NULL;
        expressionP->X_op_symbol  = NULL;

        return true;
    }
    else
    {
    /* 不正 */
        /* reset the line as if we had not done anything */
        input_line_pointer = start;
         
        return false;
    }
}

/******************************************************************************
    INPUT   expressionS *   The pointer to a command code information structure object
            boolean
            boolean
    RETURN  boolean     TRUE    ok
                        FALSE   error
    Explanation     A system register operand is changed into a command code.
******************************************************************************/
/* Summary of system_register_name().
 *
 * in:  Input_line_pointer points to 1st char of operand.
 *      expressionP points to an expression structure to be filled in.
 *      accept_numbers is true iff numerical register names may be used.
 *      accept_list_names is true iff the special names PS and SR may be 
 *      accepted.
 *
 * out: A expressionS structure in expressionP.
 *  The operand may have been a register: in this case, X_op == O_register,
 *  X_add_number is set to the register number, and truth is returned.
 *  Input_line_pointer->(next non-blank) char after operand, or is in
 *  its original state.
 */
static boolean
system_register_name (expressionP, accept_numbers, accept_list_names)
     expressionS * expressionP;
     boolean       accept_numbers;
     boolean       accept_list_names;
{
    int    reg_number;
    char * name;
    char * start;
    char   c;


    /* Find the spelling of the operand */
    start = name = input_line_pointer;

    c = get_symbol_end ();

    /* get register number */
    if( g_iAdvance == 0 )
    {
	    if( g_iPE == 0 )
	    {
			/* STD */
	        reg_number = reg_name_search (system_registers, SYSREG_NAME_CNT, name,
	                accept_numbers);
	    }
	    else
	    {
			/* PE */	/* add T.Tazaki 2003/11/18 */
	        reg_number = reg_name_search (pe_system_registers, PE_SYSREG_NAME_CNT, name,
	                accept_numbers);
		}
    }else{
        /* add tazaki 2001.11.12 */
        /* ADV */
        reg_number = reg_name_search (adv_system_registers, ADV_SYSREG_NAME_CNT, name,
                accept_numbers);
    }
    
    * input_line_pointer = c;   /* put back the delimiting char */

#if 0
    if (reg_number < 0 && accept_numbers)
    {
        input_line_pointer   = start; /* reset input_line pointer */

        if (isdigit (* input_line_pointer))
        {
            reg_number = strtol (input_line_pointer, & input_line_pointer, 10);

            /* Make sure that the register number is allowable. */
            if (   reg_number < 0
                 || reg_number > 5
                 && reg_number < 16
                 || reg_number > 20
                 )
                {
                reg_number = -1;
            }
        }
        else if (accept_list_names)
        {
            c = get_symbol_end ();

#if 0   /* c33 */
            reg_number = reg_name_search (system_list_registers,
                    SYSREGLIST_NAME_CNT, name, FALSE);
#endif
            * input_line_pointer = c;   /* put back the delimiting char */

        }
    }
#endif
      /* look to see if it's in the register table */
    if (reg_number >= 0) 
    {
        expressionP->X_op         = O_register;
        expressionP->X_add_number = reg_number;

        /* make the rest nice */
        expressionP->X_add_symbol = NULL;
        expressionP->X_op_symbol  = NULL;

        return true;
    }
    else
    {
        /* reset the line as if we had not done anything */
        input_line_pointer = start;

        return false;
    }
}


CONST char * md_shortopts = "m:";

struct option md_longopts[] =
{
  {NULL, no_argument, NULL, 0}
};
size_t md_longopts_size = sizeof md_longopts; 

/******************************************************************************
    INPUT   expressionS *   The pointer to a command code information structure object
            boolean
            boolean
    RETURN  boolean     TRUE    ok
                        FALSE   error
    Explanation     A system register operand is changed into a command code.
                    false : ld.w %pc,%rs
******************************************************************************/
static boolean
load_system_register_name (expressionP, accept_numbers, accept_list_names)
     expressionS * expressionP;
     boolean       accept_numbers;
     boolean       accept_list_names;
{
    int    reg_number;
    char * name;
    char * start;
    char   c;


    /* Find the spelling of the operand */
    start = name = input_line_pointer;

    c = get_symbol_end ();

    /* get register number */
    if( g_iAdvance == 0 )
    {
	    if( g_iPE == 0 )
	    {
			/* STD */
	        reg_number = reg_name_search (system_registers, SYSREG_NAME_CNT, name,
	                accept_numbers);
	    }
	    else
	    {
			/* PE 	add T.Tazaki 2003/11/18 */
	        reg_number = reg_name_search (pe_load_system_registers, PE_LOAD_SYSREG_NAME_CNT, name,
	                accept_numbers);
	    }
    }else{
		/* ADV */
        reg_number = reg_name_search (adv_load_system_registers, ADV_LOAD_SYSREG_NAME_CNT, name,
                accept_numbers);
    }
    
    * input_line_pointer = c;   /* put back the delimiting char */

    /* look to see if it's in the register table */
    if (reg_number >= 0) 
    {
        expressionP->X_op         = O_register;
        expressionP->X_add_number = reg_number;

        /* make the rest nice */
        expressionP->X_add_symbol = NULL;
        expressionP->X_op_symbol  = NULL;

        return true;
    }
    else
    {
        /* reset the line as if we had not done anything */
        input_line_pointer = start;

        return false;
    }
}
/******************************************************************************
    INPUT   expressionS *   The pointer to a command code information structure object
            boolean
            boolean
    RETURN  boolean     TRUE    ok
                        FALSE   error
    Explanation     A system register operand is changed into a command code.
                    true : pushs %psr,%sp,%alr,%ahr,%lco,%lsa,%lea,%sor,%ttbr,%dp,%usp,%ssp,%pc
******************************************************************************/
static boolean
pushs_system_register_name (expressionP, accept_numbers, accept_list_names)
     expressionS * expressionP;
     boolean       accept_numbers;
     boolean       accept_list_names;
{
    int    reg_number;
    char * name;
    char * start;
    char   c;


    /* Find the spelling of the operand */
    start = name = input_line_pointer;

    c = get_symbol_end ();

    /* get register number */
    if( g_iAdvance == 0 )
    {
	    if( g_iPE == 0 )
	    {
			/* STD */
	        reg_number = reg_name_search (system_registers, SYSREG_NAME_CNT, name,
	                accept_numbers);
		}
		else
		{
			/* PE	add T.Tazaki 2003/11/18 */
	        reg_number = reg_name_search (pe_pushs_system_registers, PE_PUSHS_SYSREG_NAME_CNT, name,
	                accept_numbers);
	    }
    }else{
		/* ADV */
        reg_number = reg_name_search (adv_pushs_system_registers, ADV_PUSHS_SYSREG_NAME_CNT, name,
                accept_numbers);
    }
    
    * input_line_pointer = c;   /* put back the delimiting char */

    /* look to see if it's in the register table */
    if (reg_number >= 0) 
    {
        expressionP->X_op         = O_register;
        expressionP->X_add_number = reg_number;

        /* make the rest nice */
        expressionP->X_add_symbol = NULL;
        expressionP->X_op_symbol  = NULL;

        return true;
    }
    else
    {
        /* reset the line as if we had not done anything */
        input_line_pointer = start;

        return false;
    }
}

/******************************************************************************
    INPUT   FILE*   not used
    RETURN  void
    Explanation display option
******************************************************************************/
void
md_show_usage (stream)
  FILE * stream;
{
    /* The special option for c33 is nothing. */
}

/******************************************************************************
    INPUT   int     not used
            char*   not used
    RETURN  int     TRUE    ok
    Explanation     Argument (option) analysis
******************************************************************************/
int
md_parse_option (c, arg)
     int    c;
     char * arg;
{
#if 0
    int Num;
    int i;
    if (c != 'm')
    {
      /* xgettext:c-format */
      fprintf (stderr, _("unknown command line option: -%c%s\n"), c, arg);
      return 0;
    }

#endif
  return 1;
}

/******************************************************************************
    INPUT   char*       not used
    RETURN  symbolS*    NULL
    Explanation Treatment of the symbol which is not defined
******************************************************************************/
symbolS *
md_undefined_symbol (name)
  char * name;
{
  return 0;
}

char *
md_atof (type, litp, sizep)
  int    type;
  char * litp;
  int *  sizep;
{
  int            prec;
  LITTLENUM_TYPE words[4];
  char *         t;
  int            i;

  switch (type)
    {
    case 'f':
      prec = 2;
      break;

    case 'd':
      prec = 4;
      break;

    default:
      *sizep = 0;
      return _("bad call to md_atof");
    }
  
  t = atof_ieee (input_line_pointer, type, words);
  if (t)
    input_line_pointer = t;

  *sizep = prec * 2;

  for (i = prec - 1; i >= 0; i--)
    {
      md_number_to_chars (litp, (valueT) words[i], 2);
      litp += 2;
    }

  return NULL;
}


/* Very gross.  */
void
md_convert_frag (abfd, sec, fragP)
  bfd *      abfd;
  asection * sec;
  fragS *    fragP;
{
  subseg_change (sec, 0);
  
  /* In range conditional or unconditional branch.  */
  if (fragP->fr_subtype == 0 || fragP->fr_subtype == 2)
    {
      fix_new (fragP, fragP->fr_fix, 2, fragP->fr_symbol,
           fragP->fr_offset, 1, BFD_RELOC_UNUSED + (int)fragP->fr_opcode);
      fragP->fr_var = 0;
      fragP->fr_fix += 2;
    }
  /* Out of range conditional branch.  Emit a branch around a jump.  */
  else if (fragP->fr_subtype == 1)
    {
      unsigned char *buffer = 
    (unsigned char *) (fragP->fr_fix + fragP->fr_literal);

      /* Reverse the condition of the first branch.  */
      buffer[0] ^= 0x08;
      /* Mask off all the displacement bits.  */
      buffer[0] &= 0x8f;
      buffer[1] &= 0x07;
      /* Now set the displacement bits so that we branch
     around the unconditional branch.  */
      buffer[0] |= 0x30;

      /* Now create the unconditional branch + fixup to the final
     target.  */
      md_number_to_chars (buffer + 2, 0x00000780, 4);
      fix_new (fragP, fragP->fr_fix + 2, 4, fragP->fr_symbol,
           fragP->fr_offset, 1, BFD_RELOC_UNUSED +
           (int) fragP->fr_opcode + 1);
      fragP->fr_var = 0;
      fragP->fr_fix += 6;
    }
  /* Out of range unconditional branch.  Emit a jump.  */
  else if (fragP->fr_subtype == 3)
    {
      md_number_to_chars (fragP->fr_fix + fragP->fr_literal, 0x00000780, 4);
      fix_new (fragP, fragP->fr_fix, 4, fragP->fr_symbol,
           fragP->fr_offset, 1, BFD_RELOC_UNUSED +
           (int) fragP->fr_opcode + 1);
      fragP->fr_var = 0;
      fragP->fr_fix += 4;
    }
  else
    abort ();
}

valueT
md_section_align (seg, addr)
     asection * seg;
     valueT     addr;
{
  int align = bfd_get_section_alignment (stdoutput, seg);
  return ((addr + (1 << align) - 1) & (-1 << align));
}

/******************************************************************************
    INPUT   NONE
    RETURN  void
    Explanation An assembler initial cofiguration peculiar to a model
******************************************************************************/
void
md_begin ()
{
  char *                              prev_name = "";
  register const struct c33_opcode * op;
  flagword                            applicable;

    /* Create hash table */
  c33_hash = hash_new();

  /* Insert unique names into hash table.  The C33 instruction set
     has many identical opcode names that have different opcodes based
     on the operands.  This hash table then provides a quick index to
     the first opcode with a particular name in the opcode table.  */

    /* nemonic registered into the operation code table is registered into a hash table.*/
    
    /* >>>>>> tazaki 2001.11.07 */
    if( g_iAdvance == 0 )		/* cpu = standard ? */
    {  
    	if( g_iPE == 0 )
    	{
			if( g_iMedda32 == 0 )
			{
		        op = c33_opcodes;           /* STANDARD table */
		    }
		    else
		    {
		        op = c33_opcodes32;         /* STANDARD table : No use default data area  add T.Tazaki 2004/07/30 */
		    }
	    }
	    else
	    {
			if( g_iMedda32 == 0 )
			{
		        op = c33_pe_opcodes;  	    /* PE table 	add T.Tazaki 2003/11/18 */
			}
			else
			{
		        op = c33_pe_opcodes32;      /* PE table : No use default data area  add T.Tazaki 2004/07/30 */
		    }
		}
    }
    else
    {
		if( g_iMedda32 == 0 )
		{
	        op = c33_advance_opcodes;       /* ADVANCE table */
	    }
	    else
	    {
	        op = c33_advance_opcodes32;     /* ADVANCE table : No use default data area  add T.Tazaki 2004/07/30 */
		}
    }
    /* <<<<<< tazaki 2001.11.07 */
    
    while (op->name)
    {
        if (strcmp (prev_name, op->name)) 
        {
            prev_name = (char *) op->name;
            hash_insert (c33_hash, op->name, (char *) op);
        }
        op++;
    }
#if 0   /* c33 */
  bfd_set_arch_mach (stdoutput, TARGET_ARCH, machine);
#endif  /* c33 */

    applicable = bfd_applicable_section_flags (stdoutput);
}

/******************************************************************************
    INPUT   int     NONE
    RETURN  bfd_reloc_code_real_type
    Explanation analysys symbol mask
******************************************************************************/
/* Warning: The code in this function relies upon the definitions
   in the c33_operands[] array (defined in opcodes/c33-opc.c)
   matching the hard coded values contained herein.  */

static bfd_reloc_code_real_type
c33_reloc_prefix ()
{
    
    /* Is it the prefix of a symbol mask? */
    if (*input_line_pointer == '@'){
        input_line_pointer++;
    }else{

        /* NO  */
        return BFD_RELOC_UNUSED;
    }
#define CHECK_(name, reloc)                                             \
    if (strncmp (input_line_pointer, name, strlen (name) ) == 0)    \
    {                                                               \
        input_line_pointer += strlen (name);                        \
        return reloc;                                           \
    }
        
    CHECK_ ("ah",   BFD_RELOC_C33_AH);      /* LABEL(25:13) */
    CHECK_ ("al",   BFD_RELOC_C33_AL);      /* LABEL(12:0)  */
    CHECK_ ("rh",   BFD_RELOC_C33_RH);      /* <LABEL-PC>(32:22)    */
    CHECK_ ("rm",   BFD_RELOC_C33_RM);      /* <LABEL-PC>(21:9) */
    CHECK_ ("rl",   BFD_RELOC_C33_RL);      /* <LABEL-PC>(8:0)  */
    CHECK_ ("h",    BFD_RELOC_C33_H);       /* LABEL(31:19) */
    CHECK_ ("m",    BFD_RELOC_C33_M);       /* LABEL(18:6)  */
    CHECK_ ("l",    BFD_RELOC_C33_L);       /* LABEL(5:0)   */

    CHECK_ ("AH",   BFD_RELOC_C33_AH);      /* LABEL(25:13) */
    CHECK_ ("AL",   BFD_RELOC_C33_AL);      /* LABEL(12:0)  */
    CHECK_ ("RH",   BFD_RELOC_C33_RH);      /* <LABEL-PC>(32:22)    */
    CHECK_ ("RM",   BFD_RELOC_C33_RM);      /* <LABEL-PC>(21:9) */
    CHECK_ ("RL",   BFD_RELOC_C33_RL);      /* <LABEL-PC>(8:0)  */
    CHECK_ ("H",    BFD_RELOC_C33_H);       /* LABEL(31:19) */
    CHECK_ ("M",    BFD_RELOC_C33_M);       /* LABEL(18:6)  */
    CHECK_ ("L",    BFD_RELOC_C33_L);       /* LABEL(5:0)   */

    return BFD_RELOC_UNUSED;
}

/* add tazaki 2001.12.03 >>>>> */

/* Warning: The code in this function relies upon the definitions
   in the c33_operands[] array (defined in opcodes/c33-opc.c)
   matching the hard coded values contained herein.  */

/******************************************************************************
    INPUT   int     NONE
    RETURN  bfd_reloc_code_real_type
    Explanation analisys symbol offset
******************************************************************************/
static bfd_reloc_code_real_type
c33_reloc_prefix_offset ()
{
  boolean paren_skipped = false;



/* Modify name## --> name : for gcc-3.3.6  T.Tazaki 2005/08/04 */
#define CHECK2_(name, reloc)                                            \
    if (strncmp (input_line_pointer, name"(", strlen (name) + 1 ) == 0)   \
    {                                                               \
        input_line_pointer += strlen (name);                        \
            return reloc;                                           \
    }

    CHECK2_ ("doff_hi",   BFD_RELOC_C33_DH);        /* (symbol - default data pointer)  */
    CHECK2_ ("doff_lo",   BFD_RELOC_C33_DL);        /* (symbol - default data pointer)  */
    CHECK2_ ("goff_lo",   BFD_RELOC_C33_GL);        /* (symbol - g data pointer)    */
    CHECK2_ ("soff_hi",   BFD_RELOC_C33_SH);        /* (symbol - s data pointer)    */
    CHECK2_ ("soff_lo",   BFD_RELOC_C33_SL);        /* (symbol - s data pointer)    */
    CHECK2_ ("toff_hi",   BFD_RELOC_C33_TH);        /* (symbol - t data pointer)    */
    CHECK2_ ("toff_lo",   BFD_RELOC_C33_TL);        /* (symbol - t data pointer)    */
    CHECK2_ ("zoff_hi",   BFD_RELOC_C33_ZH);        /* (symbol - z data pointer)    */
    CHECK2_ ("zoff_lo",   BFD_RELOC_C33_ZL);        /* (symbol - z data pointer)    */
    CHECK2_ ("dpoff_h",   BFD_RELOC_C33_DPH);       /* (symbol - default data pointer)  */
    CHECK2_ ("dpoff_m",   BFD_RELOC_C33_DPM);       /* (symbol - default data pointer)  */
    CHECK2_ ("dpoff_l",   BFD_RELOC_C33_DPL);       /* (symbol - default data pointer)  */

    CHECK2_ ("DOFF_HI",   BFD_RELOC_C33_DH);        /* (symbol - default data pointer)  */
    CHECK2_ ("DOFF_LO",   BFD_RELOC_C33_DL);        /* (symbol - default data pointer)  */
    CHECK2_ ("GOFF_LO",   BFD_RELOC_C33_GL);        /* (symbol - g data pointer)    */
    CHECK2_ ("SOFF_HI",   BFD_RELOC_C33_SH);        /* (symbol - s data pointer)    */
    CHECK2_ ("SOFF_LO",   BFD_RELOC_C33_SL);        /* (symbol - s data pointer)    */
    CHECK2_ ("TOFF_HI",   BFD_RELOC_C33_TH);        /* (symbol - t data pointer)    */
    CHECK2_ ("TOFF_LO",   BFD_RELOC_C33_TL);        /* (symbol - t data pointer)    */
    CHECK2_ ("ZOFF_HI",   BFD_RELOC_C33_ZH);        /* (symbol - z data pointer)    */
    CHECK2_ ("ZOFF_LO",   BFD_RELOC_C33_ZL);        /* (symbol - z data pointer)    */
    CHECK2_ ("DPOFF_H",   BFD_RELOC_C33_DPH);       /* (symbol - default data pointer)  */
    CHECK2_ ("DPOFF_M",   BFD_RELOC_C33_DPM);       /* (symbol - default data pointer)  */
    CHECK2_ ("DPOFF_L",   BFD_RELOC_C33_DPL);       /* (symbol - default data pointer)  */

    return BFD_RELOC_UNUSED;

}

/* add tazaki 2002.02.29 >>>>> */
/******************************************************************************
    INPUT   NONE
    RETURN  operand code
    advanced macro "EXT COND" instruction operand analysis
******************************************************************************/
static int
c33_condition ()
{
  boolean paren_skipped = false;


#define CHECK3_(name, cond)                                         \
    if (strncmp (input_line_pointer, name, strlen (name) ) == 0)    \
    {                                                               \
        input_line_pointer += strlen (name);                        \
            return cond;                                            \
    }

    CHECK3_ ("gt",   0x04);
    CHECK3_ ("ge",   0x05);
    CHECK3_ ("lt",   0x06);
    CHECK3_ ("le",   0x07);
    CHECK3_ ("ugt",  0x08);
    CHECK3_ ("uge",  0x09);
    CHECK3_ ("ult",  0x0a);
    CHECK3_ ("ule",  0x0b);
    CHECK3_ ("eq",   0x0c);
    CHECK3_ ("ne",   0x0d);

    return 0;

}
/* add tazaki 2002.02.29 >>>>> */
/******************************************************************************
    INPUT   NONE
    RETURN  operand code
    advanced macro "EXT OP,imm2" or "EXT %RB,OP,imm2" instruction operand analysis
******************************************************************************/
static int
c33_op_shift ()
{
  boolean paren_skipped = false;


#define CHECK4_(name, op_shift_code)                                \
    if (strncmp (input_line_pointer, name, strlen (name) ) == 0)    \
    {                                                               \
        input_line_pointer += strlen (name);                        \
            return op_shift_code;                                   \
    }

    CHECK4_ ("sra",   0x01);
    CHECK4_ ("srl",   0x02);
    CHECK4_ ("sll",   0x03);

    return 0;

}

/******************************************************************************
    INPUT   unsigned long
            const struct c33_operand*   operand
            offsetT
            char*
            unsigned int
            char*
            int
    RETURN  unsigned long
    Explanation An operand and an operation code are made into a command code.
******************************************************************************/
 unsigned long               ulrd;		/* add T.Tazaki 2004/07/23 */

/* Insert an operand value into an instruction.  */
static unsigned long
c33_insert_operand (insn, operand, val, file, line, str,flags)
     unsigned long               insn;
     const struct c33_operand * operand;
     offsetT                     val;
     char *                      file;
     unsigned int                line;
     char *                      str;
     int                        flags;
{

    long    min, max, lval;
    unsigned long   ulValue, ulMask;
    int     iSign;

    /* Does a function exist? */
    if (operand->insert)
    {
    /* YES */
    
        const char * message = NULL;

        insn = operand->insert (insn, val, & message);
        if (message != NULL)
        {
            if (str)
            {
                if (file == (char *) NULL)
                    as_warn ("%s: %s", str, message);
                else
                    as_warn_where (file, line, "%s: %s", str, message);
            }
            else
            {
                if (file == (char *) NULL)
                    as_warn (message);
                else
                    as_warn_where (file, line, message);
            }
        }
    }
    else
    {
    /* NO  */
    
//      long    min, max, lval;
//      unsigned long   ulValue, ulMask;
//      int     iSign;

        lval = val; /* T.Tazaki 2002.02.27 */

        if( operand->range < 32 ){

            /* Restore min and mix to expected values for decimal ranges.  */

            if (flags & C33_OPERAND_SIGNED){

                ulValue = val;

                ulMask = 0xffffffff >> ( operand->range - 1 );
                ulMask <<= ( operand->range - 1 );
                if(( ulValue & ulMask ) == ulMask ){
                    iSign = 1;  /* (-) */       /* "jp 0xffffff81" */
                }else{
                    if( ( ulValue & ulMask ) == (1 << ( operand->range - 1 )) ){    /* only sign bit = 1 ? */
                        iSign = 1;      /* (-) */   /* "jp 0x81" */
                    }else{
                        if(( ulValue & ulMask ) == 0 ){ /* sign bit = 0 ? */
                            iSign = 0;  /* (+) */   /* "jp 0x40" */
                        }else{
                            iSign = 2;  /* Warninng */
                        }
                    }
                }

                if( iSign == 2 ){   /* Invalid range ?  "jp 0x102" */
                    const char * err = _("operand out of range (%s not between %ld and %ld)");
                    char         buf[100];

                    max = (1 << ( operand->range - 1 )) - 1;    /* T.Tazaki 2002.02.27 */
                    min = - (1 << (operand->range - 1));        /* T.Tazaki 2002.02.27 */
                    if (str)
                    {
                      sprintf (buf, "%s: ", str);
                      
                      sprint_value (buf + strlen (buf), val);
                    }
                    else
                        sprint_value (buf, lval);
              
                    if (file == (char *) NULL)
                        as_warn (err, buf, min, max);
                    else
                        as_warn_where (file, line, err, buf, min, max);
                }
                else
                {
                    if( iSign == 0 ){   /* (+) ? */
                        lval = val;
                    }else{
                        if( iSign == 1 ){ /* (-) ? */
                            lval = ulValue | ulMask;    /* sign extend */
                        }
                    }
                    
        /*          max = (1 << ( operand->bits - 1 )) - 1;
                    min = - (1 << (operand->bits - 1));         */
                    max = (1 << ( operand->range - 1 )) - 1;    /* T.Tazaki 2002.02.27 */
                    min = - (1 << (operand->range - 1));        /* T.Tazaki 2002.02.27 */

                    if (lval < min || lval > max)
                    {
                        const char * err = _("operand out of range (%s not between %ld and %ld)");
                        char         buf[100];

                        if (str)
                        {
                          sprintf (buf, "%s: ", str);
                          
                          sprint_value (buf + strlen (buf), lval);
                        }
                        else
                            sprint_value (buf, lval);
                  
                        if (file == (char *) NULL)
                            as_warn (err, buf, min, max);
                        else
                            as_warn_where (file, line, err, buf, min, max);
                    }
                }
            }
            else {
                /* It asks for the range of effective value. */
                max = (1 << operand->bits) - 1;
                min = 0;

                /* Value is range outside. */
                if (lval < (offsetT) min || lval > (offsetT) max)
                {
                    /* xgettext:c-format */
                    const char * err = _("operand out of range (%s not between %ld and %ld)");
                    char         buf[100];

                    if (str)
                    {
                      sprintf (buf, "%s: ", str);
                      
                      sprint_value (buf + strlen (buf), lval);
                    }
                    else
                        sprint_value (buf, lval);

                    if (file == (char *) NULL)
                        as_warn (err, buf, min, max);
                    else
                        as_warn_where (file, line, err, buf, min, max);
                }
            }
        }

        /* An operand and an operation code are made into a command code. */
//      insn |= (((long) val & ((1 << operand->bits) - 1)) << operand->shift);
        insn |= (((long) lval & ((1 << operand->bits) - 1)) << operand->shift); /* T.Tazaki 2002.02.27 */
/* >>>> add tazaki advanced macro */
        if (flags & C33_OPERAND_01) {
            insn |= 0x0010; /* bit 5,4 = 0,1 set */
        }
        if (flags & C33_OPERAND_OP3_01) {
            insn |= 0x0040; /* bit 7,6 = 0,1 set : psrset imm5 */
        }
        if (flags & C33_OPERAND_OP3_10){
            insn |= 0x0080; /* bit 7,6 = 1,0 set : psrclr imm5 */
        }
/* <<<< add tazaki advanced macro */
    }
    return insn;
}


static char                 copy_of_instruction [128];

void
md_assemble (str) 
     char * str;
{
    char *                  s;
    char *                  start_of_operands;
    struct c33_opcode *     opcode;
    struct c33_opcode *     opcode2;    /* add tazaki 2001.08.10 */
    struct c33_opcode *     next_opcode;
    const unsigned char *   opindex_ptr;
    int                     next_opindex;
    int                     relaxable;
    unsigned short          insn;
    unsigned short          insn_wk;    /* add tazaki 2001.12.11 */
    unsigned short          copy_insn;
    char *                  f;      /* insn address */
    char *                  where;      /* insn address */
    int                     i;
    int                     match;
    boolean                 extra_data_befor_insn = false;
    unsigned int            extra_data_len;
    unsigned short          extraInsnBuf[10];
    char *                  saved_input_line_pointer;
    char *                  check_input_line_pointer;
    char *                  temp_input_line_pointer;
    unsigned short *        fromP;
    int                     iNumber;
    unsigned int            uiNumber;                       /* add tazaki 2002.03.11 */
    
    int                     i_ext_off;                      /* add tazaki 2001.08.07 */
    int                     iMEM_IMM26_flag;                /* add tazaki 2001.10.11 */
    char *                  pSymbolName ;                   /* for [imm26] add tazaki 2001.11.28 */
    char                    szSymbolName[300];              /* for [imm26] add tazaki 2001.11.28 */
    char *                  temp_pointer;                   /* for [imm26] add tazaki 2001.11.28 */

	int						g_iBitTest = 0;					/* for Bit Test命令のオペランド imm3 用  add T.Tazaki 2004/07/30 */
	int						g_iBitTest_range = 0;			/* for Bit Test命令のオペランド imm3 用  add T.Tazaki 2004/07/30 */
	int						g_iXload = 0;					/* for xld.x [symbol+imm],%rs命令のオペランド %rs 用  add T.Tazaki 2004/07/30 */
	int						g_iXload_range = 0;				/* for xld.x [symbol+imm],%rs命令のオペランド %rs 用  add T.Tazaki 2004/07/30 */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
	int						i_ext_cnt = -1;					// numbers of ext to add (default when -1)

	// indices for extraInsnBuf
	unsigned int			insn_idx_high = 0;				// for ext @h
	unsigned int			insn_idx_mid  = 0;				// for ext @m
	unsigned int			insn_idx_low  = 0;				// for ld  @l	(always present)
	unsigned int			insn_idx_load = 0;				// for ld.*		(always present)
	unsigned int			insn_idx_pop  = 0;				// for mem write
// ADD D.Fujimoto 2007/06/25 <<<<<<<


// ADD D.Fujimoto 2007/06/21 >>>>>>>
#ifdef EXT_REMOVE
	if( g_c33_ext == 1 ){
		// get the offset from the nearest symbol
		evaluate_offset_from_symbol();
	}
#endif
// ADD D.Fujimoto 2007/06/21 <<<<<<<

    pSymbolName = &szSymbolName[0];                         /* Symbol name pointer domain initialization for [imm26] add tazaki 2001.11.28*/

    /* The command character sequence for one line is copied. */
    strncpy (copy_of_instruction, str, sizeof (copy_of_instruction) - 1);
  
    /* It is made a character sequence as an operation code. */
    /* Get the opcode.  */
    for (s = str; *s != '\0' && ! isspace (*s); s++)
        continue;

    if (*s != '\0')
        *s++ = '\0';

    /* find the first opcode with the proper name */
    opcode = (struct c33_opcode *) hash_find (c33_hash, str);

    /* not find operation code? */
    if (opcode == NULL) {
        /* xgettext:c-format */
        as_bad (_("Unrecognized opcode: `%s'"), str);
        ignore_rest_of_line ();
        return;
    }

    /* The space from an operation code to an operand is skipped. */
    str = s;
    while (isspace (* str))
        ++ str;

    /* The start position of an operand is held. */
    start_of_operands = str;

    saved_input_line_pointer = input_line_pointer;

    /* An interpretation of an operand */
    for (;;) {
        const char * errmsg = NULL;

        match = 0;

        relaxable = 0;
        fc = 0;
        next_opindex = 0;
        insn = opcode->opcode;
        extra_data_befor_insn = false;

        iMEM_IMM26_flag = 0;    /* add tazaki 2001.10.11 */

        /* The start position of an operand is held. */
        input_line_pointer = str = start_of_operands;

        /* Only the number of operands is repeated. */
        for (opindex_ptr = opcode->operands; *opindex_ptr != 0; opindex_ptr ++)
        {
            const struct c33_operand * operand;
            char *                      hold;
            expressionS                 ex;
            expressionS                 ext_ex;
            bfd_reloc_code_real_type    reloc;  
            
            int flags;
            
            if (next_opindex == 0)
            {
                /* An operand is acquired. */
                operand = & c33_operands[ * opindex_ptr ];
            }
            else
            {
                operand      = & c33_operands[ next_opindex ];
                next_opindex = 0;
            }

            flags = operand->flags;
            errmsg = NULL;

            while (*str == ' ' || *str == ',' || *str == ']')
              ++ str;

            /* Gather the operand. */
            hold = input_line_pointer;
            input_line_pointer = str;
     
            errmsg = NULL;

            /*  tazaki 2002.01.11  >>>>>> */
            i_ext_off = 0;
            opcode2 = (struct c33_opcode *)c33_ext_opcodes;
            if( opcode->opcode == opcode2->opcode ){
                /* ext xoff_hi(sym),ext xoff_lo(sym),ext dpoff_h,m(sym) : analisys symbol offset */
                reloc = c33_reloc_prefix_offset ();
                if( reloc != BFD_RELOC_UNUSED ){
                    i_ext_off = 1;
                }
            }
            if( i_ext_off == 1 )
            {
                /* ext 命令 and xoff_hi(sym),ext xoff_lo(sym),dpoff_h,m(sym) ? */

                /* read symbol , disp */

                expression (& ex);

                if (fc >= MAX_INSN_FIXUPS)
                  as_fatal (_("too many fixups"));

                fixups[ fc ].exp     = ex;
                fixups[ fc ].opindex = * opcode->operands;
                fixups[ fc ].reloc   = reloc;
                ++fc;

            }
            else
            {
                /* >>>> add tazaki 2002.02.29 class1 : ext cond*/
                if ((operand->flags & C33_OPERAND_COND) == C33_OPERAND_COND) 
                {
                    int iCond = c33_condition();    /* get operand code */
                    if( iCond != 0 ){

                        ex.X_op = O_cond;
                        ex.X_add_number = iCond;

                    }else{
                        errmsg = _("invalid operand");
                    }
                }
                /* >>>> add tazaki 2002.02.29 class1 : ext OP,imm2 | ext %rb,OP,imm2 */
                else if ((operand->flags & C33_OPERAND_OP_SHIFT) == C33_OPERAND_OP_SHIFT) 
                {
                    int iShiftCode = c33_op_shift();    /* get operand code */
                    if( iShiftCode != 0 ){

                        ex.X_op = O_op_shift;
                        ex.X_add_number = iShiftCode;

                    }else{
                        errmsg = _("invalid operand");
                    }
                }
                /* >>>> add tazaki 2001.09.18 class7 : [%dp+imm6]*/
                else if ((operand->flags & C33_OPERAND_DPMEM) == C33_OPERAND_DPMEM) 
                {
                    if (*str == '['){
                        str++;
                        while (isspace (*str))
                            ++str;

                        input_line_pointer = str;

                        /* %dp ? */
                        if ( ( strncmp(str,"%dp",3) != 0 ) && ( strncmp(str,"%DP",3) != 0 ))
                        {
                        /* YES  */
                            errmsg = _("invalid system register name");
                        }
                        else {
                            str+=3;
                            while (isspace (*str))
                                ++str;

                            /* IF ']' */
                            if (*str == ']'){
                            /* YES only register */
                                /* Support only"[%dp]"  */
                                ex.X_op         = O_constant;
                                ex.X_add_symbol = NULL;
                                ex.X_op_symbol  = NULL;
                                ex.X_add_number = 0;
                                str++;
                                input_line_pointer = str;
                            }
                            else if (*str == '+'){
                                str++;
                                input_line_pointer = str;

                                reloc = c33_reloc_prefix_offset();
                                if( reloc != BFD_RELOC_UNUSED ){    /* dpoff_l() ? */
                                    errmsg = _("invalid operand");
                                }
                                else
                                {
                                    /* An operand is developed at a formula. */
                                    expression (& ex);

                                    iNumber = ex.X_add_number;

                                    if (operand->range <= 6){
                                        /* EMPTY */
                                    }
                                    else if (operand->range == 32) {
                                        /* update tazaki 2002.03.08 >>> */
                                        if (opcode->specialFlag == 1){
                                            /* ld.b */
                                            if ((unsigned int)iNumber <= 0x3f){
                                                ex.X_add_number /= 1;
                                                
                                            }else if ((unsigned int)iNumber <= 0x7ffff){
                                                /* 1 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 1;
                                                extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                            else {
                                                /* 2 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 2;
                                                extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                                extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                        }
                                        else if (opcode->specialFlag == 2){
                                            /* ld.h */
                                            if ((unsigned int)iNumber <= 0x7f){
                                                ex.X_add_number /= 2;
                                                
                                            }else if ((unsigned int)iNumber <= 0x7ffff){
                                                /* 1 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 1;
                                                extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                            else {
                                                /* 2 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 2;
                                                extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                                extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                        }
                                        else if (opcode->specialFlag == 4){
                                            /* ld.w */
                                            if ((unsigned int)iNumber <= 0xff){
                                                ex.X_add_number /= 4;
                                                
                                            }else if ((unsigned int)iNumber <= 0x7ffff){
                                                /* 1 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 1;
                                                extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                            else {
                                                /* 2 ext */
                                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                                extra_data_befor_insn = true;
                                                extra_data_len  = 2;
                                                extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                                extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                                ex.X_add_number = iNumber & 0x3f;
                                            }
                                        }
                                        /* update tazaki 2002.03.08 <<< */
                                    }
                                    else {
                                        errmsg = _("constant too big to fit into instruction");
                                    }
                                }
                            }
                            else{
                                /* YES  */
                                errmsg = _("invalid operand");
                            }
                        }
                    }
                    else
                        errmsg = _("invalid operand");
                }

                /* >>>> add tazaki 2002.01.15 class7 : [%dp+dpoff_l(symbol)]*/
                else if ((operand->flags & C33_OPERAND_DPSYMBOL6) == C33_OPERAND_DPSYMBOL6) 
                {
                    if (*str == '['){
                        str++;
                        while (isspace (*str))
                            ++str;

                        input_line_pointer = str;

                        /* %dp ? */
                        if ( ( strncmp(str,"%dp",3) != 0 ) && ( strncmp(str,"%DP",3) != 0 ))
                        {
                        /* YES  */
                            errmsg = _("invalid system register name");
                        }
                        else {
                            str+=3;
                            while (isspace (*str))
                                ++str;

                            /* IF ']' */
                            if (*str == ']'){
                            /* YES only register */
                                /* Support only"[%dp]  */
                                ex.X_op         = O_constant;
                                ex.X_add_symbol = NULL;
                                ex.X_op_symbol  = NULL;
                                ex.X_add_number = 0;
                                str++;
                                input_line_pointer = str;
                            }
                            else
                            {
                                if (*str == '+'){
                                    /* [%dp+dpoff_l(symbol)] */
                                    str++;
                                    input_line_pointer = str;

                                    reloc = c33_reloc_prefix_offset();
                                    if( reloc == BFD_RELOC_C33_DPL ){   /* dpoff_l() ? */

                                        expression (& ex);

                                        if (ex.X_op != O_symbol){   /* not symbol ? */
                                            errmsg = _("invalid operand");
                                        }
                                    }else{
                                        errmsg = _("invalid operand");
                                    }
                                }else{
                                    errmsg = _("invalid operand");
                                }
                            }
                        }
                    }
                    else
                        errmsg = _("invalid operand");
                }
                
                /* >>>> add tazaki 2002.01.15 class7 : dpoff_l(symbol) */
                else if ((operand->flags & C33_OPERAND_DPSYMBOL6_2) == C33_OPERAND_DPSYMBOL6_2) 
                {
                    reloc = c33_reloc_prefix_offset();
                    if( reloc == BFD_RELOC_C33_DPL ){   /* dpoff_l() ? */

                        expression (& ex);

                        if (ex.X_op != O_symbol){   /* not symbol ? */
                            errmsg = _("invalid operand");
                        }
                    }else{
                        errmsg = _("invalid operand");
                    }
                }
                /* >>>> add tazaki 2001.12.21 class0 : %dp */

                else if ((operand->flags & C33_OPERAND_DP) == C33_OPERAND_DP) 
                {
                    /* check %dp ? */
                    if ( ( strncmp(str,"%dp",3) != 0 ) && ( strncmp(str,"%DP",3) != 0 ))
                    {
                        /* NO  */
                        errmsg = _("invalid system register name");
                    }else{
                        str+=3;
                        while (isspace (*str))
                            ++str;
                        input_line_pointer = str;
                            
                        ex.X_op  = O_dpregister;
                        /* In %dp, an operand is not inserted in a command code. */
                    }
                /* <<<< add tazaki 2001.12.21 class0 : %dp */
                }
                else if ((operand->flags & C33_OPERAND_REG) != 0) 
                {
                    if (!register_name (& ex))
                    {
                    /* YES  */
                        errmsg = _("invalid register name");
                    }
                }
                else if ((operand->flags & C33_OPERAND_LD_SREG) != 0)   /* add 2002.06.19 error : ld.w %pc,%rs */
                {
                    if (!load_system_register_name (& ex, true, false))
                    {
                        errmsg = _("invalid system register name");
                    }
                }
                else if ((operand->flags & C33_OPERAND_PUSHS_SREG) != 0)    /* add 2002.06.19 : pushs %ss ,pops %sd */
                {
                    if (!pushs_system_register_name (& ex, true, false))
                    {
                        errmsg = _("invalid system register name");
                    }
                }
                else if (((operand->flags & C33_OPERAND_SREG) != 0) ||
                        ((operand->flags & C33_OPERAND_SP) != 0)) 
                {
                    if (!system_register_name (& ex, true, false))
                    {
                        errmsg = _("invalid system register name");
                    }
                    else {
                        /* NO register OK */
                        if ((operand->flags & C33_OPERAND_SP) != 0) 
                        {
                            ex.X_op  = O_spregister;
                            /* In %sp, an operand is not inserted in a command code. */
                        }
                    }
                }
                /* ELSE IF Operand is immidiate ? */
                else if ((operand->flags & C33_OPERAND_IMM) ||
                        (operand->flags & C33_OPERAND_SIGNED))
                {

                    expression (& ex);

                    /* IF Operand is constant ?*/
                    if (ex.X_op == O_constant)
                    {
                        if (opcode->specialFlag == 5){

                            /* shift/lotate */

                            iNumber = ex.X_add_number;
                            if( iNumber > 8 ){

                                errmsg = _("operand out of range (not between 0 and 8)");
                            }else{
                                extra_data_len = 0;
                                ex.X_add_number = iNumber;
                            }
                        }else if (opcode->specialFlag == 6){

                            /* 拡張命令のshift/lotate */

                            iNumber = ex.X_add_number;
                            if( iNumber > 31 ){
                                errmsg = _("operand out of range (not between 0 and 31)");
                            }else{
                                extra_data_len = 0;
                                while(iNumber > 8){
                                    extra_data_befor_insn = true;
                                    copy_insn = c33_insert_operand (insn, operand,8,
                                                  (char *) NULL, 0,
                                                  copy_of_instruction,flags);
                                    extraInsnBuf[extra_data_len] = copy_insn;
                                    extra_data_len++;
                                    ex.X_add_number =  8;
                                    iNumber -= 8;
                                }
                                ex.X_add_number = iNumber;
                            }
                        }else if (opcode->specialFlag == 7){
                            
                            /* Advanced mode "X" Extended of shift/lotate */
                            iNumber = ex.X_add_number;
                            if( iNumber > 31 ){
                                errmsg = _("operand out of range (not between 0 and 31)");
                            }else{
                                if( iNumber <= 15 ){

                                }else{
                                    ex.X_add_number -= 16;
                                    
                                    /* Convert Opecode */
                                    insn_wk = insn & 0xff00;
                                    
                                    switch( insn_wk ){
                                    case 0x8800: insn_wk = 0x2300;  break;  /* srl -->class 1 : srl */
                                    case 0x8c00: insn_wk = 0x2700;  break;  /* sll -->class 1 : sll */
                                    case 0x9000: insn_wk = 0x2b00;  break;  /* sra -->class 1 : sra */
                                    case 0x9400: insn_wk = 0x2f00;  break;  /* sla -->class 1 : sla */
                                    case 0x9800: insn_wk = 0x3300;  break;  /* rr  -->class 1 : rr  */
                                    case 0x9c00: insn_wk = 0x3700;  break;  /* rl  -->class 1 : rl  */
                                    default :   errmsg = _("invalid operand");  break;

                                    }
                                    insn = insn_wk | ( insn & 0x00ff );
                                }
                            }
                        }
                        else {
                        /* shift/lotate命令以外 */
                        
                            /* Possible to sign32 */
                            iNumber = ex.X_add_number;
                            
                            if (operand->flags & C33_OPERAND_SIGNED){
                            /* YES signed */

                                if ((operand->range == 19) && ((-262144 <= iNumber && iNumber < -32) ||
                                        (31 < iNumber && iNumber <= 262143))){
                                    opcode = (struct c33_opcode *)c33_ext_opcodes;
                                    extra_data_befor_insn = true;
                                    extra_data_len  = 1;
                                    extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                    ex.X_add_number = iNumber & 0x3f;
                                    flags = C33_OPERAND_IMM;    /* sign(5:0) is not signed */
                                }
                                else if (operand->range == 22) {
                                    if (operand->flags & C33_OPERAND_PC){
                                        if (-256 <= iNumber && iNumber <= 254){
                                            ex.X_add_number = (iNumber >> 1) & 0xff;    /* sign22(8:1) */
                                        }
                                        else if ((-2097152 <= iNumber && iNumber < -256) ||
                                                (254 < iNumber && iNumber <= 2097150)){
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 9) & 0x1fff); /* sign22(21:9) */
                                            ex.X_add_number = (iNumber >> 1) & 0xff;                     /* sign22(8:1) */
                                            flags = C33_OPERAND_IMM;    /* sign(5:0)is not signed */
                                        }else{
                                            errmsg = _("operand out of range (not between -2097152 and 2097150)");
                                        }
                                    }
                                }
                                else if (operand->range == 32){
                                    /* jp,call */
                                    if (operand->flags & C33_OPERAND_PC){
                                        if (-256 <= iNumber && iNumber <= 254){
                                            ex.X_add_number = (iNumber >> 1) & 0xff;    /* sign32(8:1) */
                                        }
                                        else if ((-2097152 <= iNumber && iNumber < -256) ||
                                            (254 < iNumber && iNumber <= 2097150)){
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 9) & 0x1fff);
                                            ex.X_add_number = (iNumber >> 1) & 0xff;    /* sign32(8:1) */
                                            flags = C33_OPERAND_IMM;    /* sign(5:0)is not signed */
                                        }
                                        else {
                                            /* 2 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 2;
                                            extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1ff8);
                                            extraInsnBuf[1] = opcode->opcode | ((iNumber >> 9) & 0x1fff);
                                            ex.X_add_number = (iNumber >> 1) & 0xff;    /* sign32(8:1) */
                                            flags = C33_OPERAND_IMM;    /* sign(5:0)is not signed */
                                        }
                                    }
                                    else {
                                        if (-32 <= iNumber && iNumber <= 31){
                                            /* EMPTY */
                                        }
                                        else if ((-262144 <= iNumber && iNumber < -32) ||
                                            (31 < iNumber && iNumber <= 262143)){
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                            flags = C33_OPERAND_IMM;    /* sign(5:0)is not signed */
                                        }
                                        else {
                                            /* ext命令　２つ */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 2;
                                            extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                            extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                            flags = C33_OPERAND_IMM;    /* sign(5:0)is not signed */
                                        }
                                    }
                                }
                            }
                            else {
                            /* IMM */
                                switch( operand->range ){
                                case 19:
                                    if ((unsigned int)iNumber <= 0x7ffff){
                                        opcode = (struct c33_opcode *)c33_ext_opcodes;
                                        extra_data_befor_insn = true;
                                        extra_data_len  = 1;
                                        extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                        ex.X_add_number = iNumber & 0x3f;
                                    }
                                    break;
                                case 32:
                                    if ((unsigned int)iNumber <= 0x3f){
                                        /* EMPTY */
                                    }
                                    else if ((unsigned int)iNumber <= 0x7ffff){
                                        opcode = (struct c33_opcode *)c33_ext_opcodes;
                                        extra_data_befor_insn = true;
                                        extra_data_len  = 1;
                                        extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                        ex.X_add_number = iNumber & 0x3f;
                                    }
                                    else {
                                        /* 2 ext */
                                        
                                        opcode = (struct c33_opcode *)c33_ext_opcodes;
                                        extra_data_befor_insn = true;
                                        extra_data_len  = 2;
                                        
                                        extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                        extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                        ex.X_add_number = iNumber & 0x3f;
                                    }
                                    break;
                                }
                            }
                        }
                    }

                    /* ELSE IF Operand is LABEL & SYMBOL */

                    else if ((ex.X_op == O_symbol) && (operand->flags & C33_OPERAND_LABEL)){
                    /* YES jp/call only */

                        if (operand->range == 13){      /* 2001.4.27 ide */
                            /* YES ext */

                            /* Get Symbol mask relocation  */
                            reloc = c33_reloc_prefix ();
                            /* Not symbol mask ?  : ERROR = ext label, OK = ext label@xx */
                            if( reloc == BFD_RELOC_UNUSED ){    /* add tazaki 2002.03.04 */
                                errmsg = _("invalid operand");
                            }
                        }
                        else if (operand->range == 5){      /* add tazaki 2002.03.04 */
                            /* YES loop */
                            reloc = BFD_RELOC_C33_LOOP;     /* Adv : loop operand */
                        }
                        else if (operand->range == 8){
                            /* YES jp,call */

                            reloc = c33_reloc_prefix ();        /* add tazaki 2001.08.23 */
                            /* Not symbol mask ? */
                            if( reloc == BFD_RELOC_UNUSED ){    /* add tazaki 2001.08.23 */
                                /* Set Symbol Mask Relocation */
                                reloc = BFD_RELOC_C33_JP;       /* Modify BFD_RELOC_C33_RL -->BFD_RELOC_C33_JP 2002.04.22 */
                            }
                        }
                        /* add 2001.08.06 tazaki >>>>> */
                        else if (operand->range == 22){
                            /* YES scall,sjp,sjrxx */
                            
// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
							// optimizing ext inst
							if (g_c33_ext == 1) {
								i_ext_cnt = evaluate_ext_count_for_jumps(ex, 1, count_ext_for_jumps);
							}

							if (i_ext_cnt == 0) {
								// no ext
								reloc = BFD_RELOC_C33_S_RL;

							} else {

								/* 1 ext  */
								opcode = (struct c33_opcode *)c33_ext_opcodes;
								extra_data_befor_insn = true;
								extra_data_len	= 1;
								extraInsnBuf[0] = opcode->opcode;
								if (fc >= MAX_INSN_FIXUPS)
								  as_fatal (_("too many fixups"));

								fixups[ fc ].exp	 = ex;
								fixups[ fc ].opindex = * opcode->operands;
								fixups[ fc ].reloc	 = BFD_RELOC_C33_S_RM;
								++fc;

								reloc = BFD_RELOC_C33_S_RL;
							}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

                            /* 1 ext  */
                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                            extra_data_befor_insn = true;
                            extra_data_len  = 1;
                            extraInsnBuf[0] = opcode->opcode;
                            if (fc >= MAX_INSN_FIXUPS)
                              as_fatal (_("too many fixups"));

                            fixups[ fc ].exp     = ex;
                            fixups[ fc ].opindex = * opcode->operands;
                            fixups[ fc ].reloc   = BFD_RELOC_C33_S_RM;
                            ++fc;

                            reloc = BFD_RELOC_C33_S_RL;
#endif	/* EXT_REMOVE */

                        
                        }
                        /* add 2001.08.06 tazaki <<<<< */
                        else {
                            /* xjp,xcall */
                            
                            /* ext  label+imm32@rh */
                            /* ext  label+imm32@rm */
                            /* call label+imm32@rl */
                            
                            /* strにシンボル名が入っている。２パス　アセンブラのとき、これを比較する。 */
                            
// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
							// optimizing ext inst
							if (g_c33_ext == 1) {
								i_ext_cnt = evaluate_ext_count_for_jumps(ex, 2, count_ext_for_jumps);
							}

							if (i_ext_cnt == 0) {
								// no ext
								reloc = BFD_RELOC_C33_S_RL;

							} else if (i_ext_cnt == 1) {
								// 1 ext
								opcode = (struct c33_opcode *)c33_ext_opcodes;
								extra_data_befor_insn = true;
								extra_data_len	= 1;
								extraInsnBuf[0] = opcode->opcode;

								if (fc >= MAX_INSN_FIXUPS)
								  as_fatal (_("too many fixups"));

								fixups[ fc ].exp	 = ex;
								fixups[ fc ].opindex = * opcode->operands;
								fixups[ fc ].reloc	 = BFD_RELOC_C33_S_RM;
								++fc;

								reloc = BFD_RELOC_C33_S_RL;

							} else {

								/* 2 ext  */
								opcode = (struct c33_opcode *)c33_ext_opcodes;
								extra_data_befor_insn = true;
								extra_data_len	= 2;
								extraInsnBuf[0] = opcode->opcode;
								extraInsnBuf[1] = opcode->opcode;

								if (fc >= MAX_INSN_FIXUPS)
								  as_fatal (_("too many fixups"));

								fixups[ fc ].exp	 = ex;
								fixups[ fc ].opindex = * opcode->operands;	/* operand of ext */
								fixups[ fc ].reloc	 = BFD_RELOC_C33_S_RH;
								++fc;

								fixups[ fc ].exp	 = ex;
								fixups[ fc ].opindex = * opcode->operands;
								fixups[ fc ].reloc	 = BFD_RELOC_C33_S_RM;
								++fc;

								reloc = BFD_RELOC_C33_S_RL;
							}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

                            /* 2 ext  */
                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                            extra_data_befor_insn = true;
                            extra_data_len  = 2;
                            extraInsnBuf[0] = opcode->opcode;
                            extraInsnBuf[1] = opcode->opcode;

                            if (fc >= MAX_INSN_FIXUPS)
                              as_fatal (_("too many fixups"));

                            fixups[ fc ].exp     = ex;
                            fixups[ fc ].opindex = * opcode->operands;  /* operand of ext */
                            fixups[ fc ].reloc   = BFD_RELOC_C33_S_RH;
                            ++fc;

                            fixups[ fc ].exp     = ex;
                            fixups[ fc ].opindex = * opcode->operands;
                            fixups[ fc ].reloc   = BFD_RELOC_C33_S_RM;
                            ++fc;

                            reloc = BFD_RELOC_C33_S_RL;

#endif	/* EXT_REMOVE */
                        }
                    }
                    else if ((ex.X_op == O_symbol) && (operand->flags & C33_OPERAND_SYMBOL)){
                    /* YES ld.w only */
                        if( *str == '[' ){
                            errmsg = _("invalid operand");
                        }
                        else if (operand->range == 6){
                            /*  ld.w %rd,LABEL@l */

                            /* Get Symbol mask relocation  */
                            reloc = c33_reloc_prefix ();
                        }
                        else if (operand->range == 19){
                        /* xld.w rd,symbol+imm19 */

                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                            extra_data_befor_insn = true;
                            extra_data_len  = 1;
                            extraInsnBuf[0] = opcode->opcode;

                            if (fc >= MAX_INSN_FIXUPS)
                              as_fatal (_("too many fixups"));

                            fixups[ fc ].exp     = ex;
                            fixups[ fc ].opindex = * opcode->operands;  /* operand of ext */
                            fixups[ fc ].reloc   = BFD_RELOC_C33_M;
                            ++fc;

                            reloc = BFD_RELOC_C33_L;

                        }
                        else{
                            if (operand->range == 32){
                                /* ext  label+imm32@h */
                                /* ext  label+imm32@m */
                                /* xld.w    label+imm32@l */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
								// optimizing ext inst
								// pattern	xld.w %rd, LABEL
								if (g_c33_ext == 1) {
									i_ext_cnt = evaluate_ext_count(ex, 0, count_ext_for_xld_rd_symbol);
								}

								if (i_ext_cnt == 0) {
									// no ext
									reloc = BFD_RELOC_C33_L;

								} else if (i_ext_cnt == 1) {
									// 1 ext
									opcode = (struct c33_opcode *)c33_ext_opcodes;
									extra_data_befor_insn = true;
									extra_data_len	= 1;
									extraInsnBuf[0] = opcode->opcode;

									if (fc >= MAX_INSN_FIXUPS)
									  as_fatal (_("too many fixups"));

									fixups[ fc ].exp	 = ex;
									fixups[ fc ].opindex = * opcode->operands;
									fixups[ fc ].reloc	 = BFD_RELOC_C33_M;
									++fc;

									reloc = BFD_RELOC_C33_L;


								} else {

									/* 2 ext */
									opcode = (struct c33_opcode *)c33_ext_opcodes;
									extra_data_befor_insn = true;
									extra_data_len	= 2;
									extraInsnBuf[0] = opcode->opcode;
									extraInsnBuf[1] = opcode->opcode;

									if (fc >= MAX_INSN_FIXUPS)
									  as_fatal (_("too many fixups"));

									fixups[ fc ].exp	 = ex;
									fixups[ fc ].opindex = * opcode->operands;	/* operand of ext */
									fixups[ fc ].reloc	 = BFD_RELOC_C33_H;
									++fc;

									fixups[ fc ].exp	 = ex;
									fixups[ fc ].opindex = * opcode->operands;
									fixups[ fc ].reloc	 = BFD_RELOC_C33_M;
									++fc;

									reloc = BFD_RELOC_C33_L;

								}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<
                                    
                                /* 2 ext */
                                opcode = (struct c33_opcode *)c33_ext_opcodes;
                                extra_data_befor_insn = true;
                                extra_data_len  = 2;
                                extraInsnBuf[0] = opcode->opcode;
                                extraInsnBuf[1] = opcode->opcode;

                                if (fc >= MAX_INSN_FIXUPS)
                                  as_fatal (_("too many fixups"));

                                fixups[ fc ].exp     = ex;
                                fixups[ fc ].opindex = * opcode->operands;  /* operand of ext */
                                fixups[ fc ].reloc   = BFD_RELOC_C33_H;
                                ++fc;

                                fixups[ fc ].exp     = ex;
                                fixups[ fc ].opindex = * opcode->operands;
                                fixups[ fc ].reloc   = BFD_RELOC_C33_M;
                                ++fc;

                                reloc = BFD_RELOC_C33_L;
#endif	/* EXT_REMOVE */
                            }
                        }
                    }
                    else {
                        errmsg = _("invalid operand");
                    }
                }
                /* Operand is register indirectness (memory). */
//              else if ((operand->flags & C33_OPERAND_RB) != 0) 																	/* del T.Tazaki 2004/07/30 */
                else if ((operand->flags & C33_OPERAND_RB) != 0 && (operand->flags & C33_OPERAND_26) == 0 && g_iMedda32 == 1  )		/* add T.Tazaki 2004/07/30 */
                {
                    /* Pattern */
                    /* [rb] */

                    if (*str == '['){
                        str++;
                        input_line_pointer = str;

                        if (register_name (& ex))
                        {
                        /* YES register */
                            
                            /* Skip space */
                            while (isspace (*input_line_pointer))
                                ++input_line_pointer;
                            
                            /* IF ']' */
                            if (*input_line_pointer == ']'){
                            /* YES pattern match */
                                input_line_pointer++;
                            }
                            else {
                            /* NO not match */
                                errmsg = _("invalid operand");
                            }
                        }
                        else {
                        /* NO not register */
                            errmsg = _("invalid operand");
                        }

                    }
                    else
                        errmsg = _("invalid operand");
               }
				/* add T.Tazaki 2004/07/30 >>> */
                /* Operand is register indirectness (memory). */
                else if ((operand->flags & C33_OPERAND_RB) != 0 && g_iMedda32 == 0 )	/* Use data area */
                {
                    /* Pattern */
                    /* [rb] */

                    if (*str == '['){
                        str++;
                        input_line_pointer = str;

                        if (register_name (& ex))
                        {
                        /* YES register */
                            
                            /* Skip space */
                            while (isspace (*input_line_pointer))
                                ++input_line_pointer;
                            
                            /* IF ']' */
                            if (*input_line_pointer == ']'){
                            /* YES pattern match */
                                input_line_pointer++;
                            }
                            else {
                            /* NO not match */
                                errmsg = _("invalid operand");
                            }
                        }
                        else {
                        /* NO not register */
                            errmsg = _("invalid operand");
                        }

                    }
                    else
                        errmsg = _("invalid operand");
               }
               else if ((operand->flags & C33_OPERAND_RB) != 0 && (operand->flags & C33_OPERAND_26) != 0 && g_iMedda32 == 1 ) 
               {
                   /* Are there any symbol and IMM which follow a register? */
                   /* Pattern */
                   /* [rb+imm26] */
                            
                   if (*str == '['){
                       str++;
                       input_line_pointer = str;

                       if (register_name (& ex))
                       {
		                   /* Skip Space */
		                   while (isspace (*input_line_pointer))
		                       ++input_line_pointer;
		                            
		                   /* IF ']' */
		                   if (*input_line_pointer == ']'){
		                   /* YES Register Only */
		                       input_line_pointer++;
		                   }
		                   /* ELSE IF plus */
		                   else if (*input_line_pointer == '+'){
		                   /* YES plus */
		                            
		                       input_line_pointer++;
		                       /* symbol,imm */
		                       expression (& ext_ex);

		                       if (ext_ex.X_op == O_constant){
		                           /* [%rd+imm26] */
		                                
		                           uiNumber = ext_ex.X_add_number;
		                                 
		                           if (uiNumber == 0){
		                               /* EMPTY */
		                           }
		                           else if (uiNumber <= 0x1fff){
		                               opcode = (struct c33_opcode *)c33_ext_opcodes;
		                               extra_data_befor_insn = true;
		                               extra_data_len  = 1;
		                               extraInsnBuf[0] = opcode->opcode| (uiNumber & 0x1fff);
		                           }
		                           else if (uiNumber <= 0x3ffffff){
		                               opcode = (struct c33_opcode *)c33_ext_opcodes;
		                               extra_data_befor_insn = true;
		                               extra_data_len  = 2;
		                               extraInsnBuf[0] = opcode->opcode| ((uiNumber >> 13) & 0x1fff);
		                               extraInsnBuf[1] = opcode->opcode| (uiNumber & 0x1fff);
		                           }
		                           else {
		                           /* NO more than 27bit ? */
		                                    
		                               /* tnot support */
		                               errmsg = _("invalid operand");
		                               iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
		                           }
		                       }
		                       else {
		                       /* NO not immidiate */
		                           errmsg = _("invalid operand");
		                           iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
		                       }
		                    }
		                    else {
		                    /* NO any other character  */
		                        /* error */
		                        errmsg = _("invalid operand");
		                        iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
		                    }
						}
						else
	                        errmsg = _("invalid operand");
					}
					else
                        errmsg = _("invalid operand");
                }
                
				/* add T.Tazaki 2004/07/30 <<< */

/* >>> add tazaki 2001.11.20 */

                else if( ( (operand->flags & C33_OPERAND_MEM) != 0 ) && 
                         ( (operand->flags & C33_OPERAND_DP_SYMBOL) == C33_OPERAND_DP_SYMBOL ) )
                {
                    if (*str == '['){
                        str++;
                        input_line_pointer = str;

                        if (!register_name (& ex))
                        {
                        /* YES not register */

                            if (system_register_name (& ex, true, false)){
                            /* system register */
                                errmsg = _("invalid operand");
                            }
                            else {
                                check_input_line_pointer = input_line_pointer;
                                
                                expression (& ex);

                                /* if Minus "-", No Support! */
                                while(check_input_line_pointer < input_line_pointer){
                                    if (*check_input_line_pointer == '-'){
                                        errmsg = _("invalid operand");
                                        break;
                                    }
                                    check_input_line_pointer++;
                                }
                            }
                            if (errmsg != NULL){
                            }
                            /* ELSE IF Operand is SYMBOL */
                            else if (ex.X_op == O_symbol){
                                /* Pattern */
                                /* [symbol+imm32] */

                                if( operand->range == 19 )
                                {
									if( g_iMedda32 == 0 )									/* add T.Tazaki 2004/07/30 */
									{
	                                    /* ext   (symbol+imm32-dp)@6-18  : ext  imm13 */
	                                    /* ld.w  (symbol+imm32-dp)@0-5   : ld.w r0,[%dp+imm6] */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
										// optimizing ext inst
										// for following patterns 
										// 
										// ald.* %rd, [LABEL]	(mem read)	(without medda32)
										// ald.* [LABEL], %rd 	(mem write)	(without medda32)
										if (g_c33_ext == 1) {
											i_ext_cnt = evaluate_ext_count(ex, g_dpAddress, count_ext_for_ald_mem_rw);
										}

										if (i_ext_cnt == 0) {
											// no ext
											reloc = BFD_RELOC_C33_DPL;

										} else {
											/* 1 ext */
											opcode = (struct c33_opcode *)c33_ext_opcodes;
											extra_data_befor_insn = true;
											extra_data_len	= 1;
											extraInsnBuf[0] = opcode->opcode;

											if (fc >= MAX_INSN_FIXUPS)
											  as_fatal (_("too many fixups"));

											fixups[ fc ].exp	 = ex;
											fixups[ fc ].opindex = * opcode->operands;
											fixups[ fc ].reloc	 = BFD_RELOC_C33_DPM;
											++fc;
											reloc = BFD_RELOC_C33_DPL;

										}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

	                                    /* 1 ext */
	                                    opcode = (struct c33_opcode *)c33_ext_opcodes;
	                                    extra_data_befor_insn = true;
	                                    extra_data_len  = 1;
	                                    extraInsnBuf[0] = opcode->opcode;

	                                    if (fc >= MAX_INSN_FIXUPS)
	                                      as_fatal (_("too many fixups"));

	                                    fixups[ fc ].exp     = ex;
	                                    fixups[ fc ].opindex = * opcode->operands;
	                                    fixups[ fc ].reloc   = BFD_RELOC_C33_DPM;
	                                    ++fc;
	                                    reloc = BFD_RELOC_C33_DPL;
#endif	/* EXT_REMOVE */
		                             }
#if 0
		                             else
		                             {
										/* add T.Tazaki 2004/08/19 >>> */
										/* 0:pushn %r0         			*/
		                                /* 1:ext  label+imm32@m 		*/
		                                /* 2:ld.x %r0,label+imm32@l 	*/
		                                /* 3:ld.x %rd,[%r0]   			*/
		                                /* 4:popn %r0					*/
		                                    
	                                    extraInsnBuf[0] = 0x0200;	/* pushn %r0 */
		                                /* 1ext */
		                                opcode = (struct c33_opcode *)c33_ext_opcodes;
		                                extra_data_befor_insn = true;
		                                extraInsnBuf[1] = opcode->opcode;

		                                if (fc >= MAX_INSN_FIXUPS)
		                                  as_fatal (_("too many fixups"));

										/* これは、リンク後、pushn %rs の %rs が、BFD_RELOC_C33_M に設定されてしまうので新規にBFD_RELOC_C33_PUSHN_R0
										   を追加して、リンカで "pushn %r0" を設定させるためにある。 */
		                                fixups[ fc ].exp     = ex;
		                                fixups[ fc ].opindex = * opcode->operands; 
		                                fixups[ fc ].reloc   = BFD_RELOC_C33_PUSHN_R0;	/* デフォルトの"pushn %r0" を設定 */
		                                ++fc;

		                                fixups[ fc ].exp     = ex;
		                                fixups[ fc ].opindex = * opcode->operands;
		                                fixups[ fc ].reloc   = BFD_RELOC_C33_M;
		                                ++fc;

		                                reloc = BFD_RELOC_C33_L;

	                                    extra_data_befor_insn = true;
	                                    ulrd = insn & 0x000f;
	                                    insn &= 0xfff0;
	                                    extraInsnBuf[2] = insn;		/* ld.w %r0,sign6 */
	                                    
					                    if ((operand->flags & C33_XLDB_RD) != 0) 
	                                    {
	                                    	insn = 0x2000 + ulrd;		/* ld.b %rd,[%r0] */
	                                    }
	                                    else if((operand->flags & C33_XLDB_WR) != 0) 
	                                    {
											g_iXload = 1;		/* オペランド値取得用にフラグを１にする。 */
											g_iXload_range = operand->range;
		                                    insn = 0x3400;		/* ld.b [%r0],%rs */
		                                }
					                    if ((operand->flags & C33_XLDH_RD) != 0) 
	                                    {
	                                    	insn = 0x2800 + ulrd;		/* ld.h %rd,[%r0] */
	                                    }
	                                    else if((operand->flags & C33_XLDH_WR) != 0) 
	                                    {
											g_iXload = 1;		/* オペランド値取得用にフラグを１にする。 */
											g_iXload_range = operand->range;
		                                    insn = 0x3800;		/* ld.h [%r0],%rs */
		                                }
					                    if ((operand->flags & C33_XLDW_RD) != 0) 
	                                    {
	                                    	insn = 0x3000 + ulrd;		/* ld.w %rd,[%r0] */
	                                    }
	                                    else if((operand->flags & C33_XLDW_WR) != 0) 
	                                    {
											g_iXload = 1;		/* オペランド値取得用にフラグを１にする。 */
											g_iXload_range = operand->range;
		                                    insn = 0x3c00;		/* ld.w [%r0],%rs */
		                                }
					                    if ((operand->flags & C33_XLDUB_RD) != 0) 
	                                    {
	                                    	insn = 0x2400 + ulrd;		/* ld.ub %rd,[%r0] */
	                                    }
	                                    else if((operand->flags & C33_XLDUH_RD) != 0) 
	                                    {
		                                    insn = 0x2c00 + ulrd;		/* ld.uh %rd,[%r0] */
		                                }
	                                    extraInsnBuf[3] = insn;
	                                    insn = 0x0240;				/* popn %r0 */
	                                    extraInsnBuf[4] = insn;
	                                    extra_data_len = 4;

									}
									/* add T.Tazaki 2004/08/19 <<< */
#endif								
                                }else if ( operand->range == 32 ){
                                    /* ext   (symbol+imm32-%dp)@19-31 : ext  imm13 */
                                    /* ext   (symbol+imm32-%dp)@6-18  : ext  imm13 */
                                    /* ld.w  (symbol+imm32-%dp)@0-5   : ld.w r0,[%dp+imm6] */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
									//  optimizing ext inst
									// for following patterns 
									// 
									// xld.* %rd, [LABEL]	(mem read)	(without medda32 and is ADV)
									// xld.* [LABEL], %rd 	(mem write)	(without medda32 and is ADV)
									if (g_c33_ext == 1) {
										i_ext_cnt = evaluate_ext_count(ex, g_dpAddress, count_ext_for_xld_mem_rw_adv);
									}

									if (i_ext_cnt == 0) {
										// no ext
										reloc = BFD_RELOC_C33_DPL;

									} else if (i_ext_cnt == 1) {
										// 1 ext
										opcode = (struct c33_opcode *)c33_ext_opcodes;
										extra_data_befor_insn = true;
										extra_data_len	= 1;
										extraInsnBuf[0] = opcode->opcode;

										if (fc >= MAX_INSN_FIXUPS)
										  as_fatal (_("too many fixups"));

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DPM;
										++fc;
										reloc = BFD_RELOC_C33_DPL;

									} else {
										/* 2 ext */
										opcode = (struct c33_opcode *)c33_ext_opcodes;
										extra_data_befor_insn = true;
										extra_data_len	= 2;
										extraInsnBuf[0] = opcode->opcode;
										extraInsnBuf[1] = opcode->opcode;

										if (fc >= MAX_INSN_FIXUPS)
										  as_fatal (_("too many fixups"));

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;	/* extのオペランド */
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DPH;
										++fc;

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DPM;
										++fc;
										reloc = BFD_RELOC_C33_DPL;

									}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

                                    /* 2 ext */
                                    opcode = (struct c33_opcode *)c33_ext_opcodes;
                                    extra_data_befor_insn = true;
                                    extra_data_len  = 2;
                                    extraInsnBuf[0] = opcode->opcode;
                                    extraInsnBuf[1] = opcode->opcode;

                                    if (fc >= MAX_INSN_FIXUPS)
                                      as_fatal (_("too many fixups"));

                                    fixups[ fc ].exp     = ex;
                                    fixups[ fc ].opindex = * opcode->operands;  /* extのオペランド */
                                    fixups[ fc ].reloc   = BFD_RELOC_C33_DPH;
                                    ++fc;

                                    fixups[ fc ].exp     = ex;
                                    fixups[ fc ].opindex = * opcode->operands;
                                    fixups[ fc ].reloc   = BFD_RELOC_C33_DPM;
                                    ++fc;
                                    reloc = BFD_RELOC_C33_DPL;
#endif	/* EXT_REMOVE */

                                }
                            }
                        }
                        else {
                            errmsg = _("invalid operand");
                        }
                    }
                    else {
                        errmsg = _("invalid operand");
                    }
                }
/* <<< add tazaki 2001.11.20 */
                
                /* An operand is register indirectness (memory). */
                else if ((operand->flags & C33_OPERAND_MEM) != 0) 
                {
                    if (*str == '['){
                        str++;
                        input_line_pointer = str;

                        if (!register_name (& ex))
                        {
                            /* YES not register */

                            if (system_register_name (& ex, true, false)){
                                /* system register */
                                errmsg = _("invalid operand");
                            }
                            else {
                                check_input_line_pointer = input_line_pointer;
                                
                                expression (& ex);

                                /* if Minus "-", No Support! */
                                while(check_input_line_pointer < input_line_pointer){
                                    if (*check_input_line_pointer == '-'){
                                        errmsg = _("invalid operand");
                                        break;
                                    }
                                    check_input_line_pointer++;
                                }
                            }
                            
                            if (errmsg != NULL){
                            }
                            /* ELSE IF Operand is Immidiate ? */
                            else if (ex.X_op == O_constant){
                                errmsg = _("invalid operand");  /* [imm26] patter : Error! 2001.11.28 */
                            }
                            /* ELSE IF Operand is SYMBOL ? */
                            else if (ex.X_op == O_symbol){

                                /* Pattern */
                                /* [symbol+imm32] */

                                
                                /* ext goff_hi(symbol+imm32) */
                                /* ext goff_lo(symbol+imm32) */
                                /* [r15] */

                                if (operand->range == 13){
                                    opcode = (struct c33_opcode *)c33_ext_opcodes;
                                    extra_data_befor_insn = true;
                                    extra_data_len  = 1;
                                    extraInsnBuf[0] = opcode->opcode;

                                    if (fc >= MAX_INSN_FIXUPS)
                                      as_fatal (_("too many fixups"));

                                    fixups[ fc ].exp     = ex;
                                    fixups[ fc ].opindex = * opcode->operands;
                                    fixups[ fc ].reloc   = BFD_RELOC_C33_GL;
                                    ++fc;
                                    
                                        
                                    /* Save Register Number. */
                                    ex.X_op         = O_register;
                                    ex.X_add_number = GP_REG;
                                    ex.X_add_symbol = NULL;
                                    ex.X_op_symbol  = NULL;
                                
                                }
                                else if (operand->range == 26 && g_iMedda32 == 0 ){		/* use data area */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
									// optimizing ext inst
									// for following patterns 
									// 
									// xld.* %rd, [LABEL]	(mem read)	(without medda32 and not ADV)
									// xld.* [LABEL], %rd 	(mem write)	(without medda32 and not ADV)
									// xb*	 [LABEL], imm3				(without medda32)
									if (g_c33_ext == 1) {
										i_ext_cnt = evaluate_ext_count(ex, g_dpAddress, count_ext_for_xld_mem_rw);
									}

									if (i_ext_cnt == 1) {
										// 1 ext
										// at least 1 ext will be appended
										opcode = (struct c33_opcode *)c33_ext_opcodes;
										extra_data_befor_insn = true;
										extra_data_len	= 1;
										extraInsnBuf[0] = opcode->opcode;

										if (fc >= MAX_INSN_FIXUPS)
										  as_fatal (_("too many fixups"));

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DL;
										++fc;

										/* Save Register Number. */
										ex.X_op 		= O_register;
										ex.X_add_number = GP_REG;
										ex.X_add_symbol = NULL;
										ex.X_op_symbol	= NULL;


									} else {

										/* 2 ext */
										opcode = (struct c33_opcode *)c33_ext_opcodes;
										extra_data_befor_insn = true;
										extra_data_len	= 2;
										extraInsnBuf[0] = opcode->opcode;
										extraInsnBuf[1] = opcode->opcode;

										if (fc >= MAX_INSN_FIXUPS)
										  as_fatal (_("too many fixups"));

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;	/* operand of ext */
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DH;
										++fc;

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_DL;
										++fc;
										
											
										/* Save Register Number. */
										ex.X_op 		= O_register;
										ex.X_add_number = GP_REG;
										ex.X_add_symbol = NULL;
										ex.X_op_symbol	= NULL;

									}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

                                    /* 2 ext */
                                    opcode = (struct c33_opcode *)c33_ext_opcodes;
                                    extra_data_befor_insn = true;
                                    extra_data_len  = 2;
                                    extraInsnBuf[0] = opcode->opcode;
                                    extraInsnBuf[1] = opcode->opcode;

                                    if (fc >= MAX_INSN_FIXUPS)
                                      as_fatal (_("too many fixups"));

                                    fixups[ fc ].exp     = ex;
                                    fixups[ fc ].opindex = * opcode->operands;  /* operand of ext */
                                    fixups[ fc ].reloc   = BFD_RELOC_C33_DH;
                                    ++fc;

                                    fixups[ fc ].exp     = ex;
                                    fixups[ fc ].opindex = * opcode->operands;
                                    fixups[ fc ].reloc   = BFD_RELOC_C33_DL;
                                    ++fc;
                                    
                                        
                                    /* Save Register Number. */
                                    ex.X_op         = O_register;
                                    ex.X_add_number = GP_REG;
                                    ex.X_add_symbol = NULL;
                                    ex.X_op_symbol  = NULL;

#endif	/* EXT_REMOVE */
								}
								/* add T.Tazaki 2004/08/19 >>> */
                                else if ((operand->range == 19 || operand->range == 26) && g_iMedda32 == 1 ){	/* xld と ald 兼用  : no use data area */

// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
									// optimizing ext inst
									// for following patterns 
									// 
									// xld.* %rd, [LABEL]	(mem read)	(with medda32)
									// xld.* [LABEL], %rd 	(mem write)	(with medda32)
									// ald.* %rd, [LABEL]	(mem read)	(with medda32)
									// ald.* [LABEL], %rd 	(mem write)	(with medda32)
									// xb*	 [LABEL], imm3				(with medda32)

									/* スクラッチは%r0だが、%rs = %r0のときは後で、スクラッチを%r1に変更される。 */
									/* 0:pushn %r0		   			*/
									/* 1:ext  label+imm32@m 		*/
									/* 2:ext  label+imm32@m 		*/
									/* 3:ld.x %r0,label+imm32@l 	*/
									/* 4:ld.x [%r0],%rs   			*/
									/* 5:popn %r0					*/

// macro which means that we are now dealing with a memory read (*ld.* %rd, [LABEL]) operation
// and not memory write or bit operation
#define IS_MEM_READ ( (operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )


									if (g_c33_ext == 1) {
										// for ald
										if (operand->range == 19) {
											i_ext_cnt = evaluate_ext_count(ex, 0, count_ext_for_ald_mem_rw32);
										} else {
											i_ext_cnt = evaluate_ext_count(ex, 0, count_ext_for_xld_mem_rw32);
										}
									}

									// determine extraInsnBuf indices from ext counts
									if (i_ext_cnt == 0) {
										// no ext
										if (IS_MEM_READ) {
											insn_idx_low  = 0;
										} else {
											insn_idx_low  = 1;
										}
									} else if (i_ext_cnt == 1) {
										// 1 ext
										if (IS_MEM_READ) {
											insn_idx_low  = 1;
										} else {
											insn_idx_low  = 2;
										}
									} else {
										// 2 ext
										if (IS_MEM_READ) {
											if (operand->range == 26) {
												insn_idx_low  = 2;
											} else {
												insn_idx_low  = 1;		// ald
											}
										} else {
											if (operand->range == 26) {
												insn_idx_low  = 3;
											} else {
												insn_idx_low  = 2;		// ald
											}
										}
									}

									// set all indices
									insn_idx_high = (insn_idx_low < 2) ? 0 : insn_idx_low - 2;	// negative -> 0
									insn_idx_mid  = (insn_idx_low < 1) ? 0 : insn_idx_low - 1;	// negative -> 0
									insn_idx_low  = insn_idx_low;
									insn_idx_load = insn_idx_low + 1;
									insn_idx_pop  = insn_idx_low + 2;	// only for mem write


									// push insn
									extraInsnBuf[0] = 0x0200;	/* pushn %r0 */ 

									// ext insn
									opcode = (struct c33_opcode *)c33_ext_opcodes;
									extra_data_befor_insn = true;

									if (i_ext_cnt == 0) {
										; // do nothing
									} else if (i_ext_cnt == 1) {
										extraInsnBuf[insn_idx_mid] = opcode->opcode;
									} else {
										// ald.* does not have ext @h
										if (operand->range == 26) {
											extraInsnBuf[insn_idx_high] = opcode->opcode;
										}
										extraInsnBuf[insn_idx_mid]  = opcode->opcode;
									}


									// fill fixup array
									if (fc >= MAX_INSN_FIXUPS)
									  as_fatal (_("too many fixups"));

									/* これは、リンク後、pushn %rs の %rs が、BFD_RELOC_C33_H に設定されてしまうので新規にBFD_RELOC_C33_PUSHN_R0
									   を追加して、リンカで "pushn %r0" を設定させるためにある。 
									   xld.x %rd,[symbol+imm] は、push/pop を行わない。 */
									if (!IS_MEM_READ)
									{
										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands; 
										fixups[ fc ].reloc	 = BFD_RELOC_C33_PUSHN_R0;
										++fc;
									}

									if (i_ext_cnt == 0) {
										// 
										reloc = BFD_RELOC_C33_L;
                                	} else if (i_ext_cnt == 1) {

										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_M;
										++fc;

										reloc = BFD_RELOC_C33_L;

									} else {
										
										// ald.* has only 1 ext
										if ( operand->range == 26 ) {
											fixups[ fc ].exp	 = ex;
											fixups[ fc ].opindex = * opcode->operands;	/* operand of ext */
											fixups[ fc ].reloc	 = BFD_RELOC_C33_H;
											++fc;
										}
										fixups[ fc ].exp	 = ex;
										fixups[ fc ].opindex = * opcode->operands;
										fixups[ fc ].reloc	 = BFD_RELOC_C33_M;
										++fc;

										reloc = BFD_RELOC_C33_L;

									} 


									// low insn (ld.w %rd, @l)
									ulrd = insn & 0x000f;
									if( operand->range == 26 )
									{
										if (IS_MEM_READ)
										{
											extraInsnBuf[insn_idx_low] = insn;		/* ld.w %rd,sign6 */
										}
										else
										{
											insn &= 0xfff0;
											extraInsnBuf[insn_idx_low] = insn;		/* ld.w %r0,sign6 */
										}
									}
									else
									{
										if (IS_MEM_READ)
										{
											/* ald.x %rd,[symbol+imm] */
											extraInsnBuf[insn_idx_low] = insn;		/* ld.w %rd,sign6 */
										}
										else
										{
											/* ald.x [symbol+imm],%rs */
											insn &= 0xfff0;
											extraInsnBuf[insn_idx_low] = insn;		/* ld.w %r0,sign6 */
										}
									}

									// load insn
									if ((operand->flags & C33_XLDB_RD) != 0) 
									{
										insn = 0x2000 + ulrd + ulrd * 16;		/* ld.b %rd,[%rd] */
									}
									else if((operand->flags & C33_XLDB_WR) != 0) 
									{
										g_iXload = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
										insn = 0x3400;							/* ld.b [%r0],%rs */
									}
									if ((operand->flags & C33_XLDH_RD) != 0) 
									{
										insn = 0x2800 + ulrd + ulrd * 16;		/* ld.h %rd,[%rd] */
									}
									else if((operand->flags & C33_XLDH_WR) != 0) 
									{
										g_iXload = 1;				/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
										insn = 0x3800;							/* ld.h [%r0],%rs */
									}
									if ((operand->flags & C33_XLDW_RD) != 0) 
									{
										insn = 0x3000 + ulrd + ulrd * 16;		/* ld.w %rd,[%rd] */
									}
									else if((operand->flags & C33_XLDW_WR) != 0) 
									{
										g_iXload = 1;				/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
										insn = 0x3c00;							/* ld.w [%r0],%rs */
									}
									if ((operand->flags & C33_XLDUB_RD) != 0) 
									{
										insn = 0x2400 + ulrd + ulrd * 16;		/* ld.ub %rd,[%rd] */
									}
									else if((operand->flags & C33_XLDUH_RD) != 0) 
									{
										insn = 0x2c00 + ulrd + ulrd * 16;		/* ld.uh %rd,[%r0] */
									}
									else if((operand->flags & C33_XBTST) != 0) 
									{
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
										insn = 0xa800;				/* btst [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
									}
									else if((operand->flags & C33_XBCLR) != 0) 
									{
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
										insn = 0xac00;				/* bclr [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
									}
									else if((operand->flags & C33_XBSET) != 0) 
									{
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
										insn = 0xb000;				/* bset [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
									}
									else if((operand->flags & C33_XBNOT) != 0) 
									{
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
										insn = 0xb400;				/* bnot [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
									}
									if( operand->range == 26 )
									{
										if (IS_MEM_READ)
										{
											extraInsnBuf[insn_idx_load] = insn;
											extra_data_len = insn_idx_load;
										}
										else
										{
											extraInsnBuf[insn_idx_load] = insn;
											insn = 0x0240;				/* popn %r0 */
											extraInsnBuf[insn_idx_pop] = insn;
											extra_data_len = insn_idx_pop;
										}
									}
									else
									{
										if (IS_MEM_READ)
										{
											extraInsnBuf[insn_idx_load] = insn;
											extra_data_len = insn_idx_load;
										}
										else
										{
											extraInsnBuf[insn_idx_load] = insn;
											insn = 0x0240;				/* popn %r0 */
											extraInsnBuf[insn_idx_pop] = insn;
											extra_data_len = insn_idx_pop;
										}
									}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<

                                	/* スクラッチは%r0だが、%rs = %r0のときは後で、スクラッチを%r1に変更される。 */
									/* 0:pushn %r0         			*/
	                                /* 1:ext  label+imm32@m 		*/
	                                /* 2:ext  label+imm32@m 		*/
	                                /* 3:ld.x %r0,label+imm32@l 	*/
	                                /* 4:ld.x [%r0],%rs   			*/
	                                /* 5:popn %r0					*/

                                    extraInsnBuf[0] = 0x0200;	/* pushn %r0 */ 

	                                /* 1 or 2 ext */
	                                opcode = (struct c33_opcode *)c33_ext_opcodes;
	                                extra_data_befor_insn = true;
				                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
				                    {
		                                extraInsnBuf[0] = opcode->opcode;
									}
									else
									{
		                                extraInsnBuf[1] = opcode->opcode;
		                            }
									if( operand->range == 26 )
									{
					                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
					                    {
			                                extraInsnBuf[1] = opcode->opcode;
										}
										else
										{
			                                extraInsnBuf[2] = opcode->opcode;
			                            }
									}
	                                if (fc >= MAX_INSN_FIXUPS)
	                                  as_fatal (_("too many fixups"));

									/* これは、リンク後、pushn %rs の %rs が、BFD_RELOC_C33_H に設定されてしまうので新規にBFD_RELOC_C33_PUSHN_R0
									   を追加して、リンカで "pushn %r0" を設定させるためにある。 
									   xld.x %rd,[symbol+imm] は、push/pop を行わない。 */
				                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) == 0 )
									{
		                                fixups[ fc ].exp     = ex;
		                                fixups[ fc ].opindex = * opcode->operands; 
		                                fixups[ fc ].reloc   = BFD_RELOC_C33_PUSHN_R0;
		                                ++fc;
		                            }

									if( operand->range == 26 )
									{
		                                fixups[ fc ].exp     = ex;
		                                fixups[ fc ].opindex = * opcode->operands;  /* operand of ext */
		                                fixups[ fc ].reloc   = BFD_RELOC_C33_H;
		                                ++fc;
									}
	                                fixups[ fc ].exp     = ex;
	                                fixups[ fc ].opindex = * opcode->operands;
	                                fixups[ fc ].reloc   = BFD_RELOC_C33_M;
	                                ++fc;

	                                reloc = BFD_RELOC_C33_L;

                                    extra_data_befor_insn = true;
                                    ulrd = insn & 0x000f;
									if( operand->range == 26 )
									{
					                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
					                    {
		                                    extraInsnBuf[2] = insn;		/* ld.w %rd,sign6 */
										}
										else
										{
		                                    insn &= 0xfff0;
		                                    extraInsnBuf[3] = insn;		/* ld.w %r0,sign6 */
		                                }
                                    }
                                    else
                                    {
					                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
					                    {
											/* xld.x %rd,[symbol+imm] */
		                                    extraInsnBuf[1] = insn;		/* ld.w %rd,sign6 */
										}
										else
										{
											/* xld.x [symbol+imm],%rs */
		                                    insn &= 0xfff0;
		                                    extraInsnBuf[2] = insn;		/* ld.w %r0,sign6 */
										}
	                                }
				                    if ((operand->flags & C33_XLDB_RD) != 0) 
                                    {
                                    	insn = 0x2000 + ulrd + ulrd * 16;		/* ld.b %rd,[%rd] */
                                    }
                                    else if((operand->flags & C33_XLDB_WR) != 0) 
                                    {
										g_iXload = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
	                                    insn = 0x3400;							/* ld.b [%r0],%rs */
	                                }
				                    if ((operand->flags & C33_XLDH_RD) != 0) 
                                    {
                                    	insn = 0x2800 + ulrd + ulrd * 16;		/* ld.h %rd,[%rd] */
                                    }
                                    else if((operand->flags & C33_XLDH_WR) != 0) 
                                    {
										g_iXload = 1;				/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
	                                    insn = 0x3800;							/* ld.h [%r0],%rs */
	                                }
				                    if ((operand->flags & C33_XLDW_RD) != 0) 
                                    {
                                    	insn = 0x3000 + ulrd + ulrd * 16;		/* ld.w %rd,[%rd] */
                                    }
                                    else if((operand->flags & C33_XLDW_WR) != 0) 
                                    {
										g_iXload = 1;				/* オペランド値取得用にフラグを１にする。 */
										g_iXload_range = operand->range;
	                                    insn = 0x3c00;							/* ld.w [%r0],%rs */
	                                }
				                    if ((operand->flags & C33_XLDUB_RD) != 0) 
                                    {
                                    	insn = 0x2400 + ulrd + ulrd * 16;		/* ld.ub %rd,[%rd] */
                                    }
                                    else if((operand->flags & C33_XLDUH_RD) != 0) 
                                    {
	                                    insn = 0x2c00 + ulrd + ulrd * 16;		/* ld.uh %rd,[%r0] */
	                                }
                                    else if((operand->flags & C33_XBTST) != 0) 
                                    {
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
	                                    insn = 0xa800;				/* btst [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
	                                }
                                    else if((operand->flags & C33_XBCLR) != 0) 
                                    {
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
	                                    insn = 0xac00;				/* bclr [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
	                                }
                                    else if((operand->flags & C33_XBSET) != 0) 
                                    {
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
	                                    insn = 0xb000;				/* bset [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
	                                }
                                    else if((operand->flags & C33_XBNOT) != 0) 
                                    {
										g_iBitTest = 1;		/* オペランド値取得用にフラグを１にする。 */
										g_iBitTest_range = operand->range;
	                                    insn = 0xb400;				/* bnot [%r0],imm3 */ /* ここではオペランド値 imm3 は設定しない。 */
	                                }
									if( operand->range == 26 )
									{
					                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
					                    {
		                                    extraInsnBuf[3] = insn;
		                                    extra_data_len = 3;
		                                }
		                                else
		                                {
		                                    extraInsnBuf[4] = insn;
		                                    insn = 0x0240;				/* popn %r0 */
		                                    extraInsnBuf[5] = insn;
		                                    extra_data_len = 5;
		                                }
	                                }
	                                else
	                                {
					                    if ((operand->flags & (C33_XLDB_RD + C33_XLDH_RD + C33_XLDW_RD + C33_XLDUB_RD + C33_XLDUH_RD)) != 0 )
					                    {
		                                    extraInsnBuf[2] = insn;
		                                    extra_data_len = 2;
		                                }
		                                else
		                                {
		                                    extraInsnBuf[3] = insn;
		                                    insn = 0x0240;				/* popn %r0 */
		                                    extraInsnBuf[4] = insn;
		                                    extra_data_len = 4;
		                                }
									}


								/* add T.Tazaki 2004/08/19 <<< */
#endif	/* EXT_REMOVE */
                                }
                                else {
                                    errmsg = _("invalid operand");
                                }
                            }
                            else {
                                errmsg = _("invalid operand");
                            }
                        }
                        else {
                        /* NO Register */
                            /* Are there any symbol and IMM which follow a register? */
                            
                            /* Pattern */
                            /* [rb+imm32] */
                            
                            /* Skip Space */
                            while (isspace (*input_line_pointer))
                                ++input_line_pointer;
                            
                            /* IF ']' */
                            if (*input_line_pointer == ']'){
                            /* YES Register Only */
    #if 0
                                errmsg = _("invalid operand");
    #else
                                input_line_pointer++;
    #endif
                            }
                            /* ELSE IF plus */
                            else if (*input_line_pointer == '+'){
                            /* YES plus */
                            
                                input_line_pointer++;
                                /* symbol,imm */
                                expression (& ext_ex);

                                if (ext_ex.X_op == O_constant){
                                    /* [%rd+imm26] */
                                
                                    uiNumber = ext_ex.X_add_number;
                                    
                                    if (uiNumber == 0){
                                        /* EMPTY */
                                    }
                                    else if (uiNumber <= 0x1fff){
                                        opcode = (struct c33_opcode *)c33_ext_opcodes;
                                        extra_data_befor_insn = true;
                                        extra_data_len  = 1;
                                        extraInsnBuf[0] = opcode->opcode| (uiNumber & 0x1fff);
                                    }
                                    else if (uiNumber <= 0x3ffffff){
                                        opcode = (struct c33_opcode *)c33_ext_opcodes;
                                        extra_data_befor_insn = true;
                                        extra_data_len  = 2;
                                        extraInsnBuf[0] = opcode->opcode| ((uiNumber >> 13) & 0x1fff);
                                        extraInsnBuf[1] = opcode->opcode| (uiNumber & 0x1fff);
                                    }
                                    else {
                                    /* NO more than 27bit ? */
                                    
                                        /* tnot support */
                                        errmsg = _("invalid operand");
                                        iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
                                    }
                                }
                                else {
                                /* NO not immidiate */
                                    errmsg = _("invalid operand");
                                    iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
                                }
                            }
                            else {
                            /* NO any other character  */
                                /* error */
                                errmsg = _("invalid operand");
                                iMEM_IMM26_flag = 1;    /* add tazaki 2001.10.11 */
                            }
                        }
                    }
                    else
                        errmsg = _("invalid operand");
                }
                /* an operand -- the register with a post increment -- or [ being indirect ] */
                else if ((operand->flags & C33_OPERAND_REGINC) != 0) 
                {
                    if (*str == '['){
                        str++;
                        input_line_pointer = str;

                        if (!register_name (& ex))
                        {
                            errmsg = _("invalid register name");
                        }
                        else {
                            str = input_line_pointer;

                            /* Skip space */
                            while (isspace (*str))
                                ++str;

                            if (*str == ']'){
                                ++str;

                                while (isspace (*str))
                                    ++str;
                                
                                if (*str == '+'){
                                    /* normal end */
                                    str++;
                                    input_line_pointer = str;
                                    
                                }
                                else {
                                    errmsg = _("invalid operand");
                                }
                            }
                            else {
                                errmsg = _("invalid operand");
                            }
                        }
                    }
                    else
                        errmsg = _("invalid operand");
                }
                
                
                /* Register indirectness with the De Dis placement */
                else if ((operand->flags & C33_OPERAND_SPMEM) != 0) 
                {
                    if (*str == '['){
                        str++;
                        /* Skip space */
                        while (isspace (*str))
                            ++str;

                        input_line_pointer = str;

                        /* check %sp */
                        if ( ( strncmp(str,"%sp",3) != 0 ) && ( strncmp(str,"%SP",3) != 0 ))
                        {
                            /* YES  */
                            errmsg = _("invalid system register name");
                        }
                        else {
                            str+=3;
                            while (isspace (*str))
                                ++str;

                            /* IF ']' */
                            if (*str == ']'){
                            /* YES only register */
                                /* Support "[%sp]"  2001.3.29 ide */
                                ex.X_op         = O_constant;
                                ex.X_add_symbol = NULL;
                                ex.X_op_symbol  = NULL;
                                ex.X_add_number = 0;
                                str++;
                                input_line_pointer = str;
                            }
                            else if (*str == '+'){
                            /* [sp+imm] */
                                str++;
                                input_line_pointer = str;

                                expression (& ex);

                                iNumber = ex.X_add_number;
                                    
                                if (operand->range <= 6){
                                    /* EMPTY */
                                }
                                else if (operand->range == 32) {
                                    /* update tazaki 2002.03.08 >>> */
                                    if (opcode->specialFlag == 1){
                                        /* ld.b */
                                        if ((unsigned int)iNumber <= 0x3f){
                                            ex.X_add_number /= 1;
                                            
                                        }else if ((unsigned int)iNumber <= 0x7ffff){
                                            /* 1 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                        else {
                                            /* 2 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 2;
                                            extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                            extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                    }
                                    else if (opcode->specialFlag == 2){
                                        /* ld.h */
                                        if ((unsigned int)iNumber <= 0x7f){
                                            ex.X_add_number /= 2;
                                             
                                        }else if ((unsigned int)iNumber <= 0x7ffff){
                                            /* 1 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                        else {
                                            /* 2 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 2;
                                            extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                            extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                    }
                                    else if (opcode->specialFlag == 4){
                                        /* ld.w */
                                        if ((unsigned int)iNumber <= 0xff){
                                            ex.X_add_number /= 4;
                                            
                                        }else if ((unsigned int)iNumber <= 0x7ffff){
                                            /* 1 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 1;
                                            extraInsnBuf[0] = opcode->opcode| ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                        else {
                                            /* 2 ext */
                                            opcode = (struct c33_opcode *)c33_ext_opcodes;
                                            extra_data_befor_insn = true;
                                            extra_data_len  = 2;
                                            extraInsnBuf[0] = opcode->opcode | ((iNumber >> 19) & 0x1fff);
                                            extraInsnBuf[1] = opcode->opcode | ((iNumber >> 6) & 0x1fff);
                                            ex.X_add_number = iNumber & 0x3f;
                                        }
                                    }
                                    /* update tazaki 2002.03.08 <<< */
                                }
                                else {
                                    errmsg = _("constant too big to fit into instruction");
                                }
                            }
                            else{
                                errmsg = _("invalid operand");
                            }
                        }
                    }
                    else
                        errmsg = _("invalid operand");
                }
                
                if (errmsg)
                    goto error;

                switch (ex.X_op) 
                {
                    case O_cond:
                        /* An operand and an operation code are made into a command code. */
                        insn = c33_insert_operand (insn, operand, ex.X_add_number,
                                      (char *) NULL, 0,
                                      copy_of_instruction,flags);
                        break;
                    case O_op_shift:
                        /* An operand and an operation code are made into a command code. */
                        insn = c33_insert_operand (insn, operand, ex.X_add_number,
                                      (char *) NULL, 0,
                                      copy_of_instruction,flags);
                        break;
                    case O_spregister :
                        /* Don't add %sp to a formula as an operand. */
                        break;
                    case O_dpregister :
                        /* Don't add %sp to a formula as an operand. */
                        break;
                    case O_illegal:
                        errmsg = _("illegal operand");
                        goto error;
                    case O_absent:
                        errmsg = _("missing operand");
                        goto error;
                    case O_register:
                        /* An operand and an operation code are made into a command code. */
// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
						if (g_iXload == 1)		/* xld.x [symbol+imm],%rs ? */
						{
							if( g_iXload_range == 26 )	/* xld.x */
							{
								if( ex.X_add_number == 0 )	/* xld.x [symbol+imm],%rs  : %rs = %r0 ? */
								{

									/* change %r0 --> %r1 */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[0] = 0x0201;								/* pushn %r1 */
										fixups[ 0 ].reloc   = BFD_RELOC_C33_PUSHN_R1;			/* pushn %r1 */
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[0] = 0x0011;								/* push %r1 */
										fixups[ 0 ].reloc   = BFD_RELOC_C33_PUSH_R1;			/* push %r1 */
									}
									extraInsnBuf[insn_idx_low] = 0x6c01;						/* ld.w %r1,symbol+imm */
									extraInsnBuf[insn_idx_load] = (extraInsnBuf[insn_idx_load] & 0xff0f) | 0x0010;	/* ld.x [%r1],%rs	   */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[insn_idx_pop] = 0x0241;					/* popn  %r1		   */
										insn = 0x0241;
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[insn_idx_pop] = 0x0051;					/* pop	 %r1		   */
										insn = 0x0051;
									}
								}
								extraInsnBuf[insn_idx_load] = c33_insert_operand (extraInsnBuf[insn_idx_load], operand, ex.X_add_number,
											  (char *) NULL, 0,
											  copy_of_instruction,flags);
							}
							else
							{
								/* g_iXload_range = 19 : ald.x */
								if( ex.X_add_number == 0 )	/* ald.x [symbol+imm],%rs  : %rs = %r0 ? */
								{

									/* change %r0 --> %r1 */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[0] = 0x0201;								/* pushn %r1 */
										fixups[ 0 ].reloc   = BFD_RELOC_C33_PUSHN_R1;			/* pushn %r1 */
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[0] = 0x0011;								/* push %r1 */
										fixups[ 0 ].reloc   = BFD_RELOC_C33_PUSH_R1;			/* push %r1 */
									}
									extraInsnBuf[insn_idx_low] = 0x6c01;						/* ld.w %r1,symbol+imm */
									extraInsnBuf[insn_idx_load] = (extraInsnBuf[insn_idx_load] & 0xff0f) | 0x0010;	/* ld.x [%r1],%rs	   */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[insn_idx_pop] = 0x0241;					/* popn  %r1		   */
										insn = 0x0241;
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[insn_idx_pop] = 0x0051;					/* pop	 %r1		   */
										insn = 0x0051;
									}
								}
								extraInsnBuf[insn_idx_load] = c33_insert_operand (extraInsnBuf[insn_idx_load], operand, ex.X_add_number,
											  (char *) NULL, 0,
											  copy_of_instruction,flags);
							}
							g_iXload = 0;
							g_iXload_range = 0;
						}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<
                        /* add T.Tazaki 2004/08/19 >>> */
				        if (g_iXload == 1)		/* xld.x [symbol+imm],%rs ? */
				        {
							if( g_iXload_range == 26 )	/* xld.x */
							{
								if( ex.X_add_number == 0 )	/* xld.x [symbol+imm],%rs  : %rs = %r0 ? */
								{
									/* change %r0 --> %r1 */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[0] = 0x0201;								/* pushn %r1 */
		                                fixups[ fc-4 ].reloc   = BFD_RELOC_C33_PUSHN_R1;		/* pushn %r1 */
		                            }
		                            else
		                            {
										/* ADV or PE */
										extraInsnBuf[0] = 0x0011;								/* push %r1 */
		                                fixups[ fc-4 ].reloc   = BFD_RELOC_C33_PUSH_R1;			/* push %r1 */
									}
									extraInsnBuf[3] = 0x6c01;								/* ld.w %r1,symbol+imm */
									extraInsnBuf[4] = (extraInsnBuf[4] & 0xff0f) | 0x0010;	/* ld.x [%r1],%rs      */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[5] = 0x0241;								/* popn  %r1           */
										insn = 0x0241;
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[5] = 0x0051;								/* pop   %r1           */
										insn = 0x0051;
									}
								}
		                        extraInsnBuf[4] = c33_insert_operand (extraInsnBuf[4], operand, ex.X_add_number,
		                                      (char *) NULL, 0,
		                                      copy_of_instruction,flags);
							}
							else
							{
								/* g_iXload_range = 19 : ald.x */
								if( ex.X_add_number == 0 )	/* ald.x [symbol+imm],%rs  : %rs = %r0 ? */
								{

									/* change %r0 --> %r1 */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[0] = 0x0201;								/* pushn %r1 */
		                                fixups[ fc-3 ].reloc   = BFD_RELOC_C33_PUSHN_R1;		/* pushn %r1 */
		                            }
		                            else
		                            {
										/* ADV or PE */
										extraInsnBuf[0] = 0x0011;								/* push %r1 */
		                                fixups[ fc-3 ].reloc   = BFD_RELOC_C33_PUSH_R1;			/* push %r1 */
									}
									extraInsnBuf[2] = 0x6c01;								/* ld.w %r1,symbol+imm */
									extraInsnBuf[3] = (extraInsnBuf[3] & 0xff0f) | 0x0010;	/* ld.x [%r1],%rs      */
									if( g_iAdvance == 0 && g_iPE == 0 )	/* STD ? */
									{
										extraInsnBuf[4] = 0x0241;								/* popn  %r1           */
										insn = 0x0241;
									}
									else
									{
										/* ADV or PE */
										extraInsnBuf[4] = 0x0051;								/* pop   %r1           */
										insn = 0x0051;
									}
								}
		                        extraInsnBuf[3] = c33_insert_operand (extraInsnBuf[3], operand, ex.X_add_number,
		                                      (char *) NULL, 0,
		                                      copy_of_instruction,flags);
							}
							g_iXload = 0;
							g_iXload_range = 0;
						}
                        /* add T.Tazaki 2004/08/19 <<< */
#endif	/* EXT_REMOVE */
						else
						{
	                        insn = c33_insert_operand (insn, operand, ex.X_add_number,
                                      (char *) NULL, 0,
                                      copy_of_instruction,flags);
						}
                        break;
                        
                    case O_constant:
                        /* An operand and an operation code are made into a command code. */
// ADD D.Fujimoto 2007/06/25 >>>>>>>
#ifdef EXT_REMOVE
						if (g_iBitTest == 1)		/* bit test inst ? */
						{
							if( g_iBitTest_range == 26 )
							{
								extraInsnBuf[insn_idx_load] = c33_insert_operand (extraInsnBuf[insn_idx_load], operand, ex.X_add_number,
											  (char *) NULL, 0,
											  copy_of_instruction,flags);
							}
							else
							{
								extraInsnBuf[insn_idx_load] = c33_insert_operand (extraInsnBuf[insn_idx_load], operand, ex.X_add_number,
											  (char *) NULL, 0,
											  copy_of_instruction,flags);
							}
							g_iBitTest = 0;
							g_iBitTest_range = 0;
						}
#else
// ADD D.Fujimoto 2007/06/25 <<<<<<<
                        /* add T.Tazaki 2004/07/30 >>> */
				        if (g_iBitTest == 1)		/* bit test inst ? */
				        {
							if( g_iBitTest_range == 26 )
							{

		                        extraInsnBuf[4] = c33_insert_operand (extraInsnBuf[4], operand, ex.X_add_number,
		                                      (char *) NULL, 0,
		                                      copy_of_instruction,flags);
							}
							else
							{
		                        extraInsnBuf[3] = c33_insert_operand (extraInsnBuf[3], operand, ex.X_add_number,
		                                      (char *) NULL, 0,
		                                      copy_of_instruction,flags);
							}
							g_iBitTest = 0;
							g_iBitTest_range = 0;
						}
                        /* add T.Tazaki 2004/07/30 <<< */
#endif	/* EXT_REMOVE */
						else
						{
	                        insn = c33_insert_operand (insn, operand, ex.X_add_number,
	                                      (char *) NULL, 0,
	                                      copy_of_instruction,flags);
						}
                        break;

                    case O_symbol:
                        /* We need to generate a fixup for this expression.  */
                        if (fc >= MAX_INSN_FIXUPS)
                          as_fatal (_("too many fixups"));

                        fixups[ fc ].exp     = ex;
                        fixups[ fc ].opindex = * opindex_ptr;
                        fixups[ fc ].reloc   = reloc;
                        ++fc;
                        break;

                    default:
                        /* We need to generate a fixup for this expression.  */
                        if (fc >= MAX_INSN_FIXUPS)
                          as_fatal (_("too many fixups"));

                        fixups[ fc ].exp     = ex;
                        fixups[ fc ].opindex = * opindex_ptr;
                        fixups[ fc ].reloc   = BFD_RELOC_UNUSED;
                        ++fc;
                        break;
                }
            }
            str = input_line_pointer;
            input_line_pointer = hold;

            while (*str == ' ' || *str == ',' || *str == ']' )
                ++str;
        }
        match = 1;

        error:
        if (match == 0)
        {

            /* xld.w rd,[rs+imm26] : imm26 > 0x3ffffff ?    add tazaki 2001.10.11 */
            if( iMEM_IMM26_flag == 0 ){ /* The following operand form is not seen at the time of range over.。*/
                next_opcode = opcode + 1;
                if (next_opcode->name != NULL
                    && strcmp (next_opcode->name, opcode->name) == 0)
                {
                    opcode = next_opcode;
                    continue;   
                }
            }
            as_bad (_("%s: %s"), copy_of_instruction, errmsg);
/*          as_bad ("%s: %s", copy_of_instruction, errmsg); Modify tazaki 2001.10.11 */

            if (* input_line_pointer == ']')
                ++ input_line_pointer;

            ignore_rest_of_line ();
            input_line_pointer = saved_input_line_pointer;
            return;
        }
        break;
    }
      
    while (isspace (*str))
      ++str;

    if (*str != '\0')
    /* xgettext:c-format */
        as_bad (_("junk at end of line: `%s'"), str);

    input_line_pointer = str;

    /* Write out the instruction. */

    if (relaxable && fc > 0)
    {

        fc = 0;

        if (!strcmp (opcode->name, "br"))
        {
            f = frag_var (rs_machine_dependent, 4, 2, 2,
            fixups[0].exp.X_add_symbol,
            fixups[0].exp.X_add_number,
            (char *)fixups[0].opindex);
            md_number_to_chars (f, insn, 2);
            md_number_to_chars (f + 2, 0, 2);
        }
        else
        {
            f = frag_var (rs_machine_dependent, 6, 4, 0,
                fixups[0].exp.X_add_symbol,
                fixups[0].exp.X_add_number,
                (char *)fixups[0].opindex);
            md_number_to_chars (f, insn, 2);
            md_number_to_chars (f + 2, 0, 4);
        }
    }
    else 
    {
        /* ext command is set before a command code formula.   . */
        if (extra_data_befor_insn)
        {
            /* Domain reservation */
            f = where = frag_more (extra_data_len*2+2); /* 2byte length instruction */

            /* The command code for extension (ext command) is acquired. */
            fromP = &extraInsnBuf[0];

            /* Only a part for the command code for FOR extensio */
            for (i = extra_data_len; i; --i)
            {
                /* The command code is stored in the buffer in order. */
                md_number_to_chars (where, (long) (*fromP), 2);
                where += 2;
                fromP++;
            }

// ADD D.Fujimoto 2007/06/25 calculating inst offset for ext remove >>>>>>>
#ifdef EXT_REMOVE
			ul_All_Offset += extra_data_len;
#endif	/* EXT_REMOVE */
// ADD D.Fujimoto 2007/06/25 calculating inst offset for ext remove <<<<<<<


            extra_data_befor_insn = false;
        }
        else {
            /* Domain reservation */
            f = where = frag_more (2);  /* 2byte length instruction */
        }

        /* >>> add tazaki 2001.09.13 */
        /* macclr,ld.cf ? */
        if (opcode->specialFlag == 10){ 
            insn |= 0x0010; /* bit 5,4 = 0,1 set */
        }
        /* <<< add tazaki 2001.09.13 */

        /* The command code of 2 byte length is stored in a buffer. */
        md_number_to_chars (where, insn, 2);
    }

    /* Create any fixups.  At this point we do not use a
         bfd_reloc_code_real_type, but instead just use the
         BFD_RELOC_UNUSED plus the operand index.  This lets us easily
         handle fixups for any operand type, although that is admittedly
         not a very exciting feature.  We pick a BFD reloc type in
         md_apply_fix.  */  
    for (i = 0; i < fc; i++)
    {
        const struct c33_operand * operand;
        bfd_reloc_code_real_type    reloc;

        operand = & c33_operands[ fixups[i].opindex ];

        reloc = fixups[i].reloc;

        if (reloc != BFD_RELOC_UNUSED)
        {
            reloc_howto_type * reloc_howto = bfd_reloc_type_lookup (stdoutput,
                                      reloc);
            int                size;
            int                address;
            fixS *             fixP;

            if (!reloc_howto){
                    ;
            /*  abort();    */  /* del tazaki 2001.10.11 */
            }else{

                size = bfd_get_reloc_size (reloc_howto);
            
                if (size != 2) {
                    ;
                /*  abort ();   */  /* del tazaki 2001.10.11 */
                }else{
    
                    address = (f - frag_now->fr_literal) + 2 - size;
        
                    f += 2;
                    
                    fixP = fix_new_exp (frag_now, address, size,
                              & fixups[i].exp, 
                              reloc_howto->pc_relative,
                              reloc);
        
                    switch (reloc)
                    {
                        case BFD_RELOC_C33_RH:
                        case BFD_RELOC_C33_RM:
                        case BFD_RELOC_C33_RL:
                        case BFD_RELOC_C33_S_RH:    /* add T.Tazaki 2002.05.02 */
                        case BFD_RELOC_C33_S_RM:    /* add T.Tazaki 2002.05.02 */
                        case BFD_RELOC_C33_S_RL:    /* add T.Tazaki 2002.05.02 */
                        case BFD_RELOC_C33_JP:      /* add T.Tazaki 2002.04.22 */
                        case BFD_RELOC_C33_AH:
                        case BFD_RELOC_C33_AL:
                        case BFD_RELOC_C33_H:
                        case BFD_RELOC_C33_M:
                        case BFD_RELOC_C33_L:
                        /* >>>> add 2002.03.05 tazaki */
                        case BFD_RELOC_C33_DH:
                        case BFD_RELOC_C33_DL:
                        case BFD_RELOC_C33_GL:
                        case BFD_RELOC_C33_SH:
                        case BFD_RELOC_C33_SL:
                        case BFD_RELOC_C33_TH:
                        case BFD_RELOC_C33_TL:
                        case BFD_RELOC_C33_ZH:
                        case BFD_RELOC_C33_ZL:
                        case BFD_RELOC_C33_DPH:
                        case BFD_RELOC_C33_DPM:
                        case BFD_RELOC_C33_DPL:
                        case BFD_RELOC_C33_LOOP:
                        /* <<<< add 2002.03.05 tazaki */
                        case BFD_RELOC_C33_PUSHN_R0:	/*  add T.Tazaki 2004/08/19 */
                        case BFD_RELOC_C33_PUSHN_R1:	/*  add T.Tazaki 2004/08/19 */
                        case BFD_RELOC_C33_PUSH_R1:		/*  add T.Tazaki 2004/08/19 */
                            fixP->fx_no_overflow = 1;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        else
        {
            fix_new_exp (
                   frag_now,
                   f - frag_now->fr_literal, 4,
                   & fixups[i].exp,
                   1 /* FIXME: C33_OPERAND_RELATIVE ??? */,
                   (bfd_reloc_code_real_type) (fixups[i].opindex
                               + (int) BFD_RELOC_UNUSED)
                   );
        }
    }

    input_line_pointer = saved_input_line_pointer;
}


/* If while processing a fixup, a reloc really needs to be created */
/* then it is done here.  */
                 
arelent *
tc_gen_reloc (seg, fixp)
     asection * seg;
     fixS *     fixp;
{
  arelent * reloc;
  
  reloc              = (arelent *) xmalloc (sizeof (arelent));
  reloc->sym_ptr_ptr = (asymbol **) xmalloc (sizeof (asymbol *));
  *reloc->sym_ptr_ptr= symbol_get_bfdsym (fixp->fx_addsy);
  reloc->address     = fixp->fx_frag->fr_address + fixp->fx_where;
  reloc->howto       = bfd_reloc_type_lookup (stdoutput, fixp->fx_r_type);

  if (reloc->howto == (reloc_howto_type *) NULL)
    {
      as_bad_where (fixp->fx_file, fixp->fx_line,
            /* xgettext:c-format */
                    _("reloc %d not supported by object file format"),
            (int) fixp->fx_r_type);

      xfree (reloc);
      
      return NULL;
    }
  
    reloc->addend = fixp->fx_addnumber;
  
  return reloc;
}

/* Assume everything will fit in two bytes, then expand as necessary.  */
int
md_estimate_size_before_relax (fragp, seg)
     fragS * fragp;
     asection * seg;
{
  if (fragp->fr_subtype == 0)
    fragp->fr_var = 4;
  else if (fragp->fr_subtype == 2)
    fragp->fr_var = 2;
  else
    abort ();
  return 2;
} 

long
c33_pcrel_from_section (fixp, section)
     fixS * fixp;
     segT   section;
{
  /* If the symbol is undefined, or in a section other than our own,
     or it is weak (in which case it may well be in another section,
     then let the linker figure it out.  */
  if (fixp->fx_addsy != (symbolS *) NULL
      && (! S_IS_DEFINED (fixp->fx_addsy)
      || S_IS_WEAK (fixp->fx_addsy)
      || (S_GET_SEGMENT (fixp->fx_addsy) != section)))
    return 0;
  
  return fixp->fx_frag->fr_address + fixp->fx_where;
}

/*
The symbol which can be decided inside a file is decided here.

The tc_gen_reloc function which is in bfd library further is passed,
 and, finally the symbol changed by referring to the exterior, the link,
  and relocation is on HOWTO broad view of bfd.
  It is processed by the defined method. 
*/

/* add T.Tazaki 2002.04.25 >>> */
long    g_where_rh = 0xffffffff;
long    g_where_rm = 0xffffffff;

char    *g_pwhere_rh = 0;
char    *g_pwhere_rm = 0;

/* add T.Tazaki 2002.04.25 <<< */

int
md_apply_fix3 (fixp, valuep, seg)
     fixS *   fixp;
     valueT * valuep;
     segT     seg;
{
  valueT value;
  char * where;
    long    insn;
    int     iNumber;    /* add T.Tazaki 2002.04.25 */
    long    lNumber;    /* add T.Tazaki 2002.04.25 */
    
    if (fixp->fx_addsy == (symbolS *) NULL)
    {
        value = * valuep;
        fixp->fx_done = 1;
    }
    else if (fixp->fx_pcrel)
        value = * valuep;
    else
    {
        value = fixp->fx_offset;
        if (fixp->fx_subsy != (symbolS *) NULL)
        {
            if (S_GET_SEGMENT (fixp->fx_subsy) == absolute_section)
                value -= S_GET_VALUE (fixp->fx_subsy);
            else
            {
                 /* We don't actually support subtracting a symbol.  */
                 as_bad_where (fixp->fx_file, fixp->fx_line,
                    _("expression too complex"));
            }
        }
    }

    if ((int) fixp->fx_r_type >= (int) BFD_RELOC_UNUSED)
    {
      int                         opindex;
      const struct c33_operand * operand;
      unsigned long               insn;

      opindex = (int) fixp->fx_r_type - (int) BFD_RELOC_UNUSED;
      operand = & c33_operands[ opindex ];

      /* Fetch the instruction, insert the fully resolved operand
         value, and stuff the instruction back again.

     Note the instruction has been stored in little endian
     format!  */
      where = fixp->fx_frag->fr_literal + fixp->fx_where;

      insn = bfd_getl16 ((unsigned char *) where);
      insn = c33_insert_operand (insn, operand, (offsetT) value,
                  fixp->fx_file, fixp->fx_line, NULL);
      bfd_putl16 ((bfd_vma) insn, (unsigned char *) where);

        if (fixp->fx_done)
        {
          /* Nothing else to do here. */
          return 1;
        }
    }
    else if (fixp->fx_done)
    {
        /* We still have to insert the value into memory!  */
        where = fixp->fx_frag->fr_literal + fixp->fx_where;

        if (fixp->fx_size == 1)
            * where = value & 0xff;
        else if (fixp->fx_size == 2){

            /* An address when a symbol is decided is buried here
             and crowded with the inside of a file. */

            /* A command code is acquired. */
            insn = bfd_getl16 ((unsigned char *) where);

            switch (fixp->fx_r_type)
            {
            case BFD_RELOC_C33_AH:  /* @ah (25:13) */   /* NO USE : Absolute symbol */
                
                insn += ((value >> 13) & 0x1fff);
                break;
                
                case BFD_RELOC_C33_AL:  /* @ah (12:0) */    /* NO USE : Absolute symbol */
                insn += (value & 0x1fff);
                break;

            case BFD_RELOC_C33_RH:  /* LABEL-PC(31:22) */
//              if( g_listing == 0 ){       /* No -a option ? */
//                  g_where_rh = fixp->fx_where;
//              }
//              else{
//                  g_pwhere_rh = where;
//              }
//
                insn += (((value - 4) >> 19) & 0x1ff8);
                break;
                
            case BFD_RELOC_C33_RM:  /* LABEL-PC(21:9)  */

//              if( g_listing == 0 ){       /* No -a option ? */
//
//                  g_where_rm = fixp->fx_where;
//                  if( g_where_rh != ( fixp->fx_where - 2 ) ){ /* add T.Tazaki 2002.04.25 */
//                      
//                      lNumber = value;
//                      /* over signed 22bit ? */
//                          if ((lNumber - 2) > 0x1ffffe || (lNumber - 2 ) < -0x200000 ) 
//                          as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
//                          /* as_bad_where = ERROR */
//                  }
//                  g_where_rh = 0xffffffff;
//              }
//              else
//              {
//                  g_pwhere_rm = where;
//                  if( g_pwhere_rh != ( where - 100 ) ){   /* add T.Tazaki 2002.05.02 */
//                      
//                      lNumber = value;
//                      /* over signed 22bit ? */
//                          if ((lNumber - 2) > 0x1ffffe || (lNumber - 2 ) < -0x200000 ) 
//                          as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
//                          /* as_bad_where = ERROR */
//                  }
//                  g_pwhere_rh = 0;
//              }
                insn += (((value - 2) >> 9) & 0x1fff);
                break;

            case BFD_RELOC_C33_RL:  /* LABEL-PC(8:0)   */

// -al オプション付きのとき、where値が必ずしも@rm のＰＣ+100とは限らないため、不採用。
// -al無し( fixp->fx_where )ではＯＫだが、-alの有無でワーニング表示が異なるのは変なので両方削除した。

//              if( g_listing == 0 ){       /* No -a option ? */
//
//                  if( g_where_rm != ( fixp->fx_where - 2 ) ){ /* add T.Tazaki 2002.04.25 */
//
//                      lNumber = value;
//                      /* over signed 8bit ? */
//                          if (lNumber > 254 || lNumber < -256)
//                          as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
//                          /* as_bad_where = ERROR */
//                  }
//                  g_where_rh = 0xffffffff;
//                  g_where_rm = 0xffffffff;
//              }
//              else
//              {
//
//                  if( g_pwhere_rm != ( where - 100 ) ){   /* add T.Tazaki 2002.05.02 */
//
//                      lNumber = value;
//                      /* over signed 8bit ? */
//                          if (lNumber > 254 || lNumber < -256)
//                          as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
//                          /* as_bad_where = ERROR */
//                  }
//                  g_pwhere_rh = 0;
//                  g_pwhere_rm = 0;
//              }
                
                insn += ((value >> 1) & 0xff);
                break;

/* add T.Tazaki 2002.05.02 >>> */

            case BFD_RELOC_C33_S_RH:    /* LABEL-PC(31:22) */   /* sjp,scall, xjp,xcall */
                g_where_rh = fixp->fx_where;

                insn += (((value - 4) >> 19) & 0x1ff8);
                break;
                
            case BFD_RELOC_C33_S_RM:    /* LABEL-PC(21:9)  */   /* sjp,scall, xjp,xcall */

                g_where_rm = fixp->fx_where;
                if( g_where_rh != ( fixp->fx_where - 2 ) ){
                    
                    lNumber = value;
                    /* over signed 22bit ? */
                        if ((lNumber - 2) > 0x1ffffe || (lNumber - 2 ) < -0x200000 ) 
                        as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
                        /* as_bad_where = ERROR */
                }
                g_where_rh = 0xffffffff;
                insn += (((value - 2) >> 9) & 0x1fff);
                break;

            case BFD_RELOC_C33_S_RL:    /* LABEL-PC(8:0)   */   /* sjp,scall, xjp,xcall */

                if( g_where_rm != ( fixp->fx_where - 2 ) ){

                    lNumber = value;
                    /* over signed 8bit ? */
                        if (lNumber > 254 || lNumber < -256)
                        as_warn_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));
                        /* as_bad_where = ERROR */
                }
                g_where_rh = 0xffffffff;
                g_where_rm = 0xffffffff;
                
                insn += ((value >> 1) & 0xff);
                break;

/* add T.Tazaki 2002.05.02 <<< */
                    
            case BFD_RELOC_C33_JP:  /* LABEL-PC(8:0)   */   /* add T.Tazaki 2002.04.22 */
                /* jp label */

                /* over signed 8bit ? */
                iNumber = value;

                if (iNumber > 254 || iNumber < -256)
                     as_bad_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));

                insn += ((value >> 1) & 0xff);
                break;
                    
            case BFD_RELOC_C33_H:   /* LABEL(31:19) */  /* NO USE : Absolute symbol */
                insn += ((value >> 19) & 0x1fff);
                break;
            
            case BFD_RELOC_C33_M:   /* LABEL(18:6)  */  /* NO USE : Absolute symbol */
                insn += ((value >> 6) & 0x1fff);
                break;
            
            case BFD_RELOC_C33_L:   /* LABEL(5:0)   */  /* NO USE : Absolute symbol */
                            /* ld.w rd,LABEL@l */
                insn += (value & 0x3f) << 4;
                break;

            case BFD_RELOC_C33_LOOP:    /* LABEL-PC(4:0)   */

                /* over imm 5bit ? */
                iNumber = value;

                if (iNumber > 30 || iNumber < 0)
                     as_bad_where (fixp->fx_file, fixp->fx_line, _("operand out of range"));

				/* "   loop %rc,Label-2 " 表現は使用し辛いので、 "loop %rc,Label" を可能とするため-１する。 add T.Tazaki 2004/09/22 >>> */
				/* "   nop              " */
				/* "   nop              " */
				/* "Label:              " */
				
				--value;

				/* add T.Tazaki 2004/09/22 <<< */

                /* imm5=imm(4:1) imm5(0)=0 */
                insn += (value & 0x1e) << 3;
                break;
                    
            default:
                break;
            }

            bfd_putl16 ((bfd_vma) insn, (unsigned char *) where);
        }
        else if (fixp->fx_size == 4)
            bfd_putl32 (value, (unsigned char *) where);
    }

    fixp->fx_addnumber = value;
    
    return 1;
}


/* Parse a cons expression.  */
void
parse_cons_expression_c33 (exp)
  expressionS * exp;
{
  /* See if there's a reloc prefix like hi() we have to handle.  */
  hold_cons_reloc = c33_reloc_prefix ();

  /* Do normal expression parsing.  */
  expression (exp);
}

/* Create a fixup for a cons expression.  If parse_cons_expression_c33
   found a reloc prefix, then we use that reloc, else we choose an
   appropriate one based on the size of the expression.  */
void
cons_fix_new_c33 (frag, where, size, exp)
     fragS * frag;
     int where;
     int size;
     expressionS *exp;
{
  if (hold_cons_reloc == BFD_RELOC_UNUSED)
    {
      if (size == 4)
    hold_cons_reloc = BFD_RELOC_32;
      if (size == 2)
    hold_cons_reloc = BFD_RELOC_16;
      if (size == 1)
    hold_cons_reloc = BFD_RELOC_8;
    }

  if (exp != NULL)
    fix_new_exp (frag, where, size, exp, 0, hold_cons_reloc);
  else
    fix_new (frag, where, size, NULL, 0, 0, hold_cons_reloc);
}

boolean
c33_fix_adjustable (fixP)
    fixS * fixP;
{
  if (fixP->fx_addsy == NULL)
    return 1;
 
  /* Prevent all adjustments to global symbols. */
  if (S_IS_EXTERN (fixP->fx_addsy))
    return 0;
  
  if (S_IS_WEAK (fixP->fx_addsy))
    return 0;
  
  /* Don't adjust function names */
  if (S_IS_FUNCTION (fixP->fx_addsy))
    return 0;

  return 1;
}
 
int
c33_force_relocation (fixP)
      struct fix * fixP;
{
  if (fixP->fx_addsy && S_IS_WEAK (fixP->fx_addsy))
    return 1;
  
  return 0;
}
