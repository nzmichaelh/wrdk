;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : fixsfui.s
;*
;*	Change type: Single float --> Unsigned integer
;*							  --> Signed integer
;*		input: %r6
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  �ѹ�		2001/01/17  O.Hinokuchi
;*  			 ���쥸�����ִ�
;*				 ��__fixunssfsi �ե�����ʬ��Τ����� 
;*	gas�б�		2001/10/15	watanabe
;*********************************************

.section .text
.align 1
.global __fixsfsi

;==============================================
;��쥸����������
;	%r4:�����(int or uint)
;	%r5:���ӥå�(0(+) or 1(-))
;	%r6:������(float)
;	%r7:�ؿ���(8bit)
;	%r8:shift
;
;==============================================

__fixsfsi:

	ld.w	%r5, %r6		; ���ӥå�(%r5) <- ������(%r6)  
	rl		%r5, 1			; ���ӥå�(%r5) >> 31 ��Ʊ��
	and		%r5, 1			; ���ӥå�(%r5) = ���ӥå�(%r5) & 1

	ld.w	%r7, %r6		; �ؿ���(%r7) <- ������(%r6) 
	sll		%r7, 1			; �ؿ���(%r7) << 1
	xsrl 	%r7, 24			; �ؿ���(%r7) >> 24		
							;
							;
							;							
	xcmp	%r7, 0x7f		; if �ؿ���(%r7) < 0x7f then goto end
	jrlt.d	end				; round to zero case (integer underflow)
	ld.w	%r4, 0			; �����(%r4) <- 0 

	xcmp	%r7, 0x9f		; if �ؿ���(%r7) >= 0x9f then integer overflow
	ld.w	%r4, 1			; �����(%r4) <- 1
	jrge.d	overflow
	rr		%r4, 1			; �����(%r4) <- 0x8000 0000

	; isolate mantissa
	ld.w	%r4,%r6			; �����(%r4) <- ������(%r6) 
	xand	%r4,0x7fffff	; �����(%r4) <- �����(%r4) & 0x7fffff) clear first 9 bits of %r6
	xoor	%r4,0x800000	; �����(%r4) <- (�����(%r4) | 0x800000) add implied bit

	xld.w	%r8, 0x96		;shift(%r8) <- 0x96

	cmp		%r7, %r8		; 0x7f + 31 - 8 = 0x96
	jrgt	shftleft		; if �ؿ���(%r7) > 0x96 then goto shftleft

	; case: exp <= 0x96
	sub		%r8, %r7		; shift(%r8) = shift(%r8=0x96) - �ؿ���(%r7) (max = 23)

	; 32-bit variable right shift routine (faster than xsrl)
shift:
	cmp		%r8, 8			; if shift(%r8) <= 8 then goto done
	jrle	done
	
	; case: shift > 8
	sub		%r8, 8			; shift(%r8) = shift(%r8) - 8
	jp.d	shift
	srl		%r4, 8			; �����(%r4) >> 8

done:						; case: shift(%r8) < 8
	jp.d	finish
	srl		%r4, %r8		; �����(%r4) >> shift(%r8) final shift of result
	
shftleft:
	sub		%r7, %r8		; �ؿ���(%r7) = �ؿ���(%r7) - shift(%r8=0x96) (max = 8)
		
	sll		%r4, %r7		; �����(%r4) >> �ؿ���(%r7) shift mantissa to the left 8-shift bits

finish:
	cmp		%r5, 0			; if ���ӥå�(%r5) = 0 then goto end
	jreq	end

	; case: sign = 1
	not		%r4, %r4		; �����(%r4) = ~�����(%r4)
	jp.d	end
	add		%r4, 1			; �����(%r4) = �����(%r4) + 1 (�������)

overflow:
	cmp		%r5, 0			; check sign
	jrne	end				; if ���ӥå�(%r5) = 1 then goto end
	not		%r4, %r4		; �����(%r4) <- 0xffffffff

end:
	ret
