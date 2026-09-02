# Game and State-Machine Design

The game adopts the grammar of mid-1980s fixed-screen and vertically staged shooters: a short attract loop teaches controls, play is divided into escalating waves, and bosses punctuate stage boundaries. Grendizer/Spazer transformations are implemented as state transitions rather than as a separate simulation subsystem, conserving both code and input complexity.

Weapons deliberately expose different computational and gameplay costs. Screw Crusher is a frequent single projectile; Double Harken allocates two projectile slots and reverses lateral motion; Space Thunder consumes energy and creates high-damage projectiles. Enemy families vary movement and firing cadence, while boss patterns use stage-specific deterministic schedules.

The design favors readable integer rules over continuous physics. This makes cause and effect visible to beginning C students and allows frame-by-frame reasoning without floating-point dependencies.

## Corrected frame semantics

These are project rules, not behavior imposed by PRG32: pause freezes frame and state timers, stars/RNG, cooldowns, invulnerability, terrain, and the music sequence. Entering pause releases music voices; resume preserves the scene and timer and advances again on the following tick. Existing one-shot SFX may finish in the host mixer. A paused boss is still rendered.

A boss-defeating player shot ends the current combat update before hostile collisions, giving stage completion priority over damage in that same tick. Fatal damage likewise stops enemy processing before wave/boss transitions. Harken reverses laterally once at its life threshold. Crawlers bounce at horizontal boundaries; bosses stay between y=24 and y=80. Enemy cadences of 80, 96 and 48 ticks use remainder tests, since bit masks do not implement these periods. Each fresh game resets animation phase, RNG and stars, and action buttons detect rising edges independently of other held buttons.
