;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : muldf3.s
;*
;*	Double floating point multiplication function
;*		input: (%r7, %r6) & (%r9, %r8)
;*		output: (%r5, %r4)
;*
;*	Begin						1996/09/12	V. Chan
;*	Fixed bug at multiply:		1997/02/18	V. Chan
;*  変更						2001/01/30  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __muldf3

;;macro	SHFTROTSHFT $1, $2, $3, $4, $5, $6, $7
	; used in 64-bit variable shifting
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
;	ld.w	$7, $1		; temp = shift amount

;$$1:
;	cmp		$7, 8		; if temp <= 8 then goto $$2
;	jrle	$$2

;	$5		$2, 8		; shift 1st register
;	$6		$3, 8		; rotate 2nd register
;	$5		$4, 8		; shift 3rd register
;	jp.d	$$1
;	sub		$7, 8		; temp = temp - 8

;$$2:
;	$5		$2, $7		; last shift
;	$6		$3, $7		; last rotate
;	$5		$4, $7		; last shift
;;endm

__muldf3:
	pushn	%r3				; save register values
	;@@@ 01/01/30 add start hinokuchi
	;sub		%sp, 4
	;ld.w	[%sp+0], %r10	; %r10待避
	;ld.w	[%sp+1], %r11	; %r11待避
	;ld.w	[%sp+2], %r12	; %r12待避
	;ld.w	[%sp+3], %r13	; %r13待避
	;@@@ 01/01/30 add end

	ld.w	%r0, %r7		; put sign of input1 (%r7) into %r0
	rl		%r0, 1
	and		%r0, 1

	ld.w	%r2, %r9		; put sign of input2 (%r9) into %r2
	rl		%r2, 1
	and		%r2, 1

	xor		%r0, %r2		; put new sign into register %r0

	ld.w	%r5, 0			; clear output register
	;ld.w	%r4, 0			; moved to line 38 for a delayed jump
	
	sll		%r7, 1			; clear MSB
	srl		%r7, 1
	cmp		%r7, 0			; checking if input1 = 0
	jrne.d	input2
	ld.w	%r4, 0
	cmp		%r6, 0			
	ext		end@rm
	jreq	end@rl			; if {%r7, %r6} = 0 then end

input2:
	sll		%r9, 1			; clear MSB
	srl		%r9, 1	
	cmp		%r9, 0			; checking if input2 = 0
	jrne	getexp
	cmp		%r8, 0
	ext		end@rm
	jreq	end@rl				; if {%r9, %r8} = 0 then end

getexp:
	ld.w	%r1, %r7		; put exponent1 into %r1
	xsrl 	%r1, 20

	xcmp	%r1, 0x7ff		; check exp1 for overflow value
	ext		overflow@rm
	jreq	overflow@rl

	ld.w	%r3, %r9		; put exponent2 into %r3
	xsrl	%r3, 20

	xcmp	%r3, 0x7ff		; check exp2 for overflow value
	ext		overflow@rm
	jreq	overflow@rl

	; free	%r2, %r11 - %r13
	; %r0 = sign, %r1 = sisu1, %r3 = sisu2
	; %r10 = implied bit

	;@@@ 01/02/16 del xld.w	%r10, 0x100000	; implied bit
	ld.w	%r2, 1			; temp = 1
	;@@@ 01/02/16 del xld.w	%r13, 0xfffff

	; isolate mantissa1
	xand	%r7, 0xfffff	; clear first 12 bits of %r6
	cmp		%r1, 0
	jreq	man2			; if exp1 = 0 (denormal)
	xoor	%r7, 0x100000	; else add implied bit to mantissa

	ld.w	%r2, 0			; temp = 0

man2:
	; isolate mantissa2
	xand	%r9, 0xfffff	; clear first 12 bits of %r7

	cmp		%r3, 0
	jreq	tempadd			; if exp2 = 0 then jump to tempadd
	xoor	%r9, 0x100000	; else add implied bit
	jp	multiply			; delayed jump

