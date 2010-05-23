/* ext_remove.h - header file for 2pass assemble.
	This program will remove redundant ext 0x0 instructions
	which emerge from memory load from LABEL/SYMBOLS or
	from function calls to LABEL/SYMBOLS
	Copyright (C) 2007 SEIKO EPSON CORP.

	Written by D.Fujimoto@Irumasoft
	DATE:2007/02/28

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
	02111-1307, USA. 
*/
#ifndef	__EXT_REMOVE_H__
#define	__EXT_REMOVE_H__

//////////////////////////////////////////
// Macro definitions
#define	EXT_REMOVE							// the ext remove feature is alive where this macro appears
											// in other source files

#define INPUT_CUR_LINE_MAX (0x800 + 1)		// the maximum number of characters per one line of a current file

#define	DATA_POINTER_SYMBOL		"__dp"		// symbol representing data pointer

#define MAX_EXT_INSN_CNT	(-1)			// default return value for evaluate_ext_count() and related functions

//////////////////////////////////////////
// Structure declarations
typedef struct cur_inf
{
	unsigned char* ucp_Symbol_Name;			// pointer for symbol name
	int i_Attribute;						// 0 -- local
											// 1 -- global
} cur_inf;

struct dump_inf
{
	unsigned char* ucp_Symbol_Name;			// pointer for symbol name ( cuurent local symbol & global symbol )
	unsigned long ul_Symbol_Addr;			// address where symbol name is placed
	unsigned char* ucp_Area_Name;			// pointer for area name in which symbol name belongs
	int i_Attribute;						// 0 -- local
											// 1 -- global
	int iCount;								// symbol occurence  ADD D.Fujimoto 2007/12/27
											// symbols that are found more than 1 should not be used for ext optimization
};


//////////////////////////////////////////
// Enum declarations
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
enum CheckStatus {format, filename, local_global, global};				// used in read_all_dump_info()
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<

////////////////////////////////////////////
// Global variables

extern int g_c33_ext;									// flag indicating that -mc33ext option is specified
														// 0=not specified, 1=-mc33ext specified

extern int i_File_Inf_Flg	;							// 0 -- initial vaule
														// 1 -- ".file" exists.

extern unsigned long g_dpAddress;						// address of data pointer(used when no-medda32)

// symbols information of current file
extern unsigned long ul_Cur_File_Symbol_Cnt;
extern struct cur_inf** stpp_Cur_Inf;
extern char *cp_Current_File_Name;						// pointer for current file name( include path )

// symbols information of dump file
extern unsigned long ul_Dump_Symbol_Cnt;
extern struct dump_inf** stpp_Dump_Inf;
extern char *cp_Dump_File_Name;							// pointer for dump file name( include path )

// variables for symbol offset calculation
extern volatile unsigned long ul_All_Offset;							// offset(instruction counts) from all symbol
extern unsigned char uc_Current_All_Symbol[INPUT_CUR_LINE_MAX];			// current symbol( local & global )
extern unsigned char uc_Pre_All_Symbol[INPUT_CUR_LINE_MAX];				// pre symbol( local & global )

// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
// symbols information of all object dump file
extern unsigned long ul_All_Dump_Symbol_Cnt;
extern struct dump_inf** stpp_All_Dump_Inf;				// only global symbols will be stored
extern char *cp_All_Dump_File_Name;							// pointer for dump file name( include path )
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<<


//////////////////////////////////////////
// Function prototypes
unsigned long chg_str_to_val PARAMS (( unsigned char* ucp_chg_ptr ));

void read_cur_file_info PARAMS(( char *cp_prm_file ));

void get_valid_string PARAMS(( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt ));
int get_label_info_from_src PARAMS(( unsigned char *uc_rd_pt,unsigned char *uc_wt_pt ));
int get_attribute_info PARAMS(( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt ));

void free_cur_info PARAMS(());
void free_ext_heap_area PARAMS(());

void read_dump_info( char* cp_dump_file_name, char* cp_out_file_name );
void get_label_info_from_dump( unsigned char *ucp_rd_pt,struct dump_inf **stpp_prm_dump_inf );
void free_dump_info();

// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
void read_all_dump_info( char* cp_dump_file_name, char* cp_out_file_name );
void free_all_dump_info();
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<<

void chk_is_file_inf( unsigned char *ucp_chk_file_pt );
void chk_is_file_inf_from_line( unsigned char *ucp_chk_pt );
int chk_is_stab( unsigned char *ucp_chk_pt );


void s_app_file_2 ();									// append .file pseudo op
void reset_current_symbol(char *input_line_pointer);
void update_current_symbol(char *s);

int chk_global_label_2 PARAMS (( char* cp_label_name ));

int chk_label_address_from_dump( const char* cp_label_name,offsetT off_offset,unsigned long *ulp_label_address );
int chk_is_same_area_from_dump( const char *ucp_chk_name_1,unsigned char *ucp_chk_name_2 );
int clc_cur_address_from_dump( unsigned long* ul_address,unsigned long ul_tmp_cnt );

void getSymbolInfo(char *symbolName, struct dump_inf **pDumpInfo);
unsigned long getDataPointerAddress(char *dataPointerSymbol);

// ADD D.Fujimoto 2007/12/27 >>>>>
void countDuplicateSymbols(struct dump_inf **pAllDumpInfo, unsigned long symbolCount);
int isDuplicateSymbol(const char *symbolName);
// ADD D.Fujimoto 2007/12/27 <<<<<

// ADD D.Fujimoto 2007/12/26 >>>>>
void printDumpInf(struct dump_inf **pDumpInfo, unsigned long symbolCount);		// print symbols for debugging
// ADD D.Fujimoto 2007/12/26 <<<<<
void evaluate_offset_from_symbol(void);					// reset or increment the offset from symbol


// function pointer for ext counter functions
typedef int (*ExtCountFunc)(unsigned long address);

int evaluate_ext_count(expressionS ex, unsigned long dpAddress, ExtCountFunc pfunc);	// ext counter for mem read/write expressions

// some ExtCountFuncs
int count_ext_for_xld_rd_symbol(unsigned long address);
int count_ext_for_xld_mem_rw(unsigned long address);
int count_ext_for_xld_mem_rw32(unsigned long address);
int count_ext_for_xld_mem_rw_adv(unsigned long address);
int count_ext_for_ald_mem_rw(unsigned long address);
int count_ext_for_ald_mem_rw32(unsigned long address);


// function pointer for ext counter functions
typedef int (*ExtCountJumpFunc)(unsigned long dstAddress, unsigned long srcAddress);

int evaluate_ext_count_for_jumps(expressionS ex, int addInstCount, ExtCountJumpFunc pfunc);			// ext counter for call/jp expressions

// some ExtCountJumpFuncs
int count_ext_for_jumps(unsigned long dstAddress, unsigned long srcAddress);


#endif	// __EXT_REMOVE_H__
