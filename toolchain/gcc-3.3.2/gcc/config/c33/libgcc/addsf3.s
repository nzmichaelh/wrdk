;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : addsf3.s
;*
;*	Single floating point add function
;*						  subtract function					
;*		input: %r6, %r7
;*		output: %r4
;*
;*	Begin					1996/09/12	V. Chan
;*  Fixed sign bug			1997/02/17	V. Chan
;*  Fixed a precision bug	1997/02/24	V. Chan
;*  変更					2001/01/15  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*********************************************

.section .text
.align 1
.global __addsf3
.global __subsf3

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット１(0(+) or 1(-))
;	%r1:指数部１(8bit)
;	%r2:符号ビット２(0(+) or 1(-))/count
;	%r3:指数部２(8bit)
;	%r4:戻り値
;	%r5:指数部オーバーフロー値(8bit)/xflag
;	%r6:引数１(float)/仮数部１/shift counter
;	%r7:引数２(float)/仮数部２
;	%r8:temp/implied bit/shift/loop counter
;	%r9:mask
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

__subsf3:
	xxor	%r7, 0x80000000	; 引数２(%r7)= 引数２(%r7) & 0x80000000 

__addsf3:
	pushn	%r3				; save register values

	xld.w	%r5, 0xff		; 指数部オーバーフロー値 <- 0xff 

	ld.w	%r0, %r6		; 符号ビット１(%r0) <- 引数１(%r6) 
	rl		%r0, 1			; 符号ビット１(%r0) rotate left 1 bit
	and		%r0, 1			; 符号ビット１(%r0) & 1 
	ld.w	%r8, %r0		; temp(%r8) = 符号ビット１(%r0)

	ld.w	%r2, %r7		; 符号ビット２(%r0) <- 引数２(%r7) 
	rl		%r2, 1			; 符号ビット２(%r2) rotate left 1 bit
	and		%r2, 1			; 符号ビット２(%r2) & 1 

	ld.w	%r1, %r6		; 指数部１(%r1) <- 引数１(%r6) 
	sll		%r1, 1			; 指数部１(%r1) << 1
	xsrl 	%r1, 24			; 指数部１(%r1) >> 24

	cmp		%r1, %r5		; if 指数部１(%r1) >= 指数部オーバーフロー値(%r5) 
	xjrge	overflow		; then jump to overflow

	ld.w	%r3, %r7		; 指数部２(%r3) <- 引数２(%r6)
	sll 	%r3, 1			; 指数部２(%r3) << 1
	xsrl	%r3, 24			; 指数部２(%r3) >> 24
	ld.w	%r0, %r2		; 符号ビット１(%r0) <- 符号ビット２(%r0) 

	cmp		%r3, %r5		; if 指数部２(%r3)  >= 指数部オーバーフロー値(%r5)  
	xjrge	overflow		; then jump to overflow
		
	cmp		%r1, %r3		; if 指数部１(%r1) < 指数部２(%r3) 
	jrlt.d	ex1ltex2		; then jump to ex1ltex2
	ld.w	%r0, %r8		; 符号ビット１(%r0) <- temp(%r8) 

	; case: exp1 >= exp2
	ld.w	%r8, %r1		; temp(%r8) = 符号ビット１(%r0) - 指数部２(%r3) (difference)
	sub		%r8, %r3		; temp(%r8) = temp(%r8) - 指数部２(%r3)
	cmp		%r8, 0x18		; if temp(%r8)difference >= 0x18 (24-bits it too large)
	ld.w	%r4, %r6		; 戻り値(%r4) <- 引数１(%r6) 
	xjrge	end				; then jump to end return first input
	jp		continue		

