# PRG32 Cartridge Size Budget

The final package must be **<= 131072 bytes**. `build.sh` retains this hard gate.

## Measured portable build (2026-09-02)

Using the sibling PRG32 builder and Espressif GCC 14.2.0:

| Resource | Bytes |
| --- | ---: |
| Sprite palette indices | 15040 |
| RGB565 palette | 28 |
| Fixed sprite decode buffer (BSS) | 2304 |
| Code and initialized data | 25556 |
| Total executable memory including BSS | 29372 |
| Packed AUD0 block | 864 |
| Final portable package including header | 26520 |

The inspected portable builder separately enforces **32768 bytes of executable RAM**. The initial direct-RGB565 build required 41692 bytes and failed that gate despite fitting the package budget. Palette indexing fixes this without increasing either limit. Remaining executable-memory headroom is 3396 bytes; package headroom is 104552 bytes. These are build measurements, not host performance or hardware execution results.

Sprite arrays contain 24 frames with their original dimensions and the refined 14-color palette. Terrain is procedural, audio uses tiny waveforms, and all game state and sprite scratch space are statically allocated. Future changes must satisfy both budgets; host object size and ZIP size do not substitute for final cartridge measurements.
