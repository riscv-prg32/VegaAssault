# AGENTS.md

## Mission
This repository is simultaneously a playable PRG32 cartridge and an undergraduate teaching artifact for C, computer architecture, embedded systems, graphics, audio, reproducible builds, and software verification. Every change must preserve both goals.

## Non-negotiable invariants
- Be sure to fit the cartridge requirements.
- Preserve `grendizer_c_init`, `grendizer_c_update`, and `grendizer_c_draw`.
- The final `.prg32` package MUST be <= 131072 bytes; never weaken the size gate.
- Use only the portable PRG32 cartridge ABI from game code; do not call ESP-IDF internals.
- Keep gameplay deterministic, fixed-memory, freestanding, and allocation-free.
- Preserve standard D-pad/A/B/START input and stereo-aware audio with mono-safe behavior.
- Treat QEMU (ESP32-C3 host firmware) and physical ESP32-C6 as separate host targets even when a portable cartridge can run on both.

## Educational-source rule
- Hand-authored C must be explained line by line. Every nonblank C source line must have an immediately adjacent `// EDUCATIONAL:` comment explaining purpose, data representation, control flow, ABI significance, performance implication, or hardware relationship.
- Generated sprite source must also remain line-commented, but comments may identify generated RGB565 data and point students to `tools/generate_assets.py`.
- New functions require a conceptual comment describing preconditions, state changed, side effects, complexity, and relevant PRG32 API calls.
- Prefer explicit code over clever compression. A student should be able to trace one frame from input to update to draw.

## Documentation rule
Any architectural or behavioral change must update README.md and the relevant files under `docs/`. Documentation must distinguish observed PRG32 behavior from project-specific design decisions and must be suitable for citation in laboratory reports.

## Intellectual property
- Don't infringe copyright material
- The game is fan made with no business target, just a tribute.

Do not add ripped animation frames, manga scans, commercial sprites, logos, recordings, MIDI transcriptions, or note-for-note copyrighted music. New audiovisual material must be original unless maintainers document rights. The project remains an unofficial fan/educational work.

## Asset workflow
Edit `tools/generate_assets.py` for sprite changes, regenerate `src/sprites.c`, `src/sprites.h`, and `assets/sprite-sheet.png`, then restore/verify educational comments. Repack `build/audio.block` after audio changes.

## Validation before merge/release
1. compile C with C99, `-Wall -Wextra -Werror`;
2. run the educational-comment checker;
3. build through current PRG32 tooling;
4. verify package <= 131072 bytes;
5. test QEMU and ESP32-C6;
6. test attract/title, controls, weapons, transformations, stages, bosses, pause, game over and victory;
7. verify stereo and mono-safe audio;
8. generate and inspect the CartridgeStore bundle and manifest.
