#pragma once 
#include <stdint.h>

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