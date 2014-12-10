.text
.global cpu_features_64
	.type cpu_features_64,@function
.global cpu_features_extended_64
	.type cpu_features_extended_64,@function
cpu_features_64:
	mov    %rdx, %r9
	mov    %rcx, %r8
	push   %rbx
	mov    %edi, %eax
	cpuid
	mov    %ebx, (%rsi)
	mov    %ecx, (%r9)
	pop    %rbx
	mov    %edx, (%r8)
	retq

cpu_features_extended_64:
	mov    %rdx, %r9
	mov    %rcx, %r8
	push   %rbx
    xor    %ecx, %ecx
	mov    %edi, %eax
	cpuid
	mov    %ebx, (%rsi)
	mov    %ecx, (%r9)
	pop    %rbx
	mov    %edx, (%r8)
	retq

