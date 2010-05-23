;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : negdfs.s
;*
;*	Double floating point negative function
;*		input: (%r7, %r6)
;*		output: (%r5, %r4)
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/15  O.Hinokuchi
;*  			 ・レジスタ置換
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __negdf2

;==============================================
;＜レジスタ説明＞
;	%r4:戻り値(L)(double)
;	%r5:戻り値(H)(double)
;	%r6:引数１(L)(double)
;	%r7:引数１(H)(double)
;==============================================

__negdf2:

	; flip the MSB of %r5 and put result in %r6
	ld.w	%r5,%r7					;戻り値(H)(5) <- 引数１(H)(7)
	xxor	%r5,0x80000000			;戻り値(H)(5) <- 戻り値(H)(5) | 0x80000000
	ld.w	%r4, %r6				;戻り値(L)(4) <- 引数１(L)(6)

	ret
