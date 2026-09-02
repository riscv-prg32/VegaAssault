# Testing and Verification

Verification is layered. Static compilation with `-Wall -Wextra -Werror` catches many C defects. A source-comment check enforces the educational annotation policy. Package-size verification prevents an artifact that cannot fit a cartridge slot. Behavioral testing covers every finite-state transition and weapon. Cross-target testing compares QEMU and ESP32-C6 behavior.

A release test matrix should record: attract loop; title start; all D-pad directions; A/B/A+B attacks; energy consumption; pause/resume; each transformation; each enemy family; each boss; loss of lives; game-over recovery; victory recovery; audio channels; and Store metadata. Failures should include reproduction steps and target information.

## Automated host checks

Run `sh tools/test_game.sh` with a sanitizer-capable C compiler and a sibling PRG32 checkout, or set `PRG32_ROOT`. The harness includes the cartridge translation unit, supplies portable-ABI mocks, and asserts observable state transitions. It compiles with C99, `-Wall -Wextra -Werror`, AddressSanitizer and UndefinedBehaviorSanitizer. The educational checker covers C and headers under both `src/` and `tests/`.

Regression cases cover attract/title input, stage introduction and transformation, movement limits, all weapons, every boss transition, game-over/victory recovery, paused timers and boss rendering, simultaneous boss death/player danger, last-life enemy escape, Harken reversal, crawler boundaries, non-power-of-two firing cadence, bounded boss motion, and deterministic restart.

## Observed local results (2026-09-02)

- Host regression suite and educational checker passed.
- Portable RV32 build passed: 26192-byte package; 29044-byte code/data/BSS requirement.
- All 24 palette-decoded sprite arrays and preview pixels matched the original RGB565 artwork.
- Packed audio matched the current upstream audio packer's output byte for byte.
- Generated Store ZIP passed CRC checks; its manifest and cartridge parsed with current PRG32 tooling and the bundled cartridge matched the build output.

No QEMU binary was found on PATH or in the local Espressif tools, and no ESP32 serial device was visible. The available sibling host configuration targets ESP32-C6, not the separate ESP32-C3 QEMU host. QEMU execution, physical-board playthroughs, frame timing, stereo listening and mono downmix checks were not performed. ABI mocks and audio-byte comparisons cannot establish those results; the release matrix above remains required before release.
