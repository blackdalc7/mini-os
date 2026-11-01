# Makefile for mini-os.
#
#   make            build the bootable image (build/os-image.bin)
#   make run        boot it in QEMU with a display
#   make test       build and run the host unit tests
#   make test-boot  boot head-less in QEMU and check the serial output
#   make clean      remove build artefacts

BUILD := build

AS      := nasm
CC      := gcc
LD      := ld
OBJCOPY := objcopy
QEMU    := qemu-system-i386

# Freestanding 32-bit kernel: no host libc, no PIE, no stack protector.
CFLAGS := -m32 -ffreestanding -fno-pie -fno-stack-protector -fno-builtin \
          -nostdlib -std=c11 -Wall -Wextra -Werror -Ikernel -c

KERNEL_SECTORS := 32

# entry.o must link first so _start sits at the kernel's load address.
KOBJS := $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/vga.o \
         $(BUILD)/serial.o $(BUILD)/util.o

.PHONY: all run test test-boot clean

all: $(BUILD)/os-image.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.bin: boot/boot.asm | $(BUILD)
	$(AS) -f bin $< -o $@

$(BUILD)/entry.o: kernel/entry.asm | $(BUILD)
	$(AS) -f elf32 $< -o $@

$(BUILD)/%.o: kernel/%.c | $(BUILD)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD)/kernel.elf: $(KOBJS) kernel/linker.ld
	$(LD) -m elf_i386 --no-warn-rwx-segments -T kernel/linker.ld -o $@ $(KOBJS)

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@

# Image = boot sector, then the kernel, padded so the boot sector's fixed-count
# disk read never runs off the end of the file.
$(BUILD)/os-image.bin: $(BUILD)/boot.bin $(BUILD)/kernel.bin
	cat $(BUILD)/boot.bin $(BUILD)/kernel.bin > $@
	truncate -s $$((512 + $(KERNEL_SECTORS) * 512)) $@

run: $(BUILD)/os-image.bin
	$(QEMU) -drive format=raw,file=$<,if=floppy

test: $(BUILD)/test_util
	$(BUILD)/test_util

$(BUILD)/test_util: tests/test_util.c kernel/util.c kernel/util.h kernel/vga.h | $(BUILD)
	$(CC) -std=c11 -Wall -Wextra -Werror -Ikernel tests/test_util.c kernel/util.c -o $@

# Boot head-less and confirm the kernel reached its last line over serial.
test-boot: $(BUILD)/os-image.bin
	@out=$$(timeout 20 $(QEMU) -drive format=raw,file=$<,if=floppy \
		-display none -serial stdio -no-reboot 2>/dev/null || true); \
	echo "$$out"; \
	echo "$$out" | grep -q "mini-os: halted." \
		&& echo "boot test PASSED" \
		|| (echo "boot test FAILED: expected halt banner on serial"; exit 1)

clean:
	rm -rf $(BUILD)