ex1ltex2:
	; case: exp2 > exp1
	ld.w 	%r8, %r3		; temp(%r8) <- 指数部２(%r3)
	sub		%r8, %r1		; temp(%r8) = 指数部２(%r3) - 指数部１(%r1)
	xcmp	%r8, 0x18		; if temp(%r8)difference < 0x18
	ld.w	%r4, %r7		; 戻り値(%r4) <- 引数２ 
	jrlt 	continue		; then jump to continue
	ld.w	%r0, %r2		; 符号ビット１(%r0) <- 符号ビット２(%r2) 
	xjp		end				; return second input

continue:
	xld.w	%r9, 0x7fffff	; mask(%r9) <- 0x7fffff 
	xld.w	%r8, 0x800000	; implied bit(%r8) <- 0x800000 

	; isolate mantissa1
	cmp		%r1, 0			; if 指数部１(%r1) = 0
	jreq.d	getman2			; then jump to getman2
	and		%r6, %r9		; 引数１(%r6) & mask(%r9)  
	or		%r6, %r8		; 引数１(%r6) & mplied bit(%r8) 

getman2:
	; isolate mantissa2
	cmp		%r3, 0			; if 指数部２(%r3) = 0
	jreq.d	cmpexp			; then jump to cmpexp
	and		%r7, %r9		; 引数２(%r7) & mask(%r9)  clear first 9 bits of %r7
	or		%r7, %r8		; 引数２(%r7) | mplied bit(%r8) if exp2 != 0 then add implied bit (normal)

cmpexp:
	; compare exponents -- %r1 will be result exponent
	; if exp1 > exp2 then mantissa2 is shifted to the right
	; if exp2 > exp1 then mantissa1 is shifted to the right

	; shift mantissa left for increased precision
	sll		%r6, 1			; 引数１(%r6) << 1
	sll		%r7, 1			; 引数２(%r6) << 1

	; xflag indicates which input (1 or 2) is the smaller input
	ld.w	%r5, 0			; xflag(%r5) <- 0

	cmp		%r1, %r3		
	jreq	negation		; if 指数部１(%r1) = 指数部２(%r3) then jump to negation
	jrgt	man2			; if 指数部１(%r1) > 指数部２(%r3) then jump to man2

	; case: exp1 < exp2
	cmp		%r1, 0			 
	ld.w	%r8, %r1		; temp(%r8) <- 指数部１(%r1)
	jrne.d	shftm1			; if 指数部１(%r1) != 0 then normal
	ld.w	%r1, %r3		; 指数部１(%r1) <- 指数部２(%r3) result exp = exp2
	sub 	%r3, 1			; 指数部２(%r3) = 指数部２(%r3) - 1 else denormal --> decrement shift

shftm1:
	sub		%r3, %r8		; 指数部２(%r3) = 指数部２(%r3) - temp(%r8) shift amount = exp2 - temp (exp1)

	;xsrl		%r6, %r3	; man1 >> shift
	; used in 32-bit variable shifting	; 	VARSHIFT %r6, %r3, srl	; 仮数部１(%r6) >> shift
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0001:
	cmp		%r3, 8		; if temp <= 8 then goto $$2
	jrle	__L0002

	srl		%r6, 8		; shift input register 8 bits
	jp.d	__L0001
	sub		%r3, 8		; temp = temp - 8

__L0002:
	srl		%r6, %r3		; last shift
	jp.d	negation
	ld.w	%r5, 2			; xflag(%r5) = 2

man2:
	; case: 指数部１ > 指数部２
	cmp		%r3, 0
	jrne.d	shftm2			; if 指数部２(%r3) != 0 then normal
	ld.w	%r8, %r1		; shift(%r8) <- 指数部１
	sub		%r8, 1			; shift(%r8) = shift(%r8) - 1 else denormal -- decrement shift

shftm2:
	sub		%r8, %r3		; shift(%r8) = 指数部１(%r1) - 指数部２(%r3)

	;srl	%r7, %r8		; man2 >> shift
	; used in 32-bit variable shifting	; 	VARSHIFT %r7, %r8, srl	; 仮数部１(%r7) >> shift(%r8)
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0003:
	cmp		%r8, 8		; if temp <= 8 then goto $$2
	jrle	__L0004

	srl		%r7, 8		; shift input register 8 bits
	jp.d	__L0003
	sub		%r8, 8		; temp = temp - 8

