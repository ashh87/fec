.text
.global cpu_features_64_extended
	.type cpu_features_64_extended,@function
cpu_features_64_extended:	
	pushq %rbx
	pushq %rcx
	movl $7,%eax
	xor %rcx, %rcx
	cpuid
	movq %rbx,(%rdi)
	movq %rcx,(%rsi)
	popq %rcx
	popq %rbx
	retq
