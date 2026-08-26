; boot.s

extern kernel_main      ; simbolo definito in kernel.c

MAGIC    equ 0x1BADB002
FLAGS    equ 0x0
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
_start:
    mov esp, stack_top
    call kernel_main
.hang:
    hlt
    jmp .hang
