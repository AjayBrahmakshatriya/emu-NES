

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
	nop
	.endm

	.macro	write_address
	nop
	.endm

	.macro	set_N
	nop
	.endm

	.macro	set_Z
	nop
	.endm

	.macro	set_C
	nop
	.endm

	
	.macro	read_indirect_X
	leaq	(%rcx, %r11), %r8
	movb 	%r8b, %r8b
	movb	(%rbx, %r8), %r9b
	incb	%r8b
	movb	(%rbx, %r8), %r8b
	shlq	$8, %r8
	orq	%r9, %r8
	movq	%r8, %rcx
	read_address
	.endm

	.macro	read_indirect_Y
	movb	(%rbx, %rcx), %r8b
	incb	%cl
	movb	(%rbx, %rcx), %r9b
	shlq	$8, %r9
	orq	%r9, %r8
	leaq	(%r8, %r12), %rcx	
	read_address
	.endm


	.macro	read_zpg_X
	leaq	(%r11, %rcx), %rax
	movb	%al, %al
	movb	(%rbx, %rax), %al
	.endm

	.macro	write_zpg_X
	leaq	(%r11, %rcx), %r8
	movb	%r8b, %r8b
	movb	%al, (%rbx, %r8)
	.endm

	.macro	write_zpg_Y
	leaq	(%r12, %rcx), %r8
	movb	%r8b, %r8b
	movb	%al, (%rbx, %r8)
	.endm

	.macro	read_zpg_Y
	leaq	(%r12, %rcx), %rax
	movb	%al, %al
	movb	(%rbx, %rax), %al
	.endm

	.macro	read_abs
	read_address
	.endm
	
	.macro	write_abs
	write_address
	.endm
	
	.macro	read_zpg
	movb	(%rbx, %rcx),  %al
	.endm

	.macro	write_zpg
	movb	%al, (%rbx, %rcx)
	.endm


	.macro	read_abs_X
	leaq	(%rcx, %r11), %rcx
	read_address
	.endm

	.macro	write_abs_X
	leaq	(%rcx, %r11), %rcx
	write_address
	.endm


	.macro	read_abs_Y
	leaq	(%rcx, %r12), %rcx
	read_address
	.endm

	.macro	write_abs_Y
	leaq	(%rcx, %r12), %rcx
	write_address
	.endm

	.text
	

## Translations for 256 opcodes to x86_64

	.globl NES_INSTRUCTION_0x00
NES_INSTRUCTION_0x00:
	movq	0x8(%r15), %rax
	jmpq	*%rax


	.globl NES_INSTRUCTION_0x01
NES_INSTRUCTION_0x01:
	movq	$__arg_01_0, %rcx
	read_indirect_X
	orb	%al, %r10b
	set_N
	set_Z 

	.globl NES_INSTRUCTION_0x02
NES_INSTRUCTION_0x02:
	nop

	.globl NES_INSTRUCTION_0x03
NES_INSTRUCTION_0x03:
	nop

	.globl NES_INSTRUCTION_0x04
NES_INSTRUCTION_0x04:
	nop

	.globl NES_INSTRUCTION_0x05
NES_INSTRUCTION_0x05:
	movq	$__arg_05_0, %rcx
	read_zpg
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x06
NES_INSTRUCTION_0x06:
	movq	$__arg_06_0, %rcx
	read_zpg
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_zpg


	.globl NES_INSTRUCTION_0x07
NES_INSTRUCTION_0x07:
	nop

	.globl NES_INSTRUCTION_0x08
NES_INSTRUCTION_0x08:
	decq	%r13
	movq	%r14, %rax
	movq	%r13, %rcx
	write_zpg
	

	.globl NES_INSTRUCTION_0x09
NES_INSTRUCTION_0x09:
	movq	$__arg_09_0, %rcx
	orb	%cl, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x0a