tempadd:
	add		%r2, 1			; temp = temp + 1 (2 if den * den : 1 if nor * den)
	
multiply:
	; %r0 = result sign, %r1 = result exponent

	; calculate and check new exponent
	add		%r1, %r3		; sisu = sisu1 + sisu2 - bias + temp

	xsub	%r1, 0x3ff

	add		%r1, %r2

	xcmp	%r1, 0x7ff		; if exp >= 0x7ff then goto overflow
	ext		overflow@rm
	jrge	overflow@rl

	; max. shift from normal to denormal = 52
	; min. exp = 1 - max. shift = -51
	xcmp	%r1, -51		; if exp < -51 then goto underflow
	jrlt	underflow

	; 64-bit * 64-bit = 128-bit
	; 1st(128 ~ 97), 2nd(96 ~ 65), 3rd(64 ~ 33), 4th(32 ~ 0)
	; (%r7,%r6) * (%r9,%r8)
	; %r7 * a15 --> 1st,2nd
	; %r7 * a14 --> 2nd,3rd
	; %r6 * a15 --> 2nd,3rd
	; %r6 * a14 --> 3rd,4th
	; %r5 = result1, %r4 = result2, %r10 = result3, %r11 = result4, %r12 = temp
	ld.w	%r2, 0			; temp variable for adc and loop counter
	mltu.w	%r7, %r9		; kasu1 * kasu2
	ld.w	%r5, %ahr		; %r5 = result1	
	ld.w	%r4, %alr		; %r4 = result2
	mltu.w	%r7, %r8		; kasu1 * kasu2_2
	ld.w	%r7, %ahr
	add		%r4, %r7
	adc		%r5, %r2		; add carry
	ld.w	%r3, %alr		; %r10 = result3
	mltu.w	%r6, %r9		; kasu1_2 * kasu2
	ld.w	%r7, %alr
	add		%r3, %r7
	adc		%r4, %r2		; add carry
	adc		%r5, %r2		; add carry
	ld.w	%r7, %ahr
	add		%r4, %r7
	adc		%r5, %r2		; add carry
	mltu.w	%r6, %r8		; kasu1_2 * kasu2_2
	ld.w	%r7, %ahr
	add		%r3, %r7
	adc		%r4, %r2		; add carry
	adc		%r5, %r2
	ld.w	%r6, %alr		; %r11 = result4
	
	; %r8 will be first result register > 0
	cmp		%r5, 0			; if %r5 !=0 then count it
	jrne.d	count
	ld.w	%r8, %r5

	xld.w	%r2, 32			; %r2 = loop counter = 32 
	cmp		%r4, 0			; elsif %r4 != 0 then count it
	jrne.d	count
	ld.w	%r8, %r4

	xld.w	%r2, 64			; %r2 = loop counter = 64
	cmp		%r3, 0			; elsif %r10 != 0 then count it
	jrne.d	count
	ld.w	%r8, %r3

	xld.w	%r2, 96			; %r2 = loop counter = 96
	ld.w	%r8, %r6		; else count 0's leading %r6
	
count:
	scan1	%r7, %r8		; %r7 = count
	jruge	normalize		; if count !=8 then goto normalize
	add		%r2, 8			; add 8 to loop counter
	jp.d	count
	sll		%r8, 8			; shift register to the left 8 bits

normalize:
	; free	%r2 - %r11, %r13
	; %r0 = sign, %r1 = exp, %r7 = count
	; note: max. count = 127, min. count = 22

	add		%r7, %r2		; count = count + loopcounter

	cmp		%r1, 0			; if exp > 0 then jump to expgtz
	jrgt.d	expgtz
	sub		%r7, 23			; count = count - 23

	; case: exp <= 0
