# Aether Docs

This folder documents the current state of the Aether repository.

## Docs Map

- `kernel_entry.md`
  - Current 32-bit and 64-bit boot paths, linker split, and MMU handoff details.
- `vga_console.md`
  - VGA text buffer driver structure, hardware memory mapping, and why the freestanding C++ design uses a static class.
- `repo_status.md`
  - Up-to-date implementation snapshot, dual-kernel structure, and current limitations/next milestones.

## Scope

These docs describe what is implemented in code today, not planned architecture.
Use `PROGRESS.md` for a commit-level timeline.
