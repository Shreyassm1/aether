# Repository Status (Current)

## Implemented

- Dual Multiboot2-compatible kernel entries:
  - [boot/boot.S](../boot/boot.S) for the primitive 32-bit kernel
  - [boot/boot_long.S](../boot/boot_long.S) for the 64-bit long-mode kernel
- Dual linker scripts with kernel load base `0x00100000`:
  - [boot/linker32.ld](../boot/linker32.ld)
  - [boot/linker64.ld](../boot/linker64.ld)
- A repo-root [Makefile](../Makefile) that builds both kernel images and the bootable ISO.
- Freestanding C++ entry function `kernel_main` in [kernel/kernel_main.cpp](../kernel/kernel_main.cpp).
- VGA console driver in:
  - [vga.hpp](../kernel/drivers/vga.hpp)
  - [vga.cpp](../kernel/drivers/vga.cpp)
  - static class design for freestanding C++
  - direct MMIO write to physical address `0xB8000`
  - newline handling
  - screen clearing
  - text scrolling
  - foreground/background color control
- Active x86_64 paging implementation in:
  - [paging.hpp](../kernel/arch/x86_64/mmu/paging.hpp)
  - [paging.cpp](../kernel/arch/x86_64/mmu/paging.cpp)
  - Bootstrap tables for the long-mode transition
  - Runtime `PML4/PDPT/PD/PT`
  - Identity map of first `2 MiB` with `4 KiB` pages
  - `mmu::init()` and `mmu::enable()` wired into the 64-bit kernel path

## Replaced Structure

- The previous single-kernel structure based on `boot/linker.ld` and one `iso/boot/kernel.elf` artifact has been replaced.
- The repository now uses:
  - `kernel32.elf` for the 32-bit boot path
  - `kernel64.elf` for the long-mode boot path
- GRUB exposes both paths as separate menu entries.

## Current Code Layout

- `boot/`
  - Early boot assembly and split linker scripts for 32-bit and 64-bit kernels.
- `kernel/`
  - Kernel C++ source and x86_64 MMU code.
- `iso/`
  - Bootable ISO tree (`iso/boot/kernel32.elf`, `iso/boot/kernel64.elf`, `iso/boot/grub/grub.cfg`).
- `docs/`
  - Project documentation.
- `learnings/`
  - Standalone systems experiments.

## Current Boot Artifact Path

- GRUB menu entries in [grub.cfg](../iso/boot/grub/grub.cfg) load:
  - `multiboot2 /boot/kernel32.elf`
  - `multiboot2 /boot/kernel64.elf`

## Current Runtime Behavior

- 32-bit kernel:
  - boots through `boot.S`
  - initializes and clears the VGA console
  - writes status lines through `VGA::Console`
  - halts
- 64-bit kernel:
  - boots through `boot_long.S`
  - enters long mode using bootstrap paging
  - calls `mmu::init()` and `mmu::enable()`
  - initializes and clears the VGA console
  - writes status lines through `VGA::Console`
  - halts

## Gaps / Not Yet Wired

- No IDT/interrupt handling yet.
- No physical memory manager (PMM) or virtual memory manager (VMM) yet.
- No scheduler/process model yet.
- No formatted output layer above the VGA console yet.
- No hardware cursor programming yet.
- No software-managed per-cell metadata beyond the VGA hardware attribute byte.

## Recommended Next Documentation Sync Points

When code changes, update this doc for:
- Any new boot steps or CPU mode transitions.
- Memory subsystem evolution beyond identity-mapped bootstrap/runtime paging.
- Formatted output once a `printf`-like layer is added above the VGA console.
