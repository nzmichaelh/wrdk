;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : flosidf.s
;*
;*	Change type: Signed single integer --> Double float
;*		input: %r6
;*		output: (%r5, %r4)
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/15  O.Hinokuchi
;*  			 ・レジスタ置換
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __floatsidf

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット(0(+) or 1(-))
;	%r1:指数部(8bit)
;	%r4:戻り値[L](double)
;	%r5:戻り値[H](double)
;	%r6:引数１(int)/loop counter
;	%r8:temp
;	%r9:count/shift
;==============================================

__floatsidf:
	pushn	%r1				; %r0〜%r1 待避

	ld.w	%r5, 0			; 戻り値[H](%r5) <- 0
	
	cmp		%r6, 0			; if 引数１(%r6) = 0 then goto end
	jreq.d	end
	ld.w	%r4, 0			; 戻り値[L](%r4) <- 0

	ld.w	%r0, 0			; 符号ビット(%r0) <- 0
	ld.w	%r5, %r6		; 戻り値[H](%r5) <- 引数１(%r6)
	ld.w	%r8, %r5		; temp(%r8) <- 戻り値[H](%r5)
	
	cmp		%r5, 0			; if 戻り値[H](%r5) >= 0 then goto count
	jrge.d	count
	ld.w	%r6, 0			; loop counter(%r6) <- 0 

	; case: result1 < 0
	not		%r5, %r5		; 戻り値[H](%r5) = ~戻り値[H](%r5) + 1
	add		%r5, 1
	ld.w	%r0, 1			; 符号ビット(%r0) <- 1
	ld.w	%r8, %r5		; temp(%r8) <- 戻り値(%r4)

count:						; 最上位ビット位置検索
	scan1	%r9, %r8		; count(%r9) <- temp(8)[b31〜b24]の最上位ビット位置   
	jruge	continue		; 最上位ビットが見つかったら->continue
	add		%r6, 8			; loop counter(%r6) = loop counter(%r6) + 8
	jp.d	count
	sll		%r8, 8			; temp(%r8) << 8 

continue:
	add		%r9, %r6		; count(%r9) = count(%r9) + loop counter(%r6)
	xld.w	%r1, 31			; 指数部(%r1) = 31 - count(%r9) 
	sub		%r1, %r9

	; normalize result
	add		%r9, 1			; shift(%r9) = count(%r9) + 1

	;xsll	%r5, %r9		; result1 << count + 1 (clear implied bit)
	; variable shift routine (faster than xsrl)
shift:
	cmp		%r9, 8			; if shift <= 8 then goto done
	jrle	done
	
	; case: shift > 8
	sub		%r9, 8			; shift(%r9) = shift(%r9) - 8
	jp.d	shift
	sll		%r5, 8			; 戻り値[H](%r5) << 8

done:						; case: shift(%r9) <= 8
	sll		%r5, %r9		; 戻り値[H](%r5) << shift(%r9) 

	ld.w	%r4, %r5		; 戻り値[L](%r4) <- 戻り値[H](%r5)
	xsrl	%r5, 12			; 戻り値[H](%r5) >> 12
	xsll	%r4, 20			; 戻り値[L](%r4) << 20

	; finishing steps
	xadd	%r1, 0x3ff		; 指数部(%r1) = 指数部(%r1) + 0x3ff(bias) (EXT33 Ve%r2)

	xrr		%r1, 12			; position exponent bits
	or		%r5, %r1		; 戻り値[H](%r5) | 指数部(%r1) 

	rr		%r0, 1			; position sign bit
	or		%r5, %r0		; 戻り値[H](%r5) | 符号ビット

end:
	popn	%r1				; %r0〜%r1 復帰
	ret
