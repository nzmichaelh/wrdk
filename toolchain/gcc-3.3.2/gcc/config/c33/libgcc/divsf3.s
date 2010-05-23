;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : divsf3.s
;*
;*	Single floating point division function
;*		input: %r6, %r7
;*		output: %r4
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/18  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __divsf3

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット１(0(+) or 1(-))
;	%r1:指数部１(8bit)
;	%r2:符号ビット２(0(+) or 1(-))/shift/flag
;	%r3:指数部２(8bit)/count
;	%r4:戻り値(float)
;	%r5:temp
;	%r6:引数１(被除数)
;	%r7:引数２(除数)
;	%r8:mask
;	%r9:overflow value
;	%r10:implied bit/lshift
;	%r11:count1/shift
;	%r12:temp/count2
;	%r13:loop counter/temp/shift
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

__divsf3:
	pushn	%r3				; save register values
	;@@@ 01/01/30 add start hinokuchi
	;sub		%sp, 4
	;ld.w	[%sp+0], %r10	; %r10待避
	;ld.w	[%sp+1], %r11	; %r11待避
	;ld.w	[%sp+2], %r12	; %r12待避
	;ld.w	[%sp+3], %r13	; %r13待避
	;@@@ 01/01/30 add end

	;@@@ 01/02/15 del xld.w	%r9, 0xff		; set overflow comparison value

	ld.w	%r0, %r6		; 符号ビット１(%r0) <- 引数１(%r6) 
	rl		%r0, 1			; 符号ビット１(%r0) rotate left 1 bit
	and		%r0, 1			; 符号ビット１(%r0) & 1 

	ld.w	%r2, %r7		; 符号ビット２(%r2) <- 引数２(%r7)
	rl		%r2, 1			; 符号ビット２(%r2) rotate left 1 bit
	and		%r2, 1			; 符号ビット２(%r2) & 1

	xor		%r0, %r2		; 符号ビット１(%r0) = ~符号ビット１(%r0)
	ld.w	%r4, 0			; 戻り値(%r4) <- 0

	sll		%r6, 1			; 引数１(%r6) << 1  clear MSB
	srl		%r6, 1 			; 引数１(%r6) >> 1
	cmp		%r6, 0
	ext		end@rm
	jreq	end@rl			; if 引数１(%r6) = 0 then end

	cmp		%r7, 0
	sll		%r7, 1			; 引数２(%r7) << 1 clear
	srl		%r7, 1			; 引数２(%r7) >> 1
	ext		overflow@rm
	jreq	overflow@rl		; if 引数２(%r7) = 0 then overflow

	ld.w	%r1, %r6		; 指数部１(%r1) <- 引数１(%r6)
	xsrl 	%r1, 23			; 指数部１(%r1) >> 23

	;@@@ 01/02/15 del cmp		%r1, %r9		; if 指数部１(%r1) >= overflow value
	xcmp	%r1, 0xff		; if 指数部１(%r1) >= overflow value
	ext		overflow@rm
	jrge	overflow@rl		; result is overflow

	ld.w	%r3, %r7		; 指数部２(%r3) <- 引数２(%r7)
	xsrl	%r3, 23			; 指数部２(%r3) >> 23

	;@@@ 01/02/15 del cmp		%r3, %r9		; if 指数部２(%r3) >= overflow value
	xcmp		%r3, 0xff	; if 指数部２(%r3) >= overflow value

	ext		end@rm
	jreq	end@rl			; result is 0 (xxx/NaN = 0)

	;@@@ 01/02/15 del xld.w	%r8, 0x7fffff	; mask(%r8) <- 0x7fffff  set mask for isolating mantissa
	;@@@ 01/02/15 del xld.w	%r10, 0x800000	; implied bit(%r10)
	xld.w	%r5, 0x7fffff	; mask(%r8) <- 0x7fffff @@@ 01/02/16 add
	ld.w	%r2, 1			; flag(%r2) = 1
	;@@@ 01/02/15 del ld.w	%r13, 0			; loop counter(%r13)  <- 0
	ld.w	%r8, 0			; loop counter(%r8)  <- 0

	; isolate mantissa1
	;and		%r6, %r8	; line 62
	cmp		%r1, 0			; 
	jreq.d	count1			; if 指数部１(%r1) = 0 (denormal)
	;@@@ 01/02/15 del and		%r6, %r8		; 引数１(%r6) = 引数１(%r6) & mask(%r8)  clear first 9 bits of %r6
	and		%r6, %r5 		; 引数１(%r6) = 引数１(%r6) & 0x7fffff  clear first 9 bits of %r6

	; case: normal input
	;@@@ 01/02/15 del or		%r6, %r10		; 引数１(%r6) = 引数１(%r6) & implied bit(%r10)   else add implied bit to mantissa
	xoor	%r6, 0x800000	; 引数１(%r6) = 引数１(%r6) & implied bit(0x800000)   else add implied bit to mantissa
	ld.w	%r2, 0			; flag(%r2) <- 0
	jp.d	getman2
	;@@@ 01/02/15 del ld.w	%r11, 8			; count1(%r11) <- 8
	ld.w	%r9, 8			; count1(%r9) <- 8
	
