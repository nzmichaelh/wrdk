/* libgcc1 routines for EPSON C33.
   Copyright (C) 1996, 1997 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file with other programs, and to distribute
those programs without any restriction coming from the use of this
file.  (The General Public License restrictions do apply in other
respects; for example, they cover modification of the file, and
distribution when not linked into another program.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* As a special exception, if you link this library with files
   compiled with GCC to produce an executable, this does not cause
   the resulting executable to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

/*
	Copyright (C) SEIKO EPSON CORP. 1996

	Filename : divsi3.c
	Function :
	  This module defines the functions
		that emulate signed and unsigned integer division.
	Revision :
	  10/18/1996	ESD T.Katahira		start			    */

#ifdef L_divsi3

//	Function :	__divsi3
//	Input :		r6	--- dividend
//			r7	--- divisor
//	Output :	r4	--- quotient
//	Function :	calculate signed integer division

	.section .text
	.align 1
	.global	__divsi3
__divsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (alr)

	div0s	%r7		; initializer of signed division

#ifdef	FAST
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	div1	%r7		; execute division #9
	div1	%r7		; execute division #10
	div1	%r7		; execute division #11
	div1	%r7		; execute division #12
	div1	%r7		; execute division #13
	div1	%r7		; execute division #14
	div1	%r7		; execute division #15
	div1	%r7		; execute division #16
	div1	%r7		; execute division #17
	div1	%r7		; execute division #18
	div1	%r7		; execute division #19
	div1	%r7		; execute division #20
	div1	%r7		; execute division #21
	div1	%r7		; execute division #22
	div1	%r7		; execute division #23
	div1	%r7		; execute division #24
	div1	%r7		; execute division #25
	div1	%r7		; execute division #26
	div1	%r7		; execute division #27
	div1	%r7		; execute division #28
	div1	%r7		; execute division #29
	div1	%r7		; execute division #30
	div1	%r7		; execute division #31
	div1	%r7		; execute division #32
#else
	ld.w	%r8,0x4		; set loop counter (N = 4)
	ld.w	%r9,%psr	; save flag register
___divsi3_loop_start:	
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	sub		%r8,0x1		; decrement loop counter
	jrne.d	___divsi3_loop_start	; if (loop counter != 0) goto loop top
	ld.w	%psr,%r9	; restore flag register (delayed slot)
#endif
	div2s	%r7		; post divistion process #1
	div3s			; post divistion process #2
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%alr	; set quotient to return reg (delayed slot)
#endif /* L_divsi3 */

#ifdef L_udivsi3

//	Function :	__udivsi3
//	Input :		r6	--- dividend
//			r7	--- divisor
//	Output :	r4	--- quotient
//	Function :	calculate unsigned integer division

	.section .text
	.align 1
	.global	__udivsi3
__udivsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (alr)
	div0u	%r7		; initializer of signed division
#ifdef	FAST
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	div1	%r7		; execute division #9
	div1	%r7		; execute division #10
	div1	%r7		; execute division #11
	div1	%r7		; execute division #12
	div1	%r7		; execute division #13
	div1	%r7		; execute division #14
	div1	%r7		; execute division #15
	div1	%r7		; execute division #16
	div1	%r7		; execute division #17
	div1	%r7		; execute division #18
	div1	%r7		; execute division #19
	div1	%r7		; execute division #20
	div1	%r7		; execute division #21
	div1	%r7		; execute division #22
	div1	%r7		; execute division #23
	div1	%r7		; execute division #24
	div1	%r7		; execute division #25
	div1	%r7		; execute division #26
	div1	%r7		; execute division #27
	div1	%r7		; execute division #28
	div1	%r7		; execute division #29
	div1	%r7		; execute division #30
	div1	%r7		; execute division #31
	div1	%r7		; execute division #32
#else
	ld.w	%r8,0x4		; set loop counter (N = 4)
___udivsi3_loop_start:	
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	sub		%r8,0x1		; decrement loop counter
	jrne	___udivsi3_loop_start	; if (loop counter != 0) goto loop top
