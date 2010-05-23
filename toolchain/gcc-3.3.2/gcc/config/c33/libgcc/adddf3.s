;*********************************************
;*	Copyright (C), 1996-2001 SEIKO EPSON Corp.
;*		ALL RIGHTS RESERVED
;*
;*	filename : adddf3.s
;*
;*	Double floating point addition function
;*						& subtract function
;*		input: (%r7, %r6) & (%r9, %r8)
;*		output: (%r5, %r4)
;*
;*	Begin				1996/09/12	V. Chan
;*  Fixed sign bug		1997/02/17	V. Chan
;*  �ѹ�				2001/01/18  O.Hinokuchi
;*  			 		 ���쥸�����֤�����
;*	gas�б�		2001/10/15	watanabe
;*
;*****************************************

.section .text
.align 1
.global	__adddf3
.global __subdf3

;==============================================
;��쥸����������
;	%r0:���ӥåȣ�(0(+) or 1(-))
;	%r1:�ؿ�����(8bit)
;	%r2:���ӥåȣ�(0(+) or 1(-))/count
;	%r3:�ؿ�����(8bit)
;	%r4:�����[L]
;	%r5:�����[H]
;	%r6:������[L](double)/��������[L]/shift counter
;	%r7:������[H](double)/��������[H]
;	%r8:������[L](double)/��������[L]
;	%r9:������[H](double)/��������[H]
;	%r10:temp/difference/implied
;	%r11:xflag
;	%r13:count/shift
;	-------------------------------------------
;	%r0:��ͭ�쥸����
;		bit31:�ؿ�����
;		bit30:�ؿ�����
;		bit29-22:TEMP(1byte)
;		bit21-11:�ؿ�����
;		bit10-0 :�ؿ�����
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

__subdf3:
	xxor	%r9, 0x80000000			; ������[H}(%r9)= ������[H](%r9) & 0x80000000 

__adddf3:
	pushn	%r3				; save register values

	;@@@ 01/01/23 add start hinokuchi
	;sub		%sp, 4
	;ld.w	[%sp+0], %r10	; %r10����
	;ld.w	[%sp+1], %r11	; %r11����
	;ld.w	[%sp+2], %r12	; %r12����
	;ld.w	[%sp+3], %r13	; %r13����
	;@@@ 01/01/23 add end
	
	;@@@ 01/02/14 del start
	;ld.w	%r0, %r7		; ���ӥåȣ�(%r0) <- ������[H](%r7) 
	;rl		%r0, 1			; ���ӥåȣ�(%r0) rotate left 1 bit
	;and		%r0, 1			; ���ӥåȣ�(%r0) & 1 
	;@@@ 01/01/23 del end
	ld.w	%r0, 0			; ��ͭ�쥸�������ꥢ(%r0) @@@ 01/02/14 add

	;@@@ 01/02/14 add start
	ld.w	%r1, %r7		; ���ӥåȣ�(%r1) <- ������[H](%r7) 
	rl		%r1, 1			; ���ӥåȣ�(%r1) rotate left 1 bit
	and		%r1, 1			; ���ӥåȣ�(%r1) & 1 
	; 	SET_SIGN1 %r0, %r1		; ��ͭ�쥸���������ӥåȣ���¸ 
__L0001:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r1,1			; 1�ӥåȥޥ���
	rr		%r1,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r1			; ���ӥåȣ����å�
	rl		%r1,1			; ���ӥå�����
	;@@@ 01/01/23 add end



	;@@@ 01/02/14 del start
	;ld.w	%r2, %r9		; ���ӥåȣ�(%r2) <- ������[H](%r9)
	;rl		%r2, 1			; ���ӥåȣ�(%r2) rotate left 1 bit
	;and	%r2, 1			; ���ӥåȣ�(%r2) & 1 
	;@@@ 01/01/23 del end

	;@@@ 01/02/14 add start
	ld.w	%r2, %r9		; ���ӥåȣ�(%r1) <- ������[H](%r9)
	rl		%r2, 1			; ���ӥåȣ�(%r1) rotate left 1 bit
	and		%r2, 1			; ���ӥåȣ�(%r1) & 1 
	; 	SET_SIGN2 %r0, %r2		; ��ͭ�쥸���������ӥåȣ���¸ 
