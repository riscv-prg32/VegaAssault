# Game Design — Vega Assault

## Design target

A compact PRG32 game that feels like an early/mid-1980s Japanese arcade shooter: immediate controls, short attack loops, readable sprites, hard projectile patterns, short inter-stage cinematics, a persistent score chase and an attract sequence.

## Player systems

### Screw Crusher
Fast, low-damage straight projectile. Six-frame reload. Intended as the default attack and score-combo builder.

### Double Harken
Two blades launch diagonally outward and then reverse their horizontal component late in flight. Each hit causes double damage. The longer cooldown forces deliberate use.

### Space Thunder
A two-column vertical energy strike. Four damage per hit, 44-frame cooldown and three-energy cost. Energy is gained from kills to prevent unlimited boss melting.

### Survival
Three lives. A hit grants temporary invulnerability and breaks the combo. D-pad movement is constrained to the lower combat area to preserve classic shooter composition.

## Enemies

- Saucer: fast horizontal patrol and simple downward fire.
- Blade robot: lateral movement with periodic aimed-lane fire.
- Crawler robot: slow descent with horizontal bursts and angled shot.
- Horn beast: stepped descent with paired crossing projectiles.
- Gunner robot: rapid lateral movement with twin diagonal shots.

Enemy visuals are original fan-game designs, not traced or extracted production art.

## Boss patterns

### Stage 1 — Vega Fortress
Sweeping horizontal movement. Central cannon every attack cycle, side shots on alternating cycles.

### Stage 2 — Sea Destroyer
Moves horizontally and periodically changes altitude. Fires a central diagonal shot plus symmetric side shots.

### Stage 3 — Star Commander
Alternating fan/crossfire pattern using phase-dependent horizontal velocity and additional outer cannons every third attack cycle.

## Stage presentation

Stage 1 uses primitive-drawn mountain/earth terrain. Stage 2 uses sea/coast bands and scrolling wave marks. Stage 3 is starfield-only. This preserves visual variety without spending tens of kilobytes on background bitmaps.

Stage 2 and 3 begin with a four-frame Grendizer/Spazer docking sequence. After docking, the player sprite uses the 32×24 Spazer configuration.

## Attract sequence

The idle loop intentionally mirrors coin-op presentation: title + INSERT COIN, instructions, enemy file, then repeat. Player input jumps to the title; title inactivity returns to attract mode.
