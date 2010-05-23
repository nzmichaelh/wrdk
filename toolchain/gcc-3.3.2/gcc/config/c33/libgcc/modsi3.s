;	Function :	__modsi3
;	Input :		%r6	--- dividend
;			%r7	--- divisor
;	Output :	%r4	--- remainder
;	Function :	calculate signed integer modulo arithmetic

	.section .text
	.align 1
	.global	__modsi3
__modsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (%alr)
	div0s	%r7		; initializer of signed division
;ifdef	FAST
;	div1	%r7		; execute division ;1
;	div1	%r7		; execute division ;2
;	div1	%r7		; execute division ;3
;	div1	%r7		; execute division ;4
;	div1	%r7		; execute division ;5
;	div1	%r7		; execute division ;6
;	div1	%r7		; execute division ;7
;	div1	%r7		; execute division ;8
;	div1	%r7		; execute division ;9
;	div1	%r7		; execute division ;10
;	div1	%r7		; execute division ;11
;	div1	%r7		; execute division ;12
;	div1	%r7		; execute division ;13
;	div1	%r7		; execute division ;14
;	div1	%r7		; execute division ;15
;	div1	%r7		; execute division ;16
;	div1	%r7		; execute division ;17
;	div1	%r7		; execute division ;18
;	div1	%r7		; execute division ;19
;	div1	%r7		; execute division ;20
;	div1	%r7		; execute division ;21
;	div1	%r7		; execute division ;22
;	div1	%r7		; execute division ;23
;	div1	%r7		; execute division ;24
;	div1	%r7		; execute division ;25
;	div1	%r7		; execute division ;26
;	div1	%r7		; execute division ;27
;	div1	%r7		; execute division ;28
;	div1	%r7		; execute division ;29
;	div1	%r7		; execute division ;30
;	div1	%r7		; execute division ;31
;	div1	%r7		; execute division ;32
;else
	ld.w	%r8,0x4		; set loop counter (N = 4)
	ld.w	%r9,%psr	; save flag register
__modsi3_loop_start:	
	div1	%r7		; execute division ;1
	div1	%r7		; execute division ;2
	div1	%r7		; execute division ;3
	div1	%r7		; execute division ;4
	div1	%r7		; execute division ;5
	div1	%r7		; execute division ;6
	div1	%r7		; execute division ;7
	div1	%r7		; execute division ;8
	sub	%r8,0x1		; decrement loop counter
	jrne.d	__modsi3_loop_start	; if (loop counter != 0) goto loop top
	ld.w	%psr,%r9	; restore flag register (delayed slot)
;endif
	div2s	%r7		; post divistion process ;1
	div3s			; post divistion process ;2
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%ahr	; set remainder to return reg (delayed slot)


;	Function :	__umodsi3
;	Input :		%r6	--- dividend
;			%r7	--- divisor
;	Output :	%r4	--- remainder
;	Function :	calculate unsigned integer modulo arithmetic

	.section .text
	.align 1
	.global	__umodsi3
__umodsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (%alr)
	div0u	%r7		; initializer of signed division
;ifdef	FAST
;	div1	%r7		; execute division ;1
;	div1	%r7		; execute division ;2
;	div1	%r7		; execute division ;3
;	div1	%r7		; execute division ;4
;	div1	%r7		; execute division ;5
;	div1	%r7		; execute division ;6
;	div1	%r7		; execute division ;7
;	div1	%r7		; execute division ;8
;	div1	%r7		; execute division ;9
;	div1	%r7		; execute division ;10
;	div1	%r7		; execute division ;11
;	div1	%r7		; execute division ;12
;	div1	%r7		; execute division ;13
;	div1	%r7		; execute division ;14
;	div1	%r7		; execute division ;15
;	div1	%r7		; execute division ;16
;	div1	%r7		; execute division ;17
;	div1	%r7		; execute division ;18
;	div1	%r7		; execute division ;19
;	div1	%r7		; execute division ;20
;	div1	%r7		; execute division ;21
;	div1	%r7		; execute division ;22
;	div1	%r7		; execute division ;23
;	div1	%r7		; execute division ;24
;	div1	%r7		; execute division ;25
;	div1	%r7		; execute division ;26
;	div1	%r7		; execute division ;27
;	div1	%r7		; execute division ;28
;	div1	%r7		; execute division ;29
;	div1	%r7		; execute division ;30
;	div1	%r7		; execute division ;31
;	div1	%r7		; execute division ;32
;else
	ld.w	%r8,0x4		; set loop counter (N = 4)
__umodsi3_loop_start:	
	div1	%r7		; execute division ;1
	div1	%r7		; execute division ;2
	div1	%r7		; execute division ;3
	div1	%r7		; execute division ;4
	div1	%r7		; execute division ;5
	div1	%r7		; execute division ;6
	div1	%r7		; execute division ;7
	div1	%r7		; execute division ;8
	sub	%r8,0x1		; decrement loop counter
	jrne	__umodsi3_loop_start	; if (loop counter != 0) goto loop top
;endif
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%ahr	; set remainder to return reg (delayed slot)

