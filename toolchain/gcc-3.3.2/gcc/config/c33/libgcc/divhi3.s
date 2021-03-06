;
;	Copyright (C) SEIKO EPSON CORP. 1996
;
;	Filename : divhi3.s
;	Function :
;	  This module defines the functions
;		that emulate signed and unsigned integer division.
;	Revision :
;	  10/18/1996	ESD T.Katahira		start			    */
;	  06/11/2002	watanabe		for divhi3
;
;
;	Function :	__divhi3
;	Input :		%r6	--- dividend
;			%r7	--- divisor
;	Output :	%r4	--- quotient
;	Function :	calculate signed integer division


	.section .text
	.align 1
	.global	__divhi3
__divhi3:
	xsll	%r6, 16
	ld.w	%alr,%r6	; set dividend to accumlator (%alr)
	div0s	%r7		; initializer of signed division
;ifdef	FAST
	div1	%r7		; execute division ;1
	div1	%r7		; execute division ;2
	div1	%r7		; execute division ;3
	div1	%r7		; execute division ;4
	div1	%r7		; execute division ;5
	div1	%r7		; execute division ;6
	div1	%r7		; execute division ;7
	div1	%r7		; execute division ;8
	div1	%r7		; execute division ;9
	div1	%r7		; execute division ;10
	div1	%r7		; execute division ;11
	div1	%r7		; execute division ;12
	div1	%r7		; execute division ;13
	div1	%r7		; execute division ;14
	div1	%r7		; execute division ;15
	div1	%r7		; execute division ;16
;else
;	ld.w	%r8,0x2		; set loop counter (N = 2)
;	ld.w	%r9,%psr	; save flag register
;__divhi3_loop_start:	
;	div1	%r7		; execute division ;1
;	div1	%r7		; execute division ;2
;	div1	%r7		; execute division ;3
;	div1	%r7		; execute division ;4
;	div1	%r7		; execute division ;5
;	div1	%r7		; execute division ;6
;	div1	%r7		; execute division ;7
;	div1	%r7		; execute division ;8
;	sub	%r8,0x1		; decrement loop counter
;	jrne.d	__divhi3_loop_start	; if (loop counter != 0) goto loop top
;	ld.w	%psr,%r9	; restore flag register (delayed slot)
;endif
	div2s	%r7		; post divistion process ;1
	div3s			; post divistion process ;2
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%alr	; set quotient to return reg (delayed slot)


;	Function :	__udivhi3
;	Input :		%r6	--- dividend
;			%r7	--- divisor
;	Output :	%r4	--- quotient
;	Function :	calculate unsigned integer division

	.section .text
	.align 1
	.global	__udivhi3
__udivhi3:
	xsll	%r6, 16
	ld.w	%alr,%r6	; set dividend to accumlator (%alr)
	div0u	%r7		; initializer of signed division
;ifdef	FAST
	div1	%r7		; execute division ;1
	div1	%r7		; execute division ;2
	div1	%r7		; execute division ;3
	div1	%r7		; execute division ;4
	div1	%r7		; execute division ;5
	div1	%r7		; execute division ;6
	div1	%r7		; execute division ;7
	div1	%r7		; execute division ;8
	div1	%r7		; execute division ;9
	div1	%r7		; execute division ;10
	div1	%r7		; execute division ;11
	div1	%r7		; execute division ;12
	div1	%r7		; execute division ;13
	div1	%r7		; execute division ;14
	div1	%r7		; execute division ;15
	div1	%r7		; execute division ;16
;else
;	ld.w	%r8,0x2		; set loop counter (N = 2)
;__udivhi3_loop_start:	
;	div1	%r7		; execute division ;1
;	div1	%r7		; execute division ;2
;	div1	%r7		; execute division ;3
;	div1	%r7		; execute division ;4
;	div1	%r7		; execute division ;5
;	div1	%r7		; execute division ;6
;	div1	%r7		; execute division ;7
;	div1	%r7		; execute division ;8
;	sub	%r8,0x1		; decrement loop counter
;	jrne	__udivhi3_loop_start	; if (loop counter != 0) goto loop top
;endif
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%alr	; set quotient to return reg (delayed slot)