NES_INSTRUCTION_0x0a:
	shlb	$1, %r10b
	set_N
	set_Z
	set_C

	.globl NES_INSTRUCTION_0x0b
NES_INSTRUCTION_0x0b:
	nop

	.globl NES_INSTRUCTION_0x0c
NES_INSTRUCTION_0x0c:
	nop

	.globl NES_INSTRUCTION_0x0d
NES_INSTRUCTION_0x0d:
	movq	$__arg_0d_0, %rcx
	read_abs
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x0e
NES_INSTRUCTION_0x0e:
	movq	$__arg_0e_0, %rcx
	read_abs
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_abs
	
	.globl NES_INSTRUCTION_0x0f
NES_INSTRUCTION_0x0f:
	nop

	.globl NES_INSTRUCTION_0x10
NES_INSTRUCTION_0x10:
	movq	$NEGATIVE, %rax
	andq	%r14, %rax
	jnz	inst_10_end 
	movq	$__arg_10_0, %rcx
	movq	$__arg_10_1, %rdx
	movsx	%cl, %cx
	addw	%cx, %dx
	leaq	(%rbx, %rdx), %rsi
	movq	(%r15), %rax	
	jmpq	*%rax
inst_10_end:
	
	.globl NES_INSTRUCTION_0x11
NES_INSTRUCTION_0x11:
	movq	$__arg_11_0, %rcx
	read_indirect_Y
	orb	%al, %r10b
	set_N
	set_Z

	.globl NES_INSTRUCTION_0x12
NES_INSTRUCTION_0x12:
	nop

	.globl NES_INSTRUCTION_0x13
NES_INSTRUCTION_0x13:
	nop

	.globl NES_INSTRUCTION_0x14
NES_INSTRUCTION_0x14:
	nop

	.globl NES_INSTRUCTION_0x15
NES_INSTRUCTION_0x15:
	movq	$__arg_15_0, %rcx
	read_zpg_X
	orb	%al, %r10b
	set_N
	set_Z


	.globl NES_INSTRUCTION_0x16
NES_INSTRUCTION_0x16:
	movq	$__arg_16_0, %rcx
	read_zpg_X
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_zpg_X

	.globl NES_INSTRUCTION_0x17
NES_INSTRUCTION_0x17:
	nop

	.globl NES_INSTRUCTION_0x18
NES_INSTRUCTION_0x18:
	andb	$NOT_CARRY, %r14b

	.globl NES_INSTRUCTION_0x19
NES_INSTRUCTION_0x19:
	movq	$__arg_19_0, %rcx
	read_abs_Y
	orb	%al, %r10b
	set_Z
	set_N

	.globl NES_INSTRUCTION_0x1a
NES_INSTRUCTION_0x1a:
	nop

	.globl NES_INSTRUCTION_0x1b
NES_INSTRUCTION_0x1b:
	nop

	.globl NES_INSTRUCTION_0x1c
NES_INSTRUCTION_0x1c:
	nop

	.globl NES_INSTRUCTION_0x1d
NES_INSTRUCTION_0x1d:
	movq	$__arg_1d_0, %rcx
	read_abs_X
	orb	%al, %r10b
	set_Z
	set_N

	.globl NES_INSTRUCTION_0x1e
NES_INSTRUCTION_0x1e:
	movq	$__arg_1e_0, %rcx
	read_abs_X
	shlb	$1, %al
	set_N
	set_Z
	set_C
	write_address


	.globl NES_INSTRUCTION_0x1f
NES_INSTRUCTION_0x1f:
	nop

	.globl NES_INSTRUCTION_0x20
NES_INSTRUCTION_0x20:
	nop

	.globl NES_INSTRUCTION_0x21
NES_INSTRUCTION_0x21:
	nop

	.globl NES_INSTRUCTION_0x22
NES_INSTRUCTION_0x22:
	nop

	.globl NES_INSTRUCTION_0x23
NES_INSTRUCTION_0x23:
	nop

	.globl NES_INSTRUCTION_0x24
