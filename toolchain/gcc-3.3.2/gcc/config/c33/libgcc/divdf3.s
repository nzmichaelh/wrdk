;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : divdf3.s
;*
;*	Double floating point division function
;*		input: (%r7, %r6) & (%r9, %r8)
;*		output: (%r5, %r4)
;*
;*	Begin		1996/09/12	V. Chan
;*  変更		2001/01/31  O.Hinokuchi
;*	gas対応		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global __divdf3

;==============================================
;＜レジスタ説明＞
;	%r0:符号ビット１(0(+) or 1(-))
;	%r1:指数部１(11bit)
;	%r2:符号ビット２(0(+) or 1(-))/shift/flag
;	%r3:指数部２(11bit)/count/loop
;	%r4:戻り値[L]
;	%r5:戻り値[H]
;	%r6:引数１[L](被除数)/仮数部１[L]
;	%r7:引数１[H](被除数)/仮数部１[H]
;	%r8:引数２[L](除数)/仮数部２[L]
;	%r9:引数２[H](除数)/仮数部２[H]
;	%r10:implied bit/lshift
;	%r11:count1/shift/flag/extra
;	%r12:scan64 引数１/count2
;	%r13:loop counter/scan64 引数２/shift
;==============================================

;;macro	SHFTROTSHFT $1, $2, $3, $4, $5, $6, $7
	; used in 64-bit variable shifts
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

__divdf3:
	pushn	%r3				; save register values
	;@@@ 01/01/30 add start hinokuchi
	;sub		%sp, 4
	;ld.w	[%sp+0], %r10	; %r10待避
	;ld.w	[%sp+1], %r11	; %r11待避
	;ld.w	[%sp+2], %r12	; %r12待避
	;ld.w	[%sp+3], %r13	; %r13待避
	;@@@ 01/01/30 add end
	ld.w	%r0, 0			; 共有レジスタクリア @@@ 01/02/19

	ld.w	%r1, %r7		; 符号ビット１(%r1) <- 引数１[H](%r7) 
	rl		%r1, 1			; 符号ビット１(%r1) rotate left 1 bit
	and		%r1, 1			; 符号ビット１(%r1) & 1 

	ld.w	%r2, %r9		; 符号ビット２(%r2) <- 引数２[H](%r9)
	rl		%r2, 1			; 符号ビット２(%r2) rotate left 1 bit
	and		%r2, 1			; 符号ビット２(%r2) & 1

	xor		%r1, %r2		; 符号ビット１(%r0) = ~符号ビット１(%r0)
	; 	SET_SIGN1	%r0, %r1
__L0001:
	xand	%r0,0x7fffffff	; 共有レジスタ 符号ビット１クリア
	and		%r1,1			; 1ビットマスク
	rr		%r1,1			; 符号ビット１のビット位置にローテート
	or		%r0,%r1			; 符号ビット１セット
	rl		%r1,1			; 符号ビット復帰

	ld.w	%r5, 0			; 戻り値[H](%r5) <- 0
	ld.w	%r4, 0			; 戻り値[L](%r4) <- 0

	sll		%r7, 1			; 引数１[H](%r7) << 1
	srl		%r7, 1			; 引数１[H](%r7) >> 1
	cmp		%r7, 0			; if 引数１[H](%r7) != 0 then check input2
	jrne	zerochk2
	cmp		%r6, 0			; if 引数１[L](%r6) = 0 then end
	ext		end@rm
	jreq	end@rl

zerochk2:
	sll		%r9, 1			; 引数２[H](%r9) << 1
	srl		%r9, 1			; 引数２[H](%r9) >> 1
	cmp		%r9, 0			; if 引数２[H](%r9) != 0 then goto getexp
	jrne	getexp
	cmp		%r8, 0			; if 引数２[L](%r8) = 0 then overflow
	ext		overflow@rm
	jreq	overflow@rl

