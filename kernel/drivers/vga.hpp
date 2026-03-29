#pragma once

#include <stddef.h>
#include <stdint.h>

namespace VGA {

class Console {
public:
    static void init();
    static void clear();
    static void putChar(char c);
    static void write(const char* str);

private:
    static void putEntryAt(char c, uint8_t entryColor, size_t x, size_t y);
    static void scroll();

private:
    static constexpr size_t WIDTH = 80;
    static constexpr size_t HEIGHT = 25;

    static size_t row;
    static size_t col;
    static uint8_t color;

    static volatile uint16_t* const buffer;
};

} // namespace VGA
