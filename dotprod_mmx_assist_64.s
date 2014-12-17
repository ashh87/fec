# SIMD MMX dot product
# Equivalent to the following C code:
# long dotprod(signed short *a,signed short *b,int cnt)
# {
#	long sum = 0;
#	cnt *= 4;
#	while(cnt--)
#		sum += *a++ + *b++;
#	return sum;
# }
# a and b should also be 64-bit aligned, or speed will suffer greatly
# Copyright 1999, Phil Karn KA9Q
# May be used under the terms of the GNU Lesser General Public License (LGPL)

	.text
	.global dotprod_mmx_assist_64
	.type dotprod_mmx_assist_64,@function
dotprod_mmx_assist_64:
	push %rbx

	pxor %mm0,%mm0		# clear running sum (in two 32-bit halves)

# MMX dot product loop unrolled 4 times, crunching 16 terms per loop
	.align 16
.Loop1:	subl $4,%edx
	jl   .Loop1Done

	movq (%rdi),%mm1	# mm1 = a[3],a[2],a[1],a[0]
	pmaddwd (%rsi),%mm1	# mm1 = b[3]*a[3]+b[2]*a[2],b[1]*a[1]+b[0]*a[0]
	paddd %mm1,%mm0

	movq 8(%rdi),%mm1
	pmaddwd 8(%rsi),%mm1
	paddd %mm1,%mm0

	movq 16(%rdi),%mm1
	pmaddwd 16(%rsi),%mm1
	paddd %mm1,%mm0

	movq 24(%rdi),%mm1
	add $32,%rdi
	pmaddwd 24(%rsi),%mm1
	add $32,%rsi
	paddd %mm1,%mm0

	jmp .Loop1
.Loop1Done:

	addl $4,%edx

# MMX dot product loop, not unrolled, crunching 4 terms per loop
# This could be redone as Duff's Device on the unrolled loop above
.Loop2:	subl $1,%edx
	jl   .Loop2Done

	movq (%rdi),%mm1
	add $8,%rdi
	pmaddwd (%rsi),%mm1
	add $8,%rsi
	paddd %mm1,%mm0
	jmp .Loop2
.Loop2Done:

	movd %mm0,%ebx		# right-hand word to ebx
	punpckhdq %mm0,%mm0	# left-hand word to right side of %mm0
	movd %mm0,%eax
	addl %ebx,%eax		# running sum now in %eax
	cdqe            #sign extend to 64 bits
	emms			# done with MMX

	pop %rbx
	retq