sub %sp, 1
ld.w	[%sp+0x0], %r3	; %r3待避
	ld.w	%r3, 1			; %r3 = 1 - exp
	sub		%r3, %r1
	ld.w	%r2, 12			; %r2 = shift
	sub		%r2, %r3		; shift = 12 - (1 - exp)
ld.w	%r3, [%sp+0x0]	; %r3復帰
add %sp, 1
	cmp		%r1, %r7
	jrle.d	shift			; if exp <= count then jump to shift
	ld.w	%r1, 0			; exp = 0

	; case: exp > count - 23
	jp.d	shift			; delayed jump
	ld.w	%r1, 1			; exp = 1
	
expgtz:
	cmp		%r7, %r1		; if count < exp then goto shftltexp
	jrlt.d	shftltexp
	ld.w	%r2, 12			; shift = 12

	; case: original count - 23 >= exp
	add		%r2, %r1		; shift = shift + exp - 1
	sub		%r2, 1			; shift as much as exp allows (until denormal)
	jp.d	shift
	ld.w	%r1, 0			; exp = 0
	
shftltexp:
	sub		%r1, %r7		; exp = exp - count
	add		%r2, %r7		; shift = shift + count

shift:
	cmp		%r2, 0			;if shift < 0 then shift to the right
	jrlt	rshift

	xcmp	%r2, 32			; if 0 =< shift < 32 then goto lshift
	jrlt	lshift

	xcmp	%r2, 64			; if 32 =< shift < 64 then goto shft32
	jrlt	lshft32

	; case:	shift >= 64		(note: max. shift of norm * denormal = 87)
	ld.w	%r5, %r3		; result1 = result3
	ld.w	%r4, %r6		; result2 = result4

	xsub	%r2, 64	; shift = shift - 64

	jp.d	lshift
	ld.w	%r3, 0			; result3 = 0

lshft32:
	ld.w	%r5, %r4		; result1 = result2
	ld.w	%r4, %r3		; result2 = result3
	ld.w	%r3, %r6		; result3 = result4
	jp.d	lshift
	sub		%r2, 32			; shift = shift - 32

rshift:
	; case: shift < 0
	not		%r2, %r2		; shift = ~shift + 1
	add		%r2, 1

	xcmp	%r2, 41			; if shift > 41 (64-min.count-1) then
	jrgt	underflow

	xcmp	%r2, 32			; if shift < 32 then shift right
	jrlt	doshift

	; case: 40 => shift >= 32
	ld.w	%r4, %r5		; result2 = result1
	sub		%r2, 32			; shift = shift - 32

	; xsrl	%r4, %r2		; result2 >> shift
	; 32-bit variable shift routine
L1:
	cmp		%r2, 8			; if shift <= 8 then goto L2
	jrle	L2

	srl		%r4, 8			; result >> %r2
	jp.d	L1
	sub		%r2, 8			; shift = shift - 8

L2:
	srl		%r4, %r2		; last shift

	jp.d	finish
	ld.w	%r5, 0			; result1 = 0

underflow:
	ld.w	%r5, 0			; result1 = 0
	jp.d	end
	ld.w	%r4, 0			; result2 = 0

doshift:
	; {%r5, %r4} >> shift
	;xsrl	%r4, %r2		; shift low 32-bits to the right x bits (shift amount = x)
	;xrr	%r5, %r2		; rotate high 32-bits to the right x bits
	;xsrl	%r3, %r2		; make a mask for last 32-x bits --> %r2 = 000...111

	ld.w	%r9, -1			; %r2 = 0xffff ffff
	; used in 64-bit variable shifting	; 	SHFTROTSHFT %r2, %r4, %r5, %r9, srl, rr, %r2
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r2, %r2		; temp = shift amount

__L0001:
	cmp		%r2, 8		; if temp <= 8 then goto $$2
	jrle	__L0002

	srl		%r4, 8		; shift 1st register
	rr		%r5, 8		; rotate 2nd register
	srl		%r9, 8		; shift 3rd register
	jp.d	__L0001
	sub		%r2, 8		; temp = temp - 8

