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
 movl $1, %eax
 movl %eax, -8(%rbp)
 movabsq $4612004850763007598, %rax
 movq %rax, %xmm0
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -16(%rbp)
 movslq -8(%rbp), %rax
 movslq %eax, %rax
 cvtsi2sdq %rax, %xmm0
 movl -16(%rbp), %eax
 movd %eax, %xmm1
 cvtss2sd %xmm1, %xmm1
 addsd %xmm1, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -24(%rbp)
 movl -24(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
