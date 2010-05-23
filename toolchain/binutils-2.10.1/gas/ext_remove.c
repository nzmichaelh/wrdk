/* ext_remove.c - implementation for 2pass assemble.
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
#include "as.h"
#include "ext_remove.h"

int g_c33_ext = 0;									// flag indicating that -mc33ext option is specified

int i_File_Inf_Flg = 0;								// flag indicating that .file pseudo op exists in source file
													// 0 -- initial vaule
													// 1 -- ".file" exists.

unsigned long g_dpAddress;							// address of data pointer(used when no-medda32)

// symbols information of current file
unsigned long ul_Cur_File_Symbol_Cnt;
struct cur_inf** stpp_Cur_Inf = 0;
char *cp_Current_File_Name = 0;						// pointer for current file name( include path )

// symbols information of dump file
unsigned long ul_Dump_Symbol_Cnt;
struct dump_inf** stpp_Dump_Inf = 0;
char *cp_Dump_File_Name = 0;						// pointer for dump file name( include path )

volatile unsigned long ul_All_Offset = 0;			// offset(instruction counts) from all symbol
unsigned char uc_Current_All_Symbol[INPUT_CUR_LINE_MAX] = { 0 };		// current symbol( local & global )
unsigned char uc_Pre_All_Symbol[INPUT_CUR_LINE_MAX] = { 0 };			// pre symbol( local & global )


// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
// symbols information of all object dump file
unsigned long ul_All_Dump_Symbol_Cnt;
struct dump_inf** stpp_All_Dump_Inf;
char *cp_All_Dump_File_Name;						// pointer for all objects' dump file name( include path )
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<<



/*******************************************************************************************
Format		: unsigned long chg_str_to_val( unsigned char* ucp_chg_ptr );
Input		: unsigned char* ucp_chg_ptr -- pointer for string( hex code )
Return		: value converted from string
Expnalantion: convert string to value
*******************************************************************************************/
unsigned long chg_str_to_val( unsigned char* ucp_chg_ptr )
{
	unsigned long ul_ret;
	
	ul_ret = 0;
	
	while( 1 ){
		if( ( '0' <= (*ucp_chg_ptr) ) && ( (*ucp_chg_ptr) <= '9' ) ){
			ul_ret <<= 4;
			ul_ret |= ( (*ucp_chg_ptr) - 0x30 );
		} else if( ( 'a' <= (*ucp_chg_ptr) ) && ( (*ucp_chg_ptr) <= 'f' ) ){
			ul_ret <<= 4;
			ul_ret |= ( (*ucp_chg_ptr) - 0x57 );
		} else if( ( 'A' <= (*ucp_chg_ptr) ) && ( (*ucp_chg_ptr) <= 'F' ) ){
			ul_ret <<= 4;
			ul_ret |= ( (*ucp_chg_ptr) - 0x37 );
		} else {
			break;
		}
		ucp_chg_ptr++;
	}

	return ul_ret;
}

