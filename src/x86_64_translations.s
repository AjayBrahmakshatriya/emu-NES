

	.set NEGATIVE,	0b10000000
	.set OVERFLOW,	0b01000000
	.set BIT5,	0b00100000	
	.set BIT4,	0b00010000	
	.set DECIMAL,	0b00001000	
	.set IRQD,	0b00000100	
	.set ZERO,	0b00000010	
	.set CARRY,	0b00000001	

	.set NOT_NEGATIVE,	0b01111111
	.set NOT_OVERFLOW,	0b10111111
	.set NOT_BIT5,		0b11011111	
	.set NOT_BIT4,		0b11101111	
	.set NOT_DECIMAL,	0b11110111	
	.set NOT_IRQD,		0b11111011	
	.set NOT_ZERO,		0b11111101	
	.set NOT_CARRY,		0b11111110	

	.macro	read_address
	testb	$1, 0x10000(%rbx, %rcx)
	jz 1f
	movzbq	(%rbx, %rcx), %rax
	jmp	2f
1:
	movq	0x10(%r15), %rax
	callq	*%rax
2:
	.endm

	.macro	write_address
	testb	$1, 0x10000(%rbx, %rcx)
	jz	1f
	movb	%al, (%rbx, %rcx)
	jmp	2f
1:
	movq	0x18(%r15), %r8
	callq	*%r8
2:

	.endm

	.macro	set_N
	pushf
	js	1f
	andb	$NOT_NEGATIVE, %r14b
	jmp	2f
1:
	orb	$NEGATIVE, %r14b
2:
	popf	
	.endm

	.macro	set_Z
	pushf
	jz	1f
	andb	$NOT_ZERO, %r14b
	jmp	2f
1:
	orb	$ZERO, %r14b
2:
	popf
	.endm

	.macro	set_C
	pushf
	jc	1f
	andb	$NOT_CARRY, %r14b
	jmp	2f
1:
	orb	$CARRY, %r14b
2:
	popf
	.endm

	.macro set_SC
	pushf
	jnc	1f
	andb	$NOT_CARRY, %r14b
	jmp	2f
1:
	orb	$CARRY, %r14b
2:
	popf
	.endm


	.macro	set_V
	pushf
	jo	1f
	andb	$OVERFLOW, %r14b
	jmp	2f
1:
	orb	$CARRY, %r14b
2:
	popf
	.endm

	
	.macro	read_indirect_X
	addb	%r11b, %cl
	movzbq	(%rbx, %rcx), %r9
	incb	%cl
	movzbq	(%rbx, %rcx), %rcx
	shlq	$8, %rcx
	orq	%r9, %rcx
	read_address
	.endm

	.macro	write_indirect_X
	addb	%r11b, %cl
	movzbq	(%rbx, %rcx), %r9
	incb	%cl
	movzbq	(%rbx, %rcx), %rcx
	shlq	$8, %rcx
	orq	%r9, %rcx
	write_address
	.endm

	.macro	read_indirect_Y
	movzbq	(%rbx, %rcx), %r8
	incb	%cl
	movzbq	(%rbx, %rcx), %r9
	shlq	$8, %r9
	orq	%r9, %r8
	leaq	(%r8, %r12), %rcx
	addb	%r12b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	read_address
	.endm

	.macro	write_indirect_Y
	movzbq	(%rbx, %rcx), %r8
	incb	%cl
	movzbq	(%rbx, %rcx), %r9
	shlq	$8, %r9
	orq	%r9, %r8
	leaq	(%r8, %r12), %rcx	
	addb	%r12b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	write_address
	.endm

	.macro	read_zpg_X
	leaq	(%r11, %rcx), %rax
	movzbq	%al, %rax
	movb	(%rbx, %rax), %al
	.endm

	.macro	write_zpg_X
	leaq	(%r11, %rcx), %r8
	movzbq	%r8b, %r8
	movb	%al, (%rbx, %r8)
	.endm

	.macro	write_zpg_Y
	leaq	(%r12, %rcx), %r8
	movzbq	%r8b, %r8
	movb	%al, (%rbx, %r8)
	.endm

	.macro	read_zpg_Y
	leaq	(%r12, %rcx), %rax
	movzbq	%al, %rax
	movb	(%rbx, %rax), %al
	.endm

	.macro	read_abs
	read_address
	.endm
	
	.macro	write_abs
	write_address
	.endm
	
	.macro	read_zpg
	movzbq	(%rbx, %rcx),  %rax
	.endm

	.macro	write_zpg
	movb	%al, (%rbx, %rcx)
	.endm


	.macro	read_abs_X
	movb	%cl, %r8b
	leaq	(%rcx, %r11), %rcx
	addb	%r11b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	read_address
	.endm

	.macro	write_abs_X
	movb	%cl, %r8b
	leaq	(%rcx, %r11), %rcx
	addb	%r11b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	write_address
	.endm


	.macro	read_abs_Y
	movb	%cl, %r8b
	leaq	(%rcx, %r12), %rcx
	addb	%r12b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	read_address
	.endm

	.macro	write_abs_Y
	movb	%cl, %r8b
	leaq	(%rcx, %r12), %rcx
	addb	%r12b, %r8b
	jnc	1f
	decq	0x78(%rdi)
1:
	write_address
	.endm


	.macro relative_branch	
	movq	%rdx, %rax
	movsx	%cl, %cx
	addw	%cx, %dx
	movq	%rdx, %rsi
	cmpb	%ah, %dh
	je	1f
	decq	0x78(%rdi)
1:
	decq	0x78(%rdi)
	movq	(%r15), %rax		
	jmpq	*%rax
	.endm
	

	.text
	

## Translations for 256 opcodes to x86_64

	.globl NES_INSTRUCTION_0x00
NES_INSTRUCTION_0x00:#	{"BRK", AM_impl, 7},
	movq	0x8(%r15), %rax
	jmpq	*%rax


	.globl NES_INSTRUCTION_0x01
