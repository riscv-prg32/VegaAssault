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


## Soundtrack revision validation (2026-09-02)

The subsequent soundtrack revision changes the cartridge and pan descriptors; earlier byte-identical claims above describe the earlier packer-only change. The new portable package is 26520 bytes (104552 bytes below the unchanged 131072-byte limit), with 29372 bytes of executable memory and an 864-byte audio block. C99 warning checks, educational comments, sanitizer gameplay regressions and Python packer/Store tests are rerun for this revision.

Soundtrack regressions cover all three complete phrases, deterministic replay, distinct accompaniment rhythms, held lead and bass, explicit rests, reduced attract volumes and game-over silence. Existing pause tests check frozen sequencing. These are event-level checks, not a listening test. QEMU observations above refer to the previous cartridge. The revised cartridge subsequently passed the [updated soundtrack QEMU smoke test](qemu.md#updated-soundtrack-smoke-test-2026-09-02), including mono PCM capture, firing, pause silence and resumed output. Stereo listening, complete target playthroughs and physical ESP32-C6 remain unverified.

## Sprite refinement validation (2026-09-02)

The refined 24-frame artwork passed the educational-comment checker, C99 warnings-as-errors build, sanitizer gameplay regressions, native renderer capture and all ten Python tests. Two consecutive generator runs produced identical C, headers, sprite-sheet, icon and splash bytes. Frame envelopes, transparent pixels, distinct two-frame enemy animations and equality of the final transformation/flight frame were checked. The regenerated header is unchanged because exported names, dimensions and palette length are unchanged.

The portable package remains 26520 bytes, with 29372 bytes of executable memory; the 15040-byte index data and 28-byte palette do not grow. Store ZIP CRC, manifest parsing, cartridge bytes and refreshed artwork were verified. The native gameplay capture and all sprite/Store previews were visually inspected. These checks do not establish physical ESP32-C6 rendering, full target playthrough coverage, stereo quality or frame timing.

The subsequent [sprite QEMU smoke test](qemu.md#sprite-refinement-smoke-test-2026-09-02) also passed, with captured title, gameplay, firing, pause and resume screens. The README gameplay image was refreshed from that actual ESP32-C3 framebuffer.


## Heroic soundtrack validation (2026-09-03)

The C99 warnings-as-errors build, educational checker, sanitizer gameplay tests and all ten Python audio/Store tests passed. The phrase checks now expect 48 accompaniment notes and explicitly verify its opening rest alongside held bass. Repeated sample generation preserved bytes. The cartridge is 26812 bytes, executable memory 29420 bytes, and audio block 1108 bytes. Store ZIP CRC and embedded cartridge equality passed.

The software preview captured three full phrases from the C sequencer: stereo peak 6418 and mono peak 7061 at master volume 255, with distinct channels and no full-scale clipping. This does not test target audio hardware. The fresh ESP32-C3 QEMU run loaded 25604 code bytes, 29420 memory bytes and 1108 audio bytes. Its mono capture contained 1224216 bytes (about 27.76 seconds), peak 13936, and no full-scale clipping. The last second of pause was silent; the last second after resume had peak 6525. Framebuffer inspection confirmed gameplay, pause and resumed play. See `build/qemu-heroic/` for local logs, WAV, marks and framebuffer evidence. Hardware stereo listening, full target stage/boss playthroughs and physical ESP32-C6 remain unverified.


## Opening-theme refinement validation (2026-09-03, current)

C99 warnings-as-errors, educational comments, sanitizer gameplay regressions and all ten Python tests passed. Updated music tests cover repeated-note articulation, holds, breath/pickup events, 24 bass notes, phrase wrap and deterministic replay for all three stages. Sample regeneration preserved bytes. Store ZIP CRC, manifest JSON and embedded cartridge equality passed. Package size is 28036 bytes; executable memory is 29468 bytes and audio is 2284 bytes.

The updated software preview respects attack/sustain loop offsets and covers all three complete phrases. Stereo peak was 5696, mono peak 5987, with distinct stereo channels and no full-scale clipping. A fresh ESP32-C3 QEMU run loaded this revision and captured 1225098 mono PCM bytes, peak 10685, without full-scale clipping. The final paused second was silent; the final resumed second peaked at 5945. Local evidence is in `build/qemu-opening/`. Hardware stereo listening, full target playthroughs and ESP32-C6 remain unverified.
