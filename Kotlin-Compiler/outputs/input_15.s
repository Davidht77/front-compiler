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
 subq $48, %rsp
 movl $5, %eax
 movl %eax, -8(%rbp)
 movabsq $4612811918334230528, %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -16(%rbp)
 movabsq $4607632778762754458, %rax
 movq %rax, -24(%rbp)
 movslq -8(%rbp), %rax
 movslq %eax, %rax
 cvtsi2sdq %rax, %xmm0
 movl -16(%rbp), %eax
 movd %eax, %xmm1
 cvtss2sd %xmm1, %xmm1
 addsd %xmm1, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -32(%rbp)
 movl -32(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq -24(%rbp), %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movq %xmm0, %rax
 movd %eax, %xmm1
 cvtss2sd %xmm1, %xmm1
 addsd %xmm1, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, -40(%rbp)
 movl -40(%rbp), %eax
 movq %rax, %xmm0
 cvttss2si %xmm0, %eax
 movslq %eax, %rax
 movl %eax, -48(%rbp)
 movl -32(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
 movl -40(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 leaq print_fmt_float(%rip), %rdi
 movl $1, %eax
 call printf@PLT
 movslq -48(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
