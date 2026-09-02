# Gameplay Demo

[Watch or download the gameplay demo (MP4, 761 KiB)](media/vega-assault-demo.mp4)

The 15-second clip shows the title screen, first-stage introduction and combat with the original stereo soundtrack. It is encoded as H.264 video at 960×600 with stereo AAC audio.

The video runs the actual cartridge update/draw code using the PRG32 software renderer and scripted standard button input. Music events are synchronized to game ticks and rendered through the software stereo mixer model; sound effects are omitted. The capture does not alter game state or grant invulnerability. This is a native software capture, not a QEMU or ESP32-C6 recording.

Game pixels are enlarged 3× without smoothing, with a short fade at the end. See [reproduction instructions](reproducibility.md#gameplay-video) and [soundtrack design](audio.md#opening-theme-refinement-2026-09-03-current).
