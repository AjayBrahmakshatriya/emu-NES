	.set FILE_HANDLE, 0x60
	.set EXECUTION_AREA, 0x68

	.text
	.globl	start_execution
start_execution:
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13	
	pushq	%r14	
	pushq	%r15	
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
	movq	0x30(%rdi), %rbx
	jmpq	*%rax


	.text
	.globl	basic_block_end
basic_block_end:
	#cmpq	$1, %rsi
	#je 1f	
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)
	movq	%rsi, -16(%rbp)
	call 	print_next_address
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	get_execution_address
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	jmpq	*%rax

1:
	int	$3

	.text
	.globl	read_non_ram_address
read_non_ram_address:
	movq	%rcx, %rsi
	pushq	%rcx
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)	
	callq	read_non_ram_address_internal
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	popq	%rcx
	
	retq


	.text
	.globl	write_non_ram_address
write_non_ram_address:
	movq	%rcx, %rsi
	pushq	%rcx
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)	
	movq	%rax, %rdx
	callq	write_non_ram_address_internal
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	popq	%rcx
	retq

	.text	
	.globl	ppu_event
ppu_event:
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)
	#movq	%r13, 0x18(%rdi)
	callq	ppu_event_internal
	movq	-8(%rbp), %rdi
	#movq	0x18(%rdi), %r13
	movq	0x8(%rdi), %r11
	movq	(%rdi), %r10
	movq	%rax, %rsi
	jmp	basic_block_end	

	.data
string1:
	.asciz "Non ram read at address %x\n"
string2:
	.asciz "Non ram write at address %x\n"