__L0002:
	xand	%r0,0xbfffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r2,1			; 1�ӥåȥޥ���
	rr		%r2,2			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r2			; ���ӥåȣ����å�
	rl		%r2,2			; ���ӥå�����

	;@@@ 01/01/23 add end


	ld.w	%r1, %r7		; �ؿ�����(%r1) <- ������[H](%r7)
	sll		%r1, 1			; �ؿ�����(%r1) << 1
	xsrl 	%r1, 21			; �ؿ�����(%r1) >> 21

	; 	SET_EXP1 %r0, %r1		; @@@ 01/02/14 add start
__L0003:
	xand	%r0,0xfffff800	; ��ͭ�쥸���� �ؿ��������ꥢ
	xand	%r1,0x7ff		; �����ӥåȥޥ���
	or		%r0,%r1			; �ؿ��������å�


	xcmp	%r1, 0x7ff		; if �ؿ�����(%r1) >= �ؿ��������С��ե���(0x7ff) 
	xjrge	overflow		; then jump to overflow

	ld.w	%r3, %r9		; �ؿ�����(%r3) <- ������[H](%r9)
	sll 	%r3, 1			; �ؿ�����(%r3) << 1
	xsrl	%r3, 21			; �ؿ�����(%r3) >> 21

	; 	SET_EXP2 %r0, %r3		; @@@ 01/02/14 add start
__L0004:
	xand	%r0,0xffc007ff	; ��ͭ�쥸���� �ؿ��������ꥢ
	xand	%r3,0x7ff		; �����ӥåȥޥ���
	sll		%r3,8			; �ؿ������ΥӥåȰ��֤˥��ե�
	sll		%r3,3			; �ؿ������ΥӥåȰ��֤˥��ե�
	or		%r0,%r3			; �ؿ��������å�
	srl		%r3,8			; �ؿ�������
	srl		%r3,3			; �ؿ�������

	; 	GET_SIGN1	%r0, %r4
__L0005:
	ld.w	%r4,%r0			; 
	rl		%r4,1			; ���ӥåȥ��ơ���
	and		%r4,1			; ��ͭ�쥸���� ���ӥåȣ�����


	;@@@ del 01/02/15 ld.w	%r10, %r0		; temp(%r10) = ���ӥåȣ�(%r0)
	;@@@ del 01/02/15 ld.w	%r10, %r0		; temp(%r10) = ���ӥåȣ�(%r0)

	; 	SET_SIGN1 %r0, %r2		; ��ͭ�쥸����[���ӥåȣ�](%r0) <- ���ӥåȣ�(%r2)
__L0006:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r2,1			; 1�ӥåȥޥ���
	rr		%r2,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r2			; ���ӥåȣ����å�
	rl		%r2,1			; ���ӥå�����
	xcmp	%r3, 0x7ff		; if �ؿ�����(%r3) >= �ؿ��������С��ե���(0x7ff) 
	;@@@ del 01/02/15 ld.w	%r0, %r2		; ���ӥåȣ�(%r0) <- ���ӥåȣ�(%r2)
	xjrge	overflow		; then jump to overflow
	; 	SET_SIGN1 %r0, %r4		; ��ͭ�쥸����[���ӥåȣ�](%r0) <- ���ӥåȣ�(%r2)
__L0007:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r4,1			; 1�ӥåȥޥ���
	rr		%r4,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r4			; ���ӥåȣ����å�
	rl		%r4,1			; ���ӥå�����

	cmp		%r1, %r3		; if �ؿ�����(%r1) < �ؿ�����(%r3)
	jrlt	ex1ltex2		; then jump to ex1ltex2
	;@@@ del 01/02/15 ld.w	%r0, %r10		; ���ӥåȣ�(%r0) <- temp(%r10)

	; case: exp1 >= exp2
	ld.w	%r5, %r7		; �����[H](%r5) <- ������[H](%r7)
	;@@@ del 01/02/15 ld.w	%r10, %r1		; difference(%r10) = �ؿ�����(%r1) - �ؿ�����(%r3)
	ld.w	%r4, %r1		; difference(%r4) = �ؿ�����(%r1) - �ؿ�����(%r3)
	sub		%r4, %r3
	;@@@ del 01/02/15 sub		%r10, %r3

	;@@@ del 01/02/15 xcmp	%r10, 0x35		; if difference(%r10) >= 0x35 (53-bits) then
	xcmp	%r4, 0x35		; if difference(%r4) >= 0x35 (53-bits) then

	ld.w	%r4, %r6		; �����[L](%r4) <- ������[L](%r6)
	xjrge	end				; return first input
	jp		continue
	
