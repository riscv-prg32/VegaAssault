#!/bin/sh
set -eu
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$PROJECT_ROOT"
VERSION=${VERSION:-1.0.0}
ESP=${ESP32C6_PRG32:-build/grendizer-vega-assault-86.prg32}
QEMU=${QEMU_PRG32:-}
[ -f "$ESP" ] || { echo "Missing ESP32-C6 cartridge: $ESP" >&2; echo "Run ./build.sh first or set ESP32C6_PRG32." >&2; exit 2; }
set -- --esp32c6 "$ESP" --version "$VERSION" --out "dist/grendizer-vega-assault-86-store-$VERSION.zip"
if [ -n "$QEMU" ]; then set -- "$@" --qemu "$QEMU"; fi
python3 tools/make_store_bundle.py "$@"
