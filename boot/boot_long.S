.code32

.set MAGIC,      0xe85250d6
.set ARCH,       0
.set LENGTH,     (header_end - header_start)
.set CHECKSUM,   -(MAGIC + ARCH + LENGTH)

.set IA32_EFER,  0xC0000080
.set EFER_LME,   (1 << 8)
.set CR4_PAE,    (1 << 5)
.set CR0_PG,     (1 << 31)
.set PAGE_FLAGS, 0x003
.set PAGE_2M,    0x083
.set CODE64_SEL, 0x08
.set DATA64_SEL, 0x10

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
.global stack_bottom_long
stack_bottom_long:
    .skip 16384
.global stack_top_long
stack_top_long:

.section .text
.global _start64
.type _start64, @function

.extern kernel_main
.extern bootstrap_pml4
.extern bootstrap_pdpt
.extern bootstrap_pd

_start64:
    cli
    movl $stack_top_long, %esp
    movl $stack_top_long, %ebp

    /*
     * Minimal bootstrap paging:
     *   PML4[0] -> PDPT[0] -> PD[0]
     *   PD[0] maps the first 2 MiB with a single huge page.
     * This exists only to survive the transition into 64-bit mode.
     * The reusable MMU setup lives in kernel/arch/x86_64/mmu/paging.cpp.
     */
    movl $bootstrap_pdpt, %eax
    orl  $PAGE_FLAGS, %eax
    movl $bootstrap_pml4, %edi
    movl %eax, (%edi)

    movl $bootstrap_pd, %eax
    orl  $PAGE_FLAGS, %eax
    movl $bootstrap_pdpt, %edi
    movl %eax, (%edi)

    movl $PAGE_2M, %eax
    movl $bootstrap_pd, %edi
    movl %eax, (%edi)

    movl %cr4, %eax
    orl  $CR4_PAE, %eax
    movl %eax, %cr4

    movl $bootstrap_pml4, %eax
    movl %eax, %cr3

    movl $IA32_EFER, %ecx
    rdmsr
    orl  $EFER_LME, %eax
    wrmsr

    movl $gdt64_descriptor, %eax
    lgdt (%eax)

    movl %cr0, %eax
    orl  $CR0_PG, %eax
    movl %eax, %cr0

    ljmpl $CODE64_SEL, $long_mode_start

.code64
long_mode_start:
    movw $DATA64_SEL, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    xorw %ax, %ax
    movw %ax, %fs
    movw %ax, %gs

    movq $stack_top_long, %rsp
    movq %rsp, %rbp
    call kernel_main

.hang:
    hlt
    jmp .hang

.section .rodata
.align 8
gdt64:
    .quad 0x0000000000000000
    .quad 0x00AF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt64_end:

gdt64_descriptor:
    .word gdt64_end - gdt64 - 1
    .long gdt64

.section .note.GNU-stack,"",@progbits
