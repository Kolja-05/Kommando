global _start
_start:
push rbp
mov rbp, rsp
mov rax, 10
push rax
jmp hier
mov rax, 20
push rax
hier:
mov rax, 2
push rax
mov rax, [rbp + -24]
mov rdi, rax
mov rax, 60
syscall
mov rsp, rbp
pop rbp
mov rax, 60
mov rdi, 0 
syscall
