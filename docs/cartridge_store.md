# Publishing to a PRG32 CartridgeStore Instance

CartridgeStore groups artifacts by cartridge metadata `id` and `version` and can retain architecture-specific artifacts for `esp32c6` and `qemu`. Uploads are reviewed by Store editors before public catalog exposure.

## Build the release
1. Build and test the cartridge.
2. Run `./publish-store.sh`; the helper validates the 131072-byte limit and assembles the Store ZIP from `store/manifest.template.json`, `store/icon.png`, `store/splash.png`, and available `.prg32` artifacts.
3. Inspect the generated manifest and SHA-256 file in `dist/`.
4. Test the bundle against a staging Store when possible.

## Upload
Authenticate to the target instance and submit the multipart bundle to either supported publication endpoint, for example:

```sh
curl -X POST https://STORE.example/api/publish/bundle \
  -H "Authorization: Bearer $PRG32_STORE_TOKEN" \
  -F bundle=@dist/grendizer-vega-assault-86-store-1.2.0.zip
```

The upload enters editor review. Verify title, version, author, icon, splash, architecture labels, legal notice, download, and launch behavior before approval. Never mark one binary as a different architecture merely to fill a manifest slot.

## Local validation and version selection

The bundler imports the current PRG32 parser and runtime-contract validator from `PRG32_ROOT`, defaulting to the sibling PRG32 checkout. It rejects malformed/truncated cartridges, packages above 131072 bytes, and incompatible ABI or executable-memory requirements before replacing an existing ZIP. Passing structural validation does not certify runtime behavior on either target.

The default version and ZIP filename follow `store/manifest.template.json`; `VERSION=2.0.0-rc.1 ./publish-store.sh` selects an explicit version. ZIP timestamps and permissions are fixed for repeatability. The archive is assembled in a temporary file and then renamed; invalid input leaves a previous archive intact. Output and checksum paths cannot overwrite input cartridges. `--esp32c6` and `--qemu` remain distinct manifest entries, even when both refer to a portable binary. Only include target labels after the corresponding host validation.

## Display name

The public title and cartridge header name are **Vega Assault**. The established Store ID, artifact filenames, and `grendizer_c` entry-point prefix remain stable so existing listings and build integrations continue to identify the same cartridge. Grendizer remains the playable character's name.

## Version 1.2.0 artwork and package

The manifest describes the refined 24-frame art and preserves the established Store ID. `tools/generate_assets.py` generates a 128×128 character icon and a 320×200 promotional splash featuring the flight sprite and enemies. These compositions use original project sprites and are not gameplay screenshots. Actual QEMU gamefields are available in the [screenshot gallery](screenshots.md).

Regenerate with `python3 tools/generate_assets.py`, build with `./build.sh`, and package with `./publish-store.sh`. The default outputs are `dist/grendizer-vega-assault-86-store-1.2.0.zip` and its `.zip.sha256` sidecar; generated build/distribution files remain ignored by Git. The default ESP32-C6 entry identifies the intended portable-cartridge target, not a completed physical-board test. ESP32-C3 QEMU smoke testing passed; physical ESP32-C6, full target playthroughs and stereo listening are still outstanding. Local bundle creation does not upload to a Store instance.