count1:	
	;@@@ 01/02/15 del ld.w	%r12, %r6		; temp(%r12) <- 引数１(%r6)
	ld.w	%r4, %r6		; temp(%r4) <- 引数１(%r6)

loop1:
	;@@@ 01/02/15 del scan1	%r11, %r12		; %r11 = count1
	scan1	%r9, %r4		; %r9 = count1
	jruge	getman2			; if count1 !=8 then goto getman2
	;@@@ 01/02/15 del add		%r13, 8			; loop counter(%r13) = loop counter(%r13) + 8
	add		%r8, 8			; loop counter(%r8) = loop counter(%r8) + 8
	jp.d	loop1
	;@@@ 01/02/15 del sll		%r12, 8			; temp(%r12) << 8  shift register to the left 8 bits
	sll		%r4, 8			; temp(%r4) << 8  shift register to the left 8 bits
		
getman2:
	;@@@ 01/02/15 del add		%r11, %r13		; count1(%r11) = count1(%r11) + loop counter(%r13)
	add		%r9, %r8		; count1(%r9) = count1(%r8) + loop counter(%r13)
	;@@@ 01/02/15 del ld.w	%r13, 0			; loop counter(%r13) = 0
	ld.w	%r8, 0			; loop counter(%r8) = 0

	; isolate mantissa2
	;and	%r7, %r8		; line 85
	cmp		%r3, 0
	jreq.d	count2			; if 指数部２(%r3) = 0 then jump to count2
	;@@@ 01/02/15 del and		%r7, %r8		; 引数２(%r7) = 引数２(%r7) & mask(%r8) clear first 9 bits of %r7
	and		%r7, %r5		; 引数２(%r7) = 引数２(%r7) & mask(0x7fffff) clear first 9 bits of %r7
	;@@@ 01/02/15 del or		%r7, %r10		; 引数２(%r7) = 引数２(%r7) | implied bit(%r10)  else add implied bit
	xoor	%r7, 0x800000	; 引数２(%r7) = 引数２(%r7) | implied bit(0x800000)  else add implied bit
	jp.d	cmpcount
	;@@@ 01/02/15 del ld.w	%r12, 8			; count2(%r12) <- 8
	ld.w	%r4, 8			; count2(%r12) <- 8

count2:
	ld.w	%r5, %r7		; temp(%r5) <- 仮数２(%r7) man2
	sub		%r2, 1			; flag(%r2) = flag(%r2) - 1 (0 or -1)

loop2:
	;@@@ 01/02/15 del scan1	%r12, %r5		; %r12 = count2
	scan1	%r4, %r5		; %r4 = count2

	jruge	cmpcount		; if count2 !=8 then goto cmpcount
	add		%r8, 8			; loop counter(%r8) = loop counter(%r8) + 8
	jp.d	loop2
	sll		%r5, 8			; temp(%r5) << 8 shift register to the left 8 bits	

cmpcount:
	add		%r4, %r8		; count2(%r4) = count2(%r4) + loop counter(%r8)

	cmp		%r9, %r4		; if count1(%r9) <= count2(%r4) then goto man2 shift
	jrle	man2shift

	; case: count1 > count2
	sub		%r9, %r4		; shift(%r9) = count1(%r9) - count2(%r4)
	sub		%r9, 1			; shift(%r9) = shift(%r9) - 1

	;xsll	%r6, %r11		; shift man1
	ld.w	%r8, %r9		; temp(%r8) <- shift(%r9)
	; used in 32-bit variable shifting	; 	VARSHIFT %r6, %r9, sll
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
__L0001:
	cmp		%r9, 8		; if temp <= 8 then goto $$2
	jrle	__L0002

	sll		%r6, 8		; shift input register 8 bits
	jp.d	__L0001
	sub		%r9, 8		; temp = temp - 8

__L0002:
	sll		%r6, %r9		; last shift

	add		%r8, 1			; shift(%r8) = shift(%r8) + 1
	jp.d	divide
	ld.w	%r5, %r8		; lshift(%r5) <- shift(%r8)

man2shift:
	sub		%r4, %r9		; shift(%r4) = count2(%r4) - count1(%r9)
	add		%r4, 1			; shift(%r4) = shift(%r4) + 1

	;xsll	%r7, %r12		; shift man2 to the left
	ld.w	%r8, %r4		; temp(%r8) = shift(%r4)
	; used in 32-bit variable shifting	; 	VARSHIFT %r7, %r4, sll
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
__L0003:
	cmp		%r4, 8		; if temp <= 8 then goto $$2
	jrle	__L0004

	sll		%r7, 8		; shift input register 8 bits
	jp.d	__L0003
	sub		%r4, 8		; temp = temp - 8

__L0004:
	sll		%r7, %r4		; last shift

	not		%r5, %r8		; lshift(%r5) = ~temp(%r8)
	add		%r5, 2			; lshift(%r5) = lshift(%r5) + 2

