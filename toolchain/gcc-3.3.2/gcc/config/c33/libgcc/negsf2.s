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
;*  �ѹ�		2001/01/15  O.Hinokuchi
;*  			 ���쥸�����ִ�
;*	gas�б�		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __negsf2

;==============================================
;��쥸����������
;	%r4:�����(floart)
;	%r6:������(floart)
;==============================================

__negsf2:

	; flip the MSB of %r6 and put result in %r4
	ld.w	%r4,%r6					;�����(4) <- ������(6)
	xxor	%r4, 0x80000000			;�����(4) <- �����(4) | 0x80000000

	ret