__L0004:
	srl		%r7, %r8		; last shift

	ld.w	%r5, 1			; xflag(%r5) <- 1

negation:
	; xflag = 0 if exp1 = exp2
	;		  1 if exp1 > exp2
	;		  2 if exp1 < exp2
	; %r0 will now be result sign bit

	; if exp1 = exp2 then this is the case
	; sign1	(%r0)  sign2 (%r2)	result sign (%r0)
	;	0			  0				0	<== no change
	;	1			  0				0	<== change to 1 if result < 0
	;	0			  1				0	<== change to 1 if result < 0
	;	1			  1				1	<== no change

	cmp		%r0, %r2		; if 符号ビット１(%r0) = 符号ビット２(%r2) compare sign bits
	jreq	sign			; then jump to sign
	cmp		%r0, 1			; if 符号ビット１(%r0) != 1
	jrne	negm2			; then jump to negm2

	; case: sign1 = 1
	cmp		%r5, 0			; if xflag(%r5) != 0 only change %r0 if xflag = 0
	jrne	negm1			; then jump to negm1
	ld.w	%r0, 0			; 符号ビット１(%r0) <- 0 %r0 is now temp result sign (positive)

negm1:
	not		%r6, %r6		; 引数１(%r6) = ~引数１(%r6) negate man1
	jp.d	sign			; delayed jump to sign
	add		%r6, 1			; 仮数部１(%r6) = 仮数部１(%r6) + 1
	
negm2:
	not		%r7, %r7		; 仮数部２(%r7) = ~仮数部２(%r7) negate man2
	add		%r7, 1			; 仮数部２(%r7) = 仮数部２(%r7) + 1

sign:
	; fix sign, case where exp2 > exp1 and sign2 = 1
	; if xflag = 2 then result sign = sign2 (%r0 = %r2)
	; if xflag = 1 then result sign = sign1	(%r0 = %r0)
	; if xflag = 0 then result sign = 0 or 1
	cmp		%r5, 2			; if xflag(%r5) != 2
	jrne	addition		; then jump to addition begin addition

	; case: xflag = 2
	ld.w	%r0, %r2		; 符号ビット１(%r0) <- 符号ビット２(%r2) result sign = sign2

addition:
	; %r0 = result sign, %r1 = result exponent
	; %r6 = mantissa1, %r7 = mantissa2
	; %r4 = result

	add		%r6, %r7		; 仮数部１= 仮数部１+ 仮数部２ add man1 and man2
	ld.w 	%r4, %r6		; 戻り値(%r4) <- 仮数部１+ 仮数部２ put result (%r6) in %r4

	cmp		%r4, 0			; if 戻り値(%r4) = 0
	jreq.d	end				; jump to end if result = 0
	ld.w 	%r8, 0			; loop counter(8) <- 0 clear temp register
	jrgt	precount		; if result > 0 then continue with normalize

	; case: result < 0
	not		%r4, %r4		; 戻り値(%r4) = ~戻り値(%r4) + 1
	add		%r4, 1

	cmp		%r5, 0			; if xflag(%r5) != 0 
	jrne	precount		; then goto count
	ld.w 	%r0, 1			; 符号ビット１(%r0) <- 1 sign was 0 now sign = 1 

precount:
	srl		%r4, 1			; 戻り値(%r4) >> 1 compensate for shifts on 117, 118
	ld.w	%r6, %r4		; shift counter(%r6) <- (戻り値(%r4) >> 1) copy new result to %r6 for counting

count:
	scan1	%r2, %r6		; scan1 count(%r2), shift counter(%r6) count = ; of 0's before leading 1 in result
	jruge	expchk			; if count !=8 then goto expchk
	add		%r8, 8			; loop counter(%r8) = loop counter (%r8) + 8 add 8 to loop counter
	jp.d	count
	sll		%r6, 8			; shift counter(%r6) >> 8 shift 8 leading 0's out of %r6

