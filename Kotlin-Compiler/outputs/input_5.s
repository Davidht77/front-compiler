.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
x: .quad 10
y: .quad 5
.text
.global main
.globl add
add:
 pushq %rbp
 movq %rsp, %rbp
 movl %edi,-8(%rbp)
 movl %esi,-16(%rbp)
 subq $16, %rsp
 movslq -8(%rbp), %rax
 pushq %rax
 movslq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 leave
 ret
.end_add:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movslq x(%rip), %rax
 movl %eax, %edi
 movslq y(%rip), %rax
 movl %eax, %esi
 movl $0, %eax
call add
 movl %eax, -8(%rbp)
 movslq -8(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 movl %eax, -16(%rbp)
while_0:
 movslq -16(%rbp), %rax
 pushq %rax
 movl $5, %eax
 movq %rax, %rcx
 popq %rax
 cmpb %cl, %al
 movl $0, %eax
 setl %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_0
 movslq -16(%rbp), %rax
 pushq %rax
 movl $2, %eax
 movq %rax, %rcx
 popq %rax
 cdq
 idivl %ecx
 movslq %edx, %rax
 pushq %rax
 movl $0, %eax
 movq %rax, %rcx
 popq %rax
 cmpb %cl, %al
 movl $0, %eax
 sete %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_1
 movslq -16(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_1
else_1:
endif_1:
 movslq -16(%rbp), %rax
 pushq %rax
 movl $1, %eax
 movq %rax, %rcx
 popq %rax
 addl %ecx, %eax
 movl %eax, -16(%rbp)
 jmp while_0
endwhile_0:
 movslq x(%rip), %rax
 pushq %rax
 movslq y(%rip), %rax
 movq %rax, %rcx
 popq %rax
 cmpb %cl, %al
 movl $0, %eax
 setg %al
 movzbq %al, %rax
 pushq %rax
 movslq -8(%rbp), %rax
 pushq %rax
 movl $0, %eax
 movq %rax, %rcx
 popq %rax
 cmpb %cl, %al
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 movq %rax, %rcx
 popq %rax
 andb %cl, %al
 movb %al, -24(%rbp)
 movsbq -24(%rbp), %rax
 movsbq %al, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
