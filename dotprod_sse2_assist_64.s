# SIMD SSE2 dot product
# Equivalent to the following C code:
# long dotprod(signed short *a,signed short *b,int cnt)
# {
#	long sum = 0;
#	cnt *= 8;
#	while(cnt--)
#		sum += *a++ + *b++;
#	return sum;
# }
# a and b must be 128-bit aligned
# Copyright 2001, Phil Karn KA9Q
# May be used under the terms of the GNU Lesser General Public License (LGPL)

	.text
	.global dotprod_sse2_assist_64
	.type dotprod_sse2_assist_64,@function
dotprod_sse2_assist_64:
	push %rbx

	pxor %xmm0,%xmm0		# clear running sum (in two 32-bit halves)

# SSE2 dot product loop unrolled 4 times, crunching 32 terms per loop
	.align 16
.Loop1:	subl $4,%edx
	jl   .Loop1Done

	movdqa (%rdi),%xmm1
	pmaddwd (%rsi),%xmm1
	paddd %xmm1,%xmm0

	movdqa 16(%rdi),%xmm1
	pmaddwd 16(%rsi),%xmm1
	paddd %xmm1,%xmm0

	movdqa 32(%rdi),%xmm1
	pmaddwd 32(%rsi),%xmm1
	paddd %xmm1,%xmm0

	movdqa 48(%rdi),%xmm1
	add $64,%rdi
	pmaddwd 48(%rsi),%xmm1
	add $64,%rsi
	paddd %xmm1,%xmm0

	jmp .Loop1
.Loop1Done:

	addl $4,%edx

# SSE2 dot product loop, not unrolled, crunching 4 terms per loop
# This could be redone as Duff's Device on the unrolled loop above
.Loop2:	subl $1, %edx
	jl   .Loop2Done

	movdqa (%rdi),%xmm1
	add $16,%rdi
	pmaddwd (%rsi),%xmm1
	add $16,%rsi
	paddd %xmm1,%xmm0
	jmp .Loop2
.Loop2Done:

	movdqa %xmm0,%xmm1
	psrldq $8,%xmm0
	paddd %xmm1,%xmm0
	movd %xmm0,%eax		# right-hand word to eax
	psrldq $4,%xmm0
	movd %xmm0,%ebx
	add %ebx,%eax
	cdqe

	pop %rbx
	retq