NES_INSTRUCTION_0x01:#	{"ORA", AM_X_ind, 6},
	movq	$__arg_01_0, %rcx
	read_indirect_X
	orb	%al, %r10b
	set_N
	set_Z 

	.globl NES_INSTRUCTION_0x02
NES_INSTRUCTION_0x02:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x03
NES_INSTRUCTION_0x03:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x04
NES_INSTRUCTION_0x04:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x05
NES_INSTRUCTION_0x05:#	{"ORA", AM_zpg, 3},
	movq	$__arg_05_0, %rcx
	read_zpg
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x06
NES_INSTRUCTION_0x06:#	{"ASL", AM_zpg, 5},
	movq	$__arg_06_0, %rcx
	read_zpg
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_zpg


	.globl NES_INSTRUCTION_0x07
NES_INSTRUCTION_0x07:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x08
NES_INSTRUCTION_0x08:#	{"PHP", AM_impl, 3},
	decb	%r13b
	movb	%r14b, 0x100(%rbx, %r13)

	

	.globl NES_INSTRUCTION_0x09
NES_INSTRUCTION_0x09:#	{"ORA", AM_hash, 2},
	movq	$__arg_09_0, %rcx
	orb	%cl, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x0a
NES_INSTRUCTION_0x0a:#	{"ASL", AM_A, 2},
	shlb	$1, %r10b
	set_N
	set_Z
	set_C

	.globl NES_INSTRUCTION_0x0b
NES_INSTRUCTION_0x0b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x0c
NES_INSTRUCTION_0x0c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x0d
NES_INSTRUCTION_0x0d:#	{"ORA", AM_abs, 4},
	movq	$__arg_0d_0, %rcx
	read_abs
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x0e
NES_INSTRUCTION_0x0e:#	{"ASL", AM_abs, 6},
	movq	$__arg_0e_0, %rcx
	read_abs
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_abs
	
	.globl NES_INSTRUCTION_0x0f
NES_INSTRUCTION_0x0f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x10
NES_INSTRUCTION_0x10:#	{"BPL", AM_rel, 2},
	movq	$NEGATIVE, %rax
	andq	%r14, %rax
	jnz	inst_10_end 
	movq	$__arg_10_0, %rcx
	movq	$__arg_10_p, %rdx
	relative_branch
inst_10_end:
	
	.globl NES_INSTRUCTION_0x11
NES_INSTRUCTION_0x11:#	{"ORA", AM_ind_Y, 5},
	movq	$__arg_11_0, %rcx
	read_indirect_Y
	orb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x12
NES_INSTRUCTION_0x12:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x13
NES_INSTRUCTION_0x13:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x14
NES_INSTRUCTION_0x14:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x15
NES_INSTRUCTION_0x15:#	{"ORA", AM_zpg_X, 4},
	movq	$__arg_15_0, %rcx
	read_zpg_X
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x16
NES_INSTRUCTION_0x16:#	{"ASL", AM_zpg_X, 6},
	movq	$__arg_16_0, %rcx
	read_zpg_X
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_zpg_X

	.globl NES_INSTRUCTION_0x17
NES_INSTRUCTION_0x17:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x18
NES_INSTRUCTION_0x18:#	{"CLC", AM_impl, 2},
	andb	$NOT_CARRY, %r14b

	.globl NES_INSTRUCTION_0x19
NES_INSTRUCTION_0x19:#	{"ORA", AM_abs_Y, 4},
	movq	$__arg_19_0, %rcx
	read_abs_Y
	orb	%al, %r10b
	set_Z
	set_N

	.globl NES_INSTRUCTION_0x1a
NES_INSTRUCTION_0x1a:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x1b
NES_INSTRUCTION_0x1b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x1c
NES_INSTRUCTION_0x1c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x1d
NES_INSTRUCTION_0x1d:#	{"ORA", AM_abs_X, 4},
	movq	$__arg_1d_0, %rcx
	read_abs_X
	orb	%al, %r10b
	set_Z
	set_N

	.globl NES_INSTRUCTION_0x1e
NES_INSTRUCTION_0x1e:#	{"ASL", AM_abs_X, 7},
	movq	$__arg_1e_0, %rcx
	read_abs_X
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_address


	.globl NES_INSTRUCTION_0x1f
NES_INSTRUCTION_0x1f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x20
NES_INSTRUCTION_0x20:#	{"JSR", AM_abs, 6},
	subb	$2, %r13b
	movq	$__arg_20_p, %rax
	decw	%ax
	movw	%ax, 0x100(%rbx, %r13)
	movq	$__arg_20_0, %rsi
	movq	(%r15), %rax
	jmpq	*%rax
	
	 
	.globl NES_INSTRUCTION_0x21
NES_INSTRUCTION_0x21:#	{"AND", AM_X_ind, 6},
	movq	$__arg_21_0, %rcx
	read_indirect_X
	andb	%al, %r10b
	set_N
	set_Z
	

	.globl NES_INSTRUCTION_0x22
NES_INSTRUCTION_0x22:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x23
NES_INSTRUCTION_0x23:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x24
NES_INSTRUCTION_0x24:#	{"BIT", AM_zpg, 3},
	andb	$0b00111111, %r14b
	movq	$__arg_24_0, %rcx
	read_zpg
	movq	%rax, %r8
	andb	$0b11000000, %al
	orb	%al, %r14b
	andb	%r10b, %r8b
	set_Z

	
	.globl NES_INSTRUCTION_0x25
NES_INSTRUCTION_0x25:#	{"AND", AM_zpg, 3},
	movq	$__arg_25_0, %rcx
	read_zpg
	andb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x26
NES_INSTRUCTION_0x26:#	{"ROL", AM_zpg, 5},
	movq	$__arg_26_0, %rcx
	read_zpg
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rclb	$1, %al
	set_C
	set_Z
	set_N
	write_zpg	

	.globl NES_INSTRUCTION_0x27
