# QEMU Laboratory Guide

PRG32 desktop emulation uses Espressif's ESP32-C3 QEMU graphics path, while the physical classroom target is ESP32-C6. Both expose the PRG32 cartridge ABI. Framework screens are 320x240; cartridge gameplay uses a centered 320x200 viewport.

## Procedure
1. Install ESP-IDF 5.4 or newer with ESP32-C3 support and install Espressif `qemu-riscv32`.
2. Clone PRG32 and run `python3 -m prg32 doctor`.
3. From PRG32, build the QEMU host with `python3 -m prg32 qemu build`.
4. From this game repository, run `PRG32_ROOT=/path/to/PRG32 ./build.sh` to produce the portable cartridge.
5. Stage it with `cd /path/to/PRG32 && python3 -m prg32 qemu upload /path/to/game/build/grendizer-vega-assault-86.prg32` (adjust the produced filename if necessary).
6. Launch with `python3 -m prg32 qemu run`. `python3 -m prg32 qemu build-and-run` is convenient when rebuilding host firmware.
7. Focus the QEMU terminal for keyboard input. PRG32 maps arrows/WASD to direction, J/Z to A, and K/X to B.

QEMU is the preferred first validation environment because it shortens the edit-build-run loop. Physical-hardware validation remains mandatory for timing, display, input, and audio conclusions.
