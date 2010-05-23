;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : flosidf.s
;*
;*	Change type: Signed single integer --> Double float
;*		input: %r6
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
.global __floatsidf

;==============================================
;��쥸����������
;	%r0:���ӥå�(0(+) or 1(-))
;	%r1:�ؿ���(8bit)
;	%r4:�����[L](double)
;	%r5:�����[H](double)
;	%r6:������(int)/loop counter
;	%r8:temp
;	%r9:count/shift
;==============================================

__floatsidf:
	pushn	%r1				; %r0��%r1 ����

	ld.w	%r5, 0			; �����[H](%r5) <- 0
	
	cmp		%r6, 0			; if ������(%r6) = 0 then goto end
	jreq.d	end
	ld.w	%r4, 0			; �����[L](%r4) <- 0

	ld.w	%r0, 0			; ���ӥå�(%r0) <- 0
	ld.w	%r5, %r6		; �����[H](%r5) <- ������(%r6)
	ld.w	%r8, %r5		; temp(%r8) <- �����[H](%r5)
	
	cmp		%r5, 0			; if �����[H](%r5) >= 0 then goto count
	jrge.d	count
	ld.w	%r6, 0			; loop counter(%r6) <- 0 

	; case: result1 < 0
	not		%r5, %r5		; �����[H](%r5) = ~�����[H](%r5) + 1
	add		%r5, 1
	ld.w	%r0, 1			; ���ӥå�(%r0) <- 1
	ld.w	%r8, %r5		; temp(%r8) <- �����(%r4)

count:						; �Ǿ�̥ӥåȰ��ָ���
	scan1	%r9, %r8		; count(%r9) <- temp(8)[b31��b24]�κǾ�̥ӥåȰ���   
	jruge	continue		; �Ǿ�̥ӥåȤ����Ĥ��ä���->continue
	add		%r6, 8			; loop counter(%r6) = loop counter(%r6) + 8
	jp.d	count
	sll		%r8, 8			; temp(%r8) << 8 

continue:
	add		%r9, %r6		; count(%r9) = count(%r9) + loop counter(%r6)
	xld.w	%r1, 31			; �ؿ���(%r1) = 31 - count(%r9) 
	sub		%r1, %r9

	; normalize result
	add		%r9, 1			; shift(%r9) = count(%r9) + 1

	;xsll	%r5, %r9		; result1 << count + 1 (clear implied bit)
	; variable shift routine (faster than xsrl)
shift:
	cmp		%r9, 8			; if shift <= 8 then goto done
	jrle	done
	
	; case: shift > 8
	sub		%r9, 8			; shift(%r9) = shift(%r9) - 8
	jp.d	shift
	sll		%r5, 8			; �����[H](%r5) << 8

done:						; case: shift(%r9) <= 8
	sll		%r5, %r9		; �����[H](%r5) << shift(%r9) 

	ld.w	%r4, %r5		; �����[L](%r4) <- �����[H](%r5)
	xsrl	%r5, 12			; �����[H](%r5) >> 12
	xsll	%r4, 20			; �����[L](%r4) << 20

	; finishing steps
	xadd	%r1, 0x3ff		; �ؿ���(%r1) = �ؿ���(%r1) + 0x3ff(bias) (EXT33 Ve%r2)

	xrr		%r1, 12			; position exponent bits
	or		%r5, %r1		; �����[H](%r5) | �ؿ���(%r1) 

	rr		%r0, 1			; position sign bit
	or		%r5, %r0		; �����[H](%r5) | ���ӥå�

end:
	popn	%r1				; %r0��%r1 ����
	ret