NES_INSTRUCTION_0x27:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x28
NES_INSTRUCTION_0x28:#	{"PLP", AM_impl, 4},
	movb	0x100(%rbx, %r13), %r14b
	incb	%r13b

	.globl NES_INSTRUCTION_0x29
NES_INSTRUCTION_0x29:#	{"AND", AM_hash, 2},
	movq	$__arg_29_0, %rcx
	andb	%cl, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x2a
NES_INSTRUCTION_0x2a:#	{"ROL", AM_A, 2},
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rclb	$1, %r10b
	set_C
	set_Z
	set_N
	

	.globl NES_INSTRUCTION_0x2b
NES_INSTRUCTION_0x2b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x2c
NES_INSTRUCTION_0x2c:#	{"BIT", AM_abs, 4},
	andb	$0b00111111, %r14b
	movq	$__arg_2c_0, %rcx
	read_abs
	movq	%rax, %r8
	andb	$0b11000000, %al
	orb	%al, %r14b
	andb	%r10b, %r8b
	set_Z

	.globl NES_INSTRUCTION_0x2d
NES_INSTRUCTION_0x2d:#	{"AND", AM_abs, 4},
	movq	$__arg_2d_0, %rcx
	read_abs
	andb	%al,%r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x2e
NES_INSTRUCTION_0x2e:#	{"ROL", AM_abs, 6},
	movq	$__arg_2e_0, %rcx
	read_abs
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rclb	$1, %al
	set_C
	set_Z
	set_N
	write_abs	

	.globl NES_INSTRUCTION_0x2f
NES_INSTRUCTION_0x2f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x30
NES_INSTRUCTION_0x30:	#	{"BMI", AM_rel, 2},
	movq	$NEGATIVE, %rax
	andb	%r14b, %al
	jz	inst_30_end 
	movq	$__arg_30_0, %rcx
	movq	$__arg_30_p, %rdx
	relative_branch
inst_30_end:

	.globl NES_INSTRUCTION_0x31
NES_INSTRUCTION_0x31:#	{"AND", AM_ind_Y, 5},
	movq	$__arg_31_0, %rcx
	read_indirect_Y
	andb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x32
NES_INSTRUCTION_0x32:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x33
NES_INSTRUCTION_0x33:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x34
NES_INSTRUCTION_0x34:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x35
NES_INSTRUCTION_0x35:#	{"AND", AM_zpg_X, 4},
	movq	$__arg_35_0, %rcx
	read_zpg_X
	andb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x36
NES_INSTRUCTION_0x36:#	{"ROL", AM_zpg_X, 6},
	movq	$__arg_36_0, %rcx
	read_zpg_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rclb	$1, %al
	set_C
	set_Z
	set_N
	write_zpg_X
	

	.globl NES_INSTRUCTION_0x37
NES_INSTRUCTION_0x37:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x38
NES_INSTRUCTION_0x38:#	{"SEC", AM_impl, 2},
	orb	$CARRY, %r14b


	.globl NES_INSTRUCTION_0x39
NES_INSTRUCTION_0x39:#	{"AND", AM_abs_Y, 4},
	movq	$__arg_39_0, %rcx
	read_abs_Y
	andb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x3a
NES_INSTRUCTION_0x3a:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x3b
NES_INSTRUCTION_0x3b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x3c
NES_INSTRUCTION_0x3c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x3d
NES_INSTRUCTION_0x3d:#	{"AND", AM_abs_X, 4},
	movq	$__arg_3d_0, %rcx
	read_abs_X
	andb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x3e
NES_INSTRUCTION_0x3e:#	{"ROL", AM_abs_X, 7},
	movq	$__arg_3e_0, %rcx
	read_abs_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rclb	$1, %al
	set_C
	set_Z
	set_N
	write_address

	.globl NES_INSTRUCTION_0x3f
NES_INSTRUCTION_0x3f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x40
NES_INSTRUCTION_0x40:#	{"RTI", AM_impl, 6},
	movb	0x102(%rbx, %r13), %r14b
	movzwq	0x100(%rbx, %r13), %rsi
	addb	$3, %r13b
	movq	$0, 0x88(%rdi)
	movq	(%r15), %rax
	jmpq	*%rax

	.globl NES_INSTRUCTION_0x41
NES_INSTRUCTION_0x41:#	{"EOR", AM_X_ind, 6},
	movq	$__arg_41_0, %rcx
	read_indirect_X
	xorb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x42
NES_INSTRUCTION_0x42:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x43
NES_INSTRUCTION_0x43:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x44
NES_INSTRUCTION_0x44:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x45
NES_INSTRUCTION_0x45:#	{"EOR", AM_zpg, 3},
	movq	$__arg_45_0, %rcx
	read_zpg
	xorb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x46
NES_INSTRUCTION_0x46:#	{"LSR", AM_zpg, 5},
	movq	$__arg_46_0, %rcx
	read_zpg
	shrb	$1, %al
	set_Z
	set_C
	write_zpg
	

	.globl NES_INSTRUCTION_0x47
NES_INSTRUCTION_0x47:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x48
NES_INSTRUCTION_0x48:#	{"PHA", AM_impl, 3},
	decb	%r13b
	movb	%r10b, 0x100(%rbx, %r13)
	

	.globl NES_INSTRUCTION_0x49
NES_INSTRUCTION_0x49:#	{"EOR", AM_hash, 2},
	movq	$__arg_49_0, %rcx
	xorb	%cl, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x4a
NES_INSTRUCTION_0x4a:#	{"LSR", AM_A, 2},
	shrb	$1, %r10b
	set_Z
	set_C

	.globl NES_INSTRUCTION_0x4b