ex1ltex2:
	ld.w	%r5, %r9		; �����[H](%r5) <- ������[H](%r9)
	;@@@ del 01/02/15 ld.w	%r10, %r3		; difference(%r10) = �ؿ�����(%r1) - �ؿ�����(%r3)
	ld.w	%r4, %r3		; difference(%r4) = �ؿ�����(%r1) - �ؿ�����(%r3)

	sub		%r4, %r1
	;@@@ del 01/02/15 sub		%r10, %r1
	
	;@@@ del 01/02/15 xcmp	%r10, 0x35		; if difference(%r10) >= 0x35 (53-bits) then
	xcmp	%r4, 0x35		; if difference(%r4) >= 0x35 (53-bits) then

	ld.w	%r4, %r8		; �����[L](%r4) <- ������[L](%r8)
	jrlt	continue
	;@@@ del 01/02/15 ld.w	%r0, %r2		; ���ӥåȣ�(%r0) <- ���ӥåȣ�(%r2)
	; 	SET_SIGN1 %r0, %r2		; ��ͭ�쥸����[���ӥåȣ�] <- ���ӥåȣ�(%r2)@@@ add 01/02/15
__L0008:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r2,1			; 1�ӥåȥޥ���
	rr		%r2,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r2			; ���ӥåȣ����å�
	rl		%r2,1			; ���ӥå�����
	xjp		end				; return 2nd input

continue:
	;@@@ del 01/02/15 xld.w	%r10, 0x100000	; implied bit(%r10) <- 0x100000
	xld.w	%r2, 0x100000	; implied bit(%r2) <- 0x100000

	;@@@ del 01/02/15 xld.w	%r13, 0xfffff	; mask(%r13) <- 0xfffff 
	xld.w	%r4, 0xfffff	; mask(%r4) <- 0xfffff 

	; isolate mantissa1
	cmp		%r1, 0			; if �ؿ�����(%r1) = 0 then goto getman2
	jreq.d	getman2
	;@@@ del 01/02/15 and		%r7, %r13		; ������[H](%r7) & mask(%r13) clear first 12 bits of %r7
	and		%r7, %r4		; ������[H](%r7) & mask(%r4) clear first 12 bits of %r7

	;@@@ del 01/02/15 or		%r7, %r10		; ������[H](%r7) | mplied bit(%r10)  else normal input --> add implied bit
	or		%r7, %r2		; ������[H](%r7) | mplied bit(%r2)  else normal input --> add implied bit
	
getman2:
	cmp		%r3, 0			; if �ؿ�����(%r3) = 0 then goto signs
	jreq.d	cmpexp
	;@@@ del 01/02/15 and		%r9, %r13		; ������[H](%r9) & mask(%r13) clear first 12 bits of %r9
	and		%r9, %r4		; ������[H](%r9) & mask(%r4) clear first 12 bits of %r9

	;@@@ del 01/02/15 or		%r9, %r10		; ������[H](%r9) | mplied bit(%r10) else normal input --> add implied bit
	or		%r9, %r2		; ������[H](%r9) | mplied bit(%r2) else normal input --> add implied bit

cmpexp:
	; compare exponents -- %r1 will be result exponent
	; if exp1 > exp2 then mantissa2 is shifted to the right
	; if exp2 > exp1 then mantissa1 is shifted to the right

	;@@@ del 01/02/15 ld.w	%r11, 0			; xflag(%r11) = 0
	ld.w 	%r4, 0
	; 	SET_TEMP %r0, %r4			; ��ͭ�쥸����[temp](%r0) <- xflag = 0 
__L0009:
	xand	%r0,0xc03fffff	; ��ͭ�쥸���� TEMP���ꥢ
	xand	%r4,0xff			; 1�Х��� mask
	rr		%r4,8			; TEMP�ΥӥåȰ��֤˥��ե�
	rr		%r4,2			; TEMP�ΥӥåȰ��֤˥��ե�
	or		%r0,%r4			; TEMP���å�
	rl		%r4,8			; ����������
	rl		%r4,2			; ����������


	cmp		%r1, %r3
	jreq	negation		; if �ؿ�����(%r1) = �ؿ�����(%r3) then jump to negation
	jrgt	man2			; if �ؿ�����(%r1) > �ؿ�����(%r3) then jump to man2

	; case: exp1 < exp2
	;@@@ del 01/02/15 ld.w	%r11, 2			; xflag(%r11) = 2
	ld.w 	%r4, 2
	; 	SET_TEMP %r0, %r4		; ��ͭ�쥸����[temp](%r0) <- xflag = 2 
