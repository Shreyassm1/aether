#include "vga.hpp"

namespace VGA {

// ---------------- STATIC DRIVER STATE ----------------
//
// These variables live in the kernel image, not on the stack and not on the heap.
// In a freestanding kernel this matters because we want predictable storage with no runtime support.

size_t Console::row = 0;
size_t Console::col = 0;
uint8_t Console::color = 0x0F;

// VGA text mode lives at physical memory address 0xB8000.
// volatile is required because this is memory-mapped I/O, not ordinary RAM from the compiler's perspective.
volatile uint16_t* const Console::buffer =
    reinterpret_cast<volatile uint16_t*>(0xB8000);

// ---------------- INITIALIZATION ----------------

void Console::init() {
    row = 0;
    col = 0;
    color = 0x0F;
}

// ---------------- LOW LEVEL CELL WRITE ----------------

inline void Console::putEntryAt(char c, uint8_t entryColor, size_t x, size_t y) {
    const size_t index = y * WIDTH + x;

    buffer[index] =
        (static_cast<uint16_t>(entryColor) << 8) |
        static_cast<uint16_t>(static_cast<uint8_t>(c));
}

// ---------------- HIGH LEVEL CHARACTER OUTPUT ----------------

void Console::putChar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    } else {
        putEntryAt(c, color, col, row);
        col++;
    }

    if (col >= WIDTH) {
        col = 0;
        row++;
    }

    if (row >= HEIGHT) {
        scroll();
    }
}

// ---------------- STRING OUTPUT ----------------

void Console::write(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        putChar(str[i]);
    }
}

// ---------------- SCREEN MANAGEMENT ----------------

void Console::clear() {
    for (size_t y = 0; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            putEntryAt(' ', color, x, y);
        }
    }

    row = 0;
    col = 0;
}

void Console::scroll() {
    for (size_t y = 1; y < HEIGHT; y++) {
        for (size_t x = 0; x < WIDTH; x++) {
            buffer[(y - 1) * WIDTH + x] = buffer[y * WIDTH + x];
        }
    }

    for (size_t x = 0; x < WIDTH; x++) {
        buffer[(HEIGHT - 1) * WIDTH + x] =
            (static_cast<uint16_t>(color) << 8) |
            static_cast<uint16_t>(' ');
    }

    row = HEIGHT - 1;
}

} // namespace VGA
