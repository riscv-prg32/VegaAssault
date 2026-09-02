# ESP32-C6 Laboratory Guide

## Procedure
1. Install ESP-IDF 5.4 or newer with ESP32-C6 support. On Linux, ensure the user has serial-device permissions (commonly the `dialout` group).
2. Clone PRG32 and run `python3 -m prg32 doctor`.
3. Build and flash the resident PRG32 firmware according to the PRG32 hardware guide; PlatformIO may also be used for the physical ESP32-C6 host.
4. Build this cartridge with `PRG32_ROOT=/path/to/PRG32 ./build.sh`.
5. Connect to the PRG32 board/network and identify its HTTP base URL; the common setup/AP example is `http://192.168.4.1`.
6. Upload with `cd /path/to/PRG32 && python3 -m prg32 esp32c6 upload /path/to/game/build/grendizer-vega-assault-86.prg32 --url http://192.168.4.1`. Add `--slot cart1` (or another slot) when required.
7. Run the cartridge from PRG32 setup or configure it as the default cartridge.
8. Verify display framing, physical buttons, stereo/mono behavior, and complete stage progression.

The `.prg32` cartridge is not the resident firmware. Flashing PRG32 installs the host; uploading the cartridge changes a cartridge slot without reflashing the complete firmware.
