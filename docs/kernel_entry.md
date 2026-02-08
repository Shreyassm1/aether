# Multiboot2 Entry & Freestanding Setup

## 1. `boot/boot.S` (Entry Point)

Defines the Multiboot2 header and transitions the CPU from bootloader hand-off to C++ execution.

- **Multiboot2 Header**: Located in `.multiboot_header` section. Contains Magic (`0xe85250d6`), Arch (`0` for i386), and checksum. Required for GRUB to recognize the binary as a kernel.
- **Stack Allocation**: Reserves 16KB in the `.bss` section. x86 stack grows downwards, so `esp` is initialized to the top of this block.
- **Interrupts**: Cleared (`cli`) immediately. We have no Interrupt Descriptor Table (IDT) yet.
- **Hand-off**: Calls `extern "C" kernel_main`.

## 2. `boot/linker.ld` (Memory Layout)

Directs the linker on how to map the object files into the final generic ELF binary.

- **Load Address**: `0x1M` (1 Megabyte). This avoids collision with BIOS code, video RAM, and memory-mapped I/O located in the lower 1MB.
- **Section Ordering**:
  - `.text`: Contains code. The Multiboot header is forced to the very beginning so the bootloader finds it within the first 8KB.
  - `.rodata`: Read-only data.
  - `.data`: Initialized data.
  - `.bss`: Uninitialized data (zeroed by GRUB).

## 3. `kernel/kernel_main.cpp` (Kernel Logic)

The first high-level code executing in the kernel.

- **Freestanding**: Compiled without standard library support. No `glibc`, no exceptions, no RTTI.
- **Naming**: Uses `extern "C"` to disable C++ name mangling, ensuring the symbol name matches what `boot.S` expects.
- **VGA Output**: Writes directly to Physical Address `0xB8000` (VGA Text Buffer).
  - Each screen cell is 16-bits: [Attribute (8-bit) | Character (8-bit)].
  - Attribute `0x0F` = White text on black background.
