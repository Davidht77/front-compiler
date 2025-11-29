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
 subq $32, %rsp
 movl $42, %eax
 movl %eax, -8(%rbp)
 movslq -8(%rbp), %rax
 movslq %eax, %rax
 cvtsi2ssq %rax, %xmm0
 movq %xmm0, %rax
 movl %eax, -16(%rbp)
 movl -16(%rbp), %eax
 movq %rax, %xmm0
 cvttss2siq %xmm0, %rax
 movq %rax, -24(%rbp)
 movl -16(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
 movq -24(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
