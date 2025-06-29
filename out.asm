global _start
_start:
mov rax, 4
mov rdi, rax
mov rax, 60
syscall
mov rax, 60
mov rdi, 0 
syscall
