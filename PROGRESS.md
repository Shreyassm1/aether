# Progress Timeline

This file contains a concise timeline of repository progress, grouped by date. Each entry notes the commit subject and a short explanation of the feature or experiment and how it advances the AETHER course goals.

2026-01-31
- initial commit
	- Repository scaffold and base layout: establishes the project structure used for incremental OS development.

2026-02-01
- memory_map
	- Implemented parsing/representation of the bootloader-provided memory map (e.g., E820/Multiboot2). This is foundational for physical memory management and region reservation.
- perf: verify cache locality impact
	- Microbenchmarks to measure cache-locality effects; results guide data layout and algorithm choices for later subsystems.

2026-02-02
- feat: completed ELF inspection
	- ELF reader/inspector implemented to parse headers, program headers, and sections. Enables reasoning about executable layout and loader requirements.

2026-02-03
- perf: cache behaviour for random and sequential access
	- Benchmarks comparing sequential vs random memory access patterns; validates assumptions about cache hierarchy and access cost.
- structure
	- Reorganized source into `boot/`, `kernel/`, `arch/`, and `learnings/` for clearer separation of concerns and staged learning exercises.
- context_Switch
	- Early context-switch experiment demonstrating saving/restoring CPU state and stack switching—first step toward a scheduler.

2026-02-08
- feat: multiboot2 kernel entry and freestanding C++ startup
	- Added Multiboot2 header, `boot.S`, and freestanding C++ startup flow. Moves the project from raw early boot into a controlled kernel entry and C++ runtime initialization.

2026-02-28
- feat: mmu — basic 4-level paging (identity map first 2 MiB)
	- Added simple paging implementation in `kernel/arch/x86_64/mmu/paging.cpp` and header `paging.hpp`.
	- Statically allocate PML4/PDPT/PD/PT tables and identity-map the first 2 MiB (512 x 4 KiB pages) to support kernel survival when enabling paging.
	- Provide `init()` to populate page tables and `enable()` to load CR3 and set required CR4/CR0 bits. This is intentionally minimal — a foundation for later physical memory manager and virtual mapping features.

2026-03-28
- feat: split boot flow into 32-bit and 64-bit kernel targets
	- Replaced the old single-kernel structure with two explicit boot targets: a primitive 32-bit kernel and a separate long-mode kernel.
	- Added `boot/boot_long.S`, `boot/linker32.ld`, `boot/linker64.ld`, and a repo-root `Makefile` that builds `kernel32.elf`, `kernel64.elf`, and a bootable ISO.
	- Updated GRUB to expose both kernel images as separate menu entries so boot mode can be selected without editing source.
- feat: enter x86_64 long mode and wire runtime MMU handoff
	- Implemented the real bootstrap transition from 32-bit protected mode into 64-bit long mode in `boot/boot_long.S`.
	- Added bootstrap page tables for the transition, a temporary 64-bit GDT, and the required `CR4.PAE`, `EFER.LME`, `CR0.PG`, and far-jump sequence.
	- Wired `kernel_main` to call `mmu::init()` and `mmu::enable()` in the 64-bit build so the runtime MMU mapping becomes active after long mode entry.
- docs: sync repo status with dual-kernel structure
	- Updated `docs/kernel_entry.md`, `docs/repo_status.md`, and `docs/README.md` to document the new split boot flow and note that the older `boot/linker.ld` / `kernel.elf` path was replaced.
