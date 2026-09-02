# Audio Architecture

The soundtrack and effects are original assets stored in a compact PRG32 `AUD0` block. Music is synthesized from short loopable waveforms and note events instead of long PCM recordings. This exchanges a small amount of sequencing logic for a large reduction in cartridge storage.

Lead and counter-line voices are panned to opposite stereo regions while bass remains near the center. Effects can derive pan from horizontal screen position. The same content remains usable on mono output because pan is an enhancement rather than a gameplay dependency.

The audio design is suitable for experiments in sampling, pitch, event scheduling, stereo imaging, and storage-rate trade-offs.