NES_INSTRUCTION_0x24:
	nop

	.globl NES_INSTRUCTION_0x25
NES_INSTRUCTION_0x25:
	nop

	.globl NES_INSTRUCTION_0x26
NES_INSTRUCTION_0x26:
	nop

	.globl NES_INSTRUCTION_0x27
NES_INSTRUCTION_0x27:
	nop

	.globl NES_INSTRUCTION_0x28
NES_INSTRUCTION_0x28:
	nop

	.globl NES_INSTRUCTION_0x29
NES_INSTRUCTION_0x29:
	nop

	.globl NES_INSTRUCTION_0x2a
NES_INSTRUCTION_0x2a:
	nop

	.globl NES_INSTRUCTION_0x2b
NES_INSTRUCTION_0x2b:
	nop

	.globl NES_INSTRUCTION_0x2c
NES_INSTRUCTION_0x2c:
	nop

	.globl NES_INSTRUCTION_0x2d
NES_INSTRUCTION_0x2d:
	nop

	.globl NES_INSTRUCTION_0x2e
NES_INSTRUCTION_0x2e:
	nop

	.globl NES_INSTRUCTION_0x2f
NES_INSTRUCTION_0x2f:
	nop

	.globl NES_INSTRUCTION_0x30
NES_INSTRUCTION_0x30:
	nop

	.globl NES_INSTRUCTION_0x31
NES_INSTRUCTION_0x31:
	nop

	.globl NES_INSTRUCTION_0x32
NES_INSTRUCTION_0x32:
	nop

	.globl NES_INSTRUCTION_0x33
NES_INSTRUCTION_0x33:
	nop

	.globl NES_INSTRUCTION_0x34
NES_INSTRUCTION_0x34:
	nop

	.globl NES_INSTRUCTION_0x35
NES_INSTRUCTION_0x35:
	nop

	.globl NES_INSTRUCTION_0x36
NES_INSTRUCTION_0x36:
	nop

	.globl NES_INSTRUCTION_0x37
NES_INSTRUCTION_0x37:
	nop

	.globl NES_INSTRUCTION_0x38
NES_INSTRUCTION_0x38:
	nop

	.globl NES_INSTRUCTION_0x39
NES_INSTRUCTION_0x39:
	nop

	.globl NES_INSTRUCTION_0x3a
NES_INSTRUCTION_0x3a:
	nop

	.globl NES_INSTRUCTION_0x3b
NES_INSTRUCTION_0x3b:
	nop

	.globl NES_INSTRUCTION_0x3c
NES_INSTRUCTION_0x3c:
	nop

	.globl NES_INSTRUCTION_0x3d
NES_INSTRUCTION_0x3d:
	nop

	.globl NES_INSTRUCTION_0x3e
NES_INSTRUCTION_0x3e:
	nop

	.globl NES_INSTRUCTION_0x3f
NES_INSTRUCTION_0x3f:
	nop

	.globl NES_INSTRUCTION_0x40
NES_INSTRUCTION_0x40:
	nop

	.globl NES_INSTRUCTION_0x41
NES_INSTRUCTION_0x41:
	nop

	.globl NES_INSTRUCTION_0x42
NES_INSTRUCTION_0x42:
	nop

	.globl NES_INSTRUCTION_0x43
NES_INSTRUCTION_0x43:
	nop

	.globl NES_INSTRUCTION_0x44
NES_INSTRUCTION_0x44:
	nop

	.globl NES_INSTRUCTION_0x45
NES_INSTRUCTION_0x45:
	nop

	.globl NES_INSTRUCTION_0x46
NES_INSTRUCTION_0x46:
	nop

	.globl NES_INSTRUCTION_0x47
NES_INSTRUCTION_0x47:
	nop

	.globl NES_INSTRUCTION_0x48
NES_INSTRUCTION_0x48:
	nop

	.globl NES_INSTRUCTION_0x49
