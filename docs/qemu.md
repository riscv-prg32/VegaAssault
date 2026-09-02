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

Generic QEMU and Espressif QEMU can share the executable name `qemu-system-riscv32`. Install the Espressif package selected by your ESP-IDF tool manifest with `python "$IDF_PATH/tools/idf_tools.py" install qemu-riscv32`, then source `/path/to/esp-idf/export.sh` again. Verify the selected executable with `python3 tools/check_qemu.py`; it must advertise the `esp32c3` machine.

If installation fails certificate verification, configure Python to use a trusted CA bundle appropriate for your operating system. Keep TLS verification enabled.

Use the framework's SDL display backend. In this specific Espressif QEMU build, `-display none` stalled the guest during the first RGB update; switching to `-display sdl` allowed boot to complete. QMP `screendump` is unavailable in that build. The validation capture instead paused the emulator, dumped the host `g_fb` RGB565 buffer using QMP `pmemsave`, and resumed execution. Its address was obtained from the freshly built host ELF using `riscv32-esp-elf-nm`; never reuse that address across builds.

## Observed validation

On 2026-09-02, PRG32 commit `58dde55ddc37bf23d05d489679f03f6ffce99c62` was used to build an ESP32-C3 host with ESP-IDF 5.4 and Espressif QEMU `esp_develop_9.0.0_20240606`. The normal framework uploader staged the 26368-byte portable cartridge into `cart0`. The emulator logged successful loading of 25404 bytes of code, 29220 bytes of executable memory and 864 bytes of audio.

Framebuffer captures confirmed attract, title, first-stage gameplay with Screw Crusher shots, and the pause overlay, using serial START and A inputs. `assets/qemu-gameplay.png` is the captured 320×240 framebuffer enlarged to 640×480 with nearest-neighbor sampling. No scene elements were added. The README retains the separate native capture workflow for reproducibility without running QEMU.

The smoke run collected 364266 bytes of mono UART PCM while acknowledging 882-byte chunks at approximately 20 ms intervals to provide audio flow control. This confirms audio transport, not audible quality or stereo/mono equivalence. Full three-stage playthroughs, boss patterns, resume, game-over/victory, stereo listening, frame-time measurements, and physical ESP32-C6 remain outside this QEMU smoke check. Host regression tests cover several of these logical transitions but cannot substitute for target playtesting.


## Updated soundtrack smoke test (2026-09-02)

The revised 26520-byte cartridge was staged through `python3 -m prg32 qemu upload` into the existing ESP32-C3 host. Espressif QEMU `esp_develop_9.0.0_20240606` passed the machine probe. The loader reported 25556 bytes of code, 29372 bytes of executable memory and 864 bytes of audio, confirming this run used the revised build.

SDL framebuffer captures confirmed the attract/title presentation, first-stage gameplay, Screw Crusher firing with an increased score, pause overlay and resumed gameplay. Serial input used SELECT to reach the cartridge list, A to launch, then START for title/start/pause/resume. No guest panic or reset was observed during the approximately 28-second smoke run; QEMU was terminated by the test afterward.

The existing host configuration is 22050 Hz, signed 16-bit **mono**, with stereo mode disabled. The run captured 1225980 PCM bytes (27.8 seconds), paced by 882-byte UART chunks and credit acknowledgments. Absolute peak was 13467; no samples reached the signed 16-bit clipping boundary. The last second of the paused interval was all zero; the last second after resume was nonzero (peak 6117). These measurements verify mono transport, pause silence and resumed output, not subjective sound quality or stereo separation.

Local evidence is under `build/qemu-soundtrack/`: `serial.log`, `marks.json`, framebuffer dumps/PNGs, `mono.wav` and the shorter `gameplay-mono.wav`. Dumps used `g_fb` at 0x3fc9fe7c, resolved from this host ELF with `riscv32-esp-elf-nm`; resolve it again for any rebuilt host. These generated files are not committed. Full stage/boss coverage, stereo listening and physical ESP32-C6 remain separate validation obligations.

## Sprite refinement smoke test (2026-09-02)

The 26520-byte sprite revision was staged through the normal QEMU uploader into the existing ESP32-C3 host. The loader reported 25556 bytes of code, 29372 bytes of executable memory and 864 bytes of audio. The framebuffer address was checked again against that host ELF before capture. SDL framebuffer inspection confirmed the refined title/player artwork, first-stage enemies, firing, pause overlay and resumed gameplay. `assets/qemu-gameplay.png` now shows this revision's first-stage enemy-wave capture, enlarged 2× with nearest-neighbor sampling.

Local evidence is in `build/qemu-sprites/` (serial log, raw framebuffer dumps, PNGs, contact sheet and mono WAV). The run collected 1184526 bytes of mono PCM, peak 13874, with no full-scale clipped samples; this is a transport observation, not a listening or stereo test. No guest panic or reset was observed. The sheet and native capture cover visual inspection separately; complete target stage/boss playthroughs and physical ESP32-C6 validation remain outstanding.

The [gamefield gallery](screenshots.md) includes the enemy-wave, firing, pause and resume frames from this run.


## Heroic soundtrack smoke test (2026-09-03)

The 26812-byte revision was staged with the framework uploader into the existing ESP32-C3 host. The loader confirmed 25604 code bytes, 29420 executable-memory bytes and 1108 audio bytes. The host ELF still resolved `g_fb` to 0x3fc9fe7c before capture. Gameplay, pause and resume framebuffers were inspected. The approximately 27.76-second mono capture peaked at 13936 without full-scale clipping; the last second of pause was silent and resumed output was nonzero. No guest panic was observed. Evidence is in `build/qemu-heroic/`. This host remains mono; stereo previews are separate software-model output, and ESP32-C6 listening is still pending.


## Opening-theme refinement smoke test (2026-09-03)

The current 28036-byte cartridge was staged through the framework uploader. The loader confirmed 25652 code bytes, 29468 executable-memory bytes and 2284 audio bytes. `g_fb` was resolved again from the unchanged host ELF before capture. The mono run collected 1225098 PCM bytes, peak 10685 and no full-scale clipping. Pause silence and resumed audio were measured. Evidence is under `build/qemu-opening/`; software stereo previews remain separate from target hardware validation.
