#!/usr/bin/env bash
set -euo pipefail

# run.sh — run galio.iso in QEMU
# Usage:
#   ./run.sh                # run with default args (GUI, serial -> serial.log)
#   ./run.sh --nogui        # run headless (nographic) and print serial to stdout
#   ./run.sh --iso path     # use custom ISO path
#   ./run.sh --qemu-args "...args..."  # pass extra qemu args
#   ./run.sh --fullscreen   # run in fullscreen mode
#
# Examples:
#   ./run.sh
#   ./run.sh --nogui
#   ./run.sh --qemu-args "-m 256M -display gtk -serial file:serial.log"
#   ./run.sh --fullscreen

ISO="galio.iso"
QEMU_BIN="qemu-system-i386"
DEFAULT_ARGS="-cdrom ${ISO} -m 128M -serial file:serial.log -monitor none -no-reboot"
EXTRA_ARGS=""
NOGRAPHIC=false
FULLSCREEN=false

# parse args
while [ $# -gt 0 ]; do
  case "$1" in
    --nogui|-n)
      NOGRAPHIC=true
      shift
      ;;
    --fullscreen|-f)
      FULLSCREEN=true
      shift
      ;;
    --iso)
      ISO="$2"
      shift 2
      ;;
    --qemu-args)
      EXTRA_ARGS="$2"
      shift 2
      ;;
    --help|-h)
      sed -n '1,200p' "$0"
      exit 0
      ;;
    *)
      echo "Unknown arg: $1"
      echo "Use --help for usage."
      exit 1
      ;;
  esac
done

# sanity checks
command -v "${QEMU_BIN}" >/dev/null 2>&1 || { echo "Error: ${QEMU_BIN} not found in PATH"; exit 1; }
if [ ! -f "${ISO}" ]; then
  echo "Error: ISO '${ISO}' not found. Build it first (./iso.sh) or pass --iso <path>."
  exit 1
fi

if [ "${NOGRAPHIC}" = true ]; then
  # headless: print serial to stdout
  echo "Starting QEMU (headless). Serial output will appear on stdout."
  exec ${QEMU_BIN} -cdrom "${ISO}" -m 128M -nographic -serial stdio ${EXTRA_ARGS}
elif [ "${FULLSCREEN}" = true ]; then
  # Fullscreen mode with scaling
  echo "Starting QEMU (fullscreen). Serial logged to serial.log"
  exec ${QEMU_BIN} -cdrom "${ISO}" -m 128M -vga std -full-screen -display gtk,zoom-to-fit=on -serial file:serial.log -monitor none -no-reboot ${EXTRA_ARGS}
else
  # GUI mode with scaling to fill window
  echo "Starting QEMU (GUI). Serial logged to serial.log"
  exec ${QEMU_BIN} -cdrom "${ISO}" -m 128M -vga std -display gtk,zoom-to-fit=on -serial file:serial.log -monitor none -no-reboot ${EXTRA_ARGS}
fi
