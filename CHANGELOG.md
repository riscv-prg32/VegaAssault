# Changelog

## 1.4.0 — Special Cosmo and lunar escort finale

- Animate three-craft assembly, Moon launch, base destruction and flight home.
- Clarify sky, underwater and underground environments.
- Cycle lunar Spazer/Robot/combined player forms with three firing NPC escorts.
- Replace the demo with a 30-second campaign montage and extend transition tests.

## 1.3.0 — Five-mission campaign

- Add an animated mountain/waterfall secret-base takeoff before Earth Defense.
- Add distinct Double, Marine and Drill Spazer missions and docking screens.
- Finish on the Moon with the combined craft, stationary Vega base boss and burning-base victory.
- Reuse four docking sprite slots and three music themes; preserve cartridge and executable RAM limits.
- Extend sanitizer regressions and add reproducible campaign visual fixtures.

## 1.2.0 - 2026-09-02

- Refine all 24 sprites with original inked armor, articulated poses and distinct boss silhouettes; preserve frame dimensions and the 15,068-byte indexed-art footprint.
- Add four actual QEMU gamefield screenshots and refresh the README preview.
- Refresh Store metadata, icon, splash and reproducible bundle for version 1.2.0.
- Verify the 26,520-byte portable build, sanitizer regressions and QEMU firing/pause/resume smoke test; physical ESP32-C6 validation remains outstanding.

## Earlier unreleased changes

- Rename the public game title, cartridge display name, title screen and Store artwork to Vega Assault; preserve existing identifiers and entry points.

- Verify an ESP32-C3 QEMU boot and first-stage input/rendering with Espressif QEMU; replace the README preview with a real emulator framebuffer capture.
- Add QEMU machine-support checks to distinguish generic QEMU from the Espressif build.
- Reject malformed audio authoring data before packing, preserving all current runtime audio bytes.

- Make fixed-pool weapon attacks atomic: no partial pairs or resource charges when slots are unavailable.
- Restore the gunner to waves with a firing pattern and constrain blade movement to the viewport.
- Validate cartridge structure and runtime contracts before Store bundling; use the manifest version and deterministic ZIP metadata.
- Add sanitizer and packaging regression coverage; verify the build through the configured ESP-IDF 5.4 environment.

## 1.0.0 - 2026-09-02

- Store-ready release packaging for `prg32-metadata-1.0`.
- Added Store icon and splash artwork.
- Added architecture-aware Store bundle builder with 128 KiB checks and SHA-256 output.
- Arcade edition includes animated Grendizer/Spazer sequences, Screw Crusher, Double Harken, Space Thunder, multiple robot enemies, three terrain stages, boss-specific patterns, attract mode, original stereo chiptune and SFX.

## 1.1.0 - Pixel-art redesign

- Reworked all 24 runtime sprite frames with stronger silhouettes, shading, highlights, weapon details, and boss differentiation.
- Improved Grendizer/Spazer transformation frames while preserving the existing 30,080-byte RGB565 runtime-art budget.
- Regenerated CartridgeStore icon and splash from the same original procedural pixel-art vocabulary.
- Kept all runtime graphics reproducible from `tools/generate_assets.py`; no production anime frames, scans, logos, or extracted artwork are embedded.
