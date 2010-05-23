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
;*  �ѹ�		2001/01/15  O.Hinokuchi
;*  			 ���쥸�����ִ�
;*	gas�б�		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __negdf2

;==============================================
;��쥸����������
;	%r4:�����(L)(double)
;	%r5:�����(H)(double)
;	%r6:������(L)(double)
;	%r7:������(H)(double)
;==============================================

__negdf2:

	; flip the MSB of %r5 and put result in %r6
	ld.w	%r5,%r7					;�����(H)(5) <- ������(H)(7)
	xxor	%r5,0x80000000			;�����(H)(5) <- �����(H)(5) | 0x80000000
	ld.w	%r4, %r6				;�����(L)(4) <- ������(L)(6)

	ret