getexp:
	ld.w	%r1, %r7		; 指数部１(%r1) <- 引数１[H](%r7)
	xsrl 	%r1, 20			; 指数部１(%r1) >> 20
	; 	SET_EXP1	%r0 ,%r1	; 共有レジスタ[指数部１](%r0) <- 指数部１(%r1) @@@ 01/02/19
__L0002:
	xand	%r0,0xfffff800	; 共有レジスタ 指数部１クリア
	xand	%r1,0x7ff		; １１ビットマスク
	or		%r0,%r1			; 指数部１セット


	xcmp	%r1, 0x7ff		; if 指数部１(%r1) >= overflow value
	ext		overflow@rm
	jrge	overflow@rl		; result is overflow

	ld.w	%r3, %r9		; 指数部２(%r3) <- 引数２[H](%r9)
	xsrl	%r3, 20			; 指数部２(%r3) >> 20
	; 	SET_EXP2	%r0 ,%r3	; 共有レジスタ[指数部２](%r0) <- 指数部２(%r3) @@@ 01/02/19
__L0003:
	xand	%r0,0xffc007ff	; 共有レジスタ 指数部２クリア
	xand	%r3,0x7ff		; １１ビットマスク
	sll		%r3,8			; 指数部２のビット位置にシフト
	sll		%r3,3			; 指数部２のビット位置にシフト
	or		%r0,%r3			; 指数部２セット
	srl		%r3,8			; 指数部復帰
	srl		%r3,3			; 指数部復帰


	xcmp	%r3, 0x7ff		; if 指数部２(%r3) >= overflow value
	ext		end@rm
	jreq	end@rl			; result is 0 (xxx/NaN = 0)

	; del @@@ 01/02/19 del xld.w	%r10, 0x100000	; implied bit(%r10) <- 0x100000

	; isolate mantissa1
	xand	%r7, 0xfffff	; clear first 12 bits of %r7

	cmp		%r1, 0
	jreq.d	count1			; if 指数部１(%r1) = 0 (denormal)
	ld.w	%r2, 1			; flag(%r2) = 1

	; case: normal input
	xoor	%r7, 0x100000	; else add implied bit(0x100000) to mantissa @@@ 01/02/19
	ld.w	%r2, 0			; flag(%r2) = 0
	jp.d	getman2
	ld.w	%r5, 11			; count1(%r5) = 11
	
count1:	
	ld.w	%r4, %r7		; scan64 引数１(%r4) <- 引数１[H](%r7)
	ld.w	%r5, %r6		; scan64 引数２(%r5) <- 引数１[L](%r6)
	; @@@ del 01/02/19 xcall	__scan64
	; 	SCAN64 	%r4, %r5		; @@@ add 01/02/19		
	pushn	%r1				; save register values

	ld.w	%r0, 0			; loop counter = 0

	cmp		%r4, 0			; if high 32-bits != 0 then count
	jrne	__L0004

	; case: $1 = 0
	ld.w	%r4, %r5			; count low 32-bits instead
	xld.w	%r0, 32			; loop counter = 32

__L0004:						; count ; of leading 0's
	scan1	%r1, %r4			; %r1 = count
	jruge	__L0005				; if count != 8 then goto end
	cmp		%r0, 24
	jreq.d	__L0005				; if count = 32 then jump to end
	add		%r0, 8			; increment loop counter
	jp.d	__L0004
	sll		%r4, 8			; shift register to the left 8 bits

__L0005:
	add		%r1, %r0		; count = count + loop counter
	ld.w	%r5, %r1			; put result into output register

	popn	%r1				; restore register values

getman2:
	; isolate mantissa2

	xand	%r9, 0xfffff		; 引数２[H] = 引数２[H] & 0xfffff clear first 12 bits of %r9

	cmp		%r3, 0			; if 指数部２(%r3) = 0
	jreq.d	count2			; then jump to count2
	ld.w	%r3, %r5		; %r11 = count1

	; case: normal input
	xoor	%r9, 0x100000	; else add implied bit(0x100000)	@@@ 01/02/19
	jp.d	cmpcount
	ld.w	%r4, 11			; count2(%r12) = 11

