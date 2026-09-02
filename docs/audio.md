# Audio Architecture

The soundtrack and effects are original assets stored in a compact PRG32 `AUD0` block. Music is synthesized from short loopable waveforms and note events instead of long PCM recordings. This exchanges a small amount of sequencing logic for a large reduction in cartridge storage.

Lead and counter-line voices are panned to opposite stereo regions while bass remains near the center. Effects can derive pan from horizontal screen position. The same content remains usable on mono output because pan is an enhancement rather than a gameplay dependency.

The audio design is suitable for experiments in sampling, pitch, event scheduling, stereo imaging, and storage-rate trade-offs.

## Validated packing

`tools/pack_audio.py` now validates authoring data before serializing or writing output. It rejects empty samples, invalid loop ranges, nonexistent instrument sample references, out-of-range envelope bytes, and pan outside -64..63. Counts follow the inspected PRG32 host capacities: 64 samples, 32 instruments, 16 tracks. These are explicit project validation constraints based on the current framework, rather than an expansion of the portable ABI.

Tracker commands must match known names or numeric opcodes. Delta values and unsigned arguments must fit their byte fields; values such as 256 no longer wrap to zero, and unknown names no longer become END. SET_PAN takes a signed -64..63 value and encodes its two's-complement byte explicitly. Empty tracks are rejected. Existing audio assets repack to exactly the same 864 bytes, so this tooling change does not alter music or effects. Actual stereo quality and mono downmix remain listening-test obligations; the QEMU smoke test only verified mono PCM transport.
