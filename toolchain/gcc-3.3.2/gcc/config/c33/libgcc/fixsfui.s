;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : fixsfui.s
;*
;*	Change type: Single float --> Unsigned integer
;*							  --> Signed integer
;*		input: %r6
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/17  O.Hinokuchi
;*  			 ・レジスタ置き換え
;*				 ・ファイル分割  __fixsfsi -> fixsfis.s に移動
;*				 ・マイナスの値のキャストは ０  オーバーフローする時は、
;*                0xffffffffを返すように修正。(gccの処理と同一)
;*	gas対応		2001/10/15	watanabe
;*********************************************

.section .text
.align 1
.global __fixunssfsi

;==============================================
;＜レジスタ説明＞
;	%r4:戻り値(int or uint)
;	%r5:符号ビット(0(+) or 1(-))
;	%r6:引数１(float)
;	%r7:指数部(8bit)
;	%r8:shift
;
;==============================================

__fixunssfsi:

	ld.w	%r4, %r6
	xand	%r4, 0x80000000				; if( 符号ビット == 0 )

	xjreq	start						;    goto __start
										; else
	xld.w	%r4, 0x0					;	return 0
	ret
start:

	ld.w	%r5, %r6		; 符号ビット(%r5) <- 引数１(%r6)  
	rl		%r5, 1			; 符号ビット(%r5) >> 31 と同じ
	and		%r5, 1			; 符号ビット(%r5) = 符号ビット(%r5) & 1

	ld.w	%r7, %r6		; 指数部(%r7) <- 引数１(%r6) 
	sll		%r7, 1			; 指数部(%r7) << 1
	xsrl 	%r7, 24			; 指数部(%r7) >> 24		
							;
							;
							;							
	xcmp	%r7, 0x7f		; if 指数部(%r7) < 0x7f then goto end
	jrlt.d	end				; round to zero case (integer underflow)
	ld.w	%r4, 0			; 戻り値(%r4) <- 0 

	xcmp	%r7, 0x9f		; if 指数部(%r7) >= 0x9f then integer overflow
	ld.w	%r4, 1			; 戻り値(%r4) <- 1
	jrge.d	overflow
	rr		%r4, 1			; 戻り値(%r4) <- 0x8000 0000

	; isolate mantissa
	ld.w	%r4,%r6			; 戻り値(%r4) <- 引数１(%r6) 
	xand	%r4,0x7fffff	; 戻り値(%r4) <- 戻り値(%r4) & 0x7fffff) clear first 9 bits of %r6
	xoor	%r4,0x800000	; 戻り値(%r4) <- (戻り値(%r4) | 0x800000) add implied bit

	xld.w	%r8, 0x96		;shift(%r8) <- 0x96

	cmp		%r7, %r8		; 0x7f + 31 - 8 = 0x96
	jrgt	shftleft		; if 指数部(%r7) > 0x96 then goto shftleft

	; case: exp <= 0x96
	sub		%r8, %r7		; shift(%r8) = shift(%r8=0x96) - 指数部(%r7) (max = 23)

	; 32-bit variable right shift routine (faster than xsrl)
shift:
	cmp		%r8, 8			; if shift(%r8) <= 8 then goto done
	jrle	done
	
	; case: shift > 8
	sub		%r8, 8			; shift(%r8) = shift(%r8) - 8
	jp.d	shift
	srl		%r4, 8			; 戻り値(%r4) >> 8

done:						; case: shift(%r8) < 8
	jp.d	finish
	srl		%r4, %r8		; 戻り値(%r4) >> shift(%r8) final shift of result
	
shftleft:
	sub		%r7, %r8		; 指数部(%r7) = 指数部(%r7) - shift(%r8=0x96) (max = 8)
		
	sll		%r4, %r7		; 戻り値(%r4) >> 指数部(%r7) shift mantissa to the left 8-shift bits

finish:
	cmp		%r5, 0			; if 符号ビット(%r5) = 0 then goto end
	jreq	end

	; case: sign = 1
	not		%r4, %r4		; 戻り値(%r4) = ~戻り値(%r4)
	jp.d	end
	add		%r4, 1			; 戻り値(%r4) = 戻り値(%r4) + 1 (２の補数)

overflow:
	cmp		%r5, 0			; check sign
	jrne	end				; if 符号ビット(%r5) = 1 then goto end
	ld.w	%r4, 0			; オーバーフローする時は、0xffffffffを返すように修正。01/01/17  Hinokuchi 
	not		%r4, %r4		; 戻り値(%r4) <- 0xffffffff

end:
	ret