NES_INSTRUCTION_0x49:
	nop

	.globl NES_INSTRUCTION_0x4a
NES_INSTRUCTION_0x4a:
	nop

	.globl NES_INSTRUCTION_0x4b
NES_INSTRUCTION_0x4b:
	nop

	.globl NES_INSTRUCTION_0x4c
NES_INSTRUCTION_0x4c:
	movq	(%r15), %rax
	movq	$__arg_4c_0, %rsi
	jmpq	*%rax

	.globl NES_INSTRUCTION_0x4d
NES_INSTRUCTION_0x4d:
	nop

	.globl NES_INSTRUCTION_0x4e
NES_INSTRUCTION_0x4e:
	nop

	.globl NES_INSTRUCTION_0x4f
NES_INSTRUCTION_0x4f:
	nop

	.globl NES_INSTRUCTION_0x50
NES_INSTRUCTION_0x50:
	nop

	.globl NES_INSTRUCTION_0x51
NES_INSTRUCTION_0x51:
	nop

	.globl NES_INSTRUCTION_0x52
NES_INSTRUCTION_0x52:
	nop

	.globl NES_INSTRUCTION_0x53
NES_INSTRUCTION_0x53:
	nop

	.globl NES_INSTRUCTION_0x54
NES_INSTRUCTION_0x54:
	nop

	.globl NES_INSTRUCTION_0x55
NES_INSTRUCTION_0x55:
	nop

	.globl NES_INSTRUCTION_0x56
NES_INSTRUCTION_0x56:
	nop

	.globl NES_INSTRUCTION_0x57
NES_INSTRUCTION_0x57:
	nop

	.globl NES_INSTRUCTION_0x58
NES_INSTRUCTION_0x58:
	nop

	.globl NES_INSTRUCTION_0x59
NES_INSTRUCTION_0x59:
	nop

	.globl NES_INSTRUCTION_0x5a
NES_INSTRUCTION_0x5a:
	nop

	.globl NES_INSTRUCTION_0x5b
NES_INSTRUCTION_0x5b:
	nop

	.globl NES_INSTRUCTION_0x5c
NES_INSTRUCTION_0x5c:
	nop

	.globl NES_INSTRUCTION_0x5d
NES_INSTRUCTION_0x5d:
	nop

	.globl NES_INSTRUCTION_0x5e
NES_INSTRUCTION_0x5e:
	nop

	.globl NES_INSTRUCTION_0x5f
NES_INSTRUCTION_0x5f:
	nop

	.globl NES_INSTRUCTION_0x60
NES_INSTRUCTION_0x60:
	nop

	.globl NES_INSTRUCTION_0x61
NES_INSTRUCTION_0x61:
	nop

	.globl NES_INSTRUCTION_0x62
NES_INSTRUCTION_0x62:
	nop

	.globl NES_INSTRUCTION_0x63
NES_INSTRUCTION_0x63:
	nop

	.globl NES_INSTRUCTION_0x64
NES_INSTRUCTION_0x64:
	nop

	.globl NES_INSTRUCTION_0x65
NES_INSTRUCTION_0x65:
	nop

	.globl NES_INSTRUCTION_0x66
NES_INSTRUCTION_0x66:
	nop

	.globl NES_INSTRUCTION_0x67
NES_INSTRUCTION_0x67:
	nop

	.globl NES_INSTRUCTION_0x68
NES_INSTRUCTION_0x68:
	nop

	.globl NES_INSTRUCTION_0x69
NES_INSTRUCTION_0x69:
	nop

	.globl NES_INSTRUCTION_0x6a
NES_INSTRUCTION_0x6a:
	nop

	.globl NES_INSTRUCTION_0x6b
NES_INSTRUCTION_0x6b:
	nop

	.globl NES_INSTRUCTION_0x6c
NES_INSTRUCTION_0x6c:
	movq	(%r15), %rax
	jmpq	*%rax

	.globl NES_INSTRUCTION_0x6d