__L0010:
	xand	%r0,0xc03fffff	; ��ͭ�쥸���� TEMP���ꥢ
	xand	%r4,0xff			; 1�Х��� mask
	rr		%r4,8			; TEMP�ΥӥåȰ��֤˥��ե�
	rr		%r4,2			; TEMP�ΥӥåȰ��֤˥��ե�
	or		%r0,%r4			; TEMP���å�
	rl		%r4,8			; ����������
	rl		%r4,2			; ����������


	;@@@ del 01/02/15 ld.w	%r10, %r1		; temp(%r10) = �ؿ�����(%r1)
	ld.w	%r4, %r1		; temp(%r4) = �ؿ�����(%r1)

	ld.w	%r1, %r3		; �ؿ�����(%r1) = �ؿ�����(%r3)
	; 	SET_EXP1 %r0, %r1		; @@@ 01/02/26 add start
__L0011:
	xand	%r0,0xfffff800	; ��ͭ�쥸���� �ؿ��������ꥢ
	xand	%r1,0x7ff		; �����ӥåȥޥ���
	or		%r0,%r1			; �ؿ��������å�


	;@@@ del 01/02/15 sub		%r3, %r10		; shift (%r3) = �ؿ�����(%r3) - �ؿ�����(%r1)
	sub		%r3, %r4		; shift (%r3) = �ؿ�����(%r3) - �ؿ�����(%r4)

	;@@@ del 01/02/15 cmp		%r10, 0			; if �ؿ�����(%r1) != 0 then goto shftm1 (normal)
	cmp		%r4, 0			; if �ؿ�����(%r4) != 0 then goto shftm1 (normal)

	jrne	shftm1
	sub		%r3, 1			; shift(%r3) = shift (%r3) - 1  else denormal --> decrement shift

shftm1:
	xcmp	%r3, 32			; if shift(%r3) < 32 then 64-bit shift
	jrlt	shift1

	; case: shift >=32
	ld.w	%r6, %r7		; ��������[L] <- ��������[H] (32-bit shift)
	ld.w	%r7, 0			; ��������[H] <- 0
	sub		%r3, 32			; shift(%r3) = shift(%r3) - 32
	;xsrl	%r6, %r3		; man1_2 >> shift
	; used in 32-bit variable shifting	; 	VARSHIFT %r6, %r3, srl
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0012:
	cmp		%r3, 8		; if temp <= 8 then goto $$2
	jrle	__L0013

	srl		%r6, 8		; shift input register 8 bits
	jp.d	__L0012
	sub		%r3, 8		; temp = temp - 8

__L0013:
	srl		%r6, %r3		; last shift
	jp		negation

shift1:
	; shift {man1, man1_2} right
	;xsrl	%r6, %r3		; shift low 32-bits to the right x bits (shift amount = x)
	;xrr	%r7, %r3		; rotate high 32-bits to the right x bits
	;xsrl	%r10, %r3		; make a mask for last 32-x bits --> %r10 = 000...111

	;@@@ del 01/02/15 ld.w	%r10, -1		; mask(%r10) <- 0xffff ffff
	ld.w	%r4, -1		; mask(%r4) <- 0xffff ffff

	;@@@ del 01/02/15 SHFTROTSHFT %r3, %r6, %r7, %r10, srl, rr, %r3
	; used in 64-bit variable shifting	; 	SHFTROTSHFT %r3, %r6, %r7, %r4, srl, rr, %r3
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r3, %r3		; temp = shift amount

__L0014:
	cmp		%r3, 8		; if temp <= 8 then goto $$2
	jrle	__L0015

	srl		%r6, 8		; shift 1st register
	rr		%r7, 8		; rotate 2nd register
	srl		%r4, 8		; shift 3rd register
	jp.d	__L0014
	sub		%r3, 8		; temp = temp - 8

__L0015:
	srl		%r6, %r3		; last shift
	rr		%r7, %r3		; last rotate
	srl		%r4, %r3		; last shift

	;@@@ del 01/02/15 not		%r3, %r10		; shift(%r3) = ~mask(%r10)   flip mask for first x bits --> %r3 = 111...000 (mask)
	not		%r3, %r4		; shift(%r3) = ~mask(%r4)   flip mask for first x bits --> %r3 = 111...000 (mask)

	and		%r3, %r7		; isolate first x bits of %r7
	or		%r6, %r3		; add first x bits of %r7 to %r6
	jp.d	negation
	;@@@ del 01/02/15 and		%r7, %r10		; keep the low 32-x bits of %r7
	and		%r7, %r4		; keep the low 32-x bits of %r7


