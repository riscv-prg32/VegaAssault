#!/usr/bin/env python3
"""Check the actual QEMU selected by PATH for PRG32's ESP32-C3 machine."""
import argparse
import shutil
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--qemu', default='qemu-system-riscv32', help='executable name or path')
    args = parser.parse_args()
    executable = shutil.which(args.qemu)
    if not executable:
        parser.exit(1, 'QEMU not found. Install Espressif qemu-riscv32 and source ESP-IDF export.sh.\n')
    try:
        version = subprocess.run([executable, '--version'], capture_output=True, text=True, timeout=10, check=True)
        machines = subprocess.run([executable, '-machine', 'help'], capture_output=True, text=True, timeout=10, check=True)
    except (OSError, subprocess.SubprocessError) as exc:
        parser.exit(1, f'Cannot probe {executable}: {exc}\n')
    print(executable)
    print(version.stdout.splitlines()[0])
    if not any(line.split()[:1] == ['esp32c3'] for line in machines.stdout.splitlines()):
        parser.exit(1, 'This QEMU has no esp32c3 machine. Install Espressif qemu-riscv32, then source ESP-IDF export.sh so it precedes generic QEMU on PATH.\n')
    print('ESP32-C3 machine support: PASS (host boot and cartridge tests are separate)')


if __name__ == '__main__':
    main()