NES_INSTRUCTION_0x6d:
	nop

	.globl NES_INSTRUCTION_0x6e
NES_INSTRUCTION_0x6e:
	nop

	.globl NES_INSTRUCTION_0x6f
NES_INSTRUCTION_0x6f:
	nop

	.globl NES_INSTRUCTION_0x70
NES_INSTRUCTION_0x70:
	nop

	.globl NES_INSTRUCTION_0x71
NES_INSTRUCTION_0x71:
	nop

	.globl NES_INSTRUCTION_0x72
NES_INSTRUCTION_0x72:
	nop

	.globl NES_INSTRUCTION_0x73
NES_INSTRUCTION_0x73:
	nop

	.globl NES_INSTRUCTION_0x74
NES_INSTRUCTION_0x74:
	nop

	.globl NES_INSTRUCTION_0x75
NES_INSTRUCTION_0x75:
	nop

	.globl NES_INSTRUCTION_0x76
NES_INSTRUCTION_0x76:
	nop

	.globl NES_INSTRUCTION_0x77
NES_INSTRUCTION_0x77:
	nop

	.globl NES_INSTRUCTION_0x78
NES_INSTRUCTION_0x78:
	nop

	.globl NES_INSTRUCTION_0x79
NES_INSTRUCTION_0x79:
	nop

	.globl NES_INSTRUCTION_0x7a
NES_INSTRUCTION_0x7a:
	nop

	.globl NES_INSTRUCTION_0x7b
NES_INSTRUCTION_0x7b:
	nop

	.globl NES_INSTRUCTION_0x7c
NES_INSTRUCTION_0x7c:
	nop

	.globl NES_INSTRUCTION_0x7d
NES_INSTRUCTION_0x7d:
	nop

	.globl NES_INSTRUCTION_0x7e
NES_INSTRUCTION_0x7e:
	nop

	.globl NES_INSTRUCTION_0x7f
NES_INSTRUCTION_0x7f:
	nop

	.globl NES_INSTRUCTION_0x80
NES_INSTRUCTION_0x80:
	nop

	.globl NES_INSTRUCTION_0x81
NES_INSTRUCTION_0x81:
	nop

	.globl NES_INSTRUCTION_0x82
NES_INSTRUCTION_0x82:
	nop

	.globl NES_INSTRUCTION_0x83
NES_INSTRUCTION_0x83:
	nop

	.globl NES_INSTRUCTION_0x84
NES_INSTRUCTION_0x84:
	nop

	.globl NES_INSTRUCTION_0x85
NES_INSTRUCTION_0x85:
	nop

	.globl NES_INSTRUCTION_0x86
NES_INSTRUCTION_0x86:
	nop

	.globl NES_INSTRUCTION_0x87
NES_INSTRUCTION_0x87:
	nop

	.globl NES_INSTRUCTION_0x88
NES_INSTRUCTION_0x88:
	nop

	.globl NES_INSTRUCTION_0x89
NES_INSTRUCTION_0x89:
	nop

	.globl NES_INSTRUCTION_0x8a
NES_INSTRUCTION_0x8a:
	nop

	.globl NES_INSTRUCTION_0x8b
NES_INSTRUCTION_0x8b:
	nop

	.globl NES_INSTRUCTION_0x8c
NES_INSTRUCTION_0x8c:
	nop

	.globl NES_INSTRUCTION_0x8d
NES_INSTRUCTION_0x8d:
	nop

	.globl NES_INSTRUCTION_0x8e
NES_INSTRUCTION_0x8e:
	nop

	.globl NES_INSTRUCTION_0x8f
NES_INSTRUCTION_0x8f:
	nop

	.globl NES_INSTRUCTION_0x90
NES_INSTRUCTION_0x90:
	nop

	.globl NES_INSTRUCTION_0x91
NES_INSTRUCTION_0x91:
	nop

	.globl NES_INSTRUCTION_0x92