__L0002:
	srl		%r4, %r2		; last shift
	rr		%r5, %r2		; last rotate
	srl		%r9, %r2		; last shift
	not		%r2, %r9		; flip mask for first x bits --> %r9 = 111...000 (mask)
	and		%r2, %r5		; isolate first x bits of %r5
	or		%r4, %r2		; add first x bits of %r5 to %r4
	jp.d	finish
	and		%r5, %r9		; keep the low 32-x bits of %r5

lshift:					; case: shift >= 0
	; {%r5, %r4, %r10} << shift
	;xsll	%r5, %r2		; shift high 32-bits to the left x bits (shift amount = %r3 = x)
	;xrl	%r4, %r2		; rotate mid 32-bits to the left x bits
	;xsll	%r3, %r2		; make a mask for first 32-x bits --> %r3 = 111...000

	ld.w	%r9, -1			; %r9 = 0xffff ffff
	; used in 64-bit variable shifting	; 	SHFTROTSHFT %r2, %r5, %r4, %r9, sll, rl, %r8		; %r8 = temp
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r8, %r2		; temp = shift amount

__L0003:
	cmp		%r8, 8		; if temp <= 8 then goto $$2
	jrle	__L0004

	sll		%r5, 8		; shift 1st register
	rl		%r4, 8		; rotate 2nd register
	sll		%r9, 8		; shift 3rd register
	jp.d	__L0003
	sub		%r8, 8		; temp = temp - 8

__L0004:
	sll		%r5, %r8		; last shift
	rl		%r4, %r8		; last rotate
	sll		%r9, %r8		; last shift
	not		%r8, %r9		; flip mask for last x bits --> %r8 = 000...111 (mask)
	ld.w	%r6, %r8		; temp = %r8
	and		%r8, %r4		; isolate last x bits of %r4
	or		%r5, %r8		; add last x bits of %r4 to %r5
	
	cmp		%r3, 0			; if result3 = 0 then done shifting
	jreq.d	overchk
	and		%r4 %r9			; keep the high 32-x bits of %r4

	; case: result3 != 0
	; 	MXRL	%r3, %r2		; rotate last register to the left
__L0005:
	
;	sub		%sp, 1
;	ld.w	[%sp+0], $2		; rsスタック待避
	
	and		%r2,0x1f			; 最大ローテート数=３１
__L0006:
	cmp		%r2,0x8			; if rs <= 8
	jrle	__L0007				; then $$3
	rl		%r3,0x8			; rd << 8
	jp.d	__L0006
	sub		%r2,0x8			; 残ローテート回数計算
__L0007:
	rl		%r3,%r2
;	ld.w	$2, [%sp+0]		; rsスタック復帰
;	add		%sp, 1

	and		%r3, %r6		; isolate last x bits of %r3
	or		%r4, %r3		; add last x bits to %r4

overchk:
	xcmp	%r1, 0x7ff
	jrlt	finish			; if exp < 0xff then jump to finish

overflow:
	xld.w	%r5, 0x7ff00000	; put infinity into result
	jp.d	end				; delayed jump
	ld.w	%r4, 0

finish:
	; %r0 = sign, %r1 = exponent, %r5 = mantissa

	xand	%r5, 0xfffff	; isolate mantissa

	xrr		%r1, 12			; position exponent bits to [30:23]
	or		%r5, %r1

end:
	rr		%r0, 1			; position sign bit to MSB
	or		%r5, %r0		; add sign bit

	;@@@ 01/01/23 add start hinokuchi
	;ld.w	%r13, [%sp+3]	; %r13復帰
	;ld.w	%r12, [%sp+2]	; %r12復帰
	;ld.w	%r11, [%sp+1]	; %r11復帰
	;ld.w	%r10, [%sp+0]	; %r10復帰
	;add		%sp, 4
	;@@@ 01/01/23 add end
	popn	%r3				; restore register values

	ret
