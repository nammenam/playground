.intel_syntax noprefix        # Switch from AT&T syntax to standard Intel syntax
.global _start                # Expose the entry point symbol to the linker

.section .text
_start:
    # 1. sys_write(stdout, message, length)
    mov rax, 1                # System call number 1 is 'sys_write'
    mov rdi, 1                # File descriptor 1 is 'stdout'
    lea rsi, [rip + msg]      # RIP-relative addressing to load message pointer
    mov rdx, 14               # Length of the string (including newline)
    syscall                   # Invoke the Linux kernel

    # 2. sys_exit(status)
    mov rax, 60               # System call number 60 is 'sys_exit'
    xor rdi, rdi              # Return status code 0 (success)
    syscall                   # Invoke the Linux kernel

.section .data
msg:
    .ascii "Hello, World!\n"
