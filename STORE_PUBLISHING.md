# PRG32 Cartridge Store publication

This repository contains the assets and metadata required by the current `riscv-prg32/CartridgeStore` bundle API.

## Store bundle layout

The generated ZIP has only Store-facing files at its root:

```text
manifest.json
icon.png
splash.png
grendizer-vega-assault-86-esp32c6.prg32
# optional: grendizer-vega-assault-86-qemu.prg32
```

`manifest.json` uses ABI `prg32-metadata-1.0`. Each architecture entry points to a genuine cartridge linked for that architecture. Never relabel one binary as another architecture.

## Build and package

```sh
PRG32_ROOT=/path/to/PRG32 ./build.sh
./publish-store.sh
```

To add a separately linked QEMU cartridge:

```sh
QEMU_PRG32=/path/to/game-qemu.prg32 ./publish-store.sh
```

The packer rejects cartridges larger than 131072 bytes and writes a SHA-256 sidecar next to the Store ZIP.

## Upload to a Cartridge Store instance

Create a Bearer token from the Store, then:

```sh
curl -X POST https://STORE/api/publish/bundle \
  -H "Authorization: Bearer prg32_..." \
  -F bundle=@dist/grendizer-vega-assault-86-store-1.0.0.zip
```

The upload enters the pending review queue. An editor must verify it before it appears in the public catalog.

## Editorial/IP note

This package is an unofficial, non-commercial fan project. Code, generated pixel art, waveform samples, music patterns and SFX in this repository are original project material. The Grendizer names, characters, fictional setting and related third-party IP remain with their respective rights holders. The MIT license covers only original project material and does not grant rights in those third-party properties. A Store operator should confirm its own legal/content policy before public distribution.
