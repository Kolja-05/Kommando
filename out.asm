global _start
_start:
push rbp
mov rbp, rsp
mov rax, 100
push rax
mov rax, 3
pop rbx
add rax, rbx
mov QWORD [rbp-8], rax
mov rax, 1
mov QWORD [rbp-16], rax
mov rax, QWORD [rbp-8]
push rax
mov rax, QWORD [rbp-16]
pop rbx
add rax, rbx
mov QWORD [rbp-24], rax
mov rax, QWORD [rbp-8]
mov rdi, rax
mov rax, 60
syscall
mov rsp, rbp
pop rbp
mov rax, 60
mov rdi, 0
syscall
