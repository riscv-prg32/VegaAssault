# Audio Architecture

The soundtrack and effects are original assets stored in a compact PRG32 `AUD0` block. Music is synthesized from short loopable waveforms and note events instead of long PCM recordings. This exchanges a small amount of sequencing logic for a large reduction in cartridge storage.

Lead and counter-line voices are panned to opposite stereo regions while bass remains near the center. Effects can derive pan from horizontal screen position. The same content remains usable on mono output because pan is an enhancement rather than a gameplay dependency.

The audio design is suitable for experiments in sampling, pitch, event scheduling, stereo imaging, and storage-rate trade-offs.

## Validated packing

`tools/pack_audio.py` now validates authoring data before serializing or writing output. It rejects empty samples, invalid loop ranges, nonexistent instrument sample references, out-of-range envelope bytes, and pan outside -64..63. Counts follow the inspected PRG32 host capacities: 64 samples, 32 instruments, 16 tracks. These are explicit project validation constraints based on the current framework, rather than an expansion of the portable ABI.

Tracker commands must match known names or numeric opcodes. Delta values and unsigned arguments must fit their byte fields; values such as 256 no longer wrap to zero, and unknown names no longer become END. SET_PAN takes a signed -64..63 value and encodes its two's-complement byte explicitly. Empty tracks are rejected. The block remains 864 bytes; this soundtrack revision changes instrument pan bytes while retaining the existing samples. Actual stereo quality and mono downmix remain listening-test obligations; the QEMU smoke test only verified mono PCM transport.


## Original eight-bar arrangement

Project design: each stage now has a 32-note melody over 64 seven-tick subdivisions (eight bars of eight subdivisions). Lead notes last two subdivisions, with explicit rests; bass alternates root and fifth every four subdivisions. The arpeggio follows the same four chords as the bass, with stage-specific major/minor thirds. This replaces the sixteen-step loop that stopped every voice on each subdivision. No additional mixer voices, samples, dynamic allocation, or random state are needed.

Stage one uses C–Am–F–G; stage two Am–F–C–G; stage three Gm–Eb–F–D. These harmony labels describe project-authored patterns, not adaptations of an existing theme. Music pauses with simulation, resets on stage entry, and remains silent at game over. Victory retains the final-stage arrangement. Attract mode lowers all three parts.

The lead pan is -28, arpeggio +36, and bass 0 in the -64..63 range. The quieter, accented arpeggio leaves space for melody and positional effects. There is no phase inversion, delay-based widening, or stereo-only musical information.

Observed implementation in the sibling PRG32 `components/prg32_audio/audio_mixer.c`: ordinary note-on uses instrument default pan; explicit nonzero volumes override instrument defaults. The mixer implements immediate note-off and does not apply the serialized ADSR fields. Consequently sustain comes from event timing, not an assumed envelope. Its mono mixer sums voices without pan, while stereo applies nonnegative channel gains; hardware listening is still needed to assess balance and clicks. Pitch and playback duration of these existing samples also depend on host sample rate.