man2:						; case: �ؿ�����(%r1) > �ؿ�����(%r3)
	;@@@ del 01/02/15 ld.w	%r11, 1			; xflag(%r11) = 1
	ld.w 	%r4, 1
	; 	SET_TEMP %r0, %r4		; ��ͭ�쥸����[temp](%r0) <- xflag = 1 
__L0016:
	xand	%r0,0xc03fffff	; ��ͭ�쥸���� TEMP���ꥢ
	xand	%r4,0xff			; 1�Х��� mask
	rr		%r4,8			; TEMP�ΥӥåȰ��֤˥��ե�
	rr		%r4,2			; TEMP�ΥӥåȰ��֤˥��ե�
	or		%r0,%r4			; TEMP���å�
	rl		%r4,8			; ����������
	rl		%r4,2			; ����������


	;@@@ del 01/02/15 ld.w	%r10, %r1		; temp(%r10) <- �ؿ�����(%r1)
	ld.w	%r4, %r1		; temp(%r4) <- �ؿ�����(%r1)

	;@@@ del 01/02/15 sub		%r10, %r3		; shift(%r10) = �ؿ�����(%r1) - �ؿ�����(%r3)
	sub		%r4, %r3		; shift(%r4) = �ؿ�����(%r1) - �ؿ�����(%r3)

	cmp		%r3, 0			; if �ؿ�����(%r1) != 0 then goto shftm2 (normal)
	jrne	shftm2
	;@@@ del 01/02/15 sub		%r10, 1			; else denormal --> shift(%r10) = shift(%r10) - 1
	sub		%r4, 1			; else denormal --> shift(%r4) = shift(%r4) - 1
	
shftm2:
	;@@@ del 01/02/15 xcmp	%r10, 32		; if shift(%r10) < 32 then 64-bit shift
	xcmp	%r4, 32		; if shift(%r4) < 32 then 64-bit shift

	jrlt	shift2

	; case: shift >=32
	ld.w	%r8, %r9		; ��������[L] <- ��������[H] 
	ld.w	%r9, 0			; ��������[H] <- 0
	;@@@ del 01/02/15 sub		%r10, 32		; shift(%r10) = shift(%r10) - 32
	sub		%r4, 32			; shift(%r4) = shift(%r4) - 32

	;xsrl	%r8, %r10		; man2_2 >> shift
	;@@@ del 01/02/15 VARSHIFT %r8, %r10, srl
	; used in 32-bit variable shifting	; 	VARSHIFT %r8, %r4, srl
	; $1 = input register
	; $2 = shift amount
	; $3 = shift instruction

__L0017:
	cmp		%r4, 8		; if temp <= 8 then goto $$2
	jrle	__L0018

	srl		%r8, 8		; shift input register 8 bits
	jp.d	__L0017
	sub		%r4, 8		; temp = temp - 8

__L0018:
	srl		%r8, %r4		; last shift

	jp		negation

shift2:
	; shift {man2, man2_2} right
	;xsrl	%r8, %r10		; shift low 32-bits to the right x bits (shift amount = x)
	;xrr	%r9, %r10		; rotate high 32-bits to the right x bits
	;xsrl	%r3, %r10		; make a mask for last 32-x bits --> %r3 = 000...111

	ld.w	%r3, -1			; %r3 = 0xffff ffff
	;@@@ del 01/02/15 SHFTROTSHFT %r10, %r8, %r9, %r3, srl, rr, %r13		; %r13 = temp
	; used in 64-bit variable shifting	; 	SHFTROTSHFT %r4, %r8, %r9, %r3, srl, rr, %r4		; %r4 = temp
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r4, %r4		; temp = shift amount

__L0019:
	cmp		%r4, 8		; if temp <= 8 then goto $$2
	jrle	__L0020

	srl		%r8, 8		; shift 1st register
	rr		%r9, 8		; rotate 2nd register
	srl		%r3, 8		; shift 3rd register
	jp.d	__L0019
	sub		%r4, 8		; temp = temp - 8