count2:
	ld.w	%r4, %r9		; scan64 引数１(%r4) = 引数２[H](%r9)
	ld.w	%r5, %r8		; scan64 引数２(%r5) = 引数２[L](%r8)
	; @@@ del 01/02/19 xcall	__scan64		; %r13 = count2
	; 	SCAN64 	%r4, %r5		; @@@ add 01/02/19		
	pushn	%r1				; save register values

	ld.w	%r0, 0			; loop counter = 0

	cmp		%r4, 0			; if high 32-bits != 0 then count
	jrne	__L0006

	; case: $1 = 0
	ld.w	%r4, %r5			; count low 32-bits instead
	xld.w	%r0, 32			; loop counter = 32

__L0006:						; count ; of leading 0's
	scan1	%r1, %r4			; %r1 = count
	jruge	__L0007				; if count != 8 then goto end
	cmp		%r0, 24
	jreq.d	__L0007				; if count = 32 then jump to end
	add		%r0, 8			; increment loop counter
	jp.d	__L0006
	sll		%r4, 8			; shift register to the left 8 bits

__L0007:
	add		%r1, %r0		; count = count + loop counter
	ld.w	%r5, %r1			; put result into output register

	popn	%r1				; restore register values


	sub		%r2, 1			; flag(%r2) = flag(%r2) - 1 (0 or -1)

cmpcount:
	; 	SET_TEMP 	%r0, %r2	; 共有レジスタ[TEMP](%r0) <- flag(%r2)
__L0008:
	xand	%r0,0xc03fffff	; 共有レジスタ TEMPクリア
	xand	%r2,0xff			; 1バイト mask
	rr		%r2,8			; TEMPのビット位置にシフト
	rr		%r2,2			; TEMPのビット位置にシフト
	or		%r0,%r2			; TEMPセット
	rl		%r2,8			; 設定値復帰
	rl		%r2,2			; 設定値復帰

	cmp		%r3, %r5		; if count1(%r11) <= count2(%r13) 
	jrle	man2shift		; then goto man2shift

	; case: count1 > count2
	sub		%r3, %r5		; shift(%r3) = (count1(%r3) - count2(%r5)) - 1
	sub		%r3, 1

	; {%r7, %r6} << shift
	;xsll	%r7, %r11		; shift high 32-bits to the left x bits
	;xrl	%r6, %r11		; rotate low 32-bits to the left x bits
	;xsll	%r10, %r11		; make a mask for first 32-x bits --> %r10 = 111...000

	ld.w	%r2, -1			; %r2 = 0xffff ffff
	; used in 64-bit variable shifts	; 	SHFTROTSHFT %r3, %r7, %r6, %r2, sll, rl, %r5			; %r5 = temp
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r5, %r3		; temp = shift amount

__L0009:
	cmp		%r5, 8		; if temp <= 8 then goto $$2
	jrle	__L0010

	sll		%r7, 8		; shift 1st register
	rl		%r6, 8		; rotate 2nd register
	sll		%r2, 8		; shift 3rd register
	jp.d	__L0009
	sub		%r5, 8		; temp = temp - 8

__L0010:
	sll		%r7, %r5		; last shift
	rl		%r6, %r5		; last rotate
	sll		%r2, %r5		; last shift
	not		%r5, %r2		; flip mask for last x bits --> %r5 = 000...111 (mask)
	and		%r5, %r6		; isolate last x bits of %r6
	or		%r7, %r5		; add last x bits of %r6 to %r7
	and		%r6, %r2		; keep the high 32-x bits of %r6

	add		%r3, 1			; shift(%r3) = shift(%r3) + 1
	jp.d	divide
	ld.w	%r2, %r3		; lshift(%r2) = count1 - count2

man2shift:
	sub		%r5, %r3		; shift(%r5) = count2 - count1 + 1
	add		%r5, 1

	; {%r9, %r8} << shift
	;xsll	%r9, %r13		; shift high 32-bits to the left x bits
	;xrl	%r8, %r13		; rotate low 32-bits to the left x bits
	;xsll	%r10, %r13		; make a mask for first 32-x bits --> %r10 = 111...000

	ld.w	%r2, -1			; %r2 = 0xffff ffff
	; used in 64-bit variable shifts	; 	SHFTROTSHFT %r5, %r9, %r8, %r2, sll, rl, %r4		; %r4 = temp
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r4, %r5		; temp = shift amount

