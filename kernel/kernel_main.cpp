/* * We are in a freestanding environment. 
 * We define our own types since we don't have <cstdint> yet.
 */
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#if defined(__x86_64__)
#include "arch/x86_64/mmu/paging.hpp"
#endif

#ifndef AETHER_BOOT_MESSAGE
#define AETHER_BOOT_MESSAGE "Aether-OS Kernel Initialized."
#endif

static void write_string(uint16_t* vga_buffer, int& cursor, const char* str, uint8_t color) {
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '\n') {
            cursor += 80 - (cursor % 80);
            continue;
        }

        vga_buffer[cursor++] = (uint16_t)str[i] | (uint16_t)color << 8;
    }
}

extern "C" void kernel_main() {
    uint16_t* vga_buffer = reinterpret_cast<uint16_t*>(0xB8000);
    uint8_t color = 0x0F; // White text on Black background
    int cursor = 0;

#if defined(__x86_64__)
    /*
     * We entered long mode using a tiny bootstrap identity map in assembly.
     * Replace that with the C++ MMU tables so paging setup lives in one place.
     */
    mmu::init();
    mmu::enable();
#else
    /*
     * Direct hardware access: The VGA Text Buffer
     * Located at physical address 0xB8000.
     * Each character is 2 bytes: [Attribute (8bit) | Character (8bit)]
     */
#endif
    write_string(vga_buffer, cursor, AETHER_BOOT_MESSAGE, color);

    // Hang the CPU to keep the message on screen
    while (true) {
        asm volatile("hlt");
    }
}
