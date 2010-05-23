;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : mulsf3.s
;*
;*	Single floating point multiplication function
;*		input: %r6, %r7
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/26  O.Hinokuchi
;*  			 ・レジスタ置換
;*
;*****************************************

.section .text
.align 1
.global __mulsf3

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット１(0(+) or 1(-))
;	%r1:指数部１(8bit)
;	%r2:符号ビット２(0(+) or 1(-))/temp/shift
;	%r3:指数部２(8bit)/mask
;	%r4:戻り値(float)/乗算結果[H]
;	%r5:implied bit/count
;	%r6:引数１/乗算結果[L]/仮数[L]
;	%r7:引数２
;	%r8:mask
;	%r9:overflow value
;	%r12:scan64 引数１
;	%r13:scan64 引数２
;==============================================


;;macro	VARSHIFT $1, $2, $3
	; used in 32-bit variable shifting
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
;$$1:
;	cmp		$2, 8		; if temp <= 8 then goto $$2
;	jrle	$$2

;	$3		$1, 8		; shift input register 8 bits
;	jp.d	$$1
;	sub		$2, 8		; temp = temp - 8

;$$2:
;	$3		$1, $2		; last shift
;;endm

__mulsf3:

	pushn	%r3				; save register values

	;@@@ 01/01/26 add start hinokuchi
	;sub		%sp, 2
	;ld.w	[%sp+0], %r12	; %r12待避
	;ld.w	[%sp+1], %r13	; %r13待避
	;@@@ 01/01/26 add end


	;@@@ 01/02/15 del xld.w	%r9, 0xff		; overflow value(%r9) <- 0xff

	ld.w	%r0, %r6		; 符号ビット１(%r0) <- 引数１(%r6) 
	rl		%r0, 1			; 符号ビット１(%r0) rotate left 1 bit
	and		%r0, 1			; 符号ビット１(%r0) & 1 

	ld.w	%r2, %r7		; 符号ビット２(%r2) <- 引数２(%r7)
	rl		%r2, 1			; 符号ビット２(%r2) rotate left 1 bit
	and		%r2, 1			; 符号ビット２(%r2) & 1 

	xor		%r0, %r2		; 符号ビット１(%r0) = ~符号ビット１(%r0)
	ld.w	%r4, 0			; 戻り値(%r4) <- 0

	sll		%r6, 1			; 引数１(%r6) << 1  clear MSB
	srl		%r6, 1			; 引数１(%r6) >> 1
	cmp		%r6, 0			
	ext		end@rm
	jreq	end@rl			; if 引数１(%r6) = 0 then end
	
	sll		%r7, 1			; 引数２(%r7) << 1 clear MSB
	srl		%r7, 1			; 引数２(%r7) >> 1
	cmp		%r7, 0		
	ext		end@rm
	jreq	end@rl			; if 引数２(%r7) = 0 then end

	ld.w	%r1, %r6		; 指数部１(%r1) <- 引数１(%r6)
	xsrl 	%r1, 23			; 指数部１(%r1) >> 23

	xcmp	%r1, 0xff		; if 指数部１(%r1) >= overflow value
	jrge	overflow		; then jump to overflow

	ld.w	%r3, %r7		; 指数部２(%r3) <- 引数２(%r7)
	xsrl	%r3, 23			; 指数部２(%r3) >> 23

	xcmp	%r3, 0xff		; if 指数部２(%r3) >= overflow value
	jrge	overflow		; then jump to overflow

	;@@@ 01/02/15 del xld.w	%r8, 0x7fffff 	; mask(%r8) <- 0x7fffff set mask for isolating mantissa
	;@@@ 01/02/15 del xld.w	%r5, 0x800000	; implied bit(%r5) <- 0x800000
	xld.w	%r5, 0x7fffff 	; mask(%r5) <- 0x7fffff set mask for isolating mantissa @@@ 01/02/15 add

	ld.w	%r2, 1			; temp(%r2) <- 1

	; isolate mantissa1
	cmp		%r1, 0
	jreq.d	getman2			; if 指数部１(%r1) = 0 (denormal)
	and		%r6, %r5		; clear first 9 bits of %r6
	xoor	%r6, 0x800000	; else add implied bit(0x800000) to mantissa

	ld.w	%r2, 0			; temp(%r2) = 0