expchk:
	add		%r2, %r8		; count(%r2) = count(%r2) + loop counter(%r8)

	cmp		%r1, 0			; if 指数部１(%r1) != 0 
	jrne	normalize		; then jump to normalize

	; case: denormal + denormal (sisu = 0)
	cmp		%r2, 8			; if count(%r2) != 8 
	jrne	finish			; then jump to finish
	jp.d	finish
	ld.w 	%r1, 1			; 指数部１(%r1) <- 1 if count = 8 then normal result

normalize:
	sub		%r2, 8			; count(%r2) = count(%r2) - 8
	jreq	finish			; if count(%r2) = 8 then no normalize needed
	jrlt	shftrght		; if count(%r2) <= 8 then normal result with carry-over

	; case: count > 8
	cmp		%r1, %r2		; if 指数部１(%r1) <= count(%r2) 
	jrle	denormal		; then denormal

	ld.w	%r8, %r2		; temp(%r8) <- count(%r2)
	; used in 32-bit variable shifting	; 	VARSHIFT  %r4, %r2, sll	; 戻り値(%r4) << count(%r2)
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0005:
	cmp		%r2, 8		; if temp <= 8 then goto $$2
	jrle	__L0006

	sll		%r4, 8		; shift input register 8 bits
	jp.d	__L0005
	sub		%r2, 8		; temp = temp - 8

__L0006:
	sll		%r4, %r2		; last shift
	;xsll	%r4, %r2		; result << count

	jp.d	finish
	sub		%r1, %r8		; 指数部１(%r1) = 指数部１(%r1) - count(%r2)

denormal:
	sub 	%r1, 1			; 指数部１(%r1) = 指数部１(%r1) - 1 (shift to denormal position)
	; used in 32-bit variable shifting	; 	VARSHIFT %r4, %r1, sll	; 戻り値(%r4) << 指数部１(%r1)
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0007:
	cmp		%r1, 8		; if temp <= 8 then goto $$2
	jrle	__L0008

	sll		%r4, 8		; shift input register 8 bits
	jp.d	__L0007
	sub		%r1, 8		; temp = temp - 8

__L0008:
	sll		%r4, %r1		; last shift
	;xsll 	%r4, %r1		; result << exp
	jp.d	finish
	ld.w	%r1, 0			; 指数部１(%r1) <- 0 (denormal result)

shftrght:
	srl		%r4, 1			; 戻り値(%r4) >> 1
	add		%r1, 1			; 指数部１(%r1) = 指数部１(%r1) + 1

	; overflow check
	xld.w	%r5, 0xff		; 指数部オーバーフロー値(%r5) <- 0xff add:01/01/19 Hinokuchi set overflow comparison value
	cmp		%r1, %r5		; if 指数部１(%r1)  < 0xff
	jrlt	finish			; then jump to finish

overflow:
	xld.w	%r4, 0x7f800000	; 戻り値(%r4) <- 0x7f800000 put infinity into result
	jp		end

finish:
	; %r0 = sign, %r1 = exponent, %r4 = mantissa

	and		%r4, %r9		; 戻り値(%r4) = 戻り値(%r4) & mask(%r9) isolate mantissa

	xrr		%r1, 9			; 指数部１(%r1) bit[30〜23]にシフト  position exponent bits to [30:23]
	or		%r4, %r1		; 戻り値(%r4) = 指数部１(%r1) | 仮数部(%r4)

end:
	rr		%r0, 1			; 符号ビット１(%r0)bit[31]にシフト position sign bit to MSB
	or		%r4, %r0		; 戻り値(%r4) = 符号ビット１(%r0) | 指数部１(%r1) | 仮数部(%r4) add sign bit

	popn	%r3				; restore register values

	ret
