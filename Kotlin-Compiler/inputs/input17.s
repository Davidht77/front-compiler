.data
print_fmt_num: .string "%ld \n"
print_fmt_float: .string "%f\n"
print_fmt_str: .string "%s\n"
.text
.global main
main:
 pushq %rbp
 movq %rsp, %rbp
 movl $0, %eax
 popq %rbp
 ret
.section .note.GNU-stack,"",@progbits