__L0020:
	srl		%r8, %r4		; last shift
	rr		%r9, %r4		; last rotate
	srl		%r3, %r4		; last shift

	;@@@ del 01/02/15 not		%r10, %r3		; flip mask for first x bits --> %r10 = 111...000 (mask)
	not		%r4, %r3		; flip mask for first x bits --> %r4 = 111...000 (mask)

	;@@@ del 01/02/15 and		%r10, %r9		; isolate first x bits of %r9
	and		%r4, %r9		; isolate first x bits of %r9

	;@@@ del 01/02/15 or		%r8, %r10		; add first x bits of %r9 to %r8
	or		%r8, %r4		; add first x bits of %r9 to %r8

	and		%r9, %r3		; keep the low 32-x bits of %r9

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

	; 	GET_TEMP 	%r0, %r1	; xflag(%r1) <- ��ͭ�쥸����[TEMP](%r0)	@@@ 01/02/15 add
__L0021:
	ld.w	%r1,%r0			
	rl		%r1,8
	rl		%r1,2
	xand	%r1,0xff			; ��ͭ�쥸���� TEMP����

	; 	GET_SIGN1	%r0, %r4	; ���ӥåȣ�(%r4) <- ��ͭ�쥸����[���ӥåȣ�](%r0)	@@@ 01/02/15 add
__L0022:
	ld.w	%r4,%r0			; 
	rl		%r4,1			; ���ӥåȥ��ơ���
	and		%r4,1			; ��ͭ�쥸���� ���ӥåȣ�����

	; 	GET_SIGN2	%r0, %r2	; ���ӥåȣ�(%r2) <- ��ͭ�쥸����[���ӥåȣ�](%r0)	@@@ 01/02/15 add
__L0023:
	ld.w	%r2,%r0			; 
	rl		%r2,2			; ���ӥåȥ��ơ���
	and		%r2,1			; ��ͭ�쥸���� ���ӥåȣ�����


	;@@@ del 01/02/15 cmp		%r0, %r2		; if ���ӥåȣ�(%r0) = ���ӥåȣ�(%r2)
	cmp		%r4, %r2		; if ���ӥåȣ�(%r4) = ���ӥåȣ�(%r2)

	jreq	sign			; then goto sign
	;@@@ del 01/02/15 cmp		%r0, 1			; if ���ӥåȣ�(%r0) != 1 
	cmp		%r4, 1			; if ���ӥåȣ�(%r4) != 1 

	jrne	negm2			; then goto negm2

	; case: sign1 = 1
	;@@@ del 01/02/15 cmp		%r11, 0			; only change %r0 if xflag(%r11) = 0
	cmp		%r1, 0			; only change %r0 if xflag(%r1) = 0

	jrne	negm1
	;@@@ del 01/02/15 ld.w	%r0, 0			; ���ӥåȣ�(%r0) <- 0
	ld.w	%r4, 0
	; 	SET_SIGN1	%r0, %r4		; ��ͭ�쥸����[���ӥåȣ�] <- 0
__L0024:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r4,1			; 1�ӥåȥޥ���
	rr		%r4,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r4			; ���ӥåȣ����å�
	rl		%r4,1			; ���ӥå�����

negm1:
	not		%r7, %r7		; negate the first input
	not		%r6, %r6
	add		%r6, 1
	adc		%r7, %r2		; using %r2 (equals 0) to add the carry-over
	jp		sign

negm2:
	; case: sign2 = 1
	not		%r9, %r9		; negate the second input
	not		%r8, %r8
	add		%r8, 1
	;@@@ del 01/02/15 adc		%r9, %r0		; using %r0 (equals 0) to add carry-over
	adc		%r9, %r4		; using %r4 (equals 0) to add carry-over

sign:
	; fix sign
	; if xflag = 2 then result sign = sign2 (%r0 = %r2)
	; if xflag = 1 then result sign = sign1	(%r0 = %r0)
	; if xflag = 0 then result sign = 0 or 1

	cmp		%r1, 2			; if xflag(%r1) != 2
	jrne	addition		; then goto  addition

	; case: xflag = 2
	;@@@ del 01/02/15 ld.w	%r0, %r2		; ���ӥåȣ�(%r0) <- ���ӥåȣ�(%r2)
	; 	SET_SIGN1	%r0, %r2		; ��ͭ�쥸����[���ӥåȣ�] <- ���ӥåȣ�(%r2)
__L0025:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r2,1			; 1�ӥåȥޥ���
	rr		%r2,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r2			; ���ӥåȣ����å�
	rl		%r2,1			; ���ӥå�����


addition:
	; %r0 = result sign, %r1 = result exponent
	; {%r7, %r6} = mantissa1, {%r9, %r8} = mantiss2
	; {%r5, %r5} = result

	ld.w	%r4, %r6		; �����[L](%r4) = ��������[L](6) + ��������[L](%r8)
	add		%r4, %r8

	ld.w	%r5, %r7		; �����[H](%r5) = ��������[H](%r7) + ��������[H](%r9) (with carry)
	adc		%r5, %r9

	cmp		%r5, 0
	jrgt	count			; if �����[H](%r5)  > 0 then normalize
	jrlt	negative		; if �����[H](%r5)  < 0 then negate result first

	; case: result1 = 0
	cmp		%r4, 0
	jreq	end				; if �����[L](%r4) = 0 then end
	jrgt	count			; if �����[L](%r4) > 0 then normalize

negative:					; result is negative
	not		%r5, %r5		; negate the 64-bit result
	not		%r4, %r4
	add		%r4, 1			; �����[L](%r4) = ~�����[L](%r4) + 1
	ld.w	%r2, 0			; %r2 = 0 (temp variable for carry-over add)
	adc		%r5, %r2		; �����[H](%r5) = ~�����[H](%r5) + carry over

	;@@@ 01/02/15 del cmp		%r11, 0			; if xflag(%r11) != 0 then goto count
	cmp		%r1, 0			; if xflag(%r1) != 0 then goto count

	jrne	count
	;@@@ 01/02/15 del ld.w	%r0, 1			; sign was 0 now sign = 1
	ld.w	%r6, 1
	; 	SET_SIGN1	%r0, %r6		; ��ͭ�쥸����[���ӥåȣ�] <- 1
__L0026:
	xand	%r0,0x7fffffff	; ��ͭ�쥸���� ���ӥåȣ����ꥢ
	and		%r6,1			; 1�ӥåȥޥ���
	rr		%r6,1			; ���ӥåȣ��ΥӥåȰ��֤˥��ơ���
	or		%r0,%r6			; ���ӥåȣ����å�
	rl		%r6,1			; ���ӥå�����

count:
	; %r0 = result sign, %r1 = result exponent
	; {%r5, %r4} = result, %r13 = count

	ld.w	%r6, %r1		; xflag(%r6) <- xflag(%r1) @@@ 01/02/15 add

	;@@@ 01/02/15 del ld.w	%r12, %r5		; copy result to input register of subroutine
	ld.w	%r8, %r5		; copy result to input register of subroutine

	;@@@ 01/02/15 del ld.w	%r13, %r4
	ld.w	%r9, %r4
	xcall	__scan64		; call 64-bit scan subroutine
		
	; check result exponent for normalizing	

	; 	GET_EXP1 	%r0, %r1	; �ؿ�����(%r1) <- ��ͭ�쥸����[�ؿ�����](%r0)
__L0027:
	ld.w	%r1,%r0			; 
	xand	%r1,0x7ff		; ��ͭ�쥸���� �ؿ���������

	cmp		%r1, 0			; if �ؿ�����(%r1) != 0 then goto normalize
	jrne	normalize

	; case: exp = 0
	;@@@ 01/02/15 del cmp		%r13, 11		; if count(%r13) != 11 then goto end (denormal result)
	cmp		%r9, 11			; if count(%r9) != 11 then goto end (denormal result)

	jrne	finish

	; case: count = 11 & exp = 0 (denormal + denormal with carry-over result)
	jp.d	finish
	ld.w	%r1, 1			; �ؿ�����(%r1) <- 1	

normalize:
	;@@@ 01/02/15 del cmp		%r13, 11
	cmp		%r9, 11

	jreq	finish			; if count(%r13) = 11 then goto finish (no shifting needed)
	jrlt	rshift			; if count(%r9) < 11 then need to shift right

	; case: count > 11	(need to shift left)
	;@@@ 01/02/15 del sub		%r13, 11		; count(%r13) = count(%r13) - 11
	sub		%r9, 11			; count(%r9) = count(%r9) - 11

	;@@@ 01/02/15 del cmp		%r1, %r13		; if �ؿ�����(%r1) > count then normal result
	cmp		%r1, %r9		; if �ؿ�����(%r1) > count(%r9) then normal result
	jrgt	lshift

	; case: exp <= count (denormal result)
	sub		%r1, 1			; �ؿ�����(%r1) = �ؿ�����(%r1) - 1
	jp.d	lshift
	;@@@ 01/02/15 del ld.w	%r13, %r1		; shift(%r13) = �ؿ�����(%r1)
	ld.w	%r9, %r1		; shift(%r9) = �ؿ�����(%r1)