NES_INSTRUCTION_0x4b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x4c
NES_INSTRUCTION_0x4c:#	{"JMP", AM_abs, 3},
	movq	$__arg_4c_0, %rsi
	movq	(%r15), %rax
	jmpq	*%rax

	.globl NES_INSTRUCTION_0x4d
NES_INSTRUCTION_0x4d:#	{"EOR", AM_abs, 4},
	movq	$__arg_4d_0, %rcx
	read_abs
	xorb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x4e
NES_INSTRUCTION_0x4e:#	{"LSR", AM_abs, 6},
	movq	$__arg_4e_0, %rcx
	read_abs
	shrb	$1, %al
	set_Z
	set_C
	write_abs

	.globl NES_INSTRUCTION_0x4f
NES_INSTRUCTION_0x4f:#	{NULL , AM_impl, -1},
	nop		

	.globl NES_INSTRUCTION_0x50
NES_INSTRUCTION_0x50:#	{"BVC", AM_rel, 2},
	movq	$OVERFLOW, %rax
	andb	%r14b, %al
	jnz	inst_50_end 
	movq	$__arg_50_0, %rcx
	movq	$__arg_50_p, %rdx
	relative_branch
inst_50_end:

	.globl NES_INSTRUCTION_0x51
NES_INSTRUCTION_0x51:#	{"EOR", AM_ind_Y, 5},
	movq	$__arg_51_0, %rcx
	read_indirect_Y
	xorb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x52
NES_INSTRUCTION_0x52:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x53
NES_INSTRUCTION_0x53:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x54
NES_INSTRUCTION_0x54:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x55
NES_INSTRUCTION_0x55:#	{"EOR", AM_zpg_X, 4},
	movq	$__arg_55_0, %rcx
	read_zpg_X
	xorb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x56
NES_INSTRUCTION_0x56:#	{"LSR", AM_zpg_X, 6},
	movq	$__arg_56_0, %rcx
	read_zpg_X
	shrb	$1, %al
	set_Z
	set_C
	write_zpg_X

	.globl NES_INSTRUCTION_0x57
NES_INSTRUCTION_0x57:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x58
NES_INSTRUCTION_0x58:#	{"CLI", AM_impl, 2},
	and	$NOT_IRQD, %r14b

	.globl NES_INSTRUCTION_0x59
NES_INSTRUCTION_0x59:#	{"EOR", AM_abs_Y, 4},
	movq	$__arg_59_0, %rcx
	read_abs_Y
	xorb	%al, %r10b
	set_N
	set_Z
	

	.globl NES_INSTRUCTION_0x5a
NES_INSTRUCTION_0x5a:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x5b
NES_INSTRUCTION_0x5b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x5c
NES_INSTRUCTION_0x5c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x5d
NES_INSTRUCTION_0x5d:#	{"EOR", AM_abs_X, 4},
	movq	$__arg_5d_0, %rcx
	read_abs_X
	xorb	%al, %r10b
	set_N
	set_Z
	

	.globl NES_INSTRUCTION_0x5e
NES_INSTRUCTION_0x5e:#	{"LSR", AM_abs_X, 7},
	movq	$__arg_5e_0, %rcx
	read_abs_X
	shrb	$1, %al
	set_Z
	set_C
	write_address

	.globl NES_INSTRUCTION_0x5f
NES_INSTRUCTION_0x5f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x60
NES_INSTRUCTION_0x60:#	{"RTS", AM_impl, 6},
	movzwq	0x100(%rbx, %r13), %rsi
	addb	$2, %r13b
	incw	%si
	movq	(%r15), %rax
	jmpq	*%rax
	
	.globl NES_INSTRUCTION_0x61
NES_INSTRUCTION_0x61:#	{"ADC", AM_X_ind, 6},
	movq	$__arg_61_0, %rcx
	read_indirect_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V
	

	.globl NES_INSTRUCTION_0x62
NES_INSTRUCTION_0x62:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x63
NES_INSTRUCTION_0x63:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x64
NES_INSTRUCTION_0x64:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x65
NES_INSTRUCTION_0x65:#	{"ADC", AM_zpg, 3},
	movq	$__arg_65_0, %rcx
	read_zpg
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x66
NES_INSTRUCTION_0x66:#	{"ROR", AM_zpg, 5},
	movq	$__arg_66_0, %rcx
	read_zpg
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rcrb	$1, %al
	set_C
	set_Z
	set_N
	write_zpg	
	

	.globl NES_INSTRUCTION_0x67
NES_INSTRUCTION_0x67:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x68
NES_INSTRUCTION_0x68:#	{"PLA", AM_impl, 4},
	movb	0x100(%rbx, %r13), %r10b
	incb	%r13b
	

	.globl NES_INSTRUCTION_0x69
NES_INSTRUCTION_0x69:#	{"ADC", AM_hash, 2},
	movq	$__arg_69_0, %rcx
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%cl, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x6a
NES_INSTRUCTION_0x6a:#	{"ROR", AM_A, 2},
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rcrb	$1, %r10b
	set_C
	set_Z
	set_N

	.globl NES_INSTRUCTION_0x6b
NES_INSTRUCTION_0x6b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x6c
NES_INSTRUCTION_0x6c:#	{"JMP", AM_ind, 5},
	movq	$__arg_6c_0, %rcx
	movzwq	(%rbx, %rcx), %rsi
	movq	(%r15), %rax
	jmpq	*%rax

	.globl NES_INSTRUCTION_0x6d
NES_INSTRUCTION_0x6d:#	{"ADC", AM_abs, 4},
	movq	$__arg_6d_0, %rcx
	read_abs
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x6e
NES_INSTRUCTION_0x6e:#	{"ROR", AM_abs, 6},
	movq	$__arg_6e_0, %rcx
	read_abs
	testb	$CARRY, %r14b
	jz 1f
	mov	$1, %r8
	jmp 2f
1:
	mov	$0, %r8
