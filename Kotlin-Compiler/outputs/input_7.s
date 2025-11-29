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
 movl $10, %eax
 movsbq %al, %rax
 movb %al, -8(%rbp)
 movl $20, %eax
 movswq %ax, %rax
 movw %ax, -16(%rbp)
 movl $30, %eax
 movl %eax, -24(%rbp)
 movl $40, %eax
 movq %rax, -32(%rbp)
 movsbq -8(%rbp), %rax
 pushq %rax
 movswq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addw %cx, %ax
 pushq %rax
 movslq -24(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 movl %eax, -40(%rbp)
 movslq -40(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movsbq -8(%rbp), %rax
 pushq %rax
 movswq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addw %cx, %ax
 pushq %rax
 movslq -24(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 pushq %rax
 movq -32(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -48(%rbp)
 movq -48(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
