/* * We are in a freestanding environment. 
 * We define our own types since we don't have <cstdint> yet.
 */
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

extern "C" void kernel_main() {
    /*
     * Direct hardware access: The VGA Text Buffer
     * Located at physical address 0xB8000.
     * Each character is 2 bytes: [Attribute (8bit) | Character (8bit)]
     */
    uint16_t* vga_buffer = reinterpret_cast<uint16_t*>(0xB8000);

    const char* str = "Aether-OS Kernel Initialized.";
    uint8_t color = 0x0F; // White text on Black background

    for (int i = 0; str[i] != '\0'; ++i) {
        // Construct the 16-bit VGA cell
        vga_buffer[i] = (uint16_t)str[i] | (uint16_t)color << 8;
    }

    // Hang the CPU to keep the message on screen
    while (true) {
        asm volatile("hlt");
    }
}