# Kernel Entry and Early Memory Setup

This document reflects the current code in the repository.

## 1. Boot Entry Split (`boot/boot.S` and `boot/boot_long.S`)

Sources:
- [boot.S](../boot/boot.S)
- [boot_long.S](../boot/boot_long.S)

Current structure:
- `boot.S` is the primitive 32-bit Multiboot2 entry.
- `boot_long.S` is a separate Multiboot2 entry that begins in 32-bit protected mode, performs the long-mode transition, and then calls `kernel_main` in 64-bit mode.

What `boot.S` does:
- Declares a Multiboot2 header so GRUB can recognize the 32-bit kernel image.
- Reserves a 16 KiB stack in `.bss` and sets `esp/ebp` to `stack_top`.
- Disables interrupts with `cli` because no IDT exists yet.
- Calls `kernel_main`.
- If control returns, enters a `hlt` loop.

What `boot_long.S` does:
- Declares its own Multiboot2 header so GRUB can recognize the 64-bit kernel image.
- Reserves a 16 KiB bootstrap stack in `.bss`.
- Builds a minimal bootstrap paging hierarchy using shared bootstrap tables from `paging.cpp`.
- Sets `CR4.PAE`, loads `CR3`, sets `IA32_EFER.LME`, loads a 64-bit GDT, sets `CR0.PG`, and performs a far jump into `.code64`.
- Reloads segment state, sets `rsp/rbp`, and calls `kernel_main`.

Important replacement note:
- The older single-boot structure based on `boot/boot.S` + `boot/linker.ld` + one `kernel.elf` artifact has been replaced.
- The repo now uses a dual-kernel structure with separate entry stubs and separate linker scripts for 32-bit and 64-bit builds.

## 2. Linker Layout Split (`boot/linker32.ld` and `boot/linker64.ld`)

Sources:
- [linker32.ld](../boot/linker32.ld)
- [linker64.ld](../boot/linker64.ld)

Current layout:
- `linker32.ld` uses `ENTRY(_start)` for the 32-bit kernel.
- `linker64.ld` uses `ENTRY(_start64)` for the long-mode kernel.
- Both load the kernel at `0x00100000` (1 MiB).
- Both keep `.multiboot_header` at the start of `.text`.
- Both use the same flat section ordering:
  - `.text`
  - `.rodata`
  - `.data`
  - `.bss`

Why this matters:
- Each kernel image now has an explicit entry symbol matched to its boot path.
- The split keeps the primitive 32-bit boot path intact while allowing the 64-bit kernel to evolve independently.

## 3. First C++ Stage (`kernel/kernel_main.cpp`)

Source: [kernel_main.cpp](../kernel/kernel_main.cpp)

Current behavior:
- Enters as `extern "C" void kernel_main()`.
- In the 32-bit build, writes `"Aether-OS running 32-bit kernel."` to VGA text memory (`0xB8000`).
- In the 64-bit build, first calls `mmu::init()` and `mmu::enable()`, then writes `"Aether-OS entered 64-bit long mode."`.
- Halts in an infinite `hlt` loop.

Notes:
- This stage remains intentionally minimal, but it now acts as the runtime MMU handoff point for the 64-bit kernel.

## 4. Paging Module in Active Use (`kernel/arch/x86_64/mmu`)

Sources:
- [paging.hpp](../kernel/arch/x86_64/mmu/paging.hpp)
- [paging.cpp](../kernel/arch/x86_64/mmu/paging.cpp)

Implemented in module:
- Shared bootstrap tables used only long enough to survive the 32-bit to 64-bit transition.
- Runtime 4-level page tables (`pml4`, `pdpt`, `pd`, `pt`) aligned to 4096 bytes.
- `mmu::init()` clears the runtime tables, links levels, and identity maps first `2 MiB` with 4 KiB pages.
- `mmu::enable()` loads `cr3` with the runtime PML4 and ensures paging-related control bits remain set.

Important status detail:
- This paging code is now invoked from `kernel_main` in the 64-bit build.
- The actual runtime flow for the 64-bit kernel is:
  - GRUB -> `_start64` in `boot_long.S`
  - bootstrap paging + long-mode transition
  - `kernel_main`
  - `mmu::init()`
  - `mmu::enable()`
  - VGA message + halt
