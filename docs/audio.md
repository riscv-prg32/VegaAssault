# Audio Architecture

The soundtrack and effects are original assets stored in a compact PRG32 `AUD0` block. Music is synthesized from short loopable waveforms and note events instead of long PCM recordings. This exchanges a small amount of sequencing logic for a large reduction in cartridge storage.

Lead and counter-line voices are panned to opposite stereo regions while bass remains near the center. Effects can derive pan from horizontal screen position. The same content remains usable on mono output because pan is an enhancement rather than a gameplay dependency.

The audio design is suitable for experiments in sampling, pitch, event scheduling, stereo imaging, and storage-rate trade-offs.

## Validated packing

`tools/pack_audio.py` now validates authoring data before serializing or writing output. It rejects empty samples, invalid loop ranges, nonexistent instrument sample references, out-of-range envelope bytes, and pan outside -64..63. Counts follow the inspected PRG32 host capacities: 64 samples, 32 instruments, 16 tracks. These are explicit project validation constraints based on the current framework, rather than an expansion of the portable ABI.

Tracker commands must match known names or numeric opcodes. Delta values and unsigned arguments must fit their byte fields; values such as 256 no longer wrap to zero, and unknown names no longer become END. SET_PAN takes a signed -64..63 value and encodes its two's-complement byte explicitly. Empty tracks are rejected. The block remains 864 bytes; this soundtrack revision changes instrument pan bytes while retaining the existing samples. Actual stereo quality and mono downmix remain listening-test obligations; the QEMU smoke test only verified mono PCM transport.


## Previous eight-bar arrangement (2026-09-02)

Project design: each stage now has a 32-note melody over 64 seven-tick subdivisions (eight bars of eight subdivisions). Lead notes last two subdivisions, with explicit rests; bass alternates root and fifth every four subdivisions. The arpeggio follows the same four chords as the bass, with stage-specific major/minor thirds. This replaces the sixteen-step loop that stopped every voice on each subdivision. No additional mixer voices, samples, dynamic allocation, or random state are needed.

Stage one uses C–Am–F–G; stage two Am–F–C–G; stage three Gm–Eb–F–D. These harmony labels describe project-authored patterns, not adaptations of an existing theme. Music pauses with simulation, resets on stage entry, and remains silent at game over. Victory retains the final-stage arrangement. Attract mode lowers all three parts.

The lead pan is -28, arpeggio +36, and bass 0 in the -64..63 range. The quieter, accented arpeggio leaves space for melody and positional effects. There is no phase inversion, delay-based widening, or stereo-only musical information.

Observed implementation in the sibling PRG32 `components/prg32_audio/audio_mixer.c`: ordinary note-on uses instrument default pan; explicit nonzero volumes override instrument defaults. The mixer implements immediate note-off and does not apply the serialized ADSR fields. Consequently sustain comes from event timing, not an assumed envelope. Its mono mixer sums voices without pan, while stereo applies nonnegative channel gains; hardware listening is still needed to assess balance and clicks. Pitch and playback duration of these existing samples also depend on host sample rate.


## First heroic arrangement (2026-09-03; superseded below)

Project design: three newly composed eight-bar melodies emphasize repeated rhythmic shapes, ascending fifths, answering phrases and cadential rests. Existing chord progressions remain. Lead downbeats are accented (124 versus 108 volume), with selected short articulations between held notes. The right-hand accompaniment plays six of eight subdivisions per bar, creating a galloping pulse; bass retains its held root/fifth pattern. The same three voices and seven-tick subdivision clock are used. At the inspected host's 33 ms frame interval this is approximately 130 beats/minute. Pause, stage reset, game-over silence and attract attenuation retain their existing behavior.

The new `tools/generate_music_samples.py` creates original finite harmonic sums: brass-like lead, lighter string-like accompaniment and rounded bass. It replaces only the three music cycles; all five SFX remain untouched. At 22050 Hz, the lead/accompaniment use 84 samples per cycle (262.5 Hz, approximately C4), and bass uses 169 (130.47 Hz, approximately C3). This corrects the previous 32-sample cycles' mismatch with their base-note metadata. Other host sample rates change tuning; the portable ABI does not supply sample-rate compensation here. Regenerate with `python3 tools/generate_music_samples.py`, then repack with `python3 tools/pack_audio.py audio/audio.json build/audio.block` or run `./build.sh`.

Lead pan is now -18, accompaniment +30 and bass 0. This keeps the melodic focus close to center while making the answering part distinct. No phase inversion, delay widening or additional SFX voice allocation is introduced. The audio block is 1108 bytes, an increase of 244 bytes.

`python3 tools/preview_music.py` compiles an instrumented host harness using the actual C sequencer and existing test mocks. It captures every note event for three complete phrases, then renders `build/music-preview/stereo.wav` and `mono.wav`. The software model follows the inspected host's integer pitch ratios, nearest-neighbor playback, squared volume, master volume 255, half-headroom gain and nonnegative stereo gains. These are synthesized previews at 22050 Hz and 33 ms/tick, without sound effects; they are not hardware recordings. The script checks distinct stereo channels and absence of clipping in both modes. Physical stereo/mono listening remains a separate check.


## Opening-theme refinement (2026-09-03, current)

The first-stage harmony is now Em–C–Am–B, with an original lower-register lead. Stage two retains Am–F–C–G and stage three Gm–Eb–F–D. All three lead phrases have been rewritten around repeated calls and longer answers. A shared sixteen-subdivision rhythm triggers eight events at offsets 0, 2, 3, 4, 7, 8, 12 and 14; other positions hold the last event. Zero pitches remain rests. Subdivision 6 adds a breath before the pickup at 7. These durations replace the uniform quarter-note lead and blanket offbeat cuts. Bass now adds an octave pickup on each bar's final subdivision (24 notes per phrase); accompaniment retains 48 notes. The three-voice limit, stereo pans, tempo, deterministic resets and silence behavior are unchanged.

Music samples now contain several cycles of changing amplitude followed by a single sustained cycle. This provides a brass attack and plucked accompaniment/bass despite the host ignoring ADSR. Lead is 504 bytes with loop start 420; accompaniment is 336 bytes with loop start 252; bass is 676 bytes with loop start 507. Their base periods and tuning remain unchanged. The JSON loop metadata must agree with these final-cycle offsets when regenerating assets. The complete audio block is 2284 bytes. The preview tool now follows loop-start offsets so the attack plays once per note rather than repeating through sustain.

The composition uses broad opening-theme conventions without importing a Grendizer melody, lyric, score or recording. This is an original stylistic homage, not a clearance determination for third-party rights.