NES_INSTRUCTION_0x92:
	nop

	.globl NES_INSTRUCTION_0x93
NES_INSTRUCTION_0x93:
	nop

	.globl NES_INSTRUCTION_0x94
NES_INSTRUCTION_0x94:
	nop

	.globl NES_INSTRUCTION_0x95
NES_INSTRUCTION_0x95:
	nop

	.globl NES_INSTRUCTION_0x96
NES_INSTRUCTION_0x96:
	nop

	.globl NES_INSTRUCTION_0x97
NES_INSTRUCTION_0x97:
	nop

	.globl NES_INSTRUCTION_0x98
NES_INSTRUCTION_0x98:
	nop

	.globl NES_INSTRUCTION_0x99
NES_INSTRUCTION_0x99:
	nop

	.globl NES_INSTRUCTION_0x9a
NES_INSTRUCTION_0x9a:
	nop

	.globl NES_INSTRUCTION_0x9b
NES_INSTRUCTION_0x9b:
	nop

	.globl NES_INSTRUCTION_0x9c
NES_INSTRUCTION_0x9c:
	nop

	.globl NES_INSTRUCTION_0x9d
NES_INSTRUCTION_0x9d:
	nop

	.globl NES_INSTRUCTION_0x9e
NES_INSTRUCTION_0x9e:
	nop

	.globl NES_INSTRUCTION_0x9f
NES_INSTRUCTION_0x9f:
	nop

	.globl NES_INSTRUCTION_0xa0
NES_INSTRUCTION_0xa0:
	nop

	.globl NES_INSTRUCTION_0xa1
NES_INSTRUCTION_0xa1:
	nop

	.globl NES_INSTRUCTION_0xa2
NES_INSTRUCTION_0xa2:
	nop

	.globl NES_INSTRUCTION_0xa3
NES_INSTRUCTION_0xa3:
	nop

	.globl NES_INSTRUCTION_0xa4
NES_INSTRUCTION_0xa4:
	nop

	.globl NES_INSTRUCTION_0xa5
NES_INSTRUCTION_0xa5:
	nop

	.globl NES_INSTRUCTION_0xa6
NES_INSTRUCTION_0xa6:
	nop

	.globl NES_INSTRUCTION_0xa7
NES_INSTRUCTION_0xa7:
	nop

	.globl NES_INSTRUCTION_0xa8
NES_INSTRUCTION_0xa8:
	nop

	.globl NES_INSTRUCTION_0xa9
NES_INSTRUCTION_0xa9:
	nop

	.globl NES_INSTRUCTION_0xaa
NES_INSTRUCTION_0xaa:
	nop

	.globl NES_INSTRUCTION_0xab
NES_INSTRUCTION_0xab:
	nop

	.globl NES_INSTRUCTION_0xac
NES_INSTRUCTION_0xac:
	nop

	.globl NES_INSTRUCTION_0xad
NES_INSTRUCTION_0xad:
	nop

	.globl NES_INSTRUCTION_0xae
NES_INSTRUCTION_0xae:
	nop

	.globl NES_INSTRUCTION_0xaf
NES_INSTRUCTION_0xaf:
	nop

	.globl NES_INSTRUCTION_0xb0
NES_INSTRUCTION_0xb0:
	nop

	.globl NES_INSTRUCTION_0xb1
NES_INSTRUCTION_0xb1:
	nop

	.globl NES_INSTRUCTION_0xb2
NES_INSTRUCTION_0xb2:
	nop

	.globl NES_INSTRUCTION_0xb3
NES_INSTRUCTION_0xb3:
	nop

	.globl NES_INSTRUCTION_0xb4
NES_INSTRUCTION_0xb4:
	nop

	.globl NES_INSTRUCTION_0xb5
NES_INSTRUCTION_0xb5:
	nop

	.globl NES_INSTRUCTION_0xb6
NES_INSTRUCTION_0xb6:
	nop

	.globl NES_INSTRUCTION_0xb7
