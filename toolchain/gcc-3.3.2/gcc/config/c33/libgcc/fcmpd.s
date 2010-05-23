;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : fcmpd.s
;*
;*	Double floating point compare
;*		input: (%r7, %r6) & (%r9, %r8)
;*		output: %psr
;*
;*	Begin		1996/10/30	V. Chan
;*  �ѹ�		2001/01/15  O.Hinokuchi
;*  			 ���쥸�����ִ�
;*	gas�б�		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __fcmpd

;==============================================
;��쥸����������
;	%r2:�ؿ�����
;	%r4:���ӥåȣ�[0(+) or 1(-)]
;	%r5:���ӥåȣ�[0(+) or 1(-)]/�ؿ�����/mask
;	%r6:������[L](double)/��������[L]
;	%r7:������[H](double)/��������[H]
;	%r8:������[L](double)/��������[L]
;	%r9:������[H](double)/��������[H]
;==============================================

__fcmpd:
	pushn	%r2				; %r0��%r2 ����

	ld.w	%r4, %r7		; ���ӥåȣ�(%r4) <- ������[H](%r7)
	rl		%r4, 1			; rotate left 1 bit
	and		%r4, 1			; use mask to keep LSB

	ld.w	%r5, %r9		; ���ӥåȣ�(%r4) <- ������[H](%r9)
	rl		%r5, 1			; rotate left 1 bit
	and		%r5, 1			; use mask to keep LSB

	; if ���ӥåȣ�(%r4) = 1 and ���ӥåȣ�(%r5) = 0 then CVZN = 1001 (lt)
	; if ���ӥåȣ�(%r4) = 0 and ���ӥåȣ�(%r5) = 1 then CVZN = 0000 (gt)
	cmp		%r5, %r4		; if ���ӥåȣ�(%r5) != ���ӥåȣ�(%r4) 
	jrne	end				; %psr is changed

	; case: ���ӥåȣ�(%r4) = ���ӥåȣ�(%r5) 
	ld.w	%r5, %r7		; �ؿ�����(%r5) <- ������[H](%r7)
	sll		%r5, 1			; �ؿ�����(%r5) << 1
	xsrl 	%r5, 21			; �ؿ�����(%r5) >> 21

	ld.w	%r2, %r9		; �ؿ�����(%r2) <- ������[H](%r9)
	sll		%r2, 1			; �ؿ�����(%r2) << 1
	xsrl 	%r2, 21			; �ؿ�����(%r2) >> 21

	cmp		%r4, 1			; if ���ӥåȣ�(%r4) = 1 
	jreq	negexp			; then goto negexp

	; max. �ؿ��� = 0x7ff
	; min. �ؿ��� = 0x00
	cmp		%r5, %r2		; if �ؿ�����(%r5) != �ؿ�����(%r2) 
	jrne	end				; then goto end
	jp		mancmp

negexp:
	cmp		%r2, %r5		; if �ؿ�����(%r2) !=  �ؿ�����(%r5)
	jrne	end				; then goto end

mancmp:
	xcmp	%r5, 0x7ff		; if �ؿ�����(%r5) = �ؿ�����(%r2) = �ü��(0xff) 
	jreq	end				; then goto end

	; case: �ؿ�����(%r5) = �ؿ�����(%r2)
	xld.w	%r5, 0xfffff	; mask(%r5) <- 0x7fffff

	and		%r7, %r5		; ��������(%r7) = ��������(%r7) & mask(%r5)
	and		%r9, %r5		; ��������(%r9) = ��������(%r9) & mask(%r5)

	cmp		%r4, 1			; if ���ӥåȣ�(%r4) = 1 
	jreq	negman

	cmp		%r7, %r9		; compare ��������[H](%r7) : ��������[H](%r9) 
	jrne	end

	; case: man1 = man2
	cmp		%r6, %r8		; compare ��������[L](%r6) : ��������[L](%r8) 
	jp		end

negman:
	cmp		%r9, %r7		; compare ��������[H](%r9) : ��������[H](%r7) 
	jrne	end

	cmp		%r8, %r6		; compare ��������[L](%r6) : ��������[L](%r8) 
	
end:
	popn	%r2				; %r0��%r2 ����
	ret
