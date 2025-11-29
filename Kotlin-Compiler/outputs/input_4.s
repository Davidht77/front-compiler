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
 leaq str_0(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $1, %eax
 movl %eax, -8(%rbp)
 movl $5, %eax
 movl %eax, -16(%rbp)
 movl $1, %eax
 movl %eax, -24(%rbp)
 movl -8(%rbp), %eax
 movl %eax, -32(%rbp)
loop_0:
 movl -32(%rbp), %eax
 movl -16(%rbp), %ecx
 cmpl %ecx, %eax
 jg endloop_0
 movslq -32(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -32(%rbp), %eax
 movl -24(%rbp), %ecx
 addl %ecx, %eax
 movl %eax, -32(%rbp)
 jmp loop_0
endloop_0:
 leaq str_1(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $1, %eax
 movl %eax, -8(%rbp)
 movl $10, %eax
 movl %eax, -16(%rbp)
 movl $2, %eax
 movl %eax, -24(%rbp)
 movl -8(%rbp), %eax
 movl %eax, -32(%rbp)
loop_1:
 movl -32(%rbp), %eax
 movl -16(%rbp), %ecx
 cmpl %ecx, %eax
 jg endloop_1
 movslq -32(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -32(%rbp), %eax
 movl -24(%rbp), %ecx
 addl %ecx, %eax
 movl %eax, -32(%rbp)
 jmp loop_1
endloop_1:
 leaq str_2(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $5, %eax
 movl %eax, -8(%rbp)
 movl $1, %eax
 movl %eax, -16(%rbp)
 movl $1, %eax
 movl %eax, -24(%rbp)
 movl -8(%rbp), %eax
 movl %eax, -32(%rbp)
loop_2:
 movl -32(%rbp), %eax
 movl -16(%rbp), %ecx
 cmpl %ecx, %eax
 jl endloop_2
 movslq -32(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -32(%rbp), %eax
 movl -24(%rbp), %ecx
 subl %ecx, %eax
 movl %eax, -32(%rbp)
 jmp loop_2
endloop_2:
 leaq str_3(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_str(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $10, %eax
 movl %eax, -8(%rbp)
 movl $1, %eax
 movl %eax, -16(%rbp)
 movl $2, %eax
 movl %eax, -24(%rbp)
 movl -8(%rbp), %eax
 movl %eax, -32(%rbp)
loop_3:
 movl -32(%rbp), %eax
 movl -16(%rbp), %ecx
 cmpl %ecx, %eax
 jl endloop_3
 movslq -32(%rbp), %rax
 movslq %eax, %rsi
 leaq print_fmt_num(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -32(%rbp), %eax
 movl -24(%rbp), %ecx
 subl %ecx, %eax
 movl %eax, -32(%rbp)
 jmp loop_3
endloop_3:
.end_main:
leave
ret
.data
str_3: .string "Test 4: 10 downTo 1 step 2"
str_2: .string "Test 3: 5 downTo 1"
str_1: .string "Test 2: 1..10 step 2"
str_0: .string "Test 1: 1..5"
.section .note.GNU-stack,"",@progbits
