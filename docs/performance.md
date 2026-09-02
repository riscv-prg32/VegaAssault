# Performance and Cartridge Budget

PRG32 classroom firmware provides cartridge slots up to 128 KiB (131072 bytes). The package contains executable code/data plus optional audio and metadata. The design therefore treats storage as a first-class engineering constraint. Small sprites, procedural terrain, short looped audio waveforms, integer state, and bounded entity arrays all serve this constraint.

Students should distinguish flash/package size from runtime memory. Multiple cartridge slots consume flash, whereas only the selected cartridge is loaded for execution. Build scripts must measure the final `.prg32`; source ZIP size is not a valid substitute.
