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

## Select the correct executable

A version number alone does not establish ESP32-C3 support. Run `python3 tools/check_qemu.py` after loading ESP-IDF, or pass `--qemu /absolute/path/to/qemu-system-riscv32` to inspect a specific installation. The check inspects the actual `-machine help` output and fails if `esp32c3` is absent.

On the inspected macOS system, `/opt/homebrew/bin/qemu-system-riscv32` reports 11.1.1 but lacks the ESP32-C3 machine. The ESP-IDF 5.4 tool manifest pins `esp_develop_9.0.0_20240606`; installation followed by sourcing `/Users/raffaelemontella/esp-idf/export.sh` selects that compatible executable from `.espressif/tools/qemu-riscv32/`. Both installations remain available.

The initial installer failed Python TLS verification. Retrying with `SSL_CERT_FILE=/etc/ssl/cert.pem python3 /Users/raffaelemontella/esp-idf/tools/idf_tools.py install qemu-riscv32` used macOS's system CA bundle and succeeded without disabling certificate verification. This is an observed local workaround, not a required flag on every platform.

Use the framework's SDL display backend. In this specific Espressif QEMU build, `-display none` stalled the guest during the first RGB update; switching to `-display sdl` allowed boot to complete. QMP `screendump` is unavailable in that build. The validation capture instead paused the emulator, dumped the host `g_fb` RGB565 buffer using QMP `pmemsave`, and resumed execution. Its address was obtained from the freshly built host ELF using `riscv32-esp-elf-nm`; never reuse that address across builds.

## Observed validation

On 2026-09-02, an isolated snapshot of PRG32 commit `58dde55ddc37bf23d05d489679f03f6ffce99c62` built an ESP32-C3 QEMU host with ESP-IDF 5.4. The normal framework uploader staged the 26368-byte portable cartridge into `cart0`. The emulator logged successful loading of 25404 bytes of code, 29220 bytes of executable memory and 864 bytes of audio.

Framebuffer captures confirmed attract, title, first-stage gameplay with Screw Crusher shots, and the pause overlay, using serial START and A inputs. `assets/qemu-gameplay.png` is the captured 320×240 framebuffer enlarged to 640×480 with nearest-neighbor sampling. No scene elements were added. The README retains the separate native capture workflow for reproducibility without running QEMU.

The smoke run collected 364266 bytes of mono UART PCM while acknowledging 882-byte chunks at approximately 20 ms intervals to provide audio flow control. This confirms audio transport, not audible quality or stereo/mono equivalence. Full three-stage playthroughs, boss patterns, resume, game-over/victory, stereo listening, frame-time measurements, and physical ESP32-C6 remain outside this QEMU smoke check. Host regression tests cover several of these logical transitions but cannot substitute for target playtesting.
