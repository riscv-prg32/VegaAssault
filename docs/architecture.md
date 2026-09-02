# Software Architecture

## Abstract
The cartridge is a freestanding RV32 C program hosted by PRG32. Its architecture follows a deterministic init-update-draw model. The resident PRG32 firmware owns display, input, audio, cartridge loading, and platform services; the cartridge owns only game state and invokes the stable PRG32 ABI. This separation is pedagogically useful because it exposes the boundary between application code and a runtime system without requiring students to manage device drivers.

## State model
Global state is deliberately explicit and statically allocated. Player shots, enemies, enemy shots, and stars are bounded arrays. This eliminates heap allocation, fragmentation, and nondeterministic allocation failure. The top-level finite-state machine separates attract, title, launch, stage introduction, transformation, play, boss, pause, game-over, and victory phases.

## Frame transaction
A frame can be understood as a transaction: input is sampled; timers and world entities advance; collisions transform state; audio events are scheduled; draw then projects the resulting state into the 320x200 game viewport. This organization supports tracing, debugging, WCET-style reasoning, and classroom instrumentation.

## Portability
The game intentionally avoids ESP-IDF calls. A portable `.prg32` cartridge is linked against the PRG32 ABI table and can therefore be staged into the QEMU host or uploaded to a compatible ESP32-C6 host. The host firmware differs; the cartridge contract remains stable.

## Frame boundaries and graphics storage

The update function handles pause before advancing clocks and returns immediately after terminal or stage transitions. This prevents later collision phases from overwriting the chosen state. New games reset the random generator and star field to make identical input sequences reproducible regardless of attract-mode duration.

Indexed sprite data is expanded into one static 2304-byte RGB565 buffer and passed synchronously to the portable sprite ABI. This is a cartridge-specific memory optimization; PRG32 still receives standard RGB565 pixels. The builder's executable RAM constraint is separate from package storage; measured values are in `SIZE_BUDGET.md`.

Campaign expansion adds one launch state and increases the stage count to five without increasing entity-pool capacity. Stage entry selects a support form from four immutable generated arrays. Launch/base backgrounds use portable rectangle calls; the Moon base boss shares the existing boss collision envelope and fixed state. The music index wraps over three phrases to avoid out-of-bounds access for stages four and five.

QEMU testing exposed a compiler-generated absolute pointer table for mission names. The portable host relocates code but cannot repair such embedded link-time addresses. Mission text therefore uses a fixed two-dimensional character array; support selection uses explicit volatile local comparisons to prevent GCC from converting it to a pointer table. The final RV32 object was inspected: no `R_RISCV_32` absolute pointer relocations remain. Relative jump-table differences (`R_RISCV_ADD32`/`R_RISCV_SUB32`) remain valid after relocation. Native host tests alone did not detect this target-specific failure.

`ST_COSMO` and `ST_ENDING` are timed noncombat states with early update returns. The existing `spazer_mode` field is now a boolean selected by explicit input in all stages, adding no persistent fields. NPC support reuses the player projectile pool after player input. Cinematics reuse existing sprite frames and rectangle primitives; Special Cosmo is a vehicle-only combined sprite, separate from Grendizer's standalone and combined flight forms.

Transformation chords are resolved before movement and attacks. They are idempotent state selection, so held chords need no extra edge-tracking state. Detached support rendering and cadence derive from the existing player position/frame clock, share the fixed projectile pool, and freeze with pause.
