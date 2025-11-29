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
 movabsq $4608218246714312622, %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -8(%rbp)
 movabsq $4616820122002590269, %rax
 movq %rax, -16(%rbp)
 movl -8(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq -16(%rbp), %rax
 movq %rax, %xmm1
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