2:
	rcrb	$1, %r8b
	rcrb	$1, %al
	set_C
	set_Z
	set_N
	write_abs


	.globl NES_INSTRUCTION_0x6f
NES_INSTRUCTION_0x6f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x70
NES_INSTRUCTION_0x70:#	{"BVS", AM_rel, 2},
	movq	$OVERFLOW, %rax
	andb	%r14b, %al
	jz	inst_70_end 
	movq	$__arg_70_0, %rcx
	movq	$__arg_70_p, %rdx
	relative_branch
inst_70_end:
	

	.globl NES_INSTRUCTION_0x71
NES_INSTRUCTION_0x71:#	{"ADC", AM_ind_Y, 5},
	movq	$__arg_71_0, %rcx
	read_indirect_Y
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x72
NES_INSTRUCTION_0x72:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x73
NES_INSTRUCTION_0x73:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x74
NES_INSTRUCTION_0x74:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x75
NES_INSTRUCTION_0x75:#	{"ADC", AM_zpg_X, 4},
	movq	$__arg_75_0, %rcx
	read_zpg_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x76
NES_INSTRUCTION_0x76:#	{"ROR", AM_zpg_X, 6},
	movq	$__arg_76_0, %rcx
	read_zpg_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rcrb	$1, %al
	set_C
	set_Z
	set_N
	write_zpg_X	

	.globl NES_INSTRUCTION_0x77
NES_INSTRUCTION_0x77:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x78
NES_INSTRUCTION_0x78:#	{"SEI", AM_impl, 2},
	orb	$IRQD, %r14b

	.globl NES_INSTRUCTION_0x79
NES_INSTRUCTION_0x79:#	{"ADC", AM_abs_Y, 4},
	movq	$__arg_79_0, %rcx
	read_abs_Y
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x7a
NES_INSTRUCTION_0x7a:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x7b
NES_INSTRUCTION_0x7b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x7c
NES_INSTRUCTION_0x7c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x7d
NES_INSTRUCTION_0x7d:#	{"ADC", AM_abs_X, 4},
	movq	$__arg_7d_0, %rcx
	read_abs_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	adcb	%al, %r10b
	set_N
	set_Z
	set_C
	set_V

	.globl NES_INSTRUCTION_0x7e
NES_INSTRUCTION_0x7e:#	{"ROR", AM_abs_X, 7},
	movq	$__arg_7e_0, %rcx
	read_abs_X
	testb	$CARRY, %r14b
	jz 1f
	movq	$1, %r8
	jmp 2f
1:
	movq	$0, %r8
2:
	rcrb	$1, %r8b
	rcrb	$1, %al
	set_C
	set_Z
	set_N
	write_address	

	.globl NES_INSTRUCTION_0x7f
NES_INSTRUCTION_0x7f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x80
NES_INSTRUCTION_0x80:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x81
NES_INSTRUCTION_0x81:#	{"STA", AM_X_ind, 6},
	movq	$__arg_81_0, %rcx
	movb	%r10b, %al
	write_indirect_X
	

	.globl NES_INSTRUCTION_0x82
NES_INSTRUCTION_0x82:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x83
NES_INSTRUCTION_0x83:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x84
NES_INSTRUCTION_0x84:#	{"STY", AM_zpg, 3},
	movq	$__arg_84_0, %rcx
	movb	%r12b, %al
	write_zpg

	.globl NES_INSTRUCTION_0x85
NES_INSTRUCTION_0x85:#	{"STA", AM_zpg, 3},
	movq	$__arg_85_0, %rcx
	movb	%r10b, %al
	write_zpg

	.globl NES_INSTRUCTION_0x86
NES_INSTRUCTION_0x86:#	{"STX", AM_zpg, 3},
	movq	$__arg_86_0, %rcx
	movb	%r11b, %al
	write_zpg

	.globl NES_INSTRUCTION_0x87
NES_INSTRUCTION_0x87:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x88
NES_INSTRUCTION_0x88:#	{"DEY", AM_impl, 2},
	decb	%r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x89
NES_INSTRUCTION_0x89:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x8a
NES_INSTRUCTION_0x8a:#	{"TXA", AM_impl, 2},
	movb	%r11b, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x8b
NES_INSTRUCTION_0x8b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x8c
NES_INSTRUCTION_0x8c:#	{"STY", AM_abs, 4},
	movq	$__arg_8c_0, %rcx
	movb	%r12b, %al
	write_abs

	.globl NES_INSTRUCTION_0x8d
NES_INSTRUCTION_0x8d:#	{"STA", AM_abs, 4},
	movq	$__arg_8d_0, %rcx
	movb	%r10b, %al
	write_abs

	.globl NES_INSTRUCTION_0x8e
NES_INSTRUCTION_0x8e:#	{"STX", AM_abs, 4},
	movq	$__arg_8e_0, %rcx
	movb	%r11b, %al
	write_abs

	.globl NES_INSTRUCTION_0x8f
NES_INSTRUCTION_0x8f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x90
NES_INSTRUCTION_0x90:#	{"BCC", AM_rel, 2},
	movq	$CARRY, %rax
	andb	%r14b, %al
	jnz	inst_90_end 
	movq	$__arg_90_0, %rcx
	movq	$__arg_90_p, %rdx
	relative_branch
inst_90_end:

	.globl NES_INSTRUCTION_0x91
NES_INSTRUCTION_0x91:#	{"STA", AM_ind_Y, 6},
	movq	$__arg_91_0, %rcx
	movb	%r10b, %al
	write_indirect_Y
	

	.globl NES_INSTRUCTION_0x92
NES_INSTRUCTION_0x92:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x93
NES_INSTRUCTION_0x93:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x94
NES_INSTRUCTION_0x94:#	{"STY", AM_zpg_X, 4},
	movq	$__arg_94_0, %rcx
	movb	%r12b, %al
	write_zpg_X

	.globl NES_INSTRUCTION_0x95
