#!/usr/bin/env python3
"""Validate cartridges and create a repeatable, local CartridgeStore bundle."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import sys
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--esp32c6')
    parser.add_argument('--qemu')
    parser.add_argument('--version', help='defaults to the manifest template version')
    parser.add_argument('--out', help='defaults to dist/<game>-store-<version>.zip')
    args = parser.parse_args()
    framework = Path(os.environ.get('PRG32_ROOT', ROOT.parent / 'PRG32')).resolve()
    sys.path.insert(0, str(framework))
    try:
        from prg32.store.cartridge_format import parse_cartridge, validate_metadata
        from prg32.utilities.runtime_handler import validate_cartridge_contract
    except ImportError as exc:
        parser.error(f'PRG32 tooling unavailable at {framework}; set PRG32_ROOT: {exc}')

    manifest = json.loads((ROOT / 'store/manifest.template.json').read_text())
    version = args.version if args.version is not None else manifest['version']
    if not re.fullmatch(r'[0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.+-]+)?', version):
        parser.error('version must have three numeric components and an optional release suffix')
    manifest['version'] = version
    manifest['architectures'] = []
    files = []
    sources = []
    for architecture, source in [('esp32c6', args.esp32c6), ('qemu', args.qemu)]:
        if source is None:
            continue
        path = Path(source).resolve()
        data = path.read_bytes()
        if len(data) > 131072:
            parser.error(f'{architecture} exceeds 128 KiB: {len(data)} bytes')
        parse_cartridge(data)
        validate_cartridge_contract(data, context=architecture)
        name = f'grendizer-vega-assault-86-{architecture}.prg32'
        manifest['architectures'].append({'id': architecture, 'file': name})
        files.append((name, data))
        sources.append(path)
    if not files:
        parser.error('provide at least --esp32c6 or --qemu with a real .prg32 artifact')
    validate_metadata(manifest)
    files = [('manifest.json', (json.dumps(manifest, indent=2, ensure_ascii=False)+'\n').encode()),
             ('icon.png', (ROOT / 'store/icon.png').read_bytes()),
             ('splash.png', (ROOT / 'store/splash.png').read_bytes())] + files
    out = (ROOT / (args.out or f'dist/grendizer-vega-assault-86-store-{version}.zip')).resolve()
    checksum = out.with_suffix(out.suffix + '.sha256')
    if out in sources or checksum in sources:
        parser.error('output and checksum must not overwrite an input cartridge')
    out.parent.mkdir(parents=True, exist_ok=True)
    # Prepare the complete archive before replacing any existing bundle.
    with tempfile.NamedTemporaryFile(dir=out.parent, delete=False) as temporary:
        temporary_path = Path(temporary.name)
    try:
        with zipfile.ZipFile(temporary_path, 'w') as archive:
            for name, data in files:
                entry = zipfile.ZipInfo(name, date_time=(1980, 1, 1, 0, 0, 0))
                entry.create_system = 3
                entry.external_attr = 0o100644 << 16
                entry.compress_type = zipfile.ZIP_DEFLATED
                archive.writestr(entry, data)
        sha = hashlib.sha256(temporary_path.read_bytes()).hexdigest()
        temporary_path.replace(out)
        checksum.write_text(f'{sha}  {out.name}\n')
    finally:
        temporary_path.unlink(missing_ok=True)
    print(out)
    print('sha256', sha)


if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError) as exc:
        raise SystemExit(str(exc)) from exc
