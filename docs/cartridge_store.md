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
  -F bundle=@dist/grendizer-vega-assault-86-store-1.0.0.zip
```

The upload enters editor review. Verify title, version, author, icon, splash, architecture labels, legal notice, download, and launch behavior before approval. Never mark one binary as a different architecture merely to fill a manifest slot.