/*******************************************************************************************
Format		: void read_cur_file_info( char *cp_prm_file );
Input		: char *cp_prm_file -- pointer for current file
Return		: None
Expnalantion: Read source file and get the symbol information of this file.
*******************************************************************************************/
void read_cur_file_info( char *cp_prm_file )
{
	FILE *f_file;
	unsigned char uc_wk_buf[INPUT_CUR_LINE_MAX];
	unsigned char uc_buf[INPUT_CUR_LINE_MAX];
	unsigned char *ucp_wk;
	unsigned long ul_cnt;
	int i_ret,i_len,i_len_2;
	int i_stab_flg;		// 0 -- normal
						// 1 -- during ".stabs" / ".stabn" line
	
	// First, empty reading is carried out and get the total of symbols.
	f_file = fopen (cp_prm_file, "r");
	if (f_file == NULL)
	{
		fprintf (stderr, _("Error : Can't open %s for reading.\n"),cp_prm_file);
		xexit (EXIT_FAILURE);
	}
	
	ul_Cur_File_Symbol_Cnt = 0;
	i_stab_flg = 0;
	while( 1 ){
		memset( uc_wk_buf,0,INPUT_CUR_LINE_MAX );
		memset( uc_buf,0,INPUT_CUR_LINE_MAX );
		ucp_wk = fgets( uc_wk_buf, INPUT_CUR_LINE_MAX, f_file );
		if( ucp_wk == 0 ){
			break;
		}
		
		i_len = strlen( uc_wk_buf );
		ucp_wk = strpbrk( uc_wk_buf,"\r\n" );
		
		if( i_stab_flg == 1 ){
			if( ucp_wk != 0 ){
				i_stab_flg = 0; 
			}
		} else {
			if( ( ucp_wk == 0 ) && ( ( INPUT_CUR_LINE_MAX - 1 ) <= i_len ) ){
				i_ret = chk_is_stab( uc_wk_buf );
				if( i_ret == 0 ){
					fprintf (stderr, _("Error : There are too many characters of one line in assembler source file.\n"));
					xexit (EXIT_FAILURE);
				}
				i_stab_flg = 1;
			} else {
				get_valid_string( uc_wk_buf,uc_buf );
				i_ret = get_label_info_from_src( uc_buf,uc_wk_buf );
				if( i_ret != 0 ){
					ul_Cur_File_Symbol_Cnt++;
				}
			}
		}
	}

    if( fclose (f_file) == EOF ){
		fprintf (stderr, _("Error : Can't close %s\n"),cp_prm_file);
		xexit (EXIT_FAILURE);
	}
	
	// Create the heap area for unsigned char*.
	if( 0 < ul_Cur_File_Symbol_Cnt ){
		stpp_Cur_Inf = xmalloc( sizeof(struct cur_inf*) * ul_Cur_File_Symbol_Cnt );
		if( stpp_Cur_Inf == 0 ){
			fprintf (stderr, _("Error : Cannot allocate memory.\n"));
			xexit (EXIT_FAILURE);
		}
		memset( stpp_Cur_Inf,0,( sizeof(struct cur_inf*) * ul_Cur_File_Symbol_Cnt ) );
		
			// heap for the pointer of each struct cur_inf
		for( ul_cnt = 0; ul_cnt < ul_Cur_File_Symbol_Cnt; ul_cnt++ ){
			*(stpp_Cur_Inf+ul_cnt) = xmalloc( sizeof(struct cur_inf) );
			if( *(stpp_Cur_Inf+ul_cnt) == 0 ){
				fprintf (stderr, _("Error : Cannot allocate memory.\n"));
				xexit (EXIT_FAILURE);
			}
			memset( *(stpp_Cur_Inf+ul_cnt),0,( sizeof(struct cur_inf) ) );
		}		
		
		// Second, file reopens and get symbol information.
		f_file = fopen (cp_prm_file, "r");
		if (f_file == NULL)
		{
			fprintf (stderr, _("Error : Can't open %s for reading.\n"),cp_prm_file);
			xexit (EXIT_FAILURE);
		}
		
		ul_cnt = 0;
		i_stab_flg = 0;
		while( 1 ){
			memset( uc_wk_buf,0,INPUT_CUR_LINE_MAX );
			memset( uc_buf,0,INPUT_CUR_LINE_MAX );
			ucp_wk = fgets( uc_wk_buf, INPUT_CUR_LINE_MAX, f_file );
			if( ucp_wk == 0 ){
				break;
			}
			
			i_len = strlen( uc_wk_buf );
			ucp_wk = strpbrk( uc_wk_buf,"\r\n" );
			
			if( i_stab_flg == 1 ){
				if( ucp_wk != 0 ){
					i_stab_flg = 0; 
				}
			} else {
				if( ( ucp_wk == 0 ) && ( ( INPUT_CUR_LINE_MAX - 1 ) <= i_len ) ){
					i_ret = chk_is_stab( uc_wk_buf );
					if( i_ret != 0 ){
						i_stab_flg = 1;
					}
				} else {
					get_valid_string( uc_wk_buf,uc_buf );
					memset( uc_wk_buf,0,INPUT_CUR_LINE_MAX );
					i_ret = get_label_info_from_src( uc_buf,uc_wk_buf );
					if( i_ret != 0 ){
						if( i_ret == 2 ){				// ".comm"
							(*(stpp_Cur_Inf + ul_cnt))->i_Attribute = 1;		// attribute is global
						}

						i_len = strlen( uc_wk_buf );
						(*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name = xmalloc(  i_len + 1 );
						if( (*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name == 0 ){
							fprintf (stderr, _("Error : Cannot allocate memory.\n"));
							xexit (EXIT_FAILURE);
						}
						memset( (*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name,0,( i_len + 1 ) );
						memcpy( (*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name,uc_wk_buf,i_len );
						
						ul_cnt++;
					}
				}
			}
		}
		
	    if( fclose (f_file) == EOF ){
			fprintf (stderr, _("Error : Can't close %s\n"),cp_prm_file);
			xexit (EXIT_FAILURE);
		}
		
		// Third, file reopens and get attribute information.
		f_file = fopen (cp_prm_file, "r");
		if (f_file == NULL)
		{
			fprintf (stderr, _("Error : Can't open %s for reading.\n"),cp_prm_file);
			xexit (EXIT_FAILURE);
		}
		
		ul_cnt = 0;
		i_stab_flg = 0;
		while( 1 ){
			memset( uc_wk_buf,0,INPUT_CUR_LINE_MAX );
			memset( uc_buf,0,INPUT_CUR_LINE_MAX );
			ucp_wk = fgets( uc_wk_buf, INPUT_CUR_LINE_MAX, f_file );
			if( ucp_wk == 0 ){
				break;
			}
			
			i_len = strlen( uc_wk_buf );
			ucp_wk = strpbrk( uc_wk_buf,"\r\n" );
			
			if( i_stab_flg == 1 ){
				if( ucp_wk != 0 ){
					i_stab_flg = 0; 
				}
			} else {
				if( ( ucp_wk == 0 ) && ( ( INPUT_CUR_LINE_MAX - 1 ) <= i_len ) ){
					i_ret = chk_is_stab( uc_wk_buf );
					if( i_ret != 0 ){
						i_stab_flg = 1;
					}
				} else {
					i_ret = get_attribute_info( uc_wk_buf,uc_buf );
					if( i_ret != 0 ){
						i_len_2 = strlen( uc_buf );
						for( ul_cnt = 0; ul_cnt < ul_Cur_File_Symbol_Cnt; ul_cnt++ ){
							i_len = strlen( (*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name );
							if( i_len == i_len_2 ){
								if( 0 == memcmp( (*(stpp_Cur_Inf + ul_cnt))->ucp_Symbol_Name,uc_buf,i_len ) ){
									if( i_ret == 1 ){
										// ".global"
										(*(stpp_Cur_Inf + ul_cnt))->i_Attribute = 1;
									} else {
										// ".local"
										(*(stpp_Cur_Inf + ul_cnt))->i_Attribute = 0;
									}
									break;
								}
							}
						}
					}
				}
			}
		}
		
	    if( fclose (f_file) == EOF ){
			fprintf (stderr, _("Error : Can't close %s\n"),cp_prm_file);
			xexit (EXIT_FAILURE);
		}
	}
}


/*******************************************************************************************
Format		: void get_valid_string( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt );
Input		: unsigned char *ucp_rd -- read pointer
              unsigned char *ucp_wt_pt -- write pointer
Return		: None
Expnalantion: Read buffer and get the valid stirng.
              Comment / tab / space / crÅElf is excepted.
              The back of a label name is not gotten even if it is effective.
*******************************************************************************************/
void get_valid_string( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt )
{
	while( 1 ){
		if( ( (*ucp_rd_pt) == '\t' ) || ( (*ucp_rd_pt) == ' ' ) ){
			// skip
			;
		} else if ( ( (*ucp_rd_pt) == ';' ) || ( (*ucp_rd_pt) == '#' ) ) {
			// this is comment
			break;
		} else if ( ( (*ucp_rd_pt) == '\r' ) || ( (*ucp_rd_pt) == '\n' ) ) {
			// this is cr/lf
			break;
		} else {
			*ucp_wt_pt = *ucp_rd_pt;
			ucp_wt_pt++;
			if( (*ucp_rd_pt) == ':' ){
			// this is the end of label
				break;
			}
		}
		ucp_rd_pt++;
	}
}


/*******************************************************************************************
Format		: int get_label_info_from_src( unsigned char *uc_rd_pt,unsigned char *uc_wt_pt );
Input		: unsigned char *uc_rd_pt -- pointer for read data
              unsigned char *uc_wt_pt -- pointer for write data
Return		: 0 -- normal
              1 -- the contents of this buffer is label( "xxxx:" )
              1 -- the contents of this buffer is label( ".comm" )
Expnalantion: Read the buffer and check whether it is label.
              The key word is "xxxx:" or ".comm".
              If it is "xxxx:", ':' is replaced to '\0'.
              If it is ".commxxxxxx,n,n", it is replaced to "xxxxxx".
*******************************************************************************************/
int get_label_info_from_src( unsigned char *uc_rd_pt,unsigned char *uc_wt_pt )
{
	int i_ret,i_len,i_len_2;
	
	i_ret = 0;
	
	if( 0 == strchr( uc_rd_pt,'"' ) ){	// exclude if it contains '"'
		
		i_len = strlen( uc_rd_pt );
		
		if( 2 <= i_len ){			// the smalles pattern is "x:"
			if( uc_rd_pt[i_len-1] == ':' ){
				// "xxxx:"
				memcpy( uc_wt_pt,uc_rd_pt,i_len-1 );
				uc_wt_pt[i_len-1] = '\0';
				i_ret = 1;
			}
		}
		
		if( i_ret == 0 ){
			if( 5 <= i_len ){
				if( 0 == memcmp( ".comm",uc_rd_pt,5 ) ){
					// ".commxxxxxx,n,n"
					i_len_2 = strcspn( uc_rd_pt,"," );
					if( 0 < i_len_2 ){
						memcpy( uc_wt_pt,(uc_rd_pt+5),(i_len_2-5 ) );
						i_ret = 2;
					}
				}
			}
		}
	}
	
	return i_ret;
}


/*******************************************************************************************
Format		: int get_attribute_info( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt );
Input		: unsigned char *ucp_rd_pt -- pointer for read buffer
              unsigned char *ucp_wt_pt -- pointer for write buffer
Return		: 0 -- normal
              1 -- the contents of read buffer is global label
              2 -- the contents of read buffer is local label
Expnalantion: Read the buffer and check whether it is global label.
              The key word is ".global" or ".local".
*******************************************************************************************/
int get_attribute_info( unsigned char *ucp_rd_pt,unsigned char *ucp_wt_pt )
{
	int i_ret;
	int i_chk_flg;
	
	i_ret = 0;
	i_chk_flg = 0;
	while( 1 ){
		if( ( (*ucp_rd_pt) == '\t' ) || ( (*ucp_rd_pt) == ' ' ) ){
			// skip
			;
		} else if ( 0 == memcmp( ".global",ucp_rd_pt,7 ) ) {
			i_chk_flg = 1;			// we find the key word
			ucp_rd_pt += 7;
			
		} else if ( 0 == memcmp( ".local",ucp_rd_pt,6 ) ) {
			i_chk_flg = 2;			// we find the key word
			ucp_rd_pt += 6;
		
		} else if ( ( (*ucp_rd_pt) == '\r' ) || ( (*ucp_rd_pt) == '\n' ) ) {
			// this is cr/lf
			break;
		} else if ( ( (*ucp_rd_pt) == ';' ) || ( (*ucp_rd_pt) == '#' ) ) {
			// this is comment
			break;
		} else {
			if( i_chk_flg == 0 ){
				break;
			} else {
				*ucp_wt_pt = *ucp_rd_pt;
				ucp_wt_pt++;
				i_ret = i_chk_flg;
			}
		}
		ucp_rd_pt++;
	}
	
	return i_ret;
}


/*******************************************************************************************
Format		: void free_cur_info();
Input		: None
Return		: None
Expnalantion: Free the heap area for the current file information.
*******************************************************************************************/
void free_cur_info()
{
	unsigned long ul;

	if( stpp_Cur_Inf != 0 ){
	
		for( ul = 0; ul < ul_Cur_File_Symbol_Cnt; ul++ ){
		
			if( *(stpp_Cur_Inf+ul) != 0 ){
				
				if( (*(stpp_Cur_Inf+ul))->ucp_Symbol_Name != 0 ){
				
					// free member pointer	
					free( (*(stpp_Cur_Inf+ul))->ucp_Symbol_Name );
				}
				
				// free the pointer of each struct cur_inf
				free( *(stpp_Cur_Inf+ul) );
			}
		}
	
		// free the pointer of the pointer of struct cur_inf
		free( stpp_Cur_Inf );
		stpp_Cur_Inf = 0;
	}
}


/*******************************************************************************************
Format		: void free_ext_heap_area();
Input		: None
Return		: None
Expnalantion: Free the heap area for ext process.
*******************************************************************************************/
void free_ext_heap_area()
{
	free_cur_info();
	free_dump_info();
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
	free_all_dump_info();
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<<


}


/*******************************************************************************************
Format		: void read_dump_info( char* cp_dump_file_name,char* cp_out_file_name );
Input		: char* cp_dump_file_name -- dump file name( include path )
              char* cp_out_file_name -- output file name( include path )
Return		: None
Expnalantion: Read the dump file, and get the symbol information which belongs current file
              and is local symbol only.
*******************************************************************************************/
void read_dump_info( char* cp_dump_file_name, char* cp_out_file_name )
{
	#define INPUT_DUMP_LINE_MAX (0x800 + 1)	// the maximum number of characters per one line of a dump file
	
	FILE *f_file;
	unsigned char uc_buf[INPUT_DUMP_LINE_MAX];
	int i_chk_sts;						// 0 -- default
										// 1 -- check the file name
										// 2 -- get the area information and the symbol information
	unsigned char *ucp_wk;
	char* cp_out_file_pt;
	int i_cur_file_len;
	unsigned long ul_cnt;
	unsigned char uc_format_chk_buf[] = {   "SYMBOL TABLE:"  };
	int i_len,i_chk_cnt;
	
	cp_out_file_pt = strrchr( cp_out_file_name,'/' );
	
	if( cp_out_file_pt != 0 ){
		cp_out_file_pt++;
	} else {
		cp_out_file_pt = cp_out_file_name;
	}
	i_cur_file_len = strcspn( cp_out_file_pt,"." );
	
	// First, empty reading is carried out and get the total of symbols.
	// A format check is also performed.
	f_file = fopen (cp_dump_file_name, "r");
	if (f_file == NULL)
	{
		fprintf (stderr, _("Error : Cannot find the dump file.\n"));
		xexit (EXIT_FAILURE);
	}
	
	ul_Dump_Symbol_Cnt = 0;
	i_chk_sts = 0;
	while( 1 ){
		memset( uc_buf,0,INPUT_DUMP_LINE_MAX );
		ucp_wk = fgets( uc_buf, INPUT_DUMP_LINE_MAX, f_file );
		if( ucp_wk == 0 ){
			break;
		}
		
		i_len = strlen( uc_buf );
		ucp_wk = strpbrk( uc_buf,"\r\n" );
		if( ( ucp_wk == 0 ) && ( ( INPUT_DUMP_LINE_MAX - 1 ) <= i_len ) ){
			fprintf (stderr, _("Error : There are too many characters of one line in dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		
		switch( i_chk_sts ){
			case 0:
				// *** check the dump file format ***
				i_len = strcspn( uc_buf,"\r\n" );
				if( i_len == 13 ){
					if( 0 == memcmp( uc_buf,uc_format_chk_buf,13 ) ){
						i_chk_sts++;
					}
				}
				break;
			case 1:
				// *** check the file name ***
				if( 0 == memcmp( &(uc_buf[14]),"df",2 ) ){
					ucp_wk = strrchr( uc_buf,' ' );
					ucp_wk++;
					i_len = strcspn( ucp_wk,".\r\n" );
					if( i_cur_file_len == i_len ){
						if( 0 == memcmp( cp_out_file_pt,ucp_wk,i_len ) ){
							i_chk_sts++;
						}
					}
				}
				break;
			case 2:
				if( 0 == memcmp( &(uc_buf[14]),"df",2 ) ){
					if( uc_buf[9] == 'g' ){
						// *** global symbol only ***
						ul_Dump_Symbol_Cnt++;
					}
					i_chk_sts++;
				} else {
					// *** get the area information and the symbol information ***
					// *** cuurent local syombol & global symbol ***
					if( ( uc_buf[9] == 'l' ) || ( uc_buf[9] == 'g' ) ){
						ul_Dump_Symbol_Cnt++;
					}
				}
				break;
			case 3:
				if( uc_buf[9] == 'g' ){
					// *** global symbol only ***
					ul_Dump_Symbol_Cnt++;
				}
				break;
			default:
				;
				break;
		}
	}
	
	if( i_chk_sts == 0 ){
		fprintf (stderr, _("Error : The format of the dump file is invalid.\n"));
		xexit (EXIT_FAILURE);
	}
	
    if( fclose (f_file) == EOF ){
		fprintf (stderr, _("Error : Can't close %s\n"),cp_dump_file_name);
		xexit (EXIT_FAILURE);
	}
	
	if( 0 < ul_Dump_Symbol_Cnt ){
		// Second, file reopens and check whether there is any file of a same name.
		f_file = fopen (cp_dump_file_name, "r");
		if (f_file == NULL)
		{
			fprintf (stderr, _("Error : Cannot find the dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		
		i_chk_cnt = 0;
		while( 1 ){
			memset( uc_buf,0,INPUT_DUMP_LINE_MAX );
			ucp_wk = fgets( uc_buf, INPUT_DUMP_LINE_MAX, f_file );
			if( ucp_wk == 0 ){
				break;
			}
			
			i_len = strlen( uc_buf );
			ucp_wk = strpbrk( uc_buf,"\r\n" );
			if( ( ucp_wk == 0 ) && ( ( INPUT_DUMP_LINE_MAX - 1 ) <= i_len ) ){
				fprintf (stderr, _("Error : There are too many characters of one line in dump file.\n"));
				xexit (EXIT_FAILURE);
			}
			
			// *** check the file name ***
			if( 0 == memcmp( &(uc_buf[14]),"df",2 ) ){
				ucp_wk = strrchr( uc_buf,' ' );
				ucp_wk++;
				i_len = strcspn( ucp_wk,".\r\n" );
				if( i_cur_file_len == i_len ){
					if( 0 == memcmp( cp_out_file_pt,ucp_wk,i_len ) ){
						i_chk_cnt++;
					}
				}
			}
		}
		
		if( fclose (f_file) == EOF ){
			fprintf (stderr, _("Error : Can't close %s\n"),cp_dump_file_name);
			xexit (EXIT_FAILURE);
		}
		
		if( 1 < i_chk_cnt ){
			ul_Dump_Symbol_Cnt = 0;
		}
	}
	
	// Create the heap area for struct dump_inf.
		// heap for the pointer of the pointer of struct dump_inf
	if( 0 < ul_Dump_Symbol_Cnt ){
		stpp_Dump_Inf = xmalloc( sizeof(struct dump_inf*) * ul_Dump_Symbol_Cnt );
		if( stpp_Dump_Inf == 0 ){
			fprintf (stderr, _("Error : Cannot allocate memory.\n"));
			xexit (EXIT_FAILURE);
		}
		memset( stpp_Dump_Inf,0,( sizeof(struct dump_inf*) * ul_Dump_Symbol_Cnt ) );
		
			// heap for the pointer of each struct dump_inf
		for( ul_cnt = 0; ul_cnt < ul_Dump_Symbol_Cnt; ul_cnt++ ){
			*(stpp_Dump_Inf+ul_cnt) = xmalloc( sizeof(struct dump_inf) );
			if( *(stpp_Dump_Inf+ul_cnt) == 0 ){
				fprintf (stderr, _("Error : Cannot allocate memory.\n"));
				xexit (EXIT_FAILURE);
			}			
			memset( *(stpp_Dump_Inf+ul_cnt),0,( sizeof(struct dump_inf) ) );
		}
		
		
		// Third, file reopens and get symbol information.
		f_file = fopen (cp_dump_file_name, "r");
		if (f_file == NULL)
		{
			fprintf (stderr, _("Error : Cannot find the dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		
		i_chk_sts = 0;
		ul_cnt = 0;
		while( 1 ){
			memset( uc_buf,0,INPUT_DUMP_LINE_MAX );
			ucp_wk = fgets( uc_buf, INPUT_DUMP_LINE_MAX, f_file );
			if( ucp_wk == 0 ){
				break;
			}
			
			i_len = strlen( uc_buf );
			ucp_wk = strpbrk( uc_buf,"\r\n" );
			if( ( ucp_wk == 0 ) && ( ( INPUT_DUMP_LINE_MAX - 1 ) <= i_len ) ){
				fprintf (stderr, _("Error : There are too many characters of one line in dump file.\n"));
				xexit (EXIT_FAILURE);
			}
			
			switch( i_chk_sts ){
				case 0:
					// *** check the dump file format ***
					i_len = strcspn( uc_buf,"\r\n" );
					if( i_len == 13 ){
						if( 0 == memcmp( uc_buf,uc_format_chk_buf,13 ) ){
							i_chk_sts++;
						}
					}
					break;
				case 1:
					// *** check the file name ***
					if( 0 == memcmp( &(uc_buf[14]),"df",2 ) ){
						ucp_wk = strrchr( uc_buf,' ' );
						ucp_wk++;
						i_len = strcspn( ucp_wk,".\r\n" );
						if( i_cur_file_len == i_len ){
							if( 0 == memcmp( cp_out_file_pt,ucp_wk,i_len ) ){
								i_chk_sts++;
							}
						}
					}
					break;
				case 2:
					if( 0 == memcmp( &(uc_buf[14]),"df",2 ) ){
						if( uc_buf[9] == 'g' ){
							// *** global symbol only ***
							get_label_info_from_dump( uc_buf,(stpp_Dump_Inf+ul_cnt) );
							ul_cnt++;
						}
						i_chk_sts++;
					} else {
						// *** get the area information and the symbol information ***
						// *** cuurent local syombol & global symbol ***
						if( ( uc_buf[9] == 'l' ) || ( uc_buf[9] == 'g' ) ){
							get_label_info_from_dump( uc_buf,(stpp_Dump_Inf+ul_cnt) );
							ul_cnt++;
						}
					}
					break;
				case 3:
					if( uc_buf[9] == 'g' ){
						// *** global symbol only ***
						get_label_info_from_dump( uc_buf,(stpp_Dump_Inf+ul_cnt) );
						ul_cnt++;
					}
					break;
				default:
					;
					break;
			}
		}
		
	    if( fclose (f_file) == EOF ){
			fprintf (stderr, _("Error : Can't close %s\n"),cp_dump_file_name);
			xexit (EXIT_FAILURE);
		}
	}
}


/*******************************************************************************************
Format		: void get_label_info_from_dump( unsigned char *ucp_rd_pt,struct dump_inf **stpp_prm_dump_inf );
Input		: unsigned char *ucp_rd_pt -- pointer for read data
              struct dump_inf **stpp_prm_dump_inf -- pointer of pointer for struct dump_inf
Return		: None
Expnalantion: Read data and get the symbol information from dump file.
*******************************************************************************************/
void get_label_info_from_dump( unsigned char *ucp_rd_pt,struct dump_inf **stpp_prm_dump_inf )
{
	unsigned long ul_len;
	char* cp_pt;
	
		// address
	(*stpp_prm_dump_inf)->ul_Symbol_Addr = chg_str_to_val( &(ucp_rd_pt[0]) );
	// no address masking
	
		// attribute
	if( ucp_rd_pt[9] == 'g' ){
		(*stpp_prm_dump_inf)->i_Attribute = 1;					// global
	} else {	
		(*stpp_prm_dump_inf)->i_Attribute = 0;					// local
	}
	
		// area
	ul_len = strcspn( &(ucp_rd_pt[17])," \t" );
	(*stpp_prm_dump_inf)->ucp_Area_Name = xmalloc( ul_len + 1 );
	if( (*stpp_prm_dump_inf)->ucp_Area_Name == 0 ){
		fprintf (stderr, _("Error : Cannot allocate memory.\n"));
		xexit (EXIT_FAILURE);
	}
	memset( (*stpp_prm_dump_inf)->ucp_Area_Name,0,( ul_len + 1 ) );
	memcpy( (*stpp_prm_dump_inf)->ucp_Area_Name, &(ucp_rd_pt[17]), ul_len );
	
		// symbol name
	cp_pt = strrchr( ucp_rd_pt,' ' );
	if( cp_pt != 0 ){
		cp_pt++;
		ul_len = strcspn( cp_pt,"\r\n" );
		(*stpp_prm_dump_inf)->ucp_Symbol_Name = xmalloc( ul_len + 1 );
		if( (*stpp_prm_dump_inf)->ucp_Symbol_Name == 0 ){
			fprintf (stderr, _("Error : Cannot allocate memory.\n"));
			xexit (EXIT_FAILURE);
		}
		memset( (*stpp_prm_dump_inf)->ucp_Symbol_Name,0,( ul_len + 1 ) );
		memcpy( (*stpp_prm_dump_inf)->ucp_Symbol_Name, cp_pt, ul_len );
	}

/* >>>>> ADDED D.Fujimoto 2007/12/06 get address for .comm */
	// Special case:
	// for .comm areas, get the address from the 24rd column of dump file
	if (strcmp((*stpp_prm_dump_inf)->ucp_Area_Name, ".comm") == 0) {
		(*stpp_prm_dump_inf)->ul_Symbol_Addr = chg_str_to_val( &(ucp_rd_pt[24]) );
	}
/* <<<<< ADDED D.Fujimoto 2007/12/06 get address for .comm */

}


/*******************************************************************************************
Format		: void free_dump_info();
Input		: None
Return		: None
Expnalantion: Free the heap area for the dump file information.
*******************************************************************************************/
void free_dump_info()
{
	unsigned long ul;

	if( stpp_Dump_Inf != 0 ){
	
		for( ul = 0; ul < ul_Dump_Symbol_Cnt; ul++ ){

			if( *(stpp_Dump_Inf+ul) != 0 ){
			
				// free member pointer	
				if( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name != 0 ){
					free( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name );
				}
				if( (*(stpp_Dump_Inf+ul))->ucp_Area_Name != 0 ){
					free( (*(stpp_Dump_Inf+ul))->ucp_Area_Name );
				}

				// free the pointer of each struct dump_inf
				free( *(stpp_Dump_Inf+ul) );
			}
		}
		
		// free the pointer of the pointer of struct dump_inf
		free( stpp_Dump_Inf );
		stpp_Dump_Inf = 0;
	}
}


// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file >>>>>
/*******************************************************************************************
Format		: void read_all_dump_info( char* cp_dump_file_name,char* cp_out_file_name );
Input		: char* cp_dump_file_name -- dump file name( include path )
              char* cp_out_file_name -- output file name( include path )
Return		: None
Expnalantion: Read the dump file (for all object files), and get the symbol information into
			  the stpp_All_Dump_Inf.
			  This function is derived from read_dump_info().
			  This function uses global symbols  and ul_All_Dump_Symbol_Cnt
*******************************************************************************************/
void read_all_dump_info( char* cp_dump_file_name, char* cp_out_file_name )
{
	#define INPUT_DUMP_LINE_MAX (0x800 + 1)	// the maximum number of characters per one line of a dump file
	
	FILE *f_file;
	unsigned char uc_buf[INPUT_DUMP_LINE_MAX];
	int i_chk_sts;						// 0 -- default
										// 1 -- check the file name
										// 2 -- get the area information and the symbol information
	unsigned char *ucp_wk;
	char* cp_out_file_pt;
	int i_cur_file_len;
	unsigned long ul_cnt;
	unsigned char uc_format_chk_buf[] = {   "SYMBOL TABLE:"  };
	int i_len,i_chk_cnt;
	enum CheckStatus status;
	
	cp_out_file_pt = strrchr( cp_out_file_name,'/' );
	
	if( cp_out_file_pt != 0 ){
		cp_out_file_pt++;
	} else {
		cp_out_file_pt = cp_out_file_name;
	}
	i_cur_file_len = strcspn( cp_out_file_pt,"." );
	
	// First, empty reading is carried out and get the total of symbols.
	// A format check is also performed.
	f_file = fopen (cp_dump_file_name, "r");
	if (f_file == NULL)
	{
		fprintf (stderr, _("Error : Cannot find the all objects\' dump file.\n"));
		xexit (EXIT_FAILURE);
	}
	
	ul_All_Dump_Symbol_Cnt = 0;
	status = format;
	i_chk_sts = 0;
	while( 1 ){
		memset( uc_buf,0,INPUT_DUMP_LINE_MAX );
		ucp_wk = fgets( uc_buf, INPUT_DUMP_LINE_MAX, f_file );
		if( ucp_wk == 0 ){
			break;
		}
		
		i_len = strlen( uc_buf );
		ucp_wk = strpbrk( uc_buf,"\r\n" );
		if( ( ucp_wk == 0 ) && ( ( INPUT_DUMP_LINE_MAX - 1 ) <= i_len ) ){
			fprintf (stderr, _("Error : There are too many characters of one line in all objects\' dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		
		switch( status ){
			case format:
				// *** check the dump file format ***
				i_len = strcspn( uc_buf,"\r\n" );
				if( i_len == 13 ){
					if( 0 == memcmp( uc_buf,uc_format_chk_buf,13 ) ){
						i_chk_sts++;		// format OK
						status = global;
					}
				}
				break;
			case global:
				if( uc_buf[9] == 'g' ){
					// *** global symbol only ***
					ul_All_Dump_Symbol_Cnt++;
				}
				break;
			default:
				// ignore the current line
				;
				break;
		}
	}
	
	if( i_chk_sts == 0 ){
		fprintf (stderr, _("Error : The format of the all objects\' dump file is invalid.\n"));
		xexit (EXIT_FAILURE);
	}
	
    if( fclose (f_file) == EOF ){
		fprintf (stderr, _("Error : Can't close %s\n"),cp_dump_file_name);
		xexit (EXIT_FAILURE);
	}
	
	
	// Create the heap area for struct dump_inf.
		// heap for the pointer of the pointer of struct dump_inf
	if( 0 < ul_All_Dump_Symbol_Cnt ){
		stpp_All_Dump_Inf = xmalloc( sizeof(struct dump_inf*) * ul_All_Dump_Symbol_Cnt );
		if( stpp_All_Dump_Inf == 0 ){
			fprintf (stderr, _("Error : Cannot allocate memory.\n"));
			xexit (EXIT_FAILURE);
		}
		memset( stpp_All_Dump_Inf,0,( sizeof(struct dump_inf*) * ul_All_Dump_Symbol_Cnt ) );
		
			// heap for the pointer of each struct dump_inf
		for( ul_cnt = 0; ul_cnt < ul_All_Dump_Symbol_Cnt; ul_cnt++ ){
			*(stpp_All_Dump_Inf+ul_cnt) = xmalloc( sizeof(struct dump_inf) );
			if( *(stpp_All_Dump_Inf+ul_cnt) == 0 ){
				fprintf (stderr, _("Error : Cannot allocate memory.\n"));
				xexit (EXIT_FAILURE);
			}			
			memset( *(stpp_All_Dump_Inf+ul_cnt),0,( sizeof(struct dump_inf) ) );
		}
		
		
		// Second, file reopens and get symbol information.
		f_file = fopen (cp_dump_file_name, "r");
		if (f_file == NULL)
		{
			fprintf (stderr, _("Error : Cannot find the all objects\' dump file.\n"));
			xexit (EXIT_FAILURE);
		}
		
		status = format;
		i_chk_sts = 0;
		ul_cnt = 0;
		while( 1 ){
			memset( uc_buf,0,INPUT_DUMP_LINE_MAX );
			ucp_wk = fgets( uc_buf, INPUT_DUMP_LINE_MAX, f_file );
			if( ucp_wk == 0 ){
				break;
			}
			
			i_len = strlen( uc_buf );
			ucp_wk = strpbrk( uc_buf,"\r\n" );
			if( ( ucp_wk == 0 ) && ( ( INPUT_DUMP_LINE_MAX - 1 ) <= i_len ) ){
				fprintf (stderr, _("Error : There are too many characters of one line in all objects\' dump file.\n"));
				xexit (EXIT_FAILURE);
			}
			
			switch( status ){
				case format:
					// *** check the dump file format ***
					i_len = strcspn( uc_buf,"\r\n" );
					if( i_len == 13 ){
						if( 0 == memcmp( uc_buf,uc_format_chk_buf,13 ) ){
							i_chk_sts++;		// format is OK
							status = global;
						}
					}
					break;
				case global:
					if( uc_buf[9] == 'g' ){
						// *** global symbol only ***
						get_label_info_from_dump( uc_buf,(stpp_All_Dump_Inf+ul_cnt) );
						ul_cnt++;
					}
					break;
				default:
					;
					break;
			}
		}
		
	    if( fclose (f_file) == EOF ){
			fprintf (stderr, _("Error : Can't close %s\n"),cp_dump_file_name);
			xexit (EXIT_FAILURE);
		}
	}
}


/*******************************************************************************************
Format		: void free_all_dump_info();
Input		: None
Return		: None
Expnalantion: Free the heap area for the dump file information.
			  Uses global symbols stpp_All_Dump_Inf and ul_All_Dump_Symbol_Cnt
*******************************************************************************************/
void free_all_dump_info()
{
	unsigned long ul;

	if( stpp_All_Dump_Inf != 0 ){
	
		for( ul = 0; ul < ul_All_Dump_Symbol_Cnt; ul++ ){

			if( *(stpp_All_Dump_Inf+ul) != 0 ){
			
				// free member pointer	
				if( (*(stpp_All_Dump_Inf+ul))->ucp_Symbol_Name != 0 ){
					free( (*(stpp_All_Dump_Inf+ul))->ucp_Symbol_Name );
				}
				if( (*(stpp_All_Dump_Inf+ul))->ucp_Area_Name != 0 ){
					free( (*(stpp_All_Dump_Inf+ul))->ucp_Area_Name );
				}

				// free the pointer of each struct dump_inf
				free( *(stpp_All_Dump_Inf+ul) );
			}
		}
		
		// free the pointer of the pointer of struct dump_inf
		free( stpp_All_Dump_Inf );
		stpp_All_Dump_Inf = 0;
	}
}
// ADD D.Fujimoto 2007/12/26 for all 1pass objects' dump file <<<<<


/*******************************************************************************************
Format		: void chk_is_file_inf( unsigned char *ucp_chk_file_pt );
Input		: unsigned char *ucp_chk_pt -- pointer for file name
Return		: None
Expnalantion: Check whether ".file" exists from souce file.
*******************************************************************************************/
void chk_is_file_inf( unsigned char *ucp_chk_file_pt )
{
	FILE *f_file;
	unsigned char uc_buf[INPUT_CUR_LINE_MAX];
	unsigned char *ucp_wk;
	int i_len;
	int i_ret;
	int i_stab_flg;		// 0 -- normal
						// 1 -- during ".stabs" / ".stabn" line
	
	f_file = fopen (ucp_chk_file_pt, "r");
	if (f_file == NULL)
	{
		fprintf (stderr, _("Error : Can't open %s for reading.\n"),ucp_chk_file_pt);
		xexit (EXIT_FAILURE);
	}
	
	i_stab_flg = 0;
	while( 1 ){
		memset( uc_buf,0,INPUT_CUR_LINE_MAX );
		ucp_wk = fgets( uc_buf, INPUT_CUR_LINE_MAX, f_file );
		if( ucp_wk == 0 ){
			break;
		}
		
		i_len = strlen( uc_buf );
		ucp_wk = strpbrk( uc_buf,"\r\n" );
		
		if( i_stab_flg == 1 ){
			if( ucp_wk != 0 ){
				i_stab_flg = 0; 
			}
		} else {
			if( ( ucp_wk == 0 ) && ( ( INPUT_CUR_LINE_MAX - 1 ) <= i_len ) ){
				i_ret = chk_is_stab( uc_buf );
				if( i_ret == 0 ){
					fprintf (stderr, _("Error : There are too many characters of one line in assembler source file.\n"));
					xexit (EXIT_FAILURE);
				}
				i_stab_flg = 1;
			} else {
				chk_is_file_inf_from_line( uc_buf );
			}
		}
	}

    if( fclose (f_file) == EOF ){
		fprintf (stderr, _("Error : Can't close %s\n"),ucp_chk_file_pt);
		xexit (EXIT_FAILURE);
	}
}


/*******************************************************************************************
Format		: void chk_is_file_inf( unsigned char *ucp_chk_pt );
Input		: unsigned char *ucp_chk_pt -- check pointer
Return		: None
Expnalantion: Check whether ".file" exists from line.
              If ".file" exists, file name is set.
*******************************************************************************************/
void chk_is_file_inf_from_line( unsigned char *ucp_chk_pt )
{
	if( i_File_Inf_Flg == 0 ){
		while( 1 ){
			if( ( (*ucp_chk_pt) == '\t' ) || ( (*ucp_chk_pt) == ' ' ) ){
				// skip
				;
			} else if ( ( (*ucp_chk_pt) == ';' ) || ( (*ucp_chk_pt) == '#' ) ) {
				// this is comment
				break;
			} else if ( ( (*ucp_chk_pt) == '\r' ) || ( (*ucp_chk_pt) == '\n' ) ) {
				// this is cr/lf
				break;
			} else {
				if( 5 <= strlen( ucp_chk_pt ) ){
					if( 0 == memcmp( ucp_chk_pt,".file",5 ) ){
						i_File_Inf_Flg = 1;
						break;
					}
				} else {
					break;
				}
			}
			ucp_chk_pt++;
		}
	}
}


/*******************************************************************************************
Format		: int chk_is_stab( unsigned char *ucp_chk_pt );
Input		: unsigned char *ucp_chk_pt -- check pointer
Return		: 0 -- ".stabs" / ".stabn" doesn't exists.
              1 -- ".stabs" / ".stabn" exists.
Expnalantion: Check whether ".stabs" / ".stabn" exists.
*******************************************************************************************/
int chk_is_stab( unsigned char *ucp_chk_pt )
{
	int i;
	int i_ret;
	
	i_ret = 0;
	for( i = 0; i < (INPUT_CUR_LINE_MAX - 1); i++ ){
		if( ( (*ucp_chk_pt) == '\t' ) || ( (*ucp_chk_pt) == ' ' ) ){
			// skip
			;
		} else if ( ( (*ucp_chk_pt) == ';' ) || ( (*ucp_chk_pt) == '#' ) ) {
			// this is comment
			break;
		} else {
			if( 6 <= strlen( ucp_chk_pt ) ){
				if( ( 0 == memcmp( ucp_chk_pt,".stabs",6 ) )
						|| ( 0 == memcmp( ucp_chk_pt,".stabn",6 ) ) ){
					i_ret = 1;
					break;
				}
			} else {
				break;
			}
		}
		ucp_chk_pt++;
	}
	
	return i_ret;
}


/*******************************************************************************************
Format		: void s_app_file_2 ();
Input		: None
Return		: None
Expnalantion: ADD ".file" information to object file.
              If source file has already ".file" information, don't add it here.
*******************************************************************************************/
void s_app_file_2 ()
{
  register char *s,*cp_wk;
//  int length;
  int may_omit;
  int appfile;  
  
  int i_len;
  char *cp_file_name_pt = 0;

  if( i_File_Inf_Flg == 0 ){
  	  if( out_file_name != 0 ){
	      	if( 0 < strlen( out_file_name ) ){
			  appfile = 0;
		        
			  /* Some assemblers tolerate immediately following '"' */
			  // if ((s = demand_copy_string (&length)) != 0)
			  //  {
			       // Get the file name from the "out_file_name".
			       // And change extension to '*.s'.
				   i_len = strlen( out_file_name );
				   cp_file_name_pt = xmalloc( i_len + 1 );
				   if( cp_file_name_pt == 0 ){
					   fprintf (stderr, _("Error : Cannot allocate memory.\n"));
					   xexit (EXIT_FAILURE);
				   }
				   memset( cp_file_name_pt,0,i_len + 1 );
				   memcpy( cp_file_name_pt,out_file_name,i_len );
			       s = strrchr( cp_file_name_pt,'/' );
			       if( s != 0 ){
				       s++;
				   } else {
					   s = cp_file_name_pt;
				   }
				   cp_wk = strrchr(  s,'.' );
				   if( cp_wk != 0 ){
					   cp_wk++;
				       (*cp_wk) = 's';
					   cp_wk++;
					   (*cp_wk) = 0;
				   }

			      /* If this is a fake .appfile, a fake newline was inserted into
			         the buffer.  Passing -2 to new_logical_line tells it to
			         account for it.  */
			       may_omit= (! new_logical_line (s, appfile ? -2 : -1) && appfile);

			      /* In MRI mode, the preprocessor may have inserted an extraneous
			         backquote.  */
			      if (flag_m68k_mri
			          && *input_line_pointer == '\''
			          && is_end_of_line[(unsigned char) input_line_pointer[1]])
			        ++input_line_pointer;

			      // demand_empty_rest_of_line ();
			      
			      if (! may_omit)
			        {
			#ifdef LISTING
			          if (listing)
			            listing_source_file (s);
			#endif
			          register_dependency (s);
			#ifdef obj_app_file
			          obj_app_file (s);
			#endif
			        }
			    // }
			    
			    if( cp_file_name_pt != 0 ){
			    	free( cp_file_name_pt );
			    }
			}
		}
	}
}

/*******************************************************************************************
Format		: int reset_current_symbol(char *input_line_pointer);
Input		: char *input_line_pointer -- pointer for reading the source file
Return		: NONE
Explanation	: Reset the global variables if the ".section" changes.
*******************************************************************************************/
void reset_current_symbol(char *input_line_pointer)
{
	// if ".section" is found,reset the "uc_Current_All_Symbol".
	if( 8 <= strlen( input_line_pointer ) ){
		if( 0 == memcmp( ".section",input_line_pointer,8 ) ){
			memset( uc_Current_All_Symbol,0,sizeof( uc_Current_All_Symbol ) );
		}
	}

}

/*******************************************************************************************
Format		: int update_current_symbol(char *input_line_pointer);
Input		: char *s -- label name
Return		: NONE
Explanation	: Check whether the label is local or global from the current file 
			  and saves it to the global variables.
*******************************************************************************************/
void update_current_symbol(char *s)
{
	int i_ret;			// return value of a function
	int i_len;			// length of the label

	// Check whether the label is local or global from the current file only.
	i_ret = chk_global_label_2( s );
	if( i_ret == 1 ){
		i_len = strlen( s );
	}

	i_len = strlen( s );
	memset( uc_Current_All_Symbol,0,sizeof( uc_Current_All_Symbol ) );
	memcpy( uc_Current_All_Symbol,s,i_len );

}


/*******************************************************************************************
Format		: int evaluate_offset_from_symbol( void );
Input		: NONE
Return		: NONE
Expnalantion: Save the current symbol to the previous symbol.
			  Count up the offset from symbol if the current remains the same or,
			  reset the offset from symbol if the current changes.
			  Uses the following global vars
				uc_Current_All_Symbol
				uc_Pre_All_Symbol
				ul_All_Offset
*******************************************************************************************/
void evaluate_offset_from_symbol(void)
{

	int i_len,i_len_2,i_chk_flg;

    
	// get the offset form all symbol
	i_chk_flg = 0;
	i_len = strlen( uc_Current_All_Symbol );
	i_len_2 = strlen( uc_Pre_All_Symbol );
	if( i_len == 0 ){
		i_chk_flg = 1;
	} else {
		if( i_len == i_len_2 ){
			if( 0 != memcmp( uc_Current_All_Symbol,uc_Pre_All_Symbol,i_len ) ){
				i_chk_flg = 1;
			}
		} else {
			i_chk_flg = 1;
		}
	}
	
	if( i_chk_flg == 0 ){
		// count up offset if the newest all symbol name is the same
		ul_All_Offset++;			// count up the offset
	} else {
		ul_All_Offset = 0;		// reset the offset
	}
	
	if( i_len != 0 ){
		memset( uc_Pre_All_Symbol,0,sizeof( uc_Pre_All_Symbol ) );
		memcpy( uc_Pre_All_Symbol,uc_Current_All_Symbol,i_len );
	} else {
		memset( uc_Pre_All_Symbol,0,sizeof( uc_Pre_All_Symbol ) );
	}

}


/*******************************************************************************************
Format		: int chk_global_label_2( char* cp_label_name );
Input		: char* cp_label_name -- pointer for label name
Return		: 0 -- local label
              1 -- global label
Expnalantion: Check whether the label is local or global from the current file only.
*******************************************************************************************/
int chk_global_label_2( char* cp_label_name )
{
	int i_ret,i_len,i_len_2;
	unsigned long ul;
	
	// compare symbol name to the current file
	i_ret = 0;		// local
	i_len = strlen( cp_label_name );
	for( ul = 0; ul < ul_Cur_File_Symbol_Cnt; ul++ ){
		i_len_2 = strlen( (*(stpp_Cur_Inf+ul))->ucp_Symbol_Name );
		if( i_len == i_len_2 ){
			if( 0 == memcmp( (*(stpp_Cur_Inf+ul))->ucp_Symbol_Name,cp_label_name,i_len ) ){
				
				// copy the attribute
				i_ret = (*(stpp_Cur_Inf+ul))->i_Attribute;		// 0 -- local
																// 1 -- global
				break;
			}
		}
	}
	
	return i_ret;
}


/*******************************************************************************************
Format		: int chk_label_address_from_dump( char* cp_label_name,offsetT off_offset,unsigned long *ulp_label_address );
Input		: const char* cp_label_name -- pointer for label name
              offsetT off_offset -- offset from label address
              unsigned long *ulp_label_address -- pointer for label address
Return		: 0 -- don't get the address
              1 -- get the address
Expnalantion: Get the label address and add offset from the dump file.
              First compare the current local symbol, and if don't get the addresss, 
              then compare the global symbol.
*******************************************************************************************/
int chk_label_address_from_dump( const char* cp_label_name,offsetT off_offset,unsigned long *ulp_label_address )
{
	int i_ret,i_len,i_len_2;
	int i_attr;			// 0 -- local
						// 1 -- global
	unsigned long ul;
	
	i_ret = 0;
	i_len = strlen( cp_label_name );
	
	// compare symbol name to the dump file
	for( i_attr = 0; i_attr < 2; i_attr++ ){
		for( ul = 0; ul < ul_Dump_Symbol_Cnt; ul++ ){
			if( (*(stpp_Dump_Inf+ul))->i_Attribute == i_attr ){		// current local symbol / global symbol
				i_len_2 = strlen( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name );
				if( i_len == i_len_2 ){
					if( 0 == memcmp( cp_label_name,(*(stpp_Dump_Inf+ul))->ucp_Symbol_Name,i_len_2 ) ){
						i_ret = 1;
						*ulp_label_address = (*(stpp_Dump_Inf+ul))->ul_Symbol_Addr + off_offset;
						break;
					}
				}
			}
		}
		if( i_ret == 1 ){
			break;
		}
	}
	
	return i_ret;
}


/*******************************************************************************************
Format		: int chk_is_same_area_from_dump( unsigned char *ucp_chk_name_1,unsigned char *ucp_chk_name_2 );
Input		: const char *ucp_chk_name_1    -- symbol name in expression
              unsigned char *ucp_chk_name_2 -- current local or global symbol
Return		: 0 -- area of check name 1 and check name 2 is not same
                   or 
                   don't get the symbol information
              1 -- area of check name 1 and check name 2 is same
Expnalantion: Check whether area of check name 1 and check name 2 is same from the dump file.
              First compare the current local symbol, and if don't get the addresss, 
              then compare the global symbol.
*******************************************************************************************/
int chk_is_same_area_from_dump( const char *ucp_chk_name_1,unsigned char *ucp_chk_name_2 )
{
	int i_ret;
	int i_len_1,i_len_2,i_chk_flg;
	int i_attr;			// 0 -- local
						// 1 -- global
	unsigned char *ucp_area_1 = NULL;
	unsigned char *ucp_area_2 = NULL;
	unsigned long ul;
	
	i_ret = 0;
	
	// compare check name 1 to the dump file
	i_chk_flg = 0;
	i_len_1 = strlen( ucp_chk_name_1 );
	if( 0 < i_len_1 ){
		// compare symbol name to the dump file
		for( i_attr = 0; i_attr < 2; i_attr++ ){
			for( ul = 0; ul < ul_Dump_Symbol_Cnt; ul++ ){
				if( (*(stpp_Dump_Inf+ul))->i_Attribute == i_attr ){		// current local symbol / global symbol
					i_len_2 = strlen( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name );
					if( i_len_1 == i_len_2 ){
						if( 0 == memcmp( ucp_chk_name_1,(*(stpp_Dump_Inf+ul))->ucp_Symbol_Name,i_len_2 ) ){
							i_chk_flg = 1;
							ucp_area_1 = (*(stpp_Dump_Inf+ul))->ucp_Area_Name;
							break;
						}
					}
				}
			}
			if( i_chk_flg == 1 ){
				break;
			}
		}
	}
	
	if( i_chk_flg == 1 ){
		// compare check name 2 to the dump file
		i_chk_flg = 0;
		i_len_1 = strlen( ucp_chk_name_2 );
		if( 0 < i_len_1 ){
			// compare symbol name to the dump file
			for( i_attr = 0; i_attr < 2; i_attr++ ){
				for( ul = 0; ul < ul_Dump_Symbol_Cnt; ul++ ){
					if( (*(stpp_Dump_Inf+ul))->i_Attribute == i_attr ){		// current local symbol / global symbol
						i_len_2 = strlen( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name );
						if( i_len_1 == i_len_2 ){
							if( 0 == memcmp( ucp_chk_name_2,(*(stpp_Dump_Inf+ul))->ucp_Symbol_Name,i_len_2 ) ){
								i_chk_flg = 1;
								ucp_area_2 = (*(stpp_Dump_Inf+ul))->ucp_Area_Name;
								break;
							}
						}
					}
				}
				if( i_chk_flg == 1 ){
					break;
				}
			}
		}
	}
	
	if( i_chk_flg == 1 ){
		i_len_1 = strlen( ucp_area_1 );
		i_len_2 = strlen( ucp_area_2 );
		if( i_len_1 == i_len_2 ){
			if( 0 < i_len_1 ){
				if( 0 == memcmp( ucp_area_1,ucp_area_2,i_len_1 ) ){
					i_ret = 1;
				}
			}
		}
	}
	return i_ret;
}


/*******************************************************************************************
Format		: int clc_cur_address_from_dump( unsigned long* ul_address,unsigned long ul_tmp_cnt );
Input		: unsigned long* ul_address -- pointer for save address
              unsigned long ul_tmp_cnt -- additional instruction(ext) counts for caluclating offset
Return		: 0 -- Address was not calculated.
              1 -- Address was calculated.
Expnalantion: Calculate the current address the form dump file and offset information.
              First compare the current local symbol, and if don't get the addresss, 
              then compare the global symbol.
*******************************************************************************************/
int clc_cur_address_from_dump( unsigned long* ul_address,unsigned long ul_tmp_cnt )
{
	int i_ret;
	int i_len_1,i_len_2;
	int i_attr;			// 0 -- local
						// 1 -- global
	unsigned long ul;
	
	i_ret = 0;
	
	i_len_1 = strlen( uc_Current_All_Symbol );
	if( 0 < i_len_1 ){
		// compare symbol name to the dump file
		for( i_attr = 0; i_attr < 2; i_attr++ ){
			for( ul = 0; ul < ul_Dump_Symbol_Cnt; ul++ ){
				if( (*(stpp_Dump_Inf+ul))->i_Attribute == i_attr ){		// current local symbol / global symbol
					i_len_2 = strlen( (*(stpp_Dump_Inf+ul))->ucp_Symbol_Name );
					if( i_len_1 == i_len_2 ){
						if( 0 == memcmp( uc_Current_All_Symbol,(*(stpp_Dump_Inf+ul))->ucp_Symbol_Name,i_len_2 ) ){
							i_ret = 1;
							*ul_address = (( ul_tmp_cnt + ul_All_Offset ) * 2) + (*(stpp_Dump_Inf+ul))->ul_Symbol_Addr;
							// no address masking
							break;
						}
					}
				}
			}
			if( i_ret == 1 ){
				break;
			}
		}
	}
	
	return i_ret;
}


/*******************************************************************************************
Format      : void getSymbolInfo(char *symbolName, struct dump_inf **pDumpInfo)
Input       : char *symbolName     the calculated address of a symbol
              struct dump_inf **pDumpInfo 
                                  pointer to the global dump_inf pointer(stpp_Dump_Inf)
                                  the result will be stored in this parameter
Return      : NONE
Expnalantion: Receives the pointer to dump info for symbolName
*******************************************************************************************/
void getSymbolInfo(char *symbolName, struct dump_inf **pDumpInfo)
{
	unsigned long i;

	struct dump_inf **pSearchDumpInfo = stpp_Dump_Inf;
	unsigned long symbolCount = ul_Dump_Symbol_Cnt;

	for (i = 0; i < symbolCount; i++) {
		if (strcmp(pSearchDumpInfo[i]->ucp_Symbol_Name, symbolName) == 0) {
			*pDumpInfo = pSearchDumpInfo[i];
			break;
		}
	}

}


/*******************************************************************************************
Format      : unsigned long getDataPointerAddress(char *dpSymbol)
Input       : char *dpSymbol     the symbol for the data pointer
Return      : unsigned long      address of data pointer
Expnalantion: Returns the address of the data pointer.
              The symbol must exist in the dump file and be a global symbol,
              otherwise the value will be 0
*******************************************************************************************/
unsigned long getDataPointerAddress(char *dpSymbol)
{
	unsigned long address = 0;				// return value
	struct dump_inf *pDumpInfo = NULL;

	getSymbolInfo(dpSymbol, &pDumpInfo);
	if (pDumpInfo != NULL) {
		// get the address when the symbol is a global symbol
		if (pDumpInfo->i_Attribute == 1) {
			address = pDumpInfo->ul_Symbol_Addr;
		}
	}

	return address;

}


// ADD D.Fujimoto 2007/12/27 >>>>>
/*******************************************************************************************
Format      : void countDuplicateSymbols(struct dump_inf **pDumpInfo, struct dump_inf **pAllDumpInfo, unsigned long symbolCount)
Input       :   struct dump_inf **pAllDumpInfo
								  pointer to the global dump_inf pointer (all object file dump).
								  This will be used to check duplicate symbols
			  unsigned long		  element count of *pAllDumpInfo
Return      : none
Expnalantion: Count up the symbol occurence in stpp_Dump_Inf that appears in pAllDumpInfo.
			  The count for pAllDumpInfo must be given as symbolCount.
			  Global variable stpp_Dump_Inf is referenced here.
			  Prior to calling this function, all elements of stpp_Dump_Inf[]->iCount should be 0
*******************************************************************************************/
void countDuplicateSymbols(struct dump_inf **pAllDumpInfo, unsigned long symbolCount)
{
	unsigned long i;
	struct dump_inf *pDumpInfo = NULL;

	for (i = 0; i < symbolCount; i++) {
		// search for global symbols and match that with pDumpInfo
		if (pAllDumpInfo[i]->i_Attribute == 1) {
			getSymbolInfo(pAllDumpInfo[i]->ucp_Symbol_Name, &pDumpInfo);

			// count it up
			if (pDumpInfo != NULL) {
				if (pDumpInfo->i_Attribute == 1) {
					pDumpInfo->iCount++;
				}
			}
		}
	}

}


/*******************************************************************************************
Format      : int isDuplicateSymbol(char *symbolName)
Input       : char *symbolName
					symbolName to search from stpp_Dump_Inf
Return      : 1 = true, 0 = false
Expnalantion: Search symbolName from stpp_Dump_Inf and check whether it is a duplicate.
			  Duplicate symbols must not be used for ext insn optimization.
			  Global variable stpp_Dump_Inf is referenced here.
*******************************************************************************************/
int isDuplicateSymbol(const char *symbolName)
{
	int ret = 0;
	unsigned long i;

	struct dump_inf **pSearchDumpInfo = stpp_Dump_Inf;
	unsigned long symbolCount = ul_Dump_Symbol_Cnt;

	// duplicate when the global symbol count is more than 1
	for (i = 0; i < symbolCount; i++) {
		if (strcmp(pSearchDumpInfo[i]->ucp_Symbol_Name, symbolName) == 0 && 
			pSearchDumpInfo[i]->i_Attribute == 1 &&
			pSearchDumpInfo[i]->iCount > 1) {
			ret = 1;
			break;
		}
	}

	return ret;

}
// ADD D.Fujimoto 2007/12/27 <<<<<


// ADD D.Fujimoto 2007/12/26 >>>>>
/*******************************************************************************************
Format      : void printDumpInf(struct dump_inf **pDumpInfo, unsigned long symbolCount)
Input       : struct dump_inf **pDumpInfo	
                                  pointer to the global dump_inf pointer
			  unsigned long		element count of *pDumpInfo
Return      : none
Expnalantion: Print symbols in struct dump_inf for debugging.
			  The count for struct dump_inf must be given as symbolCount.
*******************************************************************************************/
void printDumpInf(struct dump_inf **pDumpInfo, unsigned long symbolCount)
{
	int i;

	for (i = 0; i < symbolCount; i++) {
		printf("%s\t%s\t0x%x", pDumpInfo[i]->ucp_Symbol_Name, pDumpInfo[i]->ucp_Area_Name, pDumpInfo[i]->ul_Symbol_Addr);

		printf("\t%s\n", (pDumpInfo[i]->i_Attribute == 1 ? "g" : "l"));
	}

}
// ADD D.Fujimoto 2007/12/26 <<<<<


/*******************************************************************************************
Format		: int evaluate_ext_count(expressionS ex, ExtCountFunc pfunc)
Input		: expressionS	ex		expression of current line
              unsigned long dpAddress
                                    Address of data pointer(used when no medda32).
                                    Specify 0 when using medda32.
              ExtCountFunc	pfunc	function to count ext for the current line
Return      : The count of ext instructions, 
	          or MAX_EXT_INSN_CNT if maximum ext instructions should be added.
Expnalantion: Evaluates the needed ext counts for this expression.
			  The given function will determine the actual ext counts.
			  The function pointer arg should not be NULL.
*******************************************************************************************/
int evaluate_ext_count(expressionS ex, unsigned long dpAddress, ExtCountFunc pfunc)
{
	int i_ext_cnt = MAX_EXT_INSN_CNT;	// return value 
	int i_ret;					// return values of the called functions
	
	unsigned long ul_address;	// the address of the symbol

	if (pfunc == NULL) {
		abort();
	}

// ADD D.Fujimoto 2008/01/07 >>>>>
	// skip counting for duplicate (c++) symbols
	if (isDuplicateSymbol(S_GET_NAME( ex.X_add_symbol ))) {
		return MAX_EXT_INSN_CNT;
	}
// ADD D.Fujimoto 2008/01/07 <<<<<

	// get symbol address ( include formula )
	i_ret = chk_label_address_from_dump( S_GET_NAME( ex.X_add_symbol ),ex.X_add_number,&ul_address );
	if( i_ret == 1 ){
		// dp address is normally smaller than the symbol address
		ul_address = (ul_address >= dpAddress) ? ul_address - dpAddress : ul_address;
		i_ext_cnt = pfunc(ul_address);		// call the apropriate ExtCountFunc
	}
	
	return i_ext_cnt;
}


/*******************************************************************************************
Format      : int count_ext_for_xld_rd_symbol(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - xld.w %rd, LABEL
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_xld_rd_symbol(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x1F ) {
		// 0 - 0x1F
		ext_count = 0;
	} else if ( (0x20 <= address) && (address <= 0x3FFFF) ) {
		ext_count = 1;
	} else if ( (0x40000 <= address) && (address <= 0x7FFFF) ) {
		ext_count = 2;
	}

	return ext_count;
}


/*******************************************************************************************
Format      : int count_ext_for_xld_mem_rw(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - xld.* %rd, [LABEL]
                - xld.* [LABEL], %rd
                - xb*   [LABEL], imm3
                  for STD, PE
                      nomedda
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_xld_mem_rw(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x1FFF ) {
		// 0 - 0x1FFF
		ext_count = 1;
	}

	return ext_count;
}


/*******************************************************************************************
Format      : int count_ext_for_xld_mem_rw32(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - xld.* %rd, [LABEL]
                - xld.* [LABEL], %rd
                - xb*   [LABEL], imm3
                  for STD, PE, ADV
                      medda
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_xld_mem_rw32(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x1F ) {
		// 0 - 0x1F
		ext_count = 0;
	} else if ( (0x20 <= address) && (address <= 0x3FFFF) ) {
		ext_count = 1;
	}

	return ext_count;
}


/*******************************************************************************************
Format      : int count_ext_for_xld_mem_rw_adv(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - xld.* %rd, [LABEL]
                - xld.* [LABEL], %rd
                  for ADV
                      nomedda
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_xld_mem_rw_adv(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x7FFFF ) {
		// 0x0 - 0x7FFFF
		ext_count = 1;
	}

	return ext_count;
}


/*******************************************************************************************
Format      : int count_ext_for_ald_mem_rw(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - ald.* %rd, [LABEL]
                - ald.* [LABEL], %rd
                  for ADV
                      nomedda
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_ald_mem_rw(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x7FFFF ) {
		// 0x0 - 0x7FFFF
		ext_count = 1;
	}

	return ext_count;

}


/*******************************************************************************************
Format      : int count_ext_for_ald_mem_rw32(unsigned long address)
Input       : unsigned long address     the calculated address of a symbol
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - ald.* %rd, [LABEL]
                - ald.* [LABEL], %rd
                  for ADV
                      medda
              This function is used as ExtCountFunc to be passed to evaluate_ext_count().
*******************************************************************************************/
int count_ext_for_ald_mem_rw32(unsigned long address)
{
	int ext_count = MAX_EXT_INSN_CNT;

	if ( address <= 0x1F ) {
		// 0 - 0x1F
		ext_count = 0;
	} else if ( (0x20 <= address) && (address <= 0x7FFFF) ) {
		ext_count = 1;
	}

	return ext_count;

}


/*******************************************************************************************
Format		: int evaluate_ext_count_for_jumps(expressionS ex)
Input		: expressionS	ex		expression of current line
			: int addInstCount		additional instruction(ext) counts for calculating address
              ExtCountFunc	pfunc	function to count ext for the current line
Return      : The count of ext instructions, 
	          or MAX_EXT_INSN_CNT if maximum ext instructions should be added.
Expnalantion: Evaluates the needed ext counts for this expression(call and jp*).
			  The given function will determine the actual ext counts.
			  The function pointer arg should not be NULL.
*******************************************************************************************/
int evaluate_ext_count_for_jumps(expressionS ex, int addInstCount, ExtCountJumpFunc pfunc)
{
	
	int i_ext_cnt = MAX_EXT_INSN_CNT;	// return value 
	int i_ret;					// return values of the called functions
	
	unsigned long ul_dst_address;		// address of the LABEL(operand) 
	unsigned long ul_src_address;		// address of the instruction

	if (pfunc == NULL) {
		abort();
	}

// ADD D.Fujimoto 2008/01/07 >>>>>
	// skip counting for duplicate (c++) symbols
	if (isDuplicateSymbol(S_GET_NAME( ex.X_add_symbol ))) {
		return MAX_EXT_INSN_CNT;
	}
// ADD D.Fujimoto 2008/01/07 <<<<<

	// get symbol address ( include formula )
	i_ret = chk_label_address_from_dump( S_GET_NAME( ex.X_add_symbol ),ex.X_add_number,&ul_dst_address );
	if( i_ret == 1 ){
		// check whether area is same
		i_ret = chk_is_same_area_from_dump( S_GET_NAME( ex.X_add_symbol ),uc_Current_All_Symbol );
		if( i_ret == 1 ){
			i_ret = clc_cur_address_from_dump( &ul_src_address, addInstCount );		// get current_address
			if( i_ret == 1 ){
				i_ext_cnt = pfunc(ul_dst_address, ul_src_address);		// call the apropriate ExtCountFunc
			}
		}
	}

	return i_ext_cnt;

}


/*******************************************************************************************
Format      : int count_ext_for_jumps(unsigned long dstAddress, unsigned long srcAddress)
Input       : unsigned long dstAddress     the calculated address of a symbol
              unsigned long srcAddress     the calculated address of the instruction
Return      : The count of ext instructions, 
              or MAX_EXT_INSN_CNT if it cannot be determined
Expnalantion: Evaluates the needed ext counts for reaching the given address for patterns:
                - scall LABEL
                - xcall LABEL
                - sj*   LABEL
                - xj*   LABEL
              This function is used as ExtCountJumpFunc 
              to be passed to evaluate_ext_count_for_jumps().
*******************************************************************************************/
int count_ext_for_jumps(unsigned long dstAddress, unsigned long srcAddress)
{
	int ext_count = MAX_EXT_INSN_CNT;
	long distance;

	// distance may be negative
	distance = (long) (dstAddress - srcAddress);
	if ( ( -256 <= distance ) && ( distance <= 254 ) ) {
		ext_count = 0;
	} else if ( ((-2097152 <= distance) && (distance < -256)) || 
				((254 < distance) && (distance <= 2097150)) ) {
		ext_count = 1;
	} else if ( (distance < -2097152) ||  (2097150 < distance) ) {
		ext_count = 2;
	}

	return ext_count;

}
