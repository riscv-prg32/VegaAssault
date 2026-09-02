# Testing and Verification

Verification is layered. Static compilation with `-Wall -Wextra -Werror` catches many C defects. A source-comment check enforces the educational annotation policy. Package-size verification prevents an artifact that cannot fit a cartridge slot. Behavioral testing covers every finite-state transition and weapon. Cross-target testing compares QEMU and ESP32-C6 behavior.

A release test matrix should record: attract loop; title start; all D-pad directions; A/B/A+B attacks; energy consumption; pause/resume; each transformation; each enemy family; each boss; loss of lives; game-over recovery; victory recovery; audio channels; and Store metadata. Failures should include reproduction steps and target information.

## Automated host checks

Run `sh tools/test_game.sh` with a sanitizer-capable C compiler and a sibling PRG32 checkout, or set `PRG32_ROOT`. The harness includes the cartridge translation unit, supplies portable-ABI mocks, and asserts observable state transitions. It compiles with C99, `-Wall -Wextra -Werror`, AddressSanitizer and UndefinedBehaviorSanitizer. The educational checker covers C and headers under both `src/` and `tests/`.

Regression cases cover attract/title input, stage introduction and transformation, movement limits, all weapons, every boss transition, game-over/victory recovery, paused timers and boss rendering, simultaneous boss death/player danger, last-life enemy escape, Harken reversal, crawler boundaries, non-power-of-two firing cadence, bounded boss motion, and deterministic restart.

## Observed validation results (2026-09-02)

- Host regression suite and educational checker passed.
- Portable RV32 build passed: 26368-byte package; 29220-byte code/data/BSS requirement.
- All 24 palette-decoded sprite arrays and preview pixels matched the original RGB565 artwork.
- Packed audio matched the current upstream audio packer's output byte for byte.
- Generated Store ZIP passed CRC checks; its manifest and cartridge parsed with current PRG32 tooling and the bundled cartridge matched the build output.

Validation used ESP-IDF 5.4 and the compatible Espressif QEMU build. The ESP32-C3 host builds and boots; framebuffer captures verify attract/title, first-stage gameplay, firing, and pause. See [QEMU results](qemu.md#observed-validation) for framework revision, capture provenance, and audio transport observations. Physical-board playthroughs, stereo listening, full target gameplay coverage and frame timing remain unverified.

## Second review regression checks

The added full-pool weapon test failed on the previous implementation: attacks still consumed energy/cooldowns without adding shots. It now passes under sanitizers, together with tests for a single free slot, two nonadjacent free slots, no SFX on rejection, all five enemy families, gunner firing, and blade boundary recovery.

After building, run `python3 -m unittest discover -s tests -p 'test_*.py'`. Five integration tests cover default/explicit versions, both target labels, cartridge byte preservation, deterministic ZIP bytes after changing source timestamps, checksum verification, rejection of malformed/truncated/oversized cartridges, preservation of existing bundles on validation failure, and protection against overwriting input cartridges. They use a temporary directory and the real built cartridge. Passing both target-label checks does not imply either host was executed.

## Audio input validation

The Python suite now includes five audio-packer tests, for ten Python test methods overall. Tests verify byte-for-byte preservation of the existing 864-byte AUD0 block, rejection of bad loops/references/pan values, unknown commands, overflowing or noninteger timing/arguments, signed pan encoding, and preservation of previous output after invalid input. The valid cartridge and Store ZIP remain byte-identical to the previous build because runtime audio and C code did not change.