NES_INSTRUCTION_0x95:#	{"STA", AM_zpg_X, 4},
	movq	$__arg_95_0, %rcx
	movb	%r10b, %al
	write_zpg_X

	.globl NES_INSTRUCTION_0x96
NES_INSTRUCTION_0x96:#	{"STX", AM_zpg_Y, 4},
	movq	$__arg_96_0, %rcx
	movb	%r11b, %al
	write_zpg_Y

	.globl NES_INSTRUCTION_0x97
NES_INSTRUCTION_0x97:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x98
NES_INSTRUCTION_0x98:#	{"TYA", AM_impl, 2},
	movb	%r12b, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x99
NES_INSTRUCTION_0x99:#	{"STA", AM_abs_Y, 5},
	movq	$__arg_99_0, %rcx
	movb	%r10b, %al
	write_abs_Y

	.globl NES_INSTRUCTION_0x9a
NES_INSTRUCTION_0x9a:#	{"TXS", AM_impl, 2},
	movb	%r11b, %r13b
	testb	%r13b, %r13b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x9b
NES_INSTRUCTION_0x9b:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x9c
NES_INSTRUCTION_0x9c:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x9d
NES_INSTRUCTION_0x9d:#	{"STA", AM_abs_X, 5},
	movq	$__arg_9d_0, %rcx
	movb	%r10b, %al
	write_abs_X

	.globl NES_INSTRUCTION_0x9e
NES_INSTRUCTION_0x9e:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0x9f
NES_INSTRUCTION_0x9f:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xa0
NES_INSTRUCTION_0xa0:#	{"LDY", AM_hash, 2},
	movq	$__arg_a0_0, %rcx
	movb	%cl, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa1
NES_INSTRUCTION_0xa1:#	{"LDA", AM_X_ind, 6},
	movq	$__arg_a1_0, %rcx
	read_indirect_X
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa2
NES_INSTRUCTION_0xa2:#	{"LDX", AM_hash, 2},
	movq	$__arg_a2_0, %rcx
	movb	%cl, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa3
NES_INSTRUCTION_0xa3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xa4
NES_INSTRUCTION_0xa4:#	{"LDY", AM_zpg, 3},
	movq	$__arg_a4_0, %rcx
	read_zpg
	movb	%al, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa5
NES_INSTRUCTION_0xa5:#	{"LDA", AM_zpg, 3},
	movq	$__arg_a5_0, %rcx
	read_zpg
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa6
NES_INSTRUCTION_0xa6:#	{"LDX", AM_zpg, 3},
	movq	$__arg_a6_0, %rcx
	read_zpg
	movb	%al, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa7
NES_INSTRUCTION_0xa7:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xa8
NES_INSTRUCTION_0xa8:#	{"TAY", AM_impl, 2},
	movb	%r10b, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xa9
NES_INSTRUCTION_0xa9:#	{"LDA", AM_hash, 2},
	movq	$__arg_a9_0, %rcx
	movb	%cl, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xaa
NES_INSTRUCTION_0xaa:#	{"TAX", AM_impl, 2},
	movb	%r10b, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z	

	.globl NES_INSTRUCTION_0xab
NES_INSTRUCTION_0xab:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xac
NES_INSTRUCTION_0xac:#	{"LDY", AM_abs, 4},
	movq	$__arg_ac_0, %rcx
	read_abs
	movb	%al, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xad
NES_INSTRUCTION_0xad:#	{"LDA", AM_abs, 4},
	movq	$__arg_ad_0, %rcx
	read_abs
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xae
NES_INSTRUCTION_0xae:#	{"LDX", AM_abs, 4},
	movq	$__arg_ae_0, %rcx
	read_abs
	movb	%al, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xaf
NES_INSTRUCTION_0xaf:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xb0
NES_INSTRUCTION_0xb0:#	{"BCS", AM_rel, 2},
	movq	$CARRY, %rax
	andb	%r14b, %al
	jz	inst_b0_end 
	movq	$__arg_b0_0, %rcx
	movq	$__arg_b0_p, %rdx
	relative_branch
inst_b0_end:

	.globl NES_INSTRUCTION_0xb1
NES_INSTRUCTION_0xb1:#	{"LDA", AM_ind_Y, 5},
	movq	$__arg_b1_0, %rcx
	read_indirect_Y
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xb2
NES_INSTRUCTION_0xb2:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xb3
NES_INSTRUCTION_0xb3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xb4
NES_INSTRUCTION_0xb4:#	{"LDY", AM_zpg_X, 4},
	movq	$__arg_b4_0, %rcx
	read_zpg_X
	movb	%al, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xb5
NES_INSTRUCTION_0xb5:#	{"LDA", AM_zpg_X, 4},
	movq	$__arg_b5_0, %rcx
	read_zpg_X
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xb6
NES_INSTRUCTION_0xb6:#	{"LDX", AM_zpg_Y, 4},
	movq	$__arg_b6_0, %rcx
	read_zpg_Y
	movb	%al, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xb7
NES_INSTRUCTION_0xb7:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xb8
NES_INSTRUCTION_0xb8:#	{"CLV", AM_impl, 2},
	andb	$NOT_OVERFLOW, %r14b

	.globl NES_INSTRUCTION_0xb9
NES_INSTRUCTION_0xb9:#	{"LDA", AM_abs_Y, 4},
	movq	$__arg_b9_0, %rcx
	read_abs_Y
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xba
NES_INSTRUCTION_0xba:#	{"TSX", AM_impl, 2},
	movb	%r13b, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xbb
NES_INSTRUCTION_0xbb:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xbc
NES_INSTRUCTION_0xbc:#	{"LDY", AM_abs_X, 4},
	movq	$__arg_bc_0, %rcx
	read_abs_X
	movb	%al, %r12b
	testb	%r12b, %r12b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xbd
