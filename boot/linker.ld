ENTRY(_start)

SECTIONS
{
    /* Load kernel at 1MB */
    . = 0x00100000;

    .text :
    {
        KEEP(*(.multiboot_header))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss :
    {
        *(COMMON)
        *(.bss*)
    }
}