NES_INSTRUCTION_0xb7:
	nop

	.globl NES_INSTRUCTION_0xb8
NES_INSTRUCTION_0xb8:
	nop

	.globl NES_INSTRUCTION_0xb9
NES_INSTRUCTION_0xb9:
	nop

	.globl NES_INSTRUCTION_0xba
NES_INSTRUCTION_0xba:
	nop

	.globl NES_INSTRUCTION_0xbb
NES_INSTRUCTION_0xbb:
	nop

	.globl NES_INSTRUCTION_0xbc
NES_INSTRUCTION_0xbc:
	nop

	.globl NES_INSTRUCTION_0xbd
NES_INSTRUCTION_0xbd:
	nop

	.globl NES_INSTRUCTION_0xbe
NES_INSTRUCTION_0xbe:
	nop

	.globl NES_INSTRUCTION_0xbf
NES_INSTRUCTION_0xbf:
	nop

	.globl NES_INSTRUCTION_0xc0
NES_INSTRUCTION_0xc0:
	nop

	.globl NES_INSTRUCTION_0xc1
NES_INSTRUCTION_0xc1:
	nop

	.globl NES_INSTRUCTION_0xc2
NES_INSTRUCTION_0xc2:
	nop

	.globl NES_INSTRUCTION_0xc3
NES_INSTRUCTION_0xc3:
	nop

	.globl NES_INSTRUCTION_0xc4
NES_INSTRUCTION_0xc4:
	nop

	.globl NES_INSTRUCTION_0xc5
NES_INSTRUCTION_0xc5:
	nop

	.globl NES_INSTRUCTION_0xc6
NES_INSTRUCTION_0xc6:
	nop

	.globl NES_INSTRUCTION_0xc7
NES_INSTRUCTION_0xc7:
	nop

	.globl NES_INSTRUCTION_0xc8
NES_INSTRUCTION_0xc8:
	nop

	.globl NES_INSTRUCTION_0xc9
NES_INSTRUCTION_0xc9:
	nop

	.globl NES_INSTRUCTION_0xca
NES_INSTRUCTION_0xca:
	nop

	.globl NES_INSTRUCTION_0xcb
NES_INSTRUCTION_0xcb:
	nop

	.globl NES_INSTRUCTION_0xcc
NES_INSTRUCTION_0xcc:
	nop

	.globl NES_INSTRUCTION_0xcd
NES_INSTRUCTION_0xcd:
	nop

	.globl NES_INSTRUCTION_0xce
NES_INSTRUCTION_0xce:
	nop

	.globl NES_INSTRUCTION_0xcf
NES_INSTRUCTION_0xcf:
	nop

	.globl NES_INSTRUCTION_0xd0
NES_INSTRUCTION_0xd0:
	nop

	.globl NES_INSTRUCTION_0xd1
NES_INSTRUCTION_0xd1:
	nop

	.globl NES_INSTRUCTION_0xd2
NES_INSTRUCTION_0xd2:
	nop

	.globl NES_INSTRUCTION_0xd3
NES_INSTRUCTION_0xd3:
	nop

	.globl NES_INSTRUCTION_0xd4
NES_INSTRUCTION_0xd4:
	nop

	.globl NES_INSTRUCTION_0xd5
NES_INSTRUCTION_0xd5:
	nop

	.globl NES_INSTRUCTION_0xd6
NES_INSTRUCTION_0xd6:
	nop

	.globl NES_INSTRUCTION_0xd7
NES_INSTRUCTION_0xd7:
	nop

	.globl NES_INSTRUCTION_0xd8
NES_INSTRUCTION_0xd8:
	nop

	.globl NES_INSTRUCTION_0xd9
NES_INSTRUCTION_0xd9:
	nop

	.globl NES_INSTRUCTION_0xda
NES_INSTRUCTION_0xda:
	nop

	.globl NES_INSTRUCTION_0xdb
