#!/usr/bin/env python3
"""Pack validated original samples and tracker events into a PRG32 AUD0 block."""
import argparse
import json
from pathlib import Path
import struct

H = struct.Struct('<4sHHHHHHIIIIII')
S = struct.Struct('<IIIIHBB')
I = struct.Struct('<HBBBBBB')
T = struct.Struct('<II')
E = struct.Struct('<BBBB')
COMMANDS = {'NOTE_ON': 1, 'NOTE_OFF': 2, 'SET_VOLUME': 3, 'SET_PAN': 4,
            'SET_TEMPO': 5, 'PLAY_SAMPLE': 6, 'JUMP': 7, 'END': 255}


def integer(value, low, high, field):
    if type(value) is not int or not low <= value <= high:
        raise ValueError(f'{field}: expected integer in {low}..{high}, got {value!r}')
    return value


def aligned_extend(block, payload):
    offset = len(block)
    block.extend(payload)
    block.extend(bytes((-len(block)) % 4))
    return offset


def pack_audio(config, base_dir):
    samples = config.get('samples', [])
    instruments = config.get('instruments', [])
    tracks = config.get('tracks', [])
    for name, items, capacity in [('samples', samples, 64), ('instruments', instruments, 32), ('tracks', tracks, 16)]:
        if not isinstance(items, list) or len(items) > capacity:
            raise ValueError(f'{name}: expected a list with at most {capacity} entries')
    data, sample_descs, instrument_descs, track_descs, events = (bytearray() for _ in range(5))
    for index, sample in enumerate(samples):
        raw = (base_dir / sample['file']).read_bytes()
        if not raw:
            raise ValueError(f'samples[{index}]: sample must not be empty')
        flags = integer(sample.get('flags', 0), 0, 1, 'sample flags')
        if 'loop' in sample and type(sample['loop']) is not bool:
            raise ValueError('sample loop: expected boolean')
        if sample.get('loop', False):
            flags |= 1
        start = integer(sample.get('loop_start', 0), 0, len(raw), 'loop_start')
        end = integer(sample.get('loop_end', len(raw) if flags else 0), 0, len(raw), 'loop_end')
        if flags and start >= end:
            raise ValueError(f'samples[{index}]: loop_start must be smaller than loop_end')
        note = integer(sample.get('base_note', 60), 1, 127, 'base_note')
        sample_descs.extend(S.pack(len(data), len(raw), start, end, note, flags, 0))
        data.extend(raw)
    for instrument in instruments:
        sample_id = integer(instrument.get('sample_id', 0), 0, len(samples)-1, 'sample_id')
        volume = integer(instrument.get('default_volume', 255), 0, 255, 'default_volume')
        pan = integer(instrument.get('default_pan', 0), -64, 63, 'default_pan')
        envelope = [integer(instrument.get(key, default), 0, 255, key)
                    for key, default in [('attack', 0), ('decay', 0), ('sustain', 255), ('release', 0)]]
        instrument_descs.extend(I.pack(sample_id, volume, pan & 255, *envelope))
    for track in tracks:
        sequence = track.get('events', [])
        if not isinstance(sequence, list) or not sequence:
            raise ValueError('track events: expected a nonempty list')
        track_descs.extend(T.pack(len(events)//E.size, len(sequence)))
        for event in sequence:
            command = event.get('command', 'END')
            if isinstance(command, str):
                if command.upper() not in COMMANDS:
                    raise ValueError(f'unknown audio command: {command}')
                command = COMMANDS[command.upper()]
            if type(command) is not int or command not in COMMANDS.values():
                raise ValueError(f'unknown audio command: {command!r}')
            delta = integer(event.get('delta', event.get('delta_ticks', 0)), 0, 255, 'event delta')
            arg0 = integer(event.get('arg0', 0), 0, 255, 'event arg0')
            arg1 = integer(event.get('arg1', 0), -64 if command == 4 else 0,
                           63 if command == 4 else 255, 'event arg1')
            events.extend(E.pack(delta, command, arg0, arg1 & 255))
    block = bytearray(H.size)
    offsets = [aligned_extend(block, payload) for payload in
               [sample_descs, instrument_descs, track_descs, events, data]]
    block[:H.size] = H.pack(b'AUD0', 1, H.size, len(samples), len(instruments), len(tracks),
                           0, *offsets, len(block))
    return bytes(block)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    try:
        config = json.loads(args.input.read_text())
        if not isinstance(config, dict):
            raise ValueError('audio configuration must be a JSON object')
        block = pack_audio(config, args.input.parent)
        if args.output.resolve() == args.input.resolve():
            raise ValueError('output must not overwrite input configuration')
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(block)
    except (OSError, ValueError, KeyError, TypeError, AttributeError, struct.error) as exc:
        parser.error(str(exc))
    print(len(block))


if __name__ == '__main__':
    main()
