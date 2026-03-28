.code32

.set MAGIC,    0xe85250d6
.set ARCH,     0
.set LENGTH,   (header_end - header_start)
.set CHECKSUM, -(MAGIC + ARCH + LENGTH)

.section .multiboot_header
.align 8
header_start:
    .long MAGIC
    .long ARCH
    .long LENGTH
    .long CHECKSUM
    .short 0
    .short 0
    .long 8
header_end:

.section .bss
.align 16
.global stack_bottom
stack_bottom:
    .skip 16384
.global stack_top
stack_top:

.section .text
.global _start
.type _start, @function
.extern kernel_main

_start:
    cli
    mov $stack_top, %esp
    mov $stack_top, %ebp
    call kernel_main

.hang:
    hlt
    jmp .hang

.section .note.GNU-stack,"",@progbits
