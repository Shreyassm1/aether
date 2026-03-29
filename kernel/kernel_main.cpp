#include "drivers/vga.hpp"

#if defined(__x86_64__)
#include "arch/x86_64/mmu/paging.hpp"
#endif

#ifndef AETHER_BOOT_MESSAGE
#define AETHER_BOOT_MESSAGE "Aether-OS Kernel Initialized."
#endif

// ---------------- LEGACY LEARNING SNIPPET ----------------
//
// This preserves the earlier "raw pointer + loose function" approach for comparison.
// It is intentionally not used anymore; the active path is the VGA::Console driver below.
namespace legacy_vga_demo {

using uint16_t = unsigned short;
using uint8_t = unsigned char;

[[maybe_unused]] static void write_string(uint16_t* vga_buffer, int& cursor, const char* str, uint8_t color) {
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            cursor += 80 - (cursor % 80);
            continue;
        }

        vga_buffer[cursor++] = (uint16_t)str[i] | (uint16_t)color << 8;
    }
}

} // namespace legacy_vga_demo

// ---------------- ACTIVE KERNEL ENTRY ----------------

extern "C" void kernel_main() {
#if defined(__x86_64__)
    /*
     * We entered long mode using a tiny bootstrap identity map in assembly.
     * Replace that with the C++ MMU tables so paging setup lives in one place.
     */
    mmu::init();
    mmu::enable();
#else
#endif

    VGA::Console::init();
    VGA::Console::clear();
    VGA::Console::write(AETHER_BOOT_MESSAGE);
    VGA::Console::write("\n");
    VGA::Console::write("VGA driver working\n");

    // Hang the CPU to keep the message on screen
    while (true) {
        asm volatile("hlt");
    }
}