getman2:
	; isolate mantissa2
	cmp		%r3, 0
	jreq.d	flag			; if exp2 = 0 then jump to flag
	and		%r7, %r5		; clear first 9 bits of %r7
	xld.w	%r5, 0x800000	; implied bit(%r5) <- 0x800000 @@@ 01/02/15 add
	jp.d	multi			; delayed jump
	or	%r7, %r5			; else add implied bit(0x800000)

flag:
	add		%r2, 1			; temp(%r2) = 2 or 1 (2 if den * den : 1 if nor * den)

multi:
	; check result exponent
	add		%r1, %r3		; 指数部１(%r1) = 指数部１(%r1) + 指数部２(%r3) + temp(%r2) - bias(0x7f)
	add		%r1, %r2

	xsub	%r1, 0x7f		; 指数部１(%r1) = 指数部１(%r1) - 0x7f

	xcmp	%r1, 0xff		; if 指数部１(%r1) >= 0xff then goto overflow
	jrge	overflow

	xcmp	%r1, -22		; if 指数部１(%r1) < -22 then goto end
	jrlt.d	end
	ld.w	%r4, 0			; underflow value

	mltu.w	%r6, %r7		; %ahr,%alr <- 引数１(%r6) * 引数２(%r7)  multiply: 64-bit result is {%r4, %r6}
	ld.w	%r4, %ahr		; 乗算結果[H](%r4) <- %ahr
	ld.w	%r6, %alr		; 乗算結果[L](%r6) <- %alr

	ld.w	%r8, %r4		; scan64 引数１(%r8) <- 乗算結果[H](%r4)
	ld.w	%r9, %r6		; scan64 引数２(%r9) <- 乗算結果[L](%r6)
	xcall	__scan64
	ld.w	%r5, %r9		; %r5 = count (; of leading 0's in result)

normalize:
	; %r0 = result sign, %r1 = result exponent
	; %r5 = count, %r4 = result1, %r6 = result2
	; note: max. count = 63, min. count = 16

	cmp		%r1, 0			; if 指数部１(%r1) > 0 then jump to expgtz
	jrgt.d	expgtz
	sub		%r5, 17			; count(%r5) = count(%r5) - 17

	; case: exp <= 0
	ld.w	%r3, 1			; 指数部２(%r3) = 1 - 指数部１(%r1)
	sub		%r3, %r1
	ld.w	%r2, 9			; %r2 = shift
	sub		%r2, %r3		; shift(%r2) = 9 - (1 - 指数部１(%r1))

	cmp		%r1, %r5
	jrle.d	shift			; if 指数部１(%r1) <= count then jump to shift
	ld.w	%r1, 0			; 指数部１(%r1) <- 0

	; case: exp > count - 17  (only when exp = 0 and original count = 16)
	jp.d	shift			; delayed jump
	ld.w	%r1, 1			; 指数部１(%r1) <- 1

expgtz:
	cmp		%r5, %r1		; if count(%r5) < 指数部１(%r1) then goto shftltexp
	jrlt.d	shftltexp
	ld.w	%r2, 9			; shift(2) <- 9 (for positioning)

	; case: original count - 17 >= exp
	add		%r2, %r1		; shift(%r2) = shift(%r2) + 指数部１(%r1) - 1
	sub		%r2, 1			; shift(%r2) as much as exp allows (until denormal)
	jp.d	shift
	ld.w	%r1, 0			; 指数部１(%r1) <- 0
	
shftltexp:
	sub		%r1, %r5		; 指数部１(%r1) = 指数部１(%r1) - count(%r5)
	add		%r2, %r5		; shift(2) = shift(%r2) + count(%r5)

shift:
	; max. shift < 64
	cmp		%r2, 0			; if shift(%r2) < 0 then shift to the right
	jrlt	rshift
	xcmp	%r2, 32			; if shift(%r2) < 32 then goto lshift
	jrlt	lshift

	; case: shift >= 32 (at least one is denormal)
	ld.w	%r4, %r6		; result1 = result2
	sub		%r2, 32			; shift(%r2) = shift(%r2) - 32
	;xsll	%r4, %r2		; result1 << shift
	; used in 32-bit variable shifting	; 	VARSHIFT %r4, %r2, sll
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
__L0001:
	cmp		%r2, 8		; if temp <= 8 then goto $$2
	jrle	__L0002

	sll		%r4, 8		; shift input register 8 bits
	jp.d	__L0001
	sub		%r2, 8		; temp = temp - 8

__L0002:
	sll		%r4, %r2		; last shift
	jp		finish			; no overflow from norm * den

rshift:
	; case: shift < 0
	not		%r2, %r2		; shift(%r2) = ~shift(%r2) + 1
	add		%r2, 1

	cmp		%r2, 16			; if shift(2) < 16 then shift
	jrlt	denormal

	; case: shift >= 16
	jp.d	end
	ld.w	%r4, 0			; 戻り値(%r4) <- 0

denormal:
	; case: shift < 16
	;xsrl	%r4, %r2		; result >> shift
	; used in 32-bit variable shifting	; 	VARSHIFT %r4, %r2, srl
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
__L0003:
	cmp		%r2, 8		; if temp <= 8 then goto $$2
	jrle	__L0004

	srl		%r4, 8		; shift input register 8 bits
	jp.d	__L0003
	sub		%r2, 8		; temp = temp - 8

__L0004:
	srl		%r4, %r2		; last shift
	jp		finish

lshift:					; case: 32 > shift >= 0
	; {%r4, %r6} << shift
	;xsll	%r4, %r2		; shift high 32-bits to the left x bits (shift amount = x)
	;xrl	%r6, %r2		; rotate low 32-bits to the left x bits
	;xsll	%r3, %r2		; make a mask for first 32-x bits --> %r3 = 111...000
	
	ld.w	%r3, -1			; %r3 = 0xffff ffff
	; 64-bit variable shift and rotate routine
L1:
	cmp		%r2, 8			; if shift(%r2) amount <= 8 then goto L2
	jrle	L2

	sll		%r4, 8			; 戻り値(%r4) << 8
	rl		%r6, 8			; 仮数[L](%r6) rotate << 8
	sll		%r3, 8			; mask(%r3) << 8
	jp.d	L1
	sub		%r2, 8			; shift(%r2) = shift(%r2) - 8 

L2:
	sll		%r4, %r2		; 戻り値(%r4) << shift(%r2)
	rl		%r6, %r2		; 仮数[L] rotate << shift(%r2)
	sll		%r3, %r2		; mask(%r3) << shift(%r2)

	not		%r2, %r3		; flip mask for last x bits --> %r2 = 000...111 (mask)
	and		%r2, %r6		; isolate last x bits of %r6
	or		%r4, %r2		; add last x bits of %r4 to %r4
	and		%r6, %r3		; keep the high 32-x bits of %r6 

	; overflow check
	xcmp	%r1, 0xff		; if 指数部１(%r1) < overflow value(%r9)
	jrlt	finish			; then jump to finish

overflow:
	xld.w	%r4, 0x7f800000	; put infinity into result
	jp		end				; delayed jump

finish:
	; %r0 = sign, %r1 = exponent, %r4 = mantissa
	xand	%r4, 0x7fffff		; isolate mantissa
	xrr		%r1, 9			; position exponent bits to [30:23]
	or		%r4, %r1

end:
	rr		%r0, 1			; position sign bit to MSB
	or		%r4, %r0		; 戻り値(%r4) | 符号ビット１(%r0)

	;@@@ 01/01/23 add start hinokuchi
	;ld.w	%r13, [%sp+1]	; %r13復帰
	;ld.w	%r12, [%sp+0]	; %r12復帰
	;add		%sp, 2
	;@@@ 01/01/23 add end

	popn	%r3				; restore register values
	ret