__L0011:
	cmp		%r4, 8		; if temp <= 8 then goto $$2
	jrle	__L0012

	sll		%r9, 8		; shift 1st register
	rl		%r8, 8		; rotate 2nd register
	sll		%r2, 8		; shift 3rd register
	jp.d	__L0011
	sub		%r4, 8		; temp = temp - 8

__L0012:
	sll		%r9, %r4		; last shift
	rl		%r8, %r4		; last rotate
	sll		%r2, %r4		; last shift
	not		%r4, %r2		; flip mask for last x bits --> %r4 = 000...111 (mask)
	and		%r4, %r8		; isolate last x bits of %r8
	or		%r9, %r4		; add last x bits of %r8 to %r9
	and		%r8, %r2		; keep the high 32-x bits of %r8

	not		%r2, %r5		; lshift(%r10) = count1 - count2 (~shift + 1 + 1)
	add		%r2, 2

divide:
	; man1 has x leading 0's
	; man2 has x-1 leading 0's
	; lshift used to normalize result
	; %r0 = sign; %r1 = exp1, %r3 = exp2
	; %r10 = lshift
	; 	GET_EXP2	%r0, %r3	; 指数部２(%r3) <- 共有レジスタ[指数部２](%r0)
__L0013:
	ld.w	%r3,%r0			
	srl		%r3,8
	srl		%r3,3
	xand	%r3,0x7ff			; 共有レジスタ 指数部２取得


	sub		%r1, %r3		; new sisu = sisu1 - sisu2 + bias + flag (0,1,-1)

	xadd	%r1, 0x3ff
	; 	GET_TEMP	%r0, %r3	; flag(%r3) <- 共有レジスタ[TEMP](%r0)	@@@ 01/02/19 add
__L0014:
	ld.w	%r3,%r0			
	rl		%r3,8
	rl		%r3,2
	xand	%r3,0xff			; 共有レジスタ TEMP取得

	add		%r1, %r3

	xcmp	%r1, 0x7ff		; if 指数部１(%r1) >= 0x7ff then goto overflow
	ext		overflow@rm
	jrge	overflow@rl
	xcmp	%r1, -52		; if 指数部１(%r1) <= -52 then underflow
	ext		end@rm
	jrle	end@rl

	; DIVIDE CODE STARTS HERE
	ld.w	%r3, %r1	
	rl		%r3,1			; 指数部１符号ビット保存
	; 	SET_SIGN2	%r0, %r3	; 共有レジスタ[符号ビット２](%r0) <- 指数部１符号ビット
__L0015:
	xand	%r0,0xbfffffff	; 共有レジスタ 符号ビット２クリア
	and		%r3,1			; 1ビットマスク
	rr		%r3,2			; 符号ビット２のビット位置にローテート
	or		%r0,%r3			; 符号ビット２セット
	rl		%r3,2			; 符号ビット復帰

	; 	SET_EXP1	%r0, %r1	; 共有レジスタ[指数部１](%r0) <- 指数部１(%r1) @@@ 01/02/19 add
__L0016:
	xand	%r0,0xfffff800	; 共有レジスタ 指数部１クリア
	xand	%r1,0x7ff		; １１ビットマスク
	or		%r0,%r1			; 指数部１セット

	; 	SET_TEMP	%r0, %r2	; 共有レジスタ[TEMP](%r0) <- lshift(%r2) @@@ 01/02/19 add
