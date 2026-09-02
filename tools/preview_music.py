#!/usr/bin/env python3
"""Render actual C sequencer events through a software model of the PRG32 mixer.

Produces stereo and mono WAV previews at 22050 Hz / 33 ms per game tick.
This is a listening aid, not a hardware capture. SFX are excluded. The model
uses the inspected host's nearest-neighbor pitch, squared voice volume,
half-headroom master gain and nonnegative stereo pan gains.
"""
import array
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import wave

ROOT = Path(__file__).resolve().parents[1]
RATE = 22050
RATIOS = [1024, 1085, 1149, 1218, 1290, 1367, 1448, 1534, 1625, 1722, 1825, 1933]


def events():
    """Capture three complete phrases from game.c using existing portable mocks."""
    source = (ROOT / 'tests/game_test.c').read_text()
    source = source.replace('../src/game.c', str(ROOT / 'src/game.c'))
    source = 'static int preview_tick;\n' + source
    source = source.replace('int main(void)', 'int regression_main(void)')
    source = source.replace('assert(c<3&&i<3',
                            'printf("%d %u %u %u %u\\n",preview_tick,c,i,n,v);assert(c<3&&i<3')
    source = source.replace('assert(c<3);',
                            'printf("%d %u 0 0 0\\n",preview_tick,c);assert(c<3);')
    source += '''
int main(void){
for(int s=1;s<=3;s++){
preview_tick=(s-1)*448;reset();stage=s;music_reset();
for(int t=0;t<448;t++){preview_tick=(s-1)*448+t;music_update();}
}
return 0;
}
'''
    # Generated harness lines retain the repository's educational annotation rule.
    source = '\n'.join('// EDUCATIONAL: Host preview instrumentation; records real sequencer calls.\n' + line
                       if line.strip() and not line.lstrip().startswith('//') else line
                       for line in source.splitlines())
    framework = Path(os.environ.get('PRG32_ROOT', ROOT.parent / 'PRG32'))
    with tempfile.TemporaryDirectory() as directory:
        cfile, executable = Path(directory) / 'preview.c', Path(directory) / 'preview'
        cfile.write_text(source)
        subprocess.run([os.environ.get('CC', 'cc'), '-std=c99', '-Wall', '-Wextra', '-Werror',
                        '-I', str(framework / 'components/prg32/include'),
                        '-I', str(framework / 'components/prg32_audio/include'),
                        str(cfile), '-o', str(executable)], check=True)
        return [list(map(int, line.split())) for line in
                subprocess.check_output([str(executable)], text=True).splitlines()]


def render(note_events, ticks):
    """Render captured note events for a specified number of 33 ms game ticks."""
    config = json.loads((ROOT / 'audio/audio.json').read_text())
    samples = [(ROOT / 'audio' / s['file']).read_bytes() for s in config['samples']]
    voices = [None] * 3
    schedule = {}
    for tick, channel, instrument, note, volume in note_events:
        schedule.setdefault(tick * RATE * 33 // 1000, []).append((channel, instrument, note, volume))
    stereo, mono = array.array('h'), array.array('h')
    for frame in range(ticks * RATE * 33 // 1000):
        for channel, instrument, note, volume in schedule.get(frame, []):
            descriptor = config['instruments'][instrument]
            sample_id = descriptor['sample_id']
            octave, semitone = divmod(note - config['samples'][sample_id]['base_note'], 12)
            step = RATIOS[semitone] << octave if octave >= 0 else RATIOS[semitone] >> -octave
            voices[channel] = [sample_id, volume, descriptor['default_pan'], 0, step] if note else None
        left = right = center = 0
        for voice in voices:
            if voice is None:
                continue
            sample_id, volume, pan, position, step = voice
            data = samples[sample_id]
            value = (data[(position >> 10) % len(data)] - 128) << 8
            value = (((value * volume) >> 8) * volume) >> 8
            value = (value * 255) >> 9
            center += value
            left += int(value * (255 if pan <= 0 else 255 - pan * 255 // 63) / 255)
            right += int(value * (255 if pan >= 0 else 255 - (-pan) * 255 // 64) / 255)
            loop_start = config['samples'][sample_id].get('loop_start', 0) << 10
            position += step
            while position >= len(data) << 10:
                position -= (len(data) << 10) - loop_start
            voice[3] = position
        assert max(abs(left), abs(right), abs(center)) < 32767, 'Music clips'
        stereo.extend([left, right])
        mono.append(center)
    return stereo, mono


def main():
    stereo, mono = render(events(), 1344)
    output = ROOT / 'build/music-preview'
    output.mkdir(parents=True, exist_ok=True)
    assert any(stereo[i] != stereo[i+1] for i in range(0, len(stereo), 2))
    for name, channels, pcm in [('stereo', 2, stereo), ('mono', 1, mono)]:
        print(name, 'peak', max(map(abs, pcm)))
        if sys.byteorder != 'little':
            pcm.byteswap()
        with wave.open(str(output / f'{name}.wav'), 'wb') as target:
            target.setparams((channels, 2, RATE, 0, 'NONE', 'not compressed'))
            target.writeframes(pcm.tobytes())


if __name__ == '__main__':
    main()