divide:
	; man1 has 8 leading 0's
	; man2 has 0 leading 0's
	; lshift used to normalize result
	
	sub		%r1, %r3		; 指数部１(%r1) = 指数部１(%r1) - 指数部２(%r3)  new sisu = sisu1 - sisu2 + bias + flag (0,1,-1)

	xadd	%r1, 0x7f		; 指数部１(%r1) = 指数部１(%r1) + 0x7f

	add		%r1, %r2		; 指数部１(%r1) = 指数部１(%r1) + flag(%r2)

	xcmp	%r1, 0xff		; if 指数部１(%r1) >= 0xff then goto overflow
	jrge	overflow
	xcmp	%r1, -23		; if 指数部１(%r1) <= -23 then underflow
	jrle.d	end
	ld.w	%r4, 0			; 戻り値 <- 0

	ld.w	%alr, %r4		; extra 32-bits for accuracy
	div0u	%r7
	ld.w	%ahr, %r6		; dividend
	
;;ifdef FAST
;	div1	%r7			; 25 division instructions
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;	div1	%r7
;
;;else

	ld.w	%r8, 3		; loop counter(%r8) <- 3
	div1	%r7			; first division step (25 in total)

divloop:
	div1	%r7			; 8 division steps
	div1	%r7
	div1	%r7
	div1	%r7
	div1	%r7
	div1	%r7
	div1	%r7
	div1	%r7

	sub		%r8, 1			; loop counter(%r8) = loop counter(%r8) - 1
	jrne	divloop			; exit when zero flag is set

;;endif

	ld.w	%r4, %alr		; 戻り値(%r4) <- 商(%alr)

	; normalize
	; %r0 = result sign, %r1 = result exponent
	; %r10 = lshift, %r4 = result

	;ld.w	%r3, 7			; count = 7
	ld.w	%r3, 0			; count(%r3) <- 0
	ld.w	%r8, %r4
	xand	%r8, 0x1000000	; check 24th bit

	jrne	continue2		; if 24th bit = 1 then count = 7
	add		%r3, 1			; else count(%r3) = count(%r3) + 1

continue2:
	;sub		%r3, 7		; count = count - 7
	add		%r5, %r3		; lshift(%r5) = lshift(%r5) + count(%r3)
	cmp		%r1, %r5
	jrgt	normal			; if 指数部１(%r1) > lshift(%r5) then normal result

	; case: exp <= lshift
	add		%r5, 1
	sub		%r5, %r3		; lshift(%r5) = lshift(%r5) + (8 - (count(%r3)+7) )
	sub		%r5, %r1
	add		%r5, 1			; lshift(%r5) = lshift(%r5) - (指数部１(%r1) - 1)

	cmp		%r5, 25		; if lshift(%r5) amount >= 25 then underflow
	jrlt	notunder
	jp.d	end
	ld.w	%r4, 0			; 戻り値(%r4) <- 0

notunder:
	;xsrl	%r4, %r10		; %r4 >> lshift (shift amount)
	; used in 32-bit variable shifting	; 	VARSHIFT %r4, %r5, srl
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction
__L0005:
	cmp		%r5, 8		; if temp <= 8 then goto $$2
	jrle	__L0006

	srl		%r4, 8		; shift input register 8 bits
	jp.d	__L0005
	sub		%r5, 8		; temp = temp - 8

__L0006:
	srl		%r4, %r5		; last shift
	jp.d	finish
	ld.w	%r1, 0			; 指数部１(%r1) <- 0 for denormal result

normal:	
	; case: exp > lshift
	ld.w	%r2, 1			; shift(%r2) <- 1
	sub		%r2, %r3		; shift(%r2) = shift(%r2) - count(%r3)

	srl		%r4, %r2		; 戻り値(%r4) >> shift(%r2) <-- [1 or 0]
	sub		%r1, %r5		; 指数部１(%r1) = 指数部１(%r1) - lshift(%r5)
	
	xcmp	%r1, 0xff		; if 指数部１(%r1) < 0xff then jump to finish
	jrlt	finish

overflow:
	xld.w	%r4, 0x7f800000	; put infinity into result
	jp		end

finish:
	; %r0 = sign, %r1 = exponent, %r4 = mantissa
	xand	%r4, 0x7fffff	; isolate mantissa

	xrr		%r1, 9			; position exponent bits to [30:23]
	or		%r4, %r1

end:
	rr		%r0, 1			; position sign bit to MSB
	or		%r4, %r0		; 戻り値(%r4) =  戻り値(%r4) | 符号ビット１(%r0)t

	;@@@ 01/01/23 add start hinokuchi
	;ld.w	%r13, [%sp+3]	; %r13復帰
	;ld.w	%r12, [%sp+2]	; %r12復帰
	;ld.w	%r11, [%sp+1]	; %r11復帰
	;ld.w	%r10, [%sp+0]	; %r10復帰
	;add		%sp, 4
	;@@@ 01/01/23 add end
	popn	%r3				; restore register values

	ret
