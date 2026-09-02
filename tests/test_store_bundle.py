"""Integration checks against the built cartridge and configured PRG32 tools."""
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import zipfile

ROOT = Path(__file__).resolve().parents[1]


class StoreBundleTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix='vega-store-')
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.cartridge = self.directory / 'cartridge with spaces.prg32'
        self.cartridge.write_bytes((ROOT / 'build/grendizer-vega-assault-86.prg32').read_bytes())
        self.out = self.directory / 'bundle.zip'

    def run_bundle(self, *extra):
        return subprocess.run(
            [sys.executable, str(ROOT / 'tools/make_store_bundle.py'),
             '--esp32c6', str(self.cartridge), '--out', str(self.out), *extra],
            cwd=ROOT, capture_output=True, text=True)

    def test_version_manifest_contents_and_repeatability(self):
        first = self.run_bundle('--qemu', str(self.cartridge))
        self.assertEqual(first.returncode, 0, first.stderr)
        original = self.out.read_bytes()
        os.utime(self.cartridge, (1000000000, 1000000000))
        second = self.run_bundle('--qemu', str(self.cartridge))
        self.assertEqual(second.returncode, 0, second.stderr)
        self.assertEqual(original, self.out.read_bytes())
        template = json.loads((ROOT / 'store/manifest.template.json').read_text())
        with zipfile.ZipFile(self.out) as archive:
            self.assertIsNone(archive.testzip())
            manifest = json.loads(archive.read('manifest.json'))
            self.assertEqual(manifest['version'], template['version'])
            self.assertEqual([a['id'] for a in manifest['architectures']], ['esp32c6', 'qemu'])
            for architecture in manifest['architectures']:
                self.assertEqual(archive.read(architecture['file']), self.cartridge.read_bytes())
        self.assertEqual(self.out.with_suffix('.zip.sha256').read_text().split()[0],
                         hashlib.sha256(original).hexdigest())

    def test_explicit_version(self):
        result = self.run_bundle('--version', '2.0.0-rc.1')
        self.assertEqual(result.returncode, 0, result.stderr)
        with zipfile.ZipFile(self.out) as archive:
            self.assertEqual(json.loads(archive.read('manifest.json'))['version'], '2.0.0-rc.1')

    def test_invalid_artifacts_preserve_previous_bundle(self):
        data = self.cartridge.read_bytes()
        for invalid in (b'', b'not a cartridge', data[:100], data + bytes(131073)):
            with self.subTest(length=len(invalid)):
                self.out.write_bytes(b'previous bundle')
                self.cartridge.write_bytes(invalid)
                result = self.run_bundle()
                self.assertNotEqual(result.returncode, 0)
                self.assertEqual(self.out.read_bytes(), b'previous bundle')

    def test_reject_output_overwriting_cartridge(self):
        original = self.cartridge.read_bytes()
        self.out = self.cartridge
        result = self.run_bundle()
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(self.cartridge.read_bytes(), original)

    def test_reject_path_in_version(self):
        result = self.run_bundle('--version', '../bad')
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(self.out.exists())


if __name__ == '__main__':
    unittest.main()