NES_INSTRUCTION_0xbd:#	{"LDA", AM_abs_X, 4},
	movq	$__arg_bd_0, %rcx
	read_abs_X
	movb	%al, %r10b
	testb	%r10b, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xbe
NES_INSTRUCTION_0xbe:#	{"LDX", AM_abs_Y, 4},
	movq	$__arg_be_0, %rcx
	read_abs_Y
	movb	%al, %r11b
	testb	%r11b, %r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xbf
NES_INSTRUCTION_0xbf:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xc0
NES_INSTRUCTION_0xc0:#	{"CPY", AM_hash, 2},
	movq	$__arg_c0_0, %rax
	cmpb	%al, %r12b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xc1
NES_INSTRUCTION_0xc1:#	{"CMP", AM_X_ind, 6},
	movq	$__arg_c1_0, %rcx
	read_indirect_X
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC


	.globl NES_INSTRUCTION_0xc2
NES_INSTRUCTION_0xc2:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xc3
NES_INSTRUCTION_0xc3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xc4
NES_INSTRUCTION_0xc4:#	{"CPY", AM_zpg, 3},
	movq	$__arg_c4_0, %rcx
	read_zpg
	cmpb	%al, %r12b
	set_N
	set_Z
	set_SC


	.globl NES_INSTRUCTION_0xc5
NES_INSTRUCTION_0xc5:#	{"CMP", AM_zpg, 3},
	movq	$__arg_c5_0, %rcx
	read_zpg
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC


	.globl NES_INSTRUCTION_0xc6
NES_INSTRUCTION_0xc6:#	{"DEC", AM_zpg, 5},
	movq	$__arg_c6_0, %rcx
	read_zpg
	decb	%al
	set_N
	set_Z
	write_zpg


	.globl NES_INSTRUCTION_0xc7
NES_INSTRUCTION_0xc7:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xc8
NES_INSTRUCTION_0xc8:#	{"INY", AM_impl, 2},
	#int	$3
	incb	%r12b
	set_N
	set_Z
	

	.globl NES_INSTRUCTION_0xc9
NES_INSTRUCTION_0xc9:#	{"CMP", AM_hash, 2},
	movq	$__arg_c9_0, %rax
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xca
NES_INSTRUCTION_0xca:#	{"DEX", AM_impl, 2},
	decb	%r11b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0xcb
NES_INSTRUCTION_0xcb:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xcc
NES_INSTRUCTION_0xcc:#	{"CPY", AM_abs, 4},
	movq	$__arg_cc_0, %rcx
	read_abs
	cmpb	%al, %r12b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xcd
NES_INSTRUCTION_0xcd:#	{"CMP", AM_abs, 4},
	movq	$__arg_cd_0, %rcx
	read_abs
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xce
NES_INSTRUCTION_0xce:#	{"DEC", AM_abs, 6},
	movq	$__arg_ce_0, %rcx
	read_abs
	decb	%al
	set_N
	set_Z
	write_abs

	.globl NES_INSTRUCTION_0xcf
NES_INSTRUCTION_0xcf:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xd0
NES_INSTRUCTION_0xd0:#	{"BNE", AM_rel, 2},
	movq	$ZERO, %rax
	andb	%r14b, %al
	jnz	inst_d0_end 
	movq	$__arg_d0_0, %rcx
	movq	$__arg_d0_p, %rdx
	relative_branch
inst_d0_end:
	

	.globl NES_INSTRUCTION_0xd1
NES_INSTRUCTION_0xd1:#	{"CMP", AM_ind_Y, 5},
	movq	$__arg_d1_0, %rcx
	read_indirect_Y
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xd2
NES_INSTRUCTION_0xd2:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xd3
NES_INSTRUCTION_0xd3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xd4
NES_INSTRUCTION_0xd4:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xd5
NES_INSTRUCTION_0xd5:#	{"CMP", AM_zpg_X, 4},
	movq	$__arg_d5_0, %rcx
	read_zpg_X
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xd6
NES_INSTRUCTION_0xd6:#	{"DEC", AM_zpg_X, 6},
	movq	$__arg_d6_0, %rcx
	read_zpg_X
	decb	%al
	set_N
	set_Z
	write_zpg_X

	.globl NES_INSTRUCTION_0xd7
NES_INSTRUCTION_0xd7:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xd8
NES_INSTRUCTION_0xd8:#	{"CLD", AM_impl, 2},
	andb	$NOT_DECIMAL, %r14b

	.globl NES_INSTRUCTION_0xd9
NES_INSTRUCTION_0xd9:#	{"CMP", AM_abs_Y, 4},
	movq	$__arg_d9_0, %rcx
	read_abs_Y
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xda
NES_INSTRUCTION_0xda:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xdb
NES_INSTRUCTION_0xdb:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xdc
NES_INSTRUCTION_0xdc:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xdd
NES_INSTRUCTION_0xdd:#	{"CMP", AM_abs_X, 4},
	movq	$__arg_dd_0, %rcx
	read_abs_X
	cmpb	%al, %r10b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xde
NES_INSTRUCTION_0xde:#	{"DEC", AM_abs_X, 7},
	movq	$__arg_de_0, %rcx
	read_abs_X
	decb	%al
	set_N
	set_Z
	write_address	

	.globl NES_INSTRUCTION_0xdf
NES_INSTRUCTION_0xdf:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xe0
NES_INSTRUCTION_0xe0:#	{"CPX", AM_hash, 2},
	movq	$__arg_e0_0, %rax
	cmpb	%al, %r11b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xe1
NES_INSTRUCTION_0xe1:#	{"SBC", AM_X_ind, 6},
	movq	$__arg_e1_0, %rcx
	read_indirect_X
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V
	

	.globl NES_INSTRUCTION_0xe2
NES_INSTRUCTION_0xe2:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xe3
NES_INSTRUCTION_0xe3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xe4
NES_INSTRUCTION_0xe4:#	{"CPX", AM_zpg, 3},
	movq	$__arg_e4_0, %rcx
	read_zpg
	cmpb	%al, %r11b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xe5
