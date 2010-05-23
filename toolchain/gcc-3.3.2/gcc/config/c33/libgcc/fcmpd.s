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
;*  変更		2001/01/15  O.Hinokuchi
;*  			 ・レジスタ置換
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __fcmpd

;==============================================
;＜レジスタ説明＞
;	%r2:指数部２
;	%r4:符号ビット１[0(+) or 1(-)]
;	%r5:符号ビット２[0(+) or 1(-)]/指数部１/mask
;	%r6:引数１[L](double)/仮数部１[L]
;	%r7:引数１[H](double)/仮数部１[H]
;	%r8:引数２[L](double)/仮数部２[L]
;	%r9:引数２[H](double)/仮数部２[H]
;==============================================

__fcmpd:
	pushn	%r2				; %r0〜%r2 待避

	ld.w	%r4, %r7		; 符号ビット１(%r4) <- 引数１[H](%r7)
	rl		%r4, 1			; rotate left 1 bit
	and		%r4, 1			; use mask to keep LSB

	ld.w	%r5, %r9		; 符号ビット２(%r4) <- 引数２[H](%r9)
	rl		%r5, 1			; rotate left 1 bit
	and		%r5, 1			; use mask to keep LSB

	; if 符号ビット１(%r4) = 1 and 符号ビット２(%r5) = 0 then CVZN = 1001 (lt)
	; if 符号ビット１(%r4) = 0 and 符号ビット２(%r5) = 1 then CVZN = 0000 (gt)
	cmp		%r5, %r4		; if 符号ビット２(%r5) != 符号ビット１(%r4) 
	jrne	end				; %psr is changed

	; case: 符号ビット１(%r4) = 符号ビット２(%r5) 
	ld.w	%r5, %r7		; 指数部１(%r5) <- 引数１[H](%r7)
	sll		%r5, 1			; 指数部１(%r5) << 1
	xsrl 	%r5, 21			; 指数部１(%r5) >> 21

	ld.w	%r2, %r9		; 指数部２(%r2) <- 引数２[H](%r9)
	sll		%r2, 1			; 指数部２(%r2) << 1
	xsrl 	%r2, 21			; 指数部２(%r2) >> 21

	cmp		%r4, 1			; if 符号ビット１(%r4) = 1 
	jreq	negexp			; then goto negexp

	; max. 指数部 = 0x7ff
	; min. 指数部 = 0x00
	cmp		%r5, %r2		; if 指数部１(%r5) != 指数部２(%r2) 
	jrne	end				; then goto end
	jp		mancmp

negexp:
	cmp		%r2, %r5		; if 指数部２(%r2) !=  指数部１(%r5)
	jrne	end				; then goto end

mancmp:
	xcmp	%r5, 0x7ff		; if 指数部１(%r5) = 指数部２(%r2) = 特殊数(0xff) 
	jreq	end				; then goto end

	; case: 指数部１(%r5) = 指数部２(%r2)
	xld.w	%r5, 0xfffff	; mask(%r5) <- 0x7fffff

	and		%r7, %r5		; 仮数部１(%r7) = 仮数部１(%r7) & mask(%r5)
	and		%r9, %r5		; 仮数部２(%r9) = 仮数部２(%r9) & mask(%r5)

	cmp		%r4, 1			; if 符号ビット１(%r4) = 1 
	jreq	negman

	cmp		%r7, %r9		; compare 仮数部１[H](%r7) : 仮数部２[H](%r9) 
	jrne	end

	; case: man1 = man2
	cmp		%r6, %r8		; compare 仮数部１[L](%r6) : 仮数部２[L](%r8) 
	jp		end

negman:
	cmp		%r9, %r7		; compare 仮数部２[H](%r9) : 仮数部１[H](%r7) 
	jrne	end

	cmp		%r8, %r6		; compare 仮数部１[L](%r6) : 仮数部２[L](%r8) 
	
end:
	popn	%r2				; %r0〜%r2 復帰
	ret
