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
