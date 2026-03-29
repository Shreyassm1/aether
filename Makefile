BUILD_DIR := build
ISO_DIR := iso

KERNEL32 := $(BUILD_DIR)/kernel32.elf
KERNEL64 := $(BUILD_DIR)/kernel64.elf

COMMON_WARN := -Wall -Wextra
COMMON_CXXFLAGS := -ffreestanding -fno-exceptions -fno-rtti -fno-pie -no-pie $(COMMON_WARN)
BOOT_MESSAGE32 := '"Aether-OS running 32-bit kernel."'
BOOT_MESSAGE64 := '"Aether-OS entered 64-bit long mode."'

CXX32 := g++
CXX64 := x86_64-linux-gnu-g++
CC32 := gcc
CC64 := x86_64-linux-gnu-gcc

CXXFLAGS32 := -m32 $(COMMON_CXXFLAGS)
CXXFLAGS64 := -m64 -mno-red-zone $(COMMON_CXXFLAGS)
ASFLAGS32 := -m32 -ffreestanding
ASFLAGS64 := -m64 -ffreestanding
LDFLAGS32 := -m32 -nostdlib -ffreestanding -fno-pie -no-pie -T boot/linker32.ld
LDFLAGS64 := -m64 -nostdlib -ffreestanding -fno-pie -no-pie -T boot/linker64.ld

.PHONY: all clean iso run32 run64

all: $(KERNEL32) $(KERNEL64)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(KERNEL32): $(BUILD_DIR) $(BUILD_DIR)/boot32.o $(BUILD_DIR)/kernel_main32.o $(BUILD_DIR)/vga32.o
	$(CXX32) $(LDFLAGS32) -o $@ $(BUILD_DIR)/boot32.o $(BUILD_DIR)/kernel_main32.o $(BUILD_DIR)/vga32.o

$(KERNEL64): $(BUILD_DIR) $(BUILD_DIR)/boot64.o $(BUILD_DIR)/kernel_main64.o $(BUILD_DIR)/paging64.o $(BUILD_DIR)/vga64.o
	$(CXX64) $(LDFLAGS64) -o $@ $(BUILD_DIR)/boot64.o $(BUILD_DIR)/kernel_main64.o $(BUILD_DIR)/paging64.o $(BUILD_DIR)/vga64.o

$(BUILD_DIR)/boot32.o: boot/boot.S | $(BUILD_DIR)
	$(CC32) $(ASFLAGS32) -c $< -o $@

$(BUILD_DIR)/boot64.o: boot/boot_long.S | $(BUILD_DIR)
	$(CC64) $(ASFLAGS64) -c $< -o $@

$(BUILD_DIR)/kernel_main32.o: kernel/kernel_main.cpp | $(BUILD_DIR)
	$(CXX32) $(CXXFLAGS32) -DAETHER_BOOT_MESSAGE=$(BOOT_MESSAGE32) -c $< -o $@

$(BUILD_DIR)/kernel_main64.o: kernel/kernel_main.cpp | $(BUILD_DIR)
	$(CXX64) $(CXXFLAGS64) -DAETHER_BOOT_MESSAGE=$(BOOT_MESSAGE64) -c $< -o $@

$(BUILD_DIR)/vga32.o: kernel/drivers/vga.cpp kernel/drivers/vga.hpp | $(BUILD_DIR)
	$(CXX32) $(CXXFLAGS32) -c $< -o $@

$(BUILD_DIR)/vga64.o: kernel/drivers/vga.cpp kernel/drivers/vga.hpp | $(BUILD_DIR)
	$(CXX64) $(CXXFLAGS64) -c $< -o $@

$(BUILD_DIR)/paging64.o: kernel/arch/x86_64/mmu/paging.cpp | $(BUILD_DIR)
	$(CXX64) $(CXXFLAGS64) -c $< -o $@

iso: all
	cp $(KERNEL32) $(ISO_DIR)/boot/kernel32.elf
	cp $(KERNEL64) $(ISO_DIR)/boot/kernel64.elf
	grub-mkrescue -o $(BUILD_DIR)/aether.iso $(ISO_DIR)

run32: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/aether.iso -boot d

run64: iso
	qemu-system-x86_64 -cdrom $(BUILD_DIR)/aether.iso -boot d

clean:
	rm -rf $(BUILD_DIR)
