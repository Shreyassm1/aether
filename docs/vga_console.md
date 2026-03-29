# VGA Console Driver

This document reflects the current code in the repository.

Sources:
- [vga.hpp](../kernel/drivers/vga.hpp)
- [vga.cpp](../kernel/drivers/vga.cpp)
- [kernel_main.cpp](../kernel/kernel_main.cpp)

## 0. What VGA Text Attributes Can Actually Do

VGA text mode does support a few character attributes, but they are limited to what one attribute byte can encode.

What we can control now:

- foreground color
- background color

What we still cannot control in VGA text mode:

- per-character size
- arbitrary metadata visible on screen
- true RGB colors

Why:

- each screen cell is only 2 bytes total
- 1 byte is the character
- 1 byte is the attribute
- the hardware decides how to interpret that attribute byte

## 1. Why the Driver Is a Static C++ Class

The active VGA path is:

```cpp
namespace VGA {
class Console {
    static state;
    static methods;
};
}
```

This is deliberate for a freestanding kernel:

- No heap is required.
- No object construction order is required.
- No runtime support is required.
- State still stays grouped under one driver type instead of being spread across globals and loose functions.

What this means behind the scenes:

- `Console::row`, `Console::col`, and `Console::color` become ordinary global storage in the kernel image.
- `Console::putChar()` and `Console::write()` are just namespaced functions after compilation.
- There is no hidden `this` pointer because every method is `static`.

So this is "real C++ structure" without paying for features the kernel does not support yet.

## 2. Why `volatile uint16_t* const buffer` Points to `0xB8000`

The VGA text buffer is mapped by PC hardware at physical address `0xB8000`.

In code:

```cpp
volatile uint16_t* const Console::buffer =
    reinterpret_cast<volatile uint16_t*>(0xB8000);
```

Why each part matters:

- `0xB8000`
  - fixed hardware address for VGA text mode memory.
- `uint16_t*`
  - each screen cell is 2 bytes:
  - low 8 bits = ASCII character
  - high 8 bits = color attribute
- `volatile`
  - tells the compiler that every access must result in a real load/store.
  - without this, the compiler could cache values or remove writes it thinks are unnecessary.
- `const` on the pointer
  - the driver should never accidentally repoint `buffer` somewhere else.
- `reinterpret_cast`
  - tells the compiler to treat the integer address as a pointer.
  - this is the correct low-level C++ cast for raw hardware addresses.

What the CPU sees during a write:

- The compiler computes the target address.
- The compiler emits a store instruction to memory near `0xB8000`.
- The platform routes that write to VGA text memory.
- The display hardware later reads that memory and turns it into visible text.

## 3. Why `putEntryAt()` Packs Character and Color into One Word

In code:

```cpp
buffer[index] =
    (static_cast<uint16_t>(entryColor) << 8) |
    static_cast<uint16_t>(static_cast<uint8_t>(c));
```

Why this exact shape:

- `entryColor << 8`
  - moves the 8-bit VGA attribute into the upper byte.
- `c`
  - occupies the lower byte as the visible character.
- `|`
  - combines the two fields into one 16-bit value.

This matches the VGA hardware layout exactly, so one C++ assignment becomes one 16-bit cell update.

Why not use a struct here:

- A bitfield or packed struct would introduce layout questions that are unnecessary at this stage.
- The 16-bit value format is the hardware format directly.
- The current version is easier to verify against the VGA memory map.

## 4. How Foreground and Background Colors Fit into One Attribute Byte

The current driver exposes:

- `setColor(foreground, background)`

Behind the scenes, VGA text mode uses this layout:

```text
bits 7..4  = background color
bits 3..0  = foreground color
```

What that means in practice:

- foreground gets 4 bits, so it can use all 16 VGA colors
- background gets 4 bits in the current simplified design
- one driver call just packs those two nibbles into the attribute byte used by VGA text mode

Why the driver was simplified:

- the blink path was not producing reliable visible behavior in QEMU
- keeping only the observable color path makes the driver easier to trust and easier to learn from
- the code now demonstrates only the hardware behavior we can verify clearly on screen

## 5. What `putChar()` Really Does

`putChar()` is the policy layer above the raw hardware write.

Responsibilities:

- detect newline
- place visible characters
- advance the cursor
- wrap at column 80
- trigger scrolling at row 25

Behind the scenes:

- `row` and `col` are just integers in kernel memory.
- On a normal character, the driver:
  - computes a linear index `y * WIDTH + x`
  - writes a 16-bit entry into VGA memory
  - increments `col`
- On newline, it:
  - resets `col` to `0`
  - increments `row`

Why not manipulate the hardware cursor registers yet:

- writing text to memory is enough to get visible output
- hardware cursor programming needs extra port I/O code
- keeping that out for now keeps the first driver minimal and easier to reason about

## 6. Why `scroll()` Copies Rows Manually

When output reaches the bottom of the screen, the driver must preserve earlier lines and make space for a new last line.

In code, it:

- copies rows `1..24` into rows `0..23`
- fills the last row with spaces
- sets `row = HEIGHT - 1`

What the machine is doing:

- reading 16-bit words from VGA memory
- writing them back one row earlier
- clearing the last row with blank entries

Why not use `memmove`:

- we do not rely on a hosted runtime or libc implementation
- an explicit loop is fully visible and freestanding-safe
- this keeps the data movement obvious for learning

## 7. Why `init()` and `clear()` Are Separate

`init()`:

- resets driver state (`row`, `col`, foreground, background, cached attribute byte)

`clear()`:

- writes blank cells into every screen position
- resets cursor position afterward

This separation matters because "driver state reset" and "hardware surface clear" are different operations.

## 8. Why the Old Raw Code Was Kept

The previous direct-write example is still preserved in [kernel_main.cpp](../kernel/kernel_main.cpp) inside `legacy_vga_demo`.

That code is not active anymore, but it remains useful because it shows the progression:

- old style:
  - raw pointer
  - loose helper
  - cursor managed directly in `kernel_main`
- current style:
  - explicit driver namespace
  - explicit static state
  - explicit driver API

The goal is to show the transition from "proof of hardware access" to "first real device driver abstraction" without hiding the earlier learning step.