#endif
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%alr	; set quotient to return reg (delayed slot)

#endif /* L_udivsi3 */


#ifdef L_modsi3

//	Function :	__modsi3
//	Input :		r6	--- dividend
//			r7	--- divisor
//	Output :	r4	--- remainder
//	Function :	calculate signed integer modulo arithmetic

	.section .text
	.align 1
	.global	__modsi3
__modsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (alr)
	div0s	%r7		; initializer of signed division
#ifdef	FAST
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	div1	%r7		; execute division #9
	div1	%r7		; execute division #10
	div1	%r7		; execute division #11
	div1	%r7		; execute division #12
	div1	%r7		; execute division #13
	div1	%r7		; execute division #14
	div1	%r7		; execute division #15
	div1	%r7		; execute division #16
	div1	%r7		; execute division #17
	div1	%r7		; execute division #18
	div1	%r7		; execute division #19
	div1	%r7		; execute division #20
	div1	%r7		; execute division #21
	div1	%r7		; execute division #22
	div1	%r7		; execute division #23
	div1	%r7		; execute division #24
	div1	%r7		; execute division #25
	div1	%r7		; execute division #26
	div1	%r7		; execute division #27
	div1	%r7		; execute division #28
	div1	%r7		; execute division #29
	div1	%r7		; execute division #30
	div1	%r7		; execute division #31
	div1	%r7		; execute division #32
#else
	ld.w	%r8,0x4		; set loop counter (N = 4)
	ld.w	%r9,%psr	; save flag register
___modsi3_loop_start:	
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	sub		%r8,0x1		; decrement loop counter
	jrne.d	___modsi3_loop_start	; if (loop counter != 0) goto loop top
	ld.w	%psr,%r9	; restore flag register (delayed slot)
#endif
	div2s	%r7		; post divistion process #1
	div3s			; post divistion process #2
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%ahr	; set remainder to return reg (delayed slot)

#endif /* L_modsi3 */

#ifdef L_umodsi3

//	Function :	__umodsi3
//	Input :		r6	--- dividend
//			r7	--- divisor
//	Output :	r4	--- remainder
//	Function :	calculate unsigned integer modulo arithmetic

	.section .text
	.align 1
	.global	__umodsi3
__umodsi3:
	ld.w	%alr,%r6	; set dividend to accumlator (alr)
	div0u	%r7		; initializer of signed division
#ifdef	FAST
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	div1	%r7		; execute division #9
	div1	%r7		; execute division #10
	div1	%r7		; execute division #11
	div1	%r7		; execute division #12
	div1	%r7		; execute division #13
	div1	%r7		; execute division #14
	div1	%r7		; execute division #15
	div1	%r7		; execute division #16
	div1	%r7		; execute division #17
	div1	%r7		; execute division #18
	div1	%r7		; execute division #19
	div1	%r7		; execute division #20
	div1	%r7		; execute division #21
	div1	%r7		; execute division #22
	div1	%r7		; execute division #23
	div1	%r7		; execute division #24
	div1	%r7		; execute division #25
	div1	%r7		; execute division #26
	div1	%r7		; execute division #27
	div1	%r7		; execute division #28
	div1	%r7		; execute division #29
	div1	%r7		; execute division #30
	div1	%r7		; execute division #31
	div1	%r7		; execute division #32
#else
	ld.w	%r8,0x4		; set loop counter (N = 4)
___umodsi3_loop_start:	
	div1	%r7		; execute division #1
	div1	%r7		; execute division #2
	div1	%r7		; execute division #3
	div1	%r7		; execute division #4
	div1	%r7		; execute division #5
	div1	%r7		; execute division #6
	div1	%r7		; execute division #7
	div1	%r7		; execute division #8
	sub		%r8,0x1		; decrement loop counter
	jrne	___umodsi3_loop_start	; if (loop counter != 0) goto loop top
#endif
	ret.d			; return to the caller (use delayed return)
	ld.w	%r4,%ahr	; set remainder to return reg (delayed slot)

#endif /* L_umodsi3 */
