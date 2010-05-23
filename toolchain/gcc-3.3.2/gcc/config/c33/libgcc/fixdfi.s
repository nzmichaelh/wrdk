;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : fixdfui.s
;*
;*	Change type: Double float --> Unsigned integer
;*							  --> Signed integer
;*		input: (%r7, %r6)
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/18  O.Hinokuchi
;*  			 ・レジスタ置き換え
;*				 ・__fixunsdfsi ファイル分割のため削除 
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __fixdfsi

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット(0(+) or 1(-))
;	%r4:戻り値(int or uint)
;	%r5:指数部(11bit)
;	%r6:引数２(Double下位32bit)
;	%r7:引数１(Double上位32bit)
;	%r8:mask
;	%r9:shift
;==============================================

__fixdfsi:

	pushn	%r0				; %r0 待避

	ld.w	%r0, %r7		; 符号ビット(%r0) <- 引数２(%r7) put sign of input (%r7) into %r0
	rl		%r0, 1			; 符号ビット(%r0) >> 31 と同じ
	and		%r0, 1			; 符号ビット(%r0) = 符号ビット(%r0) & 1 moved to line 33

	ld.w	%r5, %r7		; 指数部(%r5) <- 引数１(%r7)  put exponent into %r5
	sll		%r5, 1			; 指数部(%r7) << 1
	xsrl 	%r5, 21			; 指数部(%r7) >> 24	

	xcmp	%r5, 0x3ff		; if 指数部(%r7) < 0x3ff then goto end
	jrlt.d	end				; case: round to zero (underflow)
	ld.w	%r4, 0			; 戻り値(%r4) <- 0 clear output register

	xcmp	%r5, 0x41f		; if 戻り値(%r4) >= 0x41f then integer overflow
	ld.w	%r4, 1			; 戻り値(%r4) <- 1
	jrge.d	overflow
	rr		%r4, 1			; 戻り値(%r4) = 0x8000 0000

	; isolate mantissa
	ld.w	%r4, %r7		; 戻り値(%r4) <- 引数１(%r7) 
	xand	%r4, 0xfffff	; 戻り値(%r4) <- 戻り値(%r4) & 0xfffff) clear first 12 bits of %r7
	xoor	%r4, 0x100000	; 戻り値(%r4) <- (戻り値(%r4) | 0x100000) add implied bit
	xld.w	%r9, 0x413		;shift(%r9) <- 0x413

	cmp		%r5, %r9		; 0x3ff + 31 - 11 = 0x413
	jrgt	shftleft		; if 指数部(%r7) > 0x413 then shift left

	; case: exp <= 0x413
	sub		%r9, %r5		; %r9 = 0x413 - exp (max = 20)

	;xsrl		%r4, %r3		; final shift of result
	; 32-bit variable shift routine
L1:
	cmp		%r9, 8			; if shift(%r9)  <= 8 then goto L2
	jrle	L2

	srl		%r4, 8			; 戻り値(%r4) >> 8
	jp.d	L1
	sub		%r9, 8			; shift(%r9)  = shift(%r9)  - 8

L2:
	jp.d	finish
	srl		%r4, %r9		; 戻り値(%r4) >> shift(%r9) last shift

shftleft:
	sub		%r5, %r9		; 指数部(%r5) = 指数部(%r5) - shift(%r9=0x413) (max = 11)
	
	; {result, man1_2} << 0x413-exp
	;xsll	%r4, %r1		; shift high 32-bits to the left x bits (shift amount = x)
	;xrl	%r6, %r1		; rotate low 32-bits to the left x bits
	;xsll	%r2, %r1		; make a mask for first 32-x bits --> %r2 = 111...000

	ld.w	%r8, -1			; mask(%r8) = 0xffff ffff
	; 64-bit variable shift and rotate routine
L3:
	cmp		%r5, 8			; if 指数部(%r5) <= 8 then goto L2
	jrle	L4

	sll		%r4, 8			; 戻り値(%r4) << 8
	rl		%r6, 8			; 引数２(%r6) rotate << 8
	sll		%r8, 8			; mask(%r8) << 8
	jp.d	L3
	sub		%r5, 8			; 指数部(%r5) = 指数部(%r5) - 8 

L4:
	sll		%r4, %r5		; 戻り値(%r4) << 指数部(%r5)
	rl		%r6, %r5		; 引数２(%r6) rotate << 指数部(%r5)
	sll		%r8, %r5		; mask(%r8) << 指数部(%r5)

	not		%r5, %r8		; 指数部(%r5) = ~mask(%r8) flip mask for last x bits --> %r9 = 000...111 (mask)
	and		%r5, %r6		; 指数部(%r5) = 指数部(%r5) & 引数２(%r6) isolate last x bits of %r6
	or		%r4, %r5		; 戻り値(%r4) = 戻り値(%r4) & 指数部(%r5) add last x bits of %r6 to %r4
	and		%r6, %r8		; 引数(%r6) = 引数２(%r6) & mask(%r8) keep the high 32-x bits of %r6

finish:
	cmp		%r0, 0			; if 符号ビット(%r0) = 0 then goto end
	jreq	end
	
	; case: sign = 1
	not		%r4, %r4		; 戻り値(%r4) = ~戻り値(%r4)
	jp.d	end
	add		%r4, 1			; 戻り値(%r4) = 戻り値(%r4) + 1 (２の補数)

overflow:
	cmp		%r0, 0			; check sign
	jrne	end				; if 符号ビット(%r0) = 1 then goto end
	not		%r4, %r4		; 符号ビット(%r0) <- 0xffffffff

end:
	popn	%r0				; %r0 復帰
	ret