__L0017:
	xand	%r0,0xc03fffff	; 共有レジスタ TEMPクリア
	xand	%r2,0xff			; 1バイト mask
	rr		%r2,8			; TEMPのビット位置にシフト
	rr		%r2,2			; TEMPのビット位置にシフト
	or		%r0,%r2			; TEMPセット
	rl		%r2,8			; 設定値復帰
	rl		%r2,2			; 設定値復帰


	; make a mask for first 31 bits --> %r2 = 111...110
	; @@@ del 01/02/19 ld.w	%r2, -2			; %r2 = 0xffff fffe
	ld.w	%r3, 0			; loop(%r3) <- 0
	ld.w	%r1, 0			; flag(%r1) = 0
	ld.w	%r4, 0			; @@@ 01/02/26 add
	ld.w	%r5, 0			; @@@ 01/02/26 add


loop:
	add		%r3, 1			; loop(%r3) = loop(%r3) + 1
	xcmp	%r3, 55			; if loop(%r3) >= 55 then exit divide loop
	jrge	normalize

	; {result1, result2} << 1
	sll		%r5, 1			; shift high 32-bits to the left 1 bit
	rl		%r4, 1			; rotate low 32-bits to the left 1 bit
	ld.w	%r2, 1			; mask for last bit (LSB)
	and		%r2, %r4		; isolate last bit of %r8
	or		%r5, %r2		; add last bit of %r8 to %r9
	xand	%r4, -2			; keep the high 31 bits of %r8
	
	scan1	%r1, %r7		; is there a leading 1? (0 = yes)

	; {仮数部１[H](%r7), 仮数部１[L](%r6)} << 1
	sll		%r7, 1			; shift high 32-bits to the left 1 bit
	rl		%r6, 1			; rotate low 32-bits to the left 1 bit
	ld.w	%r2, 1			; mask for last bit (LSB)
	and		%r2, %r6		; isolate last bit of %r8
	or		%r7, %r2		; add last bit of %r8 to %r9
	xand	%r6, -2			; keep the high 31 bits of %r8
	cmp		%r1, 0			; if a leading 1 was shifted out then subtract
	jreq	subtract

	cmp		%r7, %r9
	jrugt	subtract		; if 仮数部１[H](%r7) > 仮数部２[H](%r9) then subtract
	jrult	loop			; if 仮数部１[H](%r7) < 仮数部２[H](%r9) then goto loop

	; case: man1 = man2 (%r7 = %r9)
	cmp		%r6, %r8		; if 仮数部１[L](%r6) < 仮数部２[L](%r8) then goto loop
	jrult	loop

subtract:
	sub		%r6, %r8		; 仮数部１[L](%r6) = 仮数部１[L](%r6) - 仮数部２[L](%r8)
	sbc		%r7, %r9		; 仮数部１[H](%r7) = 仮数部１[H](%r7) - 仮数部２[H](%r9) - carry
	add		%r4, 1			; 戻り値[L](%r4) = 戻り値[L](%r4) + 1 

	cmp		%r7, 0			; if {仮数部１[H](%r7), 仮数部１[L](%r6) } = 0 then exit
	jrne	loop
	cmp		%r6, 0
	jrne	loop

	xcmp	%r3, 54			; if loop(%r3) >= 54 then exit divide loop
	jrge	normalize

	; fill in the extra 0's needed in the result
	xld.w	%r1, 54
	sub		%r1, %r3		; extra(%r1) = 54 - loop(%r3)	
	xcmp	%r1, 32			; if extra(%r1) < 32 then goto shift
	jrlt	xshift

	; case: extra iterations >= 32
	ld.w	%r5, %r4		; 戻り値[H](%r5) <- 戻り値[L](%r4)
	ld.w	%r4, 0			; 戻り値[L](%r4) <- 0
	sub		%r1, 32			; extra(%r1) = extra(%r1) - 32
	
xshift:
	; {%r5, %r4} << extra
	;xsll	%r5, %r11		; shift high 32-bits to the left x bits
	;xrl	%r4, %r11		; rotate low 32-bits to the left x bits
	;xsll	%r3, %r11		; make a mask for first 32-x bits --> %r3 = 111...000

	ld.w	%r3, -1			; %r3 = 0xffff ffff
	; used in 64-bit variable shifts	; 	SHFTROTSHFT %r1, %r5, %r4, %r3, sll, rl, %r1
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r1, %r1		; temp = shift amount

