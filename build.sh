#!/bin/sh
set -eu
# Resolve the project once; invocation does not depend on the caller's directory.
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
PRG32_ROOT=${PRG32_ROOT:-"$PROJECT_ROOT/../PRG32"}
PRG32_ROOT=$(CDPATH= cd -- "$PRG32_ROOT" && pwd)
OUT=${OUT:-build/grendizer-vega-assault-86.prg32}
case "$OUT" in /*) ;; *) OUT="$PROJECT_ROOT/$OUT" ;; esac
cd "$PROJECT_ROOT"
mkdir -p build "$(dirname -- "$OUT")"
python3 tools/check_educational_comments.py
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -ffreestanding -fsyntax-only \
  -I "$PRG32_ROOT/components/prg32/include" \
  -I "$PRG32_ROOT/components/prg32_audio/include" src/game.c
python3 tools/pack_audio.py audio/audio.json build/audio.block
cd "$PRG32_ROOT"
python3 -m prg32 cartridge build \
  "$PROJECT_ROOT/src/game.c" \
  --portable \
  --entry-prefix grendizer_c \
  --name grendizer-vega-assault-86 \
  --audio-block "$PROJECT_ROOT/build/audio.block" \
  --out "$OUT"
size=$(wc -c < "$OUT")
limit=131072
printf 'cartridge bytes: %s / %s\n' "$size" "$limit"
[ "$size" -le "$limit" ] || { echo 'ERROR: cartridge exceeds PRG32 128 KiB package limit' >&2; exit 2; }
