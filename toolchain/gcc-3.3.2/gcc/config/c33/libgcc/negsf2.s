;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : negsf2.s
;*
;*	Single floating point negate function
;*		input: %r6
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/15  O.Hinokuchi
;*  			 ・レジスタ置換
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __negsf2

;==============================================
;＜レジスタ説明＞
;	%r4:戻り値(floart)
;	%r6:引数１(floart)
;==============================================

__negsf2:

	; flip the MSB of %r6 and put result in %r4
	ld.w	%r4,%r6					;戻り値(4) <- 引数１(6)
	xxor	%r4, 0x80000000			;戻り値(4) <- 戻り値(4) | 0x80000000

	ret