__L0018:
	cmp		%r1, 8		; if temp <= 8 then goto $$2
	jrle	__L0019

	sll		%r5, 8		; shift 1st register
	rl		%r4, 8		; rotate 2nd register
	sll		%r3, 8		; shift 3rd register
	jp.d	__L0018
	sub		%r1, 8		; temp = temp - 8

__L0019:
	sll		%r5, %r1		; last shift
	rl		%r4, %r1		; last rotate
	sll		%r3, %r1		; last shift
	not		%r2, %r3		; flip mask for last x bits --> %r2 = 000...111 (mask)
	and		%r2, %r4		; isolate last x bits of %r6
	or		%r5, %r2		; add last x bits of %r6 to %r7
	and		%r4, %r3		; keep the high 32-x bits of %r6

	; DIVIDE CODE ENDS HERE

normalize:
	; %r0 = result sign, %r1 = result exponent
	; %r10 = lshift, (%r5, %r4) = result
	
	;ld.w	%r3, 10			; count = 10
	ld.w	%r3, 0			; count(%r3) <- 0

	ld.w	%r8, %r5
	xand	%r8, 0x200000	; check 21st bit

	jrne	continue2		; if 21st bit = 1 then count = 10
	add		%r3, 1			; else count(%r3) = count(%r3) + 1

continue2:
	; 	GET_EXP1	%r0, %r1	; 指数部１(%r1) <- 共有レジスタ[指数部１](%r0)
__L0020:
	ld.w	%r1,%r0			; 
	xand	%r1,0x7ff		; 共有レジスタ 指数部１取得

	; 	GET_SIGN2	%r0, %r2	; 指数部１符号ビット(%r2) <- 共有レジスタ[符号ビット２](%r0)
__L0021:
	ld.w	%r2,%r0			; 
	rl		%r2,2			; 符号ビットローテート
	and		%r2,1			; 共有レジスタ 符号ビット２取得

	cmp		%r2, 1			; 指数部１マイナスか？
	jrne	exp1_no_sign
	xoor	%r1, 0xfffff800	; 指数部１符号拡張
exp1_no_sign:


	; 	GET_TEMP	%r0, %r2	; lshift(%r2) <- 共有レジスタ[TEMP](%r0)
__L0022:
	ld.w	%r2,%r0			
	rl		%r2,8
	rl		%r2,2
	xand	%r2,0xff			; 共有レジスタ TEMP取得


	;sub		%r3, 10		; count = count - 10
	add		%r2, %r3		; lshift(%r2) = lshift指数部１ + count(%r3)
	cmp		%r1, %r2
	jrgt	normal			; if 指数部１(%r1) > lshift(%r10) then normal result

	; case: 指数部１(%r1) <= lshift(%r2)
	add		%r2, 1
	sub		%r2, %r3		; lshift(%r2) = lshift(%r2) + (11 - count(%r3))
	sub		%r2, %r1
	add		%r2, 1			; lshift(%r2) = lshift(%r2) - (指数部１(%r1) - 1)

	xcmp	%r2, 54		; if lshift(%r2) >= 54 then underflow
	jrlt	notunder
	ld.w	%r5, 0			; 戻り値[H](%r5) <- 0
	jp.d	end
	ld.w	%r4, 0

notunder:
	; {%r5, %r4} >> %r10
	;xsrl	%r4, %r10		; shift low 32-bits to the right x bits (shift amount = x)
	;xrr	%r5, %r10		; rotate high 32-bits to the right x bits
	;xsrl	%r3, %r10		; make a mask for last 32-x bits --> %r3 = 000...111

	ld.w	%r3, -1			; %r3 = 0xffff ffff
	; used in 64-bit variable shifts	; 	SHFTROTSHFT %r2, %r4, %r5, %r3, srl, rr, %r2
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r2, %r2		; temp = shift amount

