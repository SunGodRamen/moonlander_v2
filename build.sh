#!/usr/bin/env bash
set -euo pipefail

PERSONAL_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENDOR_QMK="${VENDOR_QMK:-$HOME/code/vendor/qmk_firmware}"
KEYMAP="${KEYMAP:-moonlander_v2}"
KEYBOARD="${KEYBOARD:-zsa/moonlander}"

# Moonlander (STM32 DfuSe) defaults
DFU_VIDPID="${DFU_VIDPID:-0483:df11}"
DFU_ALT="${DFU_ALT:-0}"                             # 0 = Internal Flash, 1 = Option Bytes
DFU_ADDR="${DFU_ADDR:-0x08000000}"                  # STM32 internal flash base
DFU_LEAVE="${DFU_LEAVE:-1}"                         # 1 => append :leave
QMK_BIN_OUT="${QMK_BIN_OUT:-zsa_moonlander_${KEYMAP}.bin}"  # copied to repo root by QMK
QMK_DFU_SERIAL="${QMK_DFU_SERIAL:-}"                # override when multiple DFU devices exist

# Wait behavior (host dfu-util path)
WAIT_DFU_SECS="${WAIT_DFU_SECS:-25}"
WAIT_DFU_POLL_MS="${WAIT_DFU_POLL_MS:-250}"

KEYMAP_SRC="$PERSONAL_ROOT/keymaps/$KEYMAP"
[[ -d "$KEYMAP_SRC" ]] || { echo "missing: $KEYMAP_SRC" >&2; exit 1; }

qmkc() {
  docker run --rm \
    -u "$(id -u):$(id -g)" \
    -v "$VENDOR_QMK:/qmk_firmware" \
    -v "$KEYMAP_SRC:/qmk_firmware/keyboards/$KEYBOARD/keymaps/$KEYMAP:ro" \
    -w /qmk_firmware \
    qmkfm/qmk_cli \
    qmk "$@"
}

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing dep: $1" >&2; exit 1; }; }

dfu_list_matching() {
  sudo dfu-util -l 2>/dev/null | sed -nE "s/^[[:space:]]*//; /^Found DFU: \\[$DFU_VIDPID\\]/p" || true
}

wait_for_dfu() {
  # waits until at least one DFU device matching DFU_VIDPID appears
  # Ctrl+C cancels (bash default), so no special handling needed
  local poll_s
  poll_s="$(awk -v ms="$WAIT_DFU_POLL_MS" 'BEGIN{ printf "%.3f", (ms/1000.0) }')"

  echo "[wait] Put the board into DFU bootloader now. Waiting up to ${WAIT_DFU_SECS}s... (Ctrl+C to cancel)" >&2

  local start now elapsed remaining
  start="$(date +%s)"
  while true; do
    if [[ -n "$(dfu_list_matching)" ]]; then
      return 0
    fi

    now="$(date +%s)"
    elapsed="$(( now - start ))"
    remaining="$(( WAIT_DFU_SECS - elapsed ))"

    if (( remaining <= 0 )); then
      echo "[wait] timed out after ${WAIT_DFU_SECS}s; no DFU devices detected for $DFU_VIDPID" >&2
      echo "hint: $0 list-dfu  # or: sudo dfu-util -l" >&2
      return 1
    fi

    # single-line countdown (no spam)
    printf '\r[wait] %2ds remaining... ' "$remaining" >&2
    sleep "$poll_s"
  done
}

pick_dfu_serial() {
  # Prefer explicit override
  if [[ -n "$QMK_DFU_SERIAL" ]]; then
    echo "$QMK_DFU_SERIAL"
    return 0
  fi

  local lines serials count
  lines="$(dfu_list_matching)"
  serials="$(printf '%s\n' "$lines" | sed -nE 's/.*serial="([^"]+)".*/\1/p' | sort -u || true)"
  count="$(printf '%s\n' "$serials" | sed '/^$/d' | wc -l | tr -d ' ')"

  if [[ "$count" == "0" ]]; then
    echo "no DFU devices for $DFU_VIDPID detected." >&2
    echo "hint: $0 list-dfu  # or: sudo dfu-util -l" >&2
    return 1
  fi

  if [[ "$count" == "1" ]]; then
    printf '%s\n' "$serials"
    return 0
  fi

  echo "multiple DFU devices for $DFU_VIDPID; set QMK_DFU_SERIAL=<serial>." >&2
  echo "matching DFU devices:" >&2
  printf '%s\n' "$lines" >&2
  echo "unique serials:" >&2
  printf '%s\n' "$serials" >&2
  return 1
}

flash_dfu_native() {
  need dfu-util

  local bin_path="$VENDOR_QMK/$QMK_BIN_OUT"
  [[ -f "$bin_path" ]] || bin_path="$VENDOR_QMK/.build/${QMK_BIN_OUT}"  # just in case
  [[ -f "$bin_path" ]] || { echo "missing firmware bin: $bin_path" >&2; exit 1; }

  wait_for_dfu

  # clear the countdown line if it printed
  printf '\r%*s\r' 40 '' >&2 || true

  local serial
  serial="$(pick_dfu_serial)"

  local addr="$DFU_ADDR"
  if [[ "$DFU_LEAVE" == "1" ]]; then
    addr="${addr}:leave"
  fi

  echo "[flash] dfu-util native: serial=$serial alt=$DFU_ALT addr=$addr bin=$(basename "$bin_path")" >&2
  sudo dfu-util -S "$serial" -a "$DFU_ALT" --dfuse-address "$addr" -D "$bin_path"
}

echo "Using KEYBOARD=$KEYBOARD KEYMAP=$KEYMAP"
echo "VENDOR_QMK=$VENDOR_QMK" >&2

case "${1:-compile}" in
  compile)
    qmkc compile -kb "$KEYBOARD" -km "$KEYMAP"
    ;;

  flash)
    qmkc compile -kb "$KEYBOARD" -km "$KEYMAP"

    set +e
    qmkc flash -kb "$KEYBOARD" -km "$KEYMAP"
    rc=$?
    set -e

    if [[ "$rc" == "0" ]]; then
      exit 0
    fi

    echo "[flash] qmk flash failed (rc=$rc). Falling back to native dfu-util." >&2
    flash_dfu_native
    ;;

  flash-native)
    flash_dfu_native
    ;;

  list-dfu)
    dfu_list_matching
    ;;

  clean)
    qmkc clean
    ;;

  *)
    cat >&2 <<USAGE
Usage:
  $0 compile
  $0 flash            # compile + try container flash + fallback native dfu-util
  $0 flash-native     # native dfu-util only (waits for DFU + flashes)
  $0 list-dfu         # show DFU devices matching $DFU_VIDPID
  $0 clean

Env overrides:
  VENDOR_QMK=... KEYBOARD=... KEYMAP=...
  QMK_DFU_SERIAL=2054336B2034
  DFU_ALT=0 DFU_ADDR=0x08000000 DFU_LEAVE=1
  WAIT_DFU_SECS=25 WAIT_DFU_POLL_MS=250
USAGE
    exit 1
    ;;
esac
