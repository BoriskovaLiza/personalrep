.globl func
func:
	movl $0, %eax
	testq %rdi, %rdi
	jl .L4
	testq %rsi, %rsi
	jl .L4
	jmp .L2
.L3:
	incq %rax
	sal $1, %rdi
.L2:
	cmpq %rdi, %rsi
	jl .L3
.L4:
	rep ret
