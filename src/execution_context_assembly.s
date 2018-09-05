#ifndef IS_MAC_OS
#define SYM(x) x
#else
#define SYM(x) _ ## x
#endif
	.set FILE_HANDLE, 0x60
	.set EXECUTION_AREA, 0x68

	.text
	.globl	SYM(start_execution)
SYM(start_execution):
	pushq	%rbp
	movq	%rsp, %rbp
	pushq	%rbx
	pushq	%r12
	pushq	%r13	
	pushq	%r14	
	pushq	%r15	
	
	#for alignment to 16 bytes
	pushq	$0	

	subq	$0x100, %rsp
	movq	%rdi, -8(%rbp)
	movq	FILE_HANDLE(%rdi), %rdi
	callq	SYM(identify_reset_address_emulation)
	movq	%rax, %rsi
	movq	-8(%rbp), %rdi
	callq	SYM(get_execution_address)
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
	.globl	SYM(basic_block_end)
SYM(basic_block_end):
	#cmpq	$1, %rsi
	#je 1f	
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)
	movq	%rsi, -16(%rbp)
	call 	SYM(print_next_address)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	SYM(get_execution_address)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	jmpq	*%rax

1:
	int	$3

	.text
	.globl	SYM(read_non_ram_address)
SYM(read_non_ram_address):
	movq	%rcx, %rsi
	pushq	%rcx
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)	
	callq	SYM(read_non_ram_address_internal)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	popq	%rcx
	
	retq


	.text
	.globl	SYM(write_non_ram_address)
SYM(write_non_ram_address):
	movq	%rcx, %rsi
	pushq	%rcx
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)	
	movq	%rax, %rdx
	callq	SYM(write_non_ram_address_internal)
	movq	-8(%rbp), %rdi
	movq	(%rdi), %r10
	movq	0x8(%rdi), %r11
	popq	%rcx
	retq

	.text	
	.globl	SYM(ppu_event)
SYM(ppu_event):
	movq	%r10, (%rdi)
	movq	%r11, 0x8(%rdi)
	#movq	%r13, 0x18(%rdi)
	#movq	%r14, 0x20(%rdi)
	callq	SYM(ppu_event_internal)
	movq	-8(%rbp), %rdi
	#movq	0x20(%rdi), %r14
	#movq	0x18(%rdi), %r13
	movq	0x8(%rdi), %r11
	movq	(%rdi), %r10
	movq	%rax, %rsi
	jmp	SYM(basic_block_end)

	.text
	.globl	SYM(nmi_trigger)
SYM(nmi_trigger):
	movq	$0, 0x80(%rdi)
	movq	$1, 0x88(%rdi)
	subb	$3, %r13b
	movb	%r14b, 0x102(%rbx, %r13)
	movw	%si, 0x100(%rbx, %r13)
	movw	0xfffa(%rbx), %si
	subq	$7, 0x78(%rdi)
	jmp	SYM(basic_block_end)
	
	



	.data
string1:
	.asciz "Non ram read at address %x\n"
string2:
	.asciz "Non ram write at address %x\n"

