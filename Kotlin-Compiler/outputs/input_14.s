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
 movl $100, %eax
 movsbq %al, %rax
 movb %al, -8(%rbp)
 movl $1000, %eax
 movswq %ax, %rax
 movw %ax, -16(%rbp)
 movsbq -8(%rbp), %rax
 pushq %rax
 movswq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addw %cx, %ax
 movw %ax, -24(%rbp)
 movswq -24(%rbp), %rax
 movswq %ax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
