#pragma once 
#include <stdint.h>

extern "C" {
    alignas(4096) extern uint64_t bootstrap_pml4[512];
    alignas(4096) extern uint64_t bootstrap_pdpt[512];
    alignas(4096) extern uint64_t bootstrap_pd[512];
}

namespace mmu {
    enum Flags : uint64_t {
        Present = 1ULL << 0,
        Writable = 1ULL << 1,
        User = 1ULL << 2,
        HugePage = 1ULL << 7,
    };
    
    void init();
    void enable();
}
