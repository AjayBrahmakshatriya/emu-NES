	.set FILE_HANDLE, 0x30
	.set EXECUTION_AREA, 0x38

	.text
	.globl	start_execution
start_execution:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$0x100, %rsp 
	movq	%rdi, -8(%rbp)
	movq	FILE_HANDLE(%rdi), %rdi
	callq	identify_reset_address_emulation
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	callq	get_execution_address
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10	
	movq	0x8(%rdi), %r11	
	movq	0x10(%rdi), %r12
	movq	0x18(%rdi), %r13	
	movq	0x20(%rdi), %r14	
	movq	0x28(%rdi), %r15
	
	jmpq	*%rax


	.text
	.globl	basic_block_end
basic_block_end:
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)
	movq	%rsi, -16(%rbp)
	call print_next_address
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	
	callq	get_execution_address
	movq	-8(%rbp), %rdi
	movq	0x8(%rdi), %r11
	movq	0x10(%rdi), %r12
	
	jmpq	*%rax