lshift:
	; {%r5, %r4} << shift
	;xsll	%r5, %r13		; shift high 32-bits to the left x bits (shift amount = x)
	;xrl	%r4, %r13		; rotate low 32-bits to the left x bits
	;xsll	%r2, %r13		; make a mask for first 32-x bits --> %r2 = 111...000

	ld.w	%r2, -1			; %r2 = 0xffff ffff
	;@@@ 01/02/15 del SHFTROTSHFT %r13, %r5, %r4, %r2, sll, rl, %r11
	; used in 64-bit variable shifting	; 	SHFTROTSHFT %r9, %r5, %r4, %r2, sll, rl, %r6
	; $1 = shift amount
	; $2 = 1st input register (shifted)
	; $3 = 2nd input register (rotated)
	; $4 = 3rd input register (mask --> shifted)
	; $5 = shift instruction
	; $6 = rotate instruction
	; $7 = temp register
	ld.w	%r6, %r9		; temp = shift amount

__L0028:
	cmp		%r6, 8		; if temp <= 8 then goto $$2
	jrle	__L0029

	sll		%r5, 8		; shift 1st register
	rl		%r4, 8		; rotate 2nd register
	sll		%r2, 8		; shift 3rd register
	jp.d	__L0028
	sub		%r6, 8		; temp = temp - 8

__L0029:
	sll		%r5, %r6		; last shift
	rl		%r4, %r6		; last rotate
	sll		%r2, %r6		; last shift

	not		%r3, %r2		; flip mask for last x bits --> %r3 = 000...111 (mask)
	and		%r3, %r4		; isolate last x bits of %r4
	or		%r5, %r3		; add last x bits of %r4 to %r5
	and		%r4, %r2		; keep the high 32-x bits of %r4

	jp.d	finish
	;@@@ 01/02/15 del sub		%r1, %r13		; �ؿ�����(%r1) = �ؿ�����(%r1) - shift(%r13)
	sub		%r1, %r9		; �ؿ�����(%r1) = �ؿ�����(%r1) - shift(%r9)

rshift:
	; shift to the right 1 bit
	srl		%r4, 1			; shift low 32-bits to the right 1 bit
	ld.w	%r3, 1			; mask
	and		%r3, %r5		; get LSB of high 32-bits
	rr		%r3, 1			; rotate to MSB position
	or		%r4, %r3		; add to %r4
	srl		%r5, 1			; shift high 32-bits to the right 1 bit
	add		%r1, 1			; exp = exp + 1

	; overflow check	
	xcmp	%r1, 0x7ff		; if �ؿ�����(%r1) < 0x7ff then jump to finish
	jrlt	finish

overflow:
	xld.w	%r5, 0x7ff00000	; put infinity into result
	jp.d	end				; delayed jump
	ld.w	%r4, 0

finish:
	; %r0 = sign, %r1 = exponent, %r5 = mantissa
	xand	%r5, 0xfffff		; isolate mantissa

	xrr		%r1, 12			; position exponent bits to [30:23]
	or		%r5, %r1

end:
	;@@@ 01/02/15 del rr		%r0, 1			; position sign bit to MSB
	; 	GET_SIGN1	%r0, %r6	; ���ӥåȣ�(%r6) <- ��ͭ�쥸����[���ӥåȣ�](%r0)
__L0030:
	ld.w	%r6,%r0			; 
	rl		%r6,1			; ���ӥåȥ��ơ���
	and		%r6,1			; ��ͭ�쥸���� ���ӥåȣ�����

	rr		%r6, 1			; position sign bit to MSB	@@@ 01/02/15 add
	;@@@ 01/02/15 del or		%r5, %r0		; �����[H](%r5) | ���ӥåȣ�(%r0)
	or		%r5, %r6		; �����[H](%r5) | ���ӥåȣ�(%r0)

	;@@@ 01/01/23 add start hinokuchi
	;ld.w	%r13, [%sp+3]	; %r13����
	;ld.w	%r12, [%sp+2]	; %r12����
	;ld.w	%r11, [%sp+1]	; %r11����
	;ld.w	%r10, [%sp+0]	; %r10����
	;add		%sp, 4
	;@@@ 01/01/23 add end
	popn	%r3				; restore register values
	ret
