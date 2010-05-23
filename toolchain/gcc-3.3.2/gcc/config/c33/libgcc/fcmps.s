;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : fcmps.s
;*
;*	Single floating point compare
;*		input: %r6 & %r7
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
.global __fcmps

;==============================================
;��쥸����������
;	%r2:�ؿ�����
;	%r4:���ӥåȣ�[0(+) or 1(-)]
;	%r5:���ӥåȣ�[0(+) or 1(-)]/�ؿ�����/mask
;	%r6:������(float)/��������
;	%r7:������(float)/��������
;==============================================


__fcmps:
	pushn	%r2				; %r0��%r2 ����

	ld.w	%r4, %r6		; ���ӥåȣ�(%r4) <- ������(%r6)
	rl		%r4, 1			; rotate left 1 bit
	and		%r4, 1			; keep LSB

	ld.w	%r5, %r7		; ���ӥåȣ�(%r5) <- ������(%r7)
	rl		%r5, 1			; rotate left 1 bit
	and		%r5, 1			; keep LSB

	; if ���ӥåȣ�(%r4) = 1 and ���ӥåȣ�(%r5) = 0 then CVZN = 1001 (lt)
	; if ���ӥåȣ�(%r4) = 0 and ���ӥåȣ�(%r5) = 1 then CVZN = 0000 (gt)
	cmp		%r5, %r4		; if ���ӥåȣ�(%r5) != ���ӥåȣ�(%r4) 
	jrne	end				; then goto end

	; case: ���ӥåȣ�(%r4) = ���ӥåȣ�(%r5) 
	ld.w	%r5, %r6		; �ؿ�����(%r5) <- ������(%r6)
	sll		%r5, 1			; �ؿ�����(%r5) << 1
	xsrl 	%r5, 24			; �ؿ�����(%r5) >> 24

	ld.w	%r2, %r7		; �ؿ�����(%r2) <- ������(%r7)
	sll		%r2, 1			; �ؿ�����(%r2) << 1
	xsrl 	%r2, 24			; �ؿ�����(%r2) >> 24

	cmp		%r4, 1			; if ���ӥåȣ�(%r4) = 1 
	jreq	negexp			; then goto negexp

	; max. �ؿ��� = 0xff
	; min. �ؿ��� = 0x00
	cmp		%r5, %r2		; if �ؿ�����(%r5) != �ؿ�����(%r2) 
	jrne	end				; then goto end
	jp		mancmp

negexp:
	cmp		%r2, %r5		; if �ؿ�����(%r2) !=  �ؿ�����(%r5)
	jrne	end				; then goto end

mancmp:
	; check if Inf or NaN values
	xcmp	%r5, 0xff		; if �ؿ�����(%r5) = �ؿ�����(%r2) = �ü��(0xff) 
	jreq	end				; then goto end

	; case: �ؿ�����(%r5) = �ؿ�����(%r2)
	xld.w	%r5, 0x7fffff	; mask(%r5) <- 0x7fffff

	and		%r6, %r5		; ��������(%r6) = ��������(%r6) & mask(%r5)
	and		%r7, %r5		; ��������(%r7) = ��������(%r7) & mask(%r5)

	cmp		%r4, 1			; if ���ӥåȣ�(%r4) = 1 
	jreq	negman			; then goto negman

	cmp		%r6, %r7		; compare ��������(%r6) : ��������(%r7) 
	jp		end

negman:
	cmp		%r7, %r6		; compare ��������(%r7) : ��������(%r6) 

end:
	popn	%r2				; %r0��%r2 ����
	ret
