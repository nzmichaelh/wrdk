;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : extsfdf.s
;*
;*	Change type: Single float --> Double float
;*		input: %r6
;*		output: (%r5, %r4)
;*
;*	Begin					1996/09/12	V. Chan
;*	Fixed bug with variable shift (ln. 108)
;*							1997/02/18	V. Chan
;*  変更		2001/01/17  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __extendsfdf2

__extendsfdf2:

	pushn	%r3				; save register values

	ld.w	%r0, %r6		; put sign of input (%r6) into %r0
	rl		%r0, 1
	and		%r0, 1

	ld.w	%r5, 0			; clear output registers
	sll		%r6, 1			; remove MSB
	srl		%r6, 1

	cmp		%r6, 0			; if input = 0 then exit
	jreq.d	end
	ld.w	%r4, 0

	ld.w	%r1, %r6		; put exponent into %r1
	sll		%r1, 1
	xsrl 	%r1, 24

	xcmp	%r1, 0xff		; if NaN or Inf input then return Inf
	xld.w	%r5, 0x7ff00000
	jrge.d	end
	ld.w	%r4, 0


	; isolate mantissa
	xand	%r6,0x7fffff		; clear first 9 bits of %r6
	
	cmp		%r1, 0
	jrne.d	expnez			; if exp != 0 then goto expnez
	ld.w	%r5, %r6		; result1 = input

	; case: exp = 0
	ld.w	%r9, %r5		; copy new result to %r9 for counting
	ld.w	%r8, 0			; initialize loop counter

count:
	scan1	%r7, %r9		; count = ; of 0's before leading 1 in result
	jruge	continue		; if count !=8 then goto continue
	add		%r8, 8			; add 8 to loop counter
	jp.d	count
	sll		%r9, 8			; shift 8 leading 0's out of %r9

continue:
	add		%r7, %r8		; count = count + loop counter
	xld.w	%r1, 0x380		; exp = 0x380
	sub		%r1, %r7		; exp = exp - count
	;add	%r1, 9			; moved to line 55

	cmp		%r7, 11
	jrge.d	shftleft		; if count >= 11 then goto shftleft
	add		%r1, 9			; exp = exp + 9

	; case: count < 11
	ld.w	%r2, 11
	sub		%r2, %r7		; %r2 = shift = 11 - count

	; {result1, result2} >> 11 - count
	;xsrl	%r4_p, %r2		; shift low 32-bits to the right 11-count bits
	;xrr	%r5_p, %r2		; rotate high 32-bits to the right 11-count bits
	;xsrl	%r3, %r2		; make a mask for last 20-count bits --> %r3 = 000...111

	ld.w	%r3, -1			; %r3 = 0xffff ffff
	; 64-bit variable shift and rotate routine
L1:
	cmp		%r2, 8			; if shift amount <= 8 then goto L2
	jrle	L2

	srl		%r4, 8			; result1 >> 8
	rr		%r5, 8			; result2 rotate >> 8
	srl		%r3, 8			; mask >> 8
	jp.d	L1
	sub		%r2, 8			; shift amount -= 8 

L2:
	srl		%r4, %r2		; result1 >> %r2
	rr		%r5, %r2		; result2 rotate >> %r2
	srl		%r3, %r2		; mask >> %r2

	not		%r8, %r3		; flip mask for first 12-count bits --> %r8 = 111...000 (mask)
	and		%r8, %r5		; isolate first 12-count bits of %r5
	or		%r4, %r8		; add first 12-count bits of %r5 to %r4
	jp.d	finish
	and		%r5, %r3		; keep the low 20-count bits of %r5

shftleft:
	ld.w	%r2, %r7
	sub		%r2, 11			; %r2 = shift = count - 11

	; 	MXSLL	%r5, %r2		; result1 << count - 11
__L0001:
	
;	sub		%sp, 1			
;	ld.w	[%sp+0], $2		; rsスタック待避
	
	and		%r2,0x1f			; 最大シフト数=３１
__L0002:
	cmp		%r2,0x8			; if rs <= 8
	jrle	__L0003				; then $$3
	sll		%r5,0x8			; rd << 8
	jp.d	__L0002
	sub		%r2,0x8			; 残シフト回数計算
__L0003:
	sll		%r5,%r2

;	ld.w	$2, [%sp+0]		; rsスタック復帰
;	add		%sp, 1

	jp		finish

expnez:
	xadd	%r1, 0x380		; exp = exp + 0x380 (0011 1000 0000 in binary)

	; {result1, result2} >> 3
	srl		%r4, 3			; shift low 32-bits to the right 3 bits
	rr		%r5, 3			; rotate high 32-bits to the right 3 bits
	ld.w	%r3, -1			; %r3 = 0xffff ffff
	srl		%r3, 3			; make a mask for last 29 bits --> %r3 = 0001...111
	not		%r8, %r3		; flip mask for first 3 bits --> %r8 = 1110...000
	and		%r8, %r5		; isolate first 3 bits of %r5
	or		%r4, %r8		; add first 3 bits of %r5 to %r4
	jp.d	finish
	and		%r5, %r3		; keep the low 29 bits of %r5

finish:
	xand	%r5, 0xfffff	; remove implied bit

	xrr		%r1, 12			; position exponent bits
	or		%r5, %r1		; add exponent

end:
	rr		%r0, 1			; position sign bit
	or		%r5, %r0		; add sign bit

	popn	%r3				; restore register values


	ret
