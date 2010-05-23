;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : flosisf.s
;*
;*	Change type: Signed single integer --> Single float
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
.global __floatsisf

;==============================================
;��쥸����������
;	%r0:���ӥå�(0(+) or 1(-))
;	%r1:�ؿ���(8bit)
;	%r4:�����(float)
;	%r6:������(int)/loop counter
;	%r8:temp
;	%r9:count/shift
;==============================================

__floatsisf:

	pushn	%r1				; %r0��%r1 ����

	cmp		%r6, 0			; if ������(6) = 0 then goto end
	jreq.d	end
	ld.w	%r4, 0			; �����(4) <- 0 

	ld.w	%r0, 0			; ���ӥå�(%r0) <- 0
	ld.w	%r4, %r6		; �����(%r4) <- ������(6)
	ld.w	%r8, %r6		; count(%r8) <- ������(6) 

	cmp		%r4, 0			; if �����(%r4) >= 0 then goto count
	jrge.d	count
	ld.w	%r6, 0			; loop counter(%r6) <- 0 

	; case: �����(%r4) < 0
	not		%r4, %r4		; �����(%r4) = ~�����(%r4) + 1
	add		%r4, 1
	ld.w	%r0, 1			; ���ӥå�(%r0) <- 1
	ld.w	%r8, %r4		; temp(%r8) <- �����(%r4)

count:						; �Ǿ�̥ӥåȰ��ָ���
	scan1	%r9, %r8		; count(%r9) <- temp(8)[b31��b24]�κǾ�̥ӥåȰ���   
	jruge	continue		; �Ǿ�̥ӥåȤ����Ĥ��ä���->continue
	add		%r6, 8			; loop counter(%r6) = loop counter(%r6) + 8
	jp.d	count
	sll		%r8, 8			; temp(%r8) << 8 

continue:
	add		%r9, %r6		; count(%r9) = count(%r9) + loop counter(%r6)
	ld.w	%r1, 31			; �ؿ���(%r1) = 31 - count(%r9) 
	sub		%r1, %r9

	; normalize result
	add		%r9, 1			; shift(%r9) = count(%r9) + 1
	
	;xsll	%r4, %r7		; result << count + 1 (clear implied bit)
	; variable shift routine (faster than xsrl)
shift:
	cmp		%r9, 8			; if shift(%r9) <= 8 then goto done
	jrle	done
	
	; case: shift(%r9) > 8
	sub		%r9, 8			; shift(%r9) = shift(%r9) - 8
	jp.d	shift
	sll		%r4, 8			; �����(%r4) << 8

done:						; case: shift(%r9) < 8
	sll		%r4, %r9		; �����(%r4) << shift(%r9) 

	xsrl	%r4, 9			; �����(%r4) >> 9 (normal position)

	; finishing steps
	xadd	%r1, 0x7f		; �ؿ���(%r1) = �ؿ���(%r1) + 0x7f(bias) (EXT33 Ve%r2)

	xrr		%r1, 9			; position exponent bits
	or		%r4, %r1		; �����(%r4) | �ؿ���(%r1) 

	rr		%r0, 1			; position sign bit
	or		%r4, %r0		; �����(%r4) | ���ӥå�

end:
	popn	%r1				; %r0��%r1 ����
	ret
