	.file	"666.c"
	.text
	.globl	func
	.type	func, @function
func:
	movl	$0, %edx
	jmp	.L2
.L3:
	movslq	%edx, %rax
	leaq	(%rdi,%rax,4), %rcx
	movl	(%rcx), %r8d
	movl	%esi, %eax
	subl	%edx, %eax
	cltq
	leaq	-4(%rdi,%rax,4), %rax
	movl	(%rax), %r9d
	movl	%r9d, (%rcx)
	movl	%r8d, (%rax)
	addl	$1, %edx
.L2:
	movl	%esi, %eax
	shrl	$31, %eax
	addl	%esi, %eax
	sarl	%eax
	cmpl	%eax, %edx
	jl	.L3
	rep ret
	.size	func, .-func
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
