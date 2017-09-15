	.set FILE_HANDLE, 0x30
	.text
	.globl	start_execution
start_execution:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	0x100, %rsp 
	movq	%rdi, -8(%rbp)
	movq	FILE_HANDLE(%rdi), %rdi
	callq	identify_reset_address_emulation
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	call	get_execution_address
	movq	-8(%rbp), %rdi
	jmpq	*%rax

