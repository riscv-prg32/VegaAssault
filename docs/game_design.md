# Game and State-Machine Design

The game adopts the grammar of mid-1980s fixed-screen and vertically staged shooters: a short attract loop teaches controls, play is divided into escalating waves, and bosses punctuate stage boundaries. Grendizer/Spazer transformations are implemented as state transitions rather than as a separate simulation subsystem, conserving both code and input complexity.

Weapons deliberately expose different computational and gameplay costs. Screw Crusher is a frequent single projectile; Double Harken allocates two projectile slots and reverses lateral motion; Space Thunder consumes energy and creates high-damage projectiles. Enemy families vary movement and firing cadence, while boss patterns use stage-specific deterministic schedules.

The design favors readable integer rules over continuous physics. This makes cause and effect visible to beginning C students and allows frame-by-frame reasoning without floating-point dependencies.

## Corrected frame semantics

These are project rules, not behavior imposed by PRG32: pause freezes frame and state timers, stars/RNG, cooldowns, invulnerability, terrain, and the music sequence. Entering pause releases music voices; resume preserves the scene and timer and advances again on the following tick. Existing one-shot SFX may finish in the host mixer. A paused boss is still rendered.

A boss-defeating player shot ends the current combat update before hostile collisions, giving stage completion priority over damage in that same tick. Fatal damage likewise stops enemy processing before wave/boss transitions. Harken reverses laterally once at its life threshold. Crawlers bounce at horizontal boundaries; bosses stay between y=24 and y=80. Enemy cadences of 80, 96 and 48 ticks use remainder tests, since bit masks do not implement these periods. Each fresh game resets animation phase, RNG and stars, and action buttons detect rising edges independently of other held buttons.

## Fixed-pool attacks and complete enemy roster

An attack is an all-or-nothing project transaction: Screw Crusher requires one free projectile slot; Harken and Thunder each require two. If capacity is insufficient, no shot is added, no energy is spent, no cooldown starts and no firing sound is emitted. The fixed pool remains allocation-free and the single-threaded update prevents capacity from changing between the check and insertion.

Each wave now includes the saucer, blade, crawler, beast and gunner. Previously the wave generator replaced every gunner with a saucer, making its art and instruction entry unreachable. The gunner descends one pixel every eight phase ticks and fires every 64 phase ticks, with horizontal velocity directed toward the player at firing time. Blade enemies now clamp and turn inward at x=8 and x=296. These movement and cadence rules are project-specific; their difficulty still needs target playtesting.

## Five-mission campaign (2026-09-03)

These timings and settings are project choices, not PRG32 requirements. A fresh run enters `ST_LAUNCH` for 180 updates: the mountain waterfall hangar opens for 45 ticks and Grendizer climbs away during the remaining ticks. No combat input or collisions run during launch. The existing attract/title controls precede it; restarting plays it again.

| Stage | Mission | Vehicle / setting |
| --- | --- | --- |
| 1 | Earth Defense | Grendizer / green foothills |
| 2 | Double Spazer | Red swept wings / cloud banks |
| 3 | Marine Spazer | Blue fins / underwater bubbles |
| 4 | Drill Spazer | Twin gold augers / rocky tunnel |
| 5 | Vega Moon Base | Manually selected player forms and three NPC escorts / lunar terrain and distant Earth |

Every stage introduction lasts 76 updates. Stages 2–4 then spend 121 updates docking before combat; individual craft receive an approaching Grendizer sprite. Stage entry resets player position and terrain scroll. Lives, score, energy and weapon rules carry through. Each mission uses the same bounded waves and its own stage-scaled boss health/cadence. The Moon base is stationary with a 48×24 collision envelope and three turret towers. Destruction starts the animated epilogue; victory follows after it completes.

The three original music phrases repeat using `(stage-1)%3`; melody and harmony use the same index. No new audio samples or voices are needed. Supporting forms share the existing 16×19 player damage box and standard controls, keeping collision difficulty independent of decorative wings. No claim is made that the combined design or mission sequence reproduces an episode; both are original game arrangements.

## Special Cosmo and lunar support

After the Drill boss, `ST_COSMO` runs for 240 ticks. For 120 ticks Double, Marine and Drill converge from three directions; for 120 more their combined Special Cosmo flies toward the Moon with animated exhaust. The Moon introduction then begins directly before combat. This cinematic and `ST_ENDING` ignore combat input and cannot be skipped by held action buttons.

All stages start in combined form. DOWN+A selects Robot; UP+A selects combined form. Chords consume the input tick without firing or moving. Repeated held chords are idempotent and play no repeated docking sound. A+B retains Thunder priority even with a vertical direction; simultaneous UP and DOWN do not transform. Pause ignores form input. Form changes preserve energy, cooldowns, lives and the common damage box, and already-fired shots retain their original damage.

| Property | Combined | Robot |
| --- | ---: | ---: |
| Horizontal pixels per tick | 4 | 2 |
| Vertical pixels per tick | 3 | 1 |
| Screw damage | 1 | 2 |
| Harken damage per projectile | 2 | 4 |
| Thunder damage per projectile | 4 | 8 |

When detached, the appropriate Spazer follows the player's horizontal position, bobs above the player and fires one damage-1 shot every 45 active ticks without energy cost. It is invulnerable, uses the existing shot pool and disappears immediately on recombination. Earth and Moon use the standalone Spazer; sky/water/tunnel use Double/Marine/Drill respectively. The Moon's three existing independent escorts continue firing staggered shots every 30 combat ticks. NPC rendering is independent of player damage blinking; full pools omit NPC shots safely. There is no automatic form cycling.

Destroying the Moon boss enters `ST_ENDING` for 240 ticks. Pulsing explosions surround the ruins while Grendizer and all three escorts fly toward Earth; only then does `ST_WIN` accept a new game action. These are original project narrative and gameplay arrangements.
