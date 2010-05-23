;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : scan64.s
;*
;*	64-bit scan function
;*		input: %r8, %r9
;*		output: %r9
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/31  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __scan64

__scan64:

	pushn	%r1				; save register values

	ld.w	%r0, 0			; loop counter = 0

	cmp		%r8, 0			; if high 32-bits != 0 then count
	jrne	loop

	; case: %r8 = 0
	ld.w	%r8, %r9		; count low 32-bits instead
	xld.w	%r0, 32			; loop counter = 32

loop:						; count ; of leading 0's
	scan1	%r1, %r8		; %r1 = count
	jruge	end				; if count != 8 then goto end
	cmp		%r0, 24
	jreq.d	end				; if count = 32 then jump to end
	add		%r0, 8			; increment loop counter
	jp.d	loop
	sll		%r8, 8			; shift register to the left 8 bits

end:
	add		%r1, %r0		; count = count + loop counter
	ld.w	%r9, %r1		; put result into output register

	popn	%r1				; restore register values
	ret
