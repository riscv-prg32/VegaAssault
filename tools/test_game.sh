#!/bin/sh
set -eu
PROJECT_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
PRG32_ROOT=${PRG32_ROOT:-"$PROJECT_ROOT/../PRG32"}
TEST_DIR=$(mktemp -d)
trap 'rm -rf "$TEST_DIR"' EXIT HUP INT TERM
"${CC:-cc}" -std=c99 -Wall -Wextra -Werror -fsanitize=address,undefined \
  -I "$PRG32_ROOT/components/prg32/include" \
  -I "$PRG32_ROOT/components/prg32_audio/include" \
  "$PROJECT_ROOT/tests/game_test.c" -o "$TEST_DIR/game_test"
"$TEST_DIR/game_test"