__L0023:
	cmp		%r2, 8		; if temp <= 8 then goto $$2
	jrle	__L0024

	srl		%r4, 8		; shift 1st register
	rr		%r5, 8		; rotate 2nd register
	srl		%r3, 8		; shift 3rd register
	jp.d	__L0023
	sub		%r2, 8		; temp = temp - 8

__L0024:
	srl		%r4, %r2		; last shift
	rr		%r5, %r2		; last rotate
	srl		%r3, %r2		; last shift
	not		%r8, %r3		; flip mask for first x bits --> %r8 = 111...000 (mask)
	and		%r8, %r5		; isolate first x bits of %r5
	or		%r4, %r8		; add first x bits of %r5 to %r4
	and		%r5, %r3		; keep the low 32-x bits of %r5

	
	ld.w	%r1, 0			; 指数部１(%r1) <- 0 for denormal result
	; 	SET_EXP1	%r0, %r1
__L0025:
	xand	%r0,0xfffff800	; 共有レジスタ 指数部１クリア
	xand	%r1,0x7ff		; １１ビットマスク
	or		%r0,%r1			; 指数部１セット

	jp	finish
	; @@@ 01/03/02 del ld.w	%r1, 0			; 指数部１(%r1) <- 0 for denormal result

normal:	
	; case: exp > lshift
	sub		%r1, %r2		; 指数部１(%r1) = 指数部１(%r1) - lshift(%r2)
	; 	SET_EXP1	%r0, %r1	; 指数部１(%r1) <- 共有レジスタ[指数部１](%r0) @@@ 01/02/26 add
__L0026:
	xand	%r0,0xfffff800	; 共有レジスタ 指数部１クリア
	xand	%r1,0x7ff		; １１ビットマスク
	or		%r0,%r1			; 指数部１セット


	cmp		%r3, 1			; if original count(%r3) = 11 then no shifting needed
	jreq	overchk

	; {戻り値[H](%r5), 戻り値[L](%r4)} >> 1 (shift to the right 1 bit)
	srl		%r4, 1			; shift low 32-bits to the right 1 bit
	ld.w	%r3, 1			; mask
	and		%r3, %r5		; get LSB of high 32-bits
	rr		%r3, 1			; rotate to MSB position
	or		%r4, %r3		; add to %r4
	srl		%r5, 1			; shift high 32-bits to the right 1 bit

overchk:
	xcmp	%r1, 0x7ff		; if 指数部１(%r1) < 0x7ff then jump to finish
	jrlt	finish

overflow:
	xld.w	%r5, 0x7ff00000	; put infinity into result
	jp.d	end
	ld.w	%r4, 0

finish:
	; %r0 = sign, %r1 = exponent, %r5 = mantissa

	xand	%r5, 0xfffff		; 戻り値[H](%r5) = 戻り値[H](%r5) & 0xfffff

	; 	GET_EXP1	%r0, %r1	; 指数部１(%r1) <- 共有レジスタ[指数部１](%r0)
__L0027:
	ld.w	%r1,%r0			; 
	xand	%r1,0x7ff		; 共有レジスタ 指数部１取得


	xrr		%r1, 12			; position exponent bits to [30:20]
	or		%r5, %r1

end:
	; 	GET_SIGN1	%r0, %r2	; 符号ビット１(%r2) <- 共有レジスタ[符号ビット１](%r0)
__L0028:
	ld.w	%r2,%r0			; 
	rl		%r2,1			; 符号ビットローテート
	and		%r2,1			; 共有レジスタ 符号ビット１取得


	rr		%r2, 1			; position sign bit to MSB
	or		%r5, %r2		; 戻り値[H](%r5) = 戻り値[H](%r5) | 符号ビット１(%r1)

	;@@@ 01/01/23 add start hinokuchi
	;ld.w	%r13, [%sp+3]	; %r13復帰
	;ld.w	%r12, [%sp+2]	; %r12復帰
	;ld.w	%r11, [%sp+1]	; %r11復帰
	;ld.w	%r10, [%sp+0]	; %r10復帰
	;add		%sp, 4
	;@@@ 01/01/23 add end

	popn	%r3				; restore register values
	ret
