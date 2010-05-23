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
;*  変更		2001/01/15  O.Hinokuchi
;*  			 ・レジスタ置換
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __fcmps

;==============================================
;＜レジスタ説明＞
;	%r2:指数部２
;	%r4:符号ビット１[0(+) or 1(-)]
;	%r5:符号ビット２[0(+) or 1(-)]/指数部１/mask
;	%r6:引数１(float)/仮数部１
;	%r7:引数２(float)/仮数部２
;==============================================


__fcmps:
	pushn	%r2				; %r0〜%r2 待避

	ld.w	%r4, %r6		; 符号ビット１(%r4) <- 引数１(%r6)
	rl		%r4, 1			; rotate left 1 bit
	and		%r4, 1			; keep LSB

	ld.w	%r5, %r7		; 符号ビット２(%r5) <- 引数２(%r7)
	rl		%r5, 1			; rotate left 1 bit
	and		%r5, 1			; keep LSB

	; if 符号ビット１(%r4) = 1 and 符号ビット２(%r5) = 0 then CVZN = 1001 (lt)
	; if 符号ビット１(%r4) = 0 and 符号ビット２(%r5) = 1 then CVZN = 0000 (gt)
	cmp		%r5, %r4		; if 符号ビット２(%r5) != 符号ビット１(%r4) 
	jrne	end				; then goto end

	; case: 符号ビット１(%r4) = 符号ビット２(%r5) 
	ld.w	%r5, %r6		; 指数部１(%r5) <- 引数１(%r6)
	sll		%r5, 1			; 指数部１(%r5) << 1
	xsrl 	%r5, 24			; 指数部１(%r5) >> 24

	ld.w	%r2, %r7		; 指数部２(%r2) <- 引数２(%r7)
	sll		%r2, 1			; 指数部２(%r2) << 1
	xsrl 	%r2, 24			; 指数部２(%r2) >> 24

	cmp		%r4, 1			; if 符号ビット１(%r4) = 1 
	jreq	negexp			; then goto negexp

	; max. 指数部 = 0xff
	; min. 指数部 = 0x00
	cmp		%r5, %r2		; if 指数部１(%r5) != 指数部２(%r2) 
	jrne	end				; then goto end
	jp		mancmp

negexp:
	cmp		%r2, %r5		; if 指数部２(%r2) !=  指数部１(%r5)
	jrne	end				; then goto end

mancmp:
	; check if Inf or NaN values
	xcmp	%r5, 0xff		; if 指数部１(%r5) = 指数部２(%r2) = 特殊数(0xff) 
	jreq	end				; then goto end

	; case: 指数部１(%r5) = 指数部２(%r2)
	xld.w	%r5, 0x7fffff	; mask(%r5) <- 0x7fffff

	and		%r6, %r5		; 仮数部１(%r6) = 仮数部１(%r6) & mask(%r5)
	and		%r7, %r5		; 仮数部２(%r7) = 仮数部２(%r7) & mask(%r5)

	cmp		%r4, 1			; if 符号ビット１(%r4) = 1 
	jreq	negman			; then goto negman

	cmp		%r6, %r7		; compare 仮数部１(%r6) : 仮数部２(%r7) 
	jp		end

negman:
	cmp		%r7, %r6		; compare 仮数部２(%r7) : 仮数部１(%r6) 

end:
	popn	%r2				; %r0〜%r2 復帰
	ret
