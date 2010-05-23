;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : trncdfsf.s
;*
;*	Change type: Double float --> Single float
;*		input: (%r7, %r6)
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/17  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __truncdfsf2

__truncdfsf2:

	pushn	%r1				; save register values

	ld.w	%r0, %r7		; put sign of input (%r7) into %r0
	rl		%r0, 1
	and		%r0, 1

	ld.w	%r1, %r7		; put exponent into %r1
	sll		%r1, 1
	xsrl 	%r1, 21

							; 0x368 - 0x3ff(bias) + 0x7f(bias) = -24
	xcmp	%r1, 0x368		; if exp < 0x368 then goto end
	jrle.d	end				; round to zero case (result = 0)
	ld.w	%r4, 0

							; 0x47f - 0x3ff(bias) + 0x7f(bias) = 255
	xcmp	%r1, 0x47f		; if exp >= 0x47f then overflow
	xld.w	%r4, 0x7f800000	; result = infinity
	jrge	end
	
	; isolate mantissa
	xand	%r7,0xfffff		; clear first 12 bits of %r7
	
							; 0x380 - 0x3ff + 0x7f = 0
	xcmp	%r1, 0x380		; if exp > 0x380 then goto normal
	jrgt	normal			; normal result

	; case: exp <= 0x380 (-127 --> denormal result)
	xoor	%r7,0x100000	; add implied bit
							; 0x37e - 0x3ff + 0x7f = -2 (2 bit shift)
	xcmp	%r1, 0x37e		; if 0x380 > exp > 0x37e then goto shftleft
	jrgt	shftleft

	; case: 0x368 =< exp <= 0x37e
	xld.w	%r5, 0x37e		; shift = 0x37e - exp
	sub		%r5, %r1
	
	;xsrl	%r7, %r5		; mantissa >> 0x37e - exp (max. shift = 22)
	; variable shift routine (faster than xsrl)
shift:
	cmp		%r5, 8			; if shift <= 8 then goto done
	jrle	done
	
	; case: shift > 8
	sub		%r5, 8			; shift = shift - 8
	jp.d	shift
	srl		%r7, 8			; result >> 8

done:						; case: %r5 < 8
	jp.d	finish
	srl		%r7, %r5		; final shift of result

shftleft:
	; case: 0x37e < exp <= 0x380
	xsub	%r1,0x37e		; shift = exp - 0x37e (max. shift = 2)

	; {man1, man1_2} << shift
	sll		%r7, %r1		; shift high 32-bits to the left x bits (x = shift)
	rl		%r6, %r1		; rotate low 32-bits to the left x bits
	ld.w	%r8, -1			; %r8 = 0xffff ffff
	sll		%r8, %r1		; make a mask for first 32-x bits --> %r8 = 111...000
	not		%r9, %r8		; flip mask for last x bits --> %r9 = 000...111 (mask)
	and		%r9, %r6		; isolate last x bits of %r6
	jp.d	finish
	or		%r7, %r9		; add last x bits of %r6 to %r7

normal:
	; case: exp > 0x380 (normal single float result)
	xsub	%r1,0x380		; exp = exp - 0x3ff + 0x7f

	; {man1, man1_2} << 3
	sll		%r7, 3			; shift high 32-bits to the left 3 bits
	rl		%r6, 3			; rotate low 32-bits to the left 3 bits
	ld.w	%r5, 0x7		; make a mask for last 3 bits --> %r5 = 000...00111
	not		%r8, %r5		; flip mask for first 29 bits --> %r8 = 111...11000
	and		%r5, %r6		; isolate last 3 bits of %r6
	or		%r7, %r5		; add last 3 bits of %r6 to %r7

	xrr		%r1, 9			; position exp
	or		%r7, %r1		; add exponent

finish:
	ld.w	%r4, %r7		; put result in output register

end:
	rr		%r0, 1			; position sign bit
	or		%r4, %r0		; add sign bit

	popn	%r1				; restore register values

	ret
