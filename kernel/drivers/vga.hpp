#pragma once

#include <stddef.h>
#include <stdint.h>

namespace VGA {

class Console {
public:
    enum class Color : uint8_t {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGray = 7,
        DarkGray = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        Yellow = 14,
        White = 15,
    };

    static void init();
    static void clear();
    static void putChar(char c);
    static void write(const char* str);
    static void setColor(Color foreground, Color background = Color::Black);

private:
    static uint8_t makeColor(Color foreground, Color background);
    static void putEntryAt(char c, uint8_t entryColor, size_t x, size_t y);
    static void scroll();
    static void refreshColor();

private:
    static constexpr size_t WIDTH = 80;
    static constexpr size_t HEIGHT = 25;

    static size_t row;
    static size_t col;
    static uint8_t color;
    static Color foreground;
    static Color background;

    static volatile uint16_t* const buffer;
};

} // namespace VGA
