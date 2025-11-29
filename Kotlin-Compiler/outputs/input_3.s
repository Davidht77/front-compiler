.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
.text
.global main
.globl sumarDos
sumarDos:
 pushq %rbp
 movq %rsp, %rbp
 movl %edi,-8(%rbp)
 subq $16, %rsp
 movslq -8(%rbp), %rax
 pushq %rax
 movl $2, %eax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 leave
 ret
.end_sumarDos:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movl $7, %eax
 movl %eax, -8(%rbp)
 movslq -8(%rbp), %rax
 movl %eax, %edi
 movl $0, %eax
call sumarDos
 movl %eax, -16(%rbp)
 movslq -16(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
