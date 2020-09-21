	.file	"mstore.c"
	.text
	.globl	check_property
	.type	check_property, @function
check_property:
	jmp	.L2
.L3:
	addq	$1, %rdi
.L2:
	cmpb	$0, (%rdi)
	jne	.L3
	movl	$0, %eax
	ret
	.size	check_property, .-check_property
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
