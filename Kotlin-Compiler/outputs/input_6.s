.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
.text
.global main
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $64, %rsp
 movl $127, %eax
 movsbq %al, %rax
 movb %al, -8(%rbp)
 movl $32, %eax
 movswq %ax, %rax
 movw %ax, -16(%rbp)
 movl $2, %eax
 movl %eax, -24(%rbp)
 movl $9545646, %eax
 movq %rax, -32(%rbp)
 movabsq $4614256650576692846, %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -40(%rbp)
 movabsq $4614256656552045848, %rax
 movq %rax, -48(%rbp)
 movslq -24(%rbp), %rax
 pushq %rax
 movq -32(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -56(%rbp)
 movq -48(%rbp), %rax
 movq %rax, %xmm0
 movl -40(%rbp), %eax
 movd %eax, %xmm1
 cvtss2sd %xmm1, %xmm1
 divsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -64(%rbp)
 movq -56(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -64(%rbp), %rax
 movq %rax, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
