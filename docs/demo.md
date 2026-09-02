# Campaign Demo

[Watch or download the 30-second campaign montage](media/vega-assault-demo.mp4).

The H.264 960×600 video shows title, secret-base takeoff, Earth, sky, marine and underground combat, Special Cosmo assembly/launch, three lunar forms with NPC escorts, the Moon base boss and the animated destruction/escape ending. Audio is stereo AAC.

The video uses actual cartridge update/draw code with the upstream software renderer. The host harness explicitly selects scenes, grants temporary invulnerability, accelerates cinematic/form timing and stages the final boss hit to fit the campaign into 30 seconds. It is a native scene montage, not an uninterrupted playthrough or QEMU/ESP32-C6 recording. Original music events follow capture ticks and use the existing stereo software mixer; SFX are omitted.

Pixels are enlarged 3× without smoothing. A 0.6-second fade closes the clip. Run `FFMPEG=/path/to/ffmpeg python3 tools/capture_video.py`, then copy `build/game-demo/vega-assault-demo.mp4` to `docs/media/`. The encoder trims 910 source ticks to exactly 30 seconds. `build/game-demo/capture.json` records timing and scene provenance.