NES_INSTRUCTION_0xdb:
	nop

	.globl NES_INSTRUCTION_0xdc
NES_INSTRUCTION_0xdc:
	nop

	.globl NES_INSTRUCTION_0xdd
NES_INSTRUCTION_0xdd:
	nop

	.globl NES_INSTRUCTION_0xde
NES_INSTRUCTION_0xde:
	nop

	.globl NES_INSTRUCTION_0xdf
NES_INSTRUCTION_0xdf:
	nop

	.globl NES_INSTRUCTION_0xe0
NES_INSTRUCTION_0xe0:
	nop

	.globl NES_INSTRUCTION_0xe1
NES_INSTRUCTION_0xe1:
	nop

	.globl NES_INSTRUCTION_0xe2
NES_INSTRUCTION_0xe2:
	nop

	.globl NES_INSTRUCTION_0xe3
NES_INSTRUCTION_0xe3:
	nop

	.globl NES_INSTRUCTION_0xe4
NES_INSTRUCTION_0xe4:
	nop

	.globl NES_INSTRUCTION_0xe5
NES_INSTRUCTION_0xe5:
	nop

	.globl NES_INSTRUCTION_0xe6
NES_INSTRUCTION_0xe6:
	nop

	.globl NES_INSTRUCTION_0xe7
NES_INSTRUCTION_0xe7:
	nop

	.globl NES_INSTRUCTION_0xe8
NES_INSTRUCTION_0xe8:
	nop

	.globl NES_INSTRUCTION_0xe9
NES_INSTRUCTION_0xe9:
	nop

	.globl NES_INSTRUCTION_0xea
NES_INSTRUCTION_0xea:
	nop

	.globl NES_INSTRUCTION_0xeb
NES_INSTRUCTION_0xeb:
	nop

	.globl NES_INSTRUCTION_0xec
NES_INSTRUCTION_0xec:
	nop

	.globl NES_INSTRUCTION_0xed
NES_INSTRUCTION_0xed:
	nop

	.globl NES_INSTRUCTION_0xee
NES_INSTRUCTION_0xee:
	nop

	.globl NES_INSTRUCTION_0xef
NES_INSTRUCTION_0xef:
	nop

	.globl NES_INSTRUCTION_0xf0
NES_INSTRUCTION_0xf0:
	nop

	.globl NES_INSTRUCTION_0xf1
NES_INSTRUCTION_0xf1:
	nop

	.globl NES_INSTRUCTION_0xf2
NES_INSTRUCTION_0xf2:
	nop

	.globl NES_INSTRUCTION_0xf3
NES_INSTRUCTION_0xf3:
	nop

	.globl NES_INSTRUCTION_0xf4
NES_INSTRUCTION_0xf4:
	nop

	.globl NES_INSTRUCTION_0xf5
NES_INSTRUCTION_0xf5:
	nop

	.globl NES_INSTRUCTION_0xf6
NES_INSTRUCTION_0xf6:
	nop

	.globl NES_INSTRUCTION_0xf7
NES_INSTRUCTION_0xf7:
	nop

	.globl NES_INSTRUCTION_0xf8
NES_INSTRUCTION_0xf8:
	nop

	.globl NES_INSTRUCTION_0xf9
NES_INSTRUCTION_0xf9:
	nop

	.globl NES_INSTRUCTION_0xfa
NES_INSTRUCTION_0xfa:
	nop

	.globl NES_INSTRUCTION_0xfb
NES_INSTRUCTION_0xfb:
	nop

	.globl NES_INSTRUCTION_0xfc
NES_INSTRUCTION_0xfc:
	nop

	.globl NES_INSTRUCTION_0xfd
NES_INSTRUCTION_0xfd:
	nop

	.globl NES_INSTRUCTION_0xfe
NES_INSTRUCTION_0xfe:
	nop

	.globl NES_INSTRUCTION_0xff
NES_INSTRUCTION_0xff:
	nop

