ENTRY(_start64)

SECTIONS
{
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
