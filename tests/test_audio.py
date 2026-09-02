"""AUD0 regression tests: preserve valid bytes and reject silent data corruption."""
import copy
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('pack_audio', ROOT / 'tools/pack_audio.py')
audio = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audio)


class AudioTests(unittest.TestCase):
    def setUp(self):
        self.config = json.loads((ROOT / 'audio/audio.json').read_text())
        self.base = ROOT / 'audio'

    def test_valid_block_matches_existing_asset(self):
        self.assertEqual(audio.pack_audio(self.config, self.base), (ROOT / 'build/audio.block').read_bytes())

    def test_invalid_instruments_and_samples(self):
        for section, field, value in [('samples', 'loop_end', 99999), ('samples', 'loop_start', 99999),
                                      ('samples', 'base_note', 128), ('samples', 'loop', 'false'),
                                      ('instruments', 'sample_id', 8), ('instruments', 'default_pan', 64),
                                      ('instruments', 'default_pan', -65), ('instruments', 'attack', -1)]:
            with self.subTest(field=field, value=value):
                config = copy.deepcopy(self.config)
                config[section][0][field] = value
                with self.assertRaises(ValueError):
                    audio.pack_audio(config, self.base)

    def test_reject_unknown_commands_and_wrapped_values(self):
        for event in [{'command': 'NOTE_OON'}, {'command': 99}, {'delta': 256},
                      {'delta': -1}, {'arg0': 256}, {'arg1': 256}, {'delta': 1.5}]:
            with self.subTest(event=event):
                self.config['tracks'] = [{'events': [event]}]
                with self.assertRaises(ValueError):
                    audio.pack_audio(self.config, self.base)

    def test_signed_pan_encoding(self):
        self.config['tracks'] = [{'events': [{'command': 'SET_PAN', 'arg1': -42}]}]
        block = audio.pack_audio(self.config, self.base)
        event_offset = audio.H.unpack_from(block)[10]
        self.assertEqual(audio.E.unpack_from(block, event_offset), (0, 4, 0, 214))

    def test_invalid_input_does_not_replace_output(self):
        with tempfile.TemporaryDirectory() as temporary:
            source, output = Path(temporary) / 'audio.json', Path(temporary) / 'audio.block'
            source.write_text(json.dumps({'tracks': [{'events': [{'delta': 256}]}]}))
            output.write_bytes(b'previous valid block')
            result = subprocess.run([sys.executable, str(ROOT / 'tools/pack_audio.py'), str(source), str(output)],
                                    capture_output=True)
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(output.read_bytes(), b'previous valid block')


if __name__ == '__main__':
    unittest.main()
