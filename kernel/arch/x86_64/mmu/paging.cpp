#include "paging.hpp"

extern "C" {
    alignas(4096) uint64_t bootstrap_pml4[512];
    alignas(4096) uint64_t bootstrap_pdpt[512];
    alignas(4096) uint64_t bootstrap_pd[512];
}

namespace mmu {
    alignas(4096) uint64_t pml4[512];
    alignas(4096) uint64_t pdpt[512];
    alignas(4096) uint64_t pd[512];
    alignas(4096) uint64_t pt[512];

    void init() {
        // 1. Clear tables to ensure no junk data
        for (int i = 0; i < 512; i++) {
            pml4[i] = 0;
            pdpt[i] = 0;
            pd[i] = 0;
            pt[i] = 0;
        }

        // 2. Link the levels (PML4 -> PDPT -> PD -> PT)
        // We map the first entry of each table to point to the next table
        // reinterpret_cast tells the compiler to treat the pointer as an int on which we can perform bitwise operations
        pml4[0] = reinterpret_cast<uint64_t>(pdpt) | Flags::Present | Flags::Writable;
        pdpt[0] = reinterpret_cast<uint64_t>(pd)   | Flags::Present | Flags::Writable;
        pd[0]   = reinterpret_cast<uint64_t>(pt)   | Flags::Present | Flags::Writable;

        // 3. Identity Map the first 2 MiB (512 * 4KiB pages)
        // This covers your kernel code, stack, and VGA buffer
        for (uint64_t i = 0; i < 512; i++) {
            uint64_t physical_addr = i * 4096;
            pt[i] = physical_addr | Flags::Present | Flags::Writable;
        }
    }

    void enable() {
        // Load the PML4 address into CR3
        asm volatile("mov %0, %%cr3" : : "r"(pml4));

        // Note: On x86-64, PAE is usually already enabled if you are in Long Mode,
        // but we ensure it's set for safety.
        uint64_t cr4;
        asm volatile("mov %%cr4, %0" : "=r"(cr4));
        cr4 |= (1 << 5); // Set PAE bit
        asm volatile("mov %0, %%cr4" : : "r"(cr4));

        // Enable Paging bit in CR0
        uint64_t cr0;
        asm volatile("mov %%cr0, %0" : "=r"(cr0));
        cr0 |= (1ULL << 31); // Set PG bit
        asm volatile("mov %0, %%cr0" : : "r"(cr0));
    }
}
