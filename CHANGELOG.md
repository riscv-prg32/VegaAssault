# Changelog

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