NES_INSTRUCTION_0xe5:#	{"SBC", AM_zpg, 3},
	movq	$__arg_e5_0, %rcx
	read_zpg
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V


	.globl NES_INSTRUCTION_0xe6
NES_INSTRUCTION_0xe6:#	{"INC", AM_zpg, 5},
	movq	$__arg_e6_0, %rcx
	read_zpg
	incb	%al
	set_N
	set_Z
	write_zpg

	.globl NES_INSTRUCTION_0xe7
NES_INSTRUCTION_0xe7:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xe8
NES_INSTRUCTION_0xe8:#	{"INX", AM_impl, 2},
	incb	%r11b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0xe9
NES_INSTRUCTION_0xe9:#	{"SBC", AM_hash, 2},
	movq	$__arg_e9_0, %rax
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V
	

	.globl NES_INSTRUCTION_0xea
NES_INSTRUCTION_0xea:#	{"NOP", AM_impl, 2},
	nop

	.globl NES_INSTRUCTION_0xeb
NES_INSTRUCTION_0xeb:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xec
NES_INSTRUCTION_0xec:#	{"CPX", AM_abs, 4},
	movq	$__arg_ec_0, %rcx
	read_abs
	cmpb	%al, %r11b
	set_N
	set_Z
	set_SC

	.globl NES_INSTRUCTION_0xed
NES_INSTRUCTION_0xed:#	{"SBC", AM_abs, 4},
	movq	$__arg_ed_0, %rcx
	read_abs
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V
	

	.globl NES_INSTRUCTION_0xee
NES_INSTRUCTION_0xee:#	{"INC", AM_abs, 6},
	movq	$__arg_ee_0, %rcx
	read_abs
	incb	%al
	set_N
	set_Z
	write_abs

	.globl NES_INSTRUCTION_0xef
NES_INSTRUCTION_0xef:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xf0
NES_INSTRUCTION_0xf0:#	{"BEQ", AM_rel, 2},
	movq	$ZERO, %rax
	andb	%r14b, %al
	jz	inst_f0_end 
	movq	$__arg_f0_0, %rcx
	movq	$__arg_f0_p, %rdx
	relative_branch
inst_f0_end:

	.globl NES_INSTRUCTION_0xf1
NES_INSTRUCTION_0xf1:#	{"SBC", AM_ind_Y, 5},
	movq	$__arg_f1_0, %rcx
	read_indirect_Y
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V
	

	.globl NES_INSTRUCTION_0xf2
NES_INSTRUCTION_0xf2:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xf3
NES_INSTRUCTION_0xf3:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xf4
NES_INSTRUCTION_0xf4:#	{NULL , AM_impl, -1},
	nop

	.globl NES_INSTRUCTION_0xf5
NES_INSTRUCTION_0xf5:#	{"SBC", AM_zpg_X, 4},
	movq	$__arg_f5_0, %rcx
	read_zpg_X
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V
	

	.globl NES_INSTRUCTION_0xf6
NES_INSTRUCTION_0xf6:#	{"INC", AM_zpg_X, 6}
	movq	$__arg_f6_0, %rcx
	read_zpg_X
	incb	%al
	set_N
	set_Z
	write_zpg_X


	.globl NES_INSTRUCTION_0xf7
NES_INSTRUCTION_0xf7:#	{NULL , AM_impl, -1}
	nop

	.globl NES_INSTRUCTION_0xf8
NES_INSTRUCTION_0xf8:#	{"SED", AM_impl, 2}
	orb	$DECIMAL, %r14b

	.globl NES_INSTRUCTION_0xf9
NES_INSTRUCTION_0xf9:#	{"SBC", AM_abs_Y, 4}
	movq	$__arg_f9_0, %rcx
	read_abs_Y
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V

	.globl NES_INSTRUCTION_0xfa
NES_INSTRUCTION_0xfa:#	{NULL , AM_impl, -1}
	nop

	.globl NES_INSTRUCTION_0xfb
NES_INSTRUCTION_0xfb:#	{NULL , AM_impl, -1}
	nop

	.globl NES_INSTRUCTION_0xfc
NES_INSTRUCTION_0xfc:#	{NULL , AM_impl, -1}
	nop

	.globl NES_INSTRUCTION_0xfd
NES_INSTRUCTION_0xfd:#	{"SBC", AM_abs_X, 4}
	movq	$__arg_fd_0, %rcx
	read_abs_X
	testb	$CARRY, %r14b
	jz 1f
	movb	$1, %r8b
	jmp 2f
1:
	movb	$0, %r8b
2:
	rcrb	$1, %r8b
	sbbb	%al, %r10b
	set_N
	set_Z
	set_SC
	set_V

	.globl NES_INSTRUCTION_0xfe
NES_INSTRUCTION_0xfe:#	{"INC", AM_abs_X, 7}
	movq	$__arg_fe_0, %rcx
	read_abs_X
	incb	%al
	set_N
	set_Z
	write_address

	.globl NES_INSTRUCTION_0xff
NES_INSTRUCTION_0xff:#	{NULL , AM_impl, -1}
	nop


	.globl NES_TEST_PPU_EVENT
NES_TEST_PPU_EVENT:
	movq	$__arg_pe_s, %rsi
	movq	0x78(%rdi), %rax
	testq	%rax, %rax
	jg	1f	
	movq	0x20(%r15), %rax
	jmpq	*%rax
1:
	movq	0x80(%rdi), %rax
	testq	%rax, %rax
	jz	2f
	movq	0x28(%r15), %rax
	jmpq	*%rax	
2:

	subq	$__arg_pe_0, 0x78(%rdi) 

	.globl NES_TEST_PPU_EVENT_END
NES_TEST_PPU_EVENT_END:
	nop
	

