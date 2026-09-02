#!/usr/bin/env python3
"""Generate original unsigned PCM8 instrumental cycles; no external audio input.

At the project's 22050 Hz host rate, 84 samples approximate C4 and 169
samples approximate C3. The host pitches these cycles from their base notes.
Finite harmonic sums provide instrumental colors without runtime synthesis.
Several attack cycles precede a sustained cycle; audio.json loops only the last
cycle, at byte offsets 420 (lead), 252 (arp), and 507 (bass).
"""
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def cycle(length, harmonics, amplitude):
    """Return a zero-centered periodic harmonic sum encoded around PCM8 128."""
    values = [sum(gain * math.sin(2 * math.pi * harmonic * n / length)
                  for harmonic, gain in enumerate(harmonics, 1))
              for n in range(length)]
    peak = max(abs(value) for value in values)
    return bytes(round(128 + amplitude * value / peak) for value in values)


def main():
    # Brass-like upper harmonics, lighter string-like answer, round centered bass.
    voices = [('lead', 84, [1, .72, .46, .28, .16, .08], 112),
              ('arp', 84, [1, .30, .18, .08], 96),
              ('bass', 169, [1, .20, .07], 112)]
    for name, length, harmonics, amplitude in voices:
        # Bake an attack into PCM because the current host ignores ADSR fields.
        envelope = {'lead': [.20, .65, 1, .88, .76, .70],
                    'arp': [1, .72, .48, .38],
                    'bass': [1, .72, .50, .42]}[name]
        raw = b''.join(cycle(length, harmonics, round(amplitude * gain))
                       for gain in envelope)
        (ROOT / 'audio' / f'{name}.raw').write_bytes(raw)


if __name__ == '__main__':
    main()
