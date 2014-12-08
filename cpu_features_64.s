.text
.global cpu_features_64
	.type cpu_features_64,@function
cpu_features_64:	
	pushq %rbx
	pushq %rcx
	pushq %rdx
	movl $1,%eax
	cpuid
	movq %rdx,(%rdi)
	movq %rcx,(%rsi)
	popq %rdx
	popq %rcx
	popq %rbx
	retq
