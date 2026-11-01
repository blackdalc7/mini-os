# mini-os

[![CI](https://github.com/blackdalc7/mini-os/actions/workflows/ci.yml/badge.svg)](https://github.com/blackdalc7/mini-os/actions/workflows/ci.yml)

A tiny "hello world" operating system: a hand-written **x86 boot sector** that
loads a small **C kernel**, switches the CPU into 32-bit protected mode, and
prints to the screen. It boots in QEMU. The point of the project is to see the
whole early-boot path with nothing hidden — BIOS → boot sector → protected mode
→ C — in about 300 lines.

This is a learning toy, not a usable OS: there is no memory management, no
interrupts, no processes. It boots, prints, and halts.

## What happens when it boots

```text
BIOS  ──loads sector 1 to 0x7C00──▶  boot/boot.asm  (16-bit real mode)
                                        │  print "loading" (BIOS int 10h)
                                        │  read the kernel off disk (int 13h)
                                        │  enable A20, load a GDT, set CR0.PE
                                        ▼
                                     32-bit protected mode
                                        │  jump to 0x1000
                                        ▼
                                     kernel/entry.asm  (_start)
                                        │  set up a stack, call kernel_main
                                        ▼
                                     kernel/kernel.c
                                        │  clear screen, print banner to VGA
                                        │  + COM1 serial, show some numbers
                                        ▼
                                     halt
```

On screen (and mirrored to the serial port) you get:

```text
mini-os kernel: booted in 32-bit protected mode
hello from C, running on bare metal
decimal 2026 in hex is 0x7ea, in binary is 11111101010
mini-os: halted.
```

## Build and run

You need `nasm`, a `gcc` that can target 32-bit (`gcc-multilib` on Debian/
Ubuntu), `binutils`, and `qemu-system-i386`.

```bash
make            # assemble + compile + link -> build/os-image.bin
make run        # boot it in QEMU with a window
```

`make run` is the same as:

```bash
qemu-system-i386 -drive format=raw,file=build/os-image.bin,if=floppy
```

## Tests

Two layers, both run in CI:

```bash
make test        # host unit tests for the pure helpers (util.c, vga encoding)
make test-boot   # boot head-less in QEMU and grep the serial output
```

- **Unit tests** (`tests/test_util.c`) compile the kernel's dependency-free
  helpers with the ordinary host compiler and assert on them: `k_utoa` in bases
  2/10/16, `k_strlen`, `k_memset`, and the VGA cell/colour packing. These behave
  identically on the host and in the kernel because they touch no hardware.
- **Boot test** runs the real image in QEMU with COM1 forwarded to stdout and
  checks that the kernel reached its final `mini-os: halted.` line — an
  end-to-end check that the boot sector, the mode switch and the C kernel all
  work, with no display required.

## Layout

```text
boot/boot.asm       512-byte boot sector: load kernel, enter protected mode
kernel/
    entry.asm       32-bit _start stub: set up stack, call kernel_main
    kernel.c        kernel_main: the banner and demo output
    vga.c / vga.h   80x25 VGA text driver (scrolling, colour) at 0xB8000
    serial.c/.h     COM1 UART output, used for the head-less boot test
    io.h            inb / outb port I/O
    util.c / util.h k_strlen, k_memset, k_utoa (freestanding, host-testable)
    linker.ld       links the flat kernel at 0x1000
Makefile            build / run / test / clean
```

## Notes and limitations

- The kernel is compiled **freestanding** (`-ffreestanding -nostdlib`): no C
  library, no runtime. Only compiler-provided headers (`stdint.h`, `stddef.h`)
  are used, which is why `make test` needs no 32-bit libc.
- Disk read uses CHS `int 13h` and loads a fixed number of sectors — fine for a
  floppy image in QEMU, deliberately not a real disk driver.
- A20 is enabled through the "fast A20" system control port, which works in
  QEMU and most emulators.

## Licence

MIT. See [LICENSE](LICENSE).
