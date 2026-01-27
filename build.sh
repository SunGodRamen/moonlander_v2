#!/usr/bin/env bash
set -euo pipefail

# ========================================
# build.sh - qmk helper (repo-local)
# VERSION: 1.0.3
# CHANGELOG:
# - fix UF2 auto-flash permissions for sudo-mounted vfat (uid/gid mount opts + sudo cp fallback)
# ========================================

PERSONAL_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"
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

# RP2040 UF2 (mass storage) defaults
UF2_LABEL="${UF2_LABEL:-RPI-RP2}"
WAIT_UF2_SECS="${WAIT_UF2_SECS:-25}"
WAIT_UF2_POLL_MS="${WAIT_UF2_POLL_MS:-250}"
UF2_MOUNT_DIR="${UF2_MOUNT_DIR:-}"  # optional fixed mountpoint override

KEYMAP_SRC="$PERSONAL_ROOT/keymaps/$KEYMAP"
LIB_SRC="$PERSONAL_ROOT/lib"
[[ -d "$LIB_SRC" ]] || { echo "missing: $LIB_SRC" >&2; exit 1; }
[[ -d "$KEYMAP_SRC" ]] || { echo "missing: $KEYMAP_SRC" >&2; exit 1; }

# Some keyboards (like ploopyco/madromys/rev1_001) keep keymaps at a parent dir.
# Walk up on the HOST until we find a directory that contains "keymaps".
keymap_mount_base_host() {
  local d="$VENDOR_QMK/keyboards/$KEYBOARD"
  while [[ "$d" != "$VENDOR_QMK/keyboards" && "$d" != "/" ]]; do
    if [[ -d "$d/keymaps" ]]; then
      echo "$d"
      return 0
    fi
    d="$(dirname "$d")"
  done
  echo "$VENDOR_QMK/keyboards/$KEYBOARD"  # fallback
}

# Convert host path under $VENDOR_QMK into container-relative path under /qmk_firmware
keymap_mount_base_rel() {
  local host
  host="$(keymap_mount_base_host)"
  case "$host" in
    "$VENDOR_QMK"/*) echo "${host#$VENDOR_QMK/}" ;;
    *)
      echo "[err] keymap_mount_base_host returned path not under VENDOR_QMK: $host" >&2
      return 1
      ;;
  esac
}

qmkc() {
  local rel
  mkdir -p "$KEYMAP_SRC/lib"
  rel="$(keymap_mount_base_rel)"

  docker run --rm \
    -u "$(id -u):$(id -g)" \
    -v "$VENDOR_QMK:/qmk_firmware" \
    -v "$KEYMAP_SRC:/qmk_firmware/$rel/keymaps/$KEYMAP" \
    -v "$LIB_SRC:/qmk_firmware/$rel/keymaps/$KEYMAP/lib:ro" \
    -w /qmk_firmware \
    qmkfm/qmk_cli \
    qmk "$@"
}

need() { command -v "$1" >/dev/null 2>&1 || { echo "missing dep: $1" >&2; exit 1; }; }

dfu_list_matching() {
  sudo dfu-util -l 2>/dev/null | sed -nE "s/^[[:space:]]*//; /^Found DFU: \\[$DFU_VIDPID\\]/p" || true
}

wait_for_dfu() {
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

    printf '\r[wait] %2ds remaining... ' "$remaining" >&2
    sleep "$poll_s"
  done
}

pick_dfu_serial() {
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
  [[ -f "$bin_path" ]] || bin_path="$VENDOR_QMK/.build/${QMK_BIN_OUT}"
  [[ -f "$bin_path" ]] || { echo "missing firmware bin: $bin_path" >&2; exit 1; }

  wait_for_dfu
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

uf2_dev_path() {
  if [[ -e "/dev/disk/by-label/$UF2_LABEL" ]]; then
    readlink -f "/dev/disk/by-label/$UF2_LABEL"
    return 0
  fi
  return 1
}

wait_for_uf2() {
  local poll_s
  poll_s="$(awk -v ms="$WAIT_UF2_POLL_MS" 'BEGIN{ printf "%.3f", (ms/1000.0) }')"

  echo "[wait] Put RP2040 into BOOTSEL/UF2 mode now. Waiting up to ${WAIT_UF2_SECS}s for label=$UF2_LABEL..." >&2

  local start now elapsed remaining
  start="$(date +%s)"
  while true; do
    if uf2_dev_path >/dev/null 2>&1; then
      return 0
    fi

    now="$(date +%s)"
    elapsed="$(( now - start ))"
    remaining="$(( WAIT_UF2_SECS - elapsed ))"

    if (( remaining <= 0 )); then
      echo "[wait] timed out after ${WAIT_UF2_SECS}s; no /dev/disk/by-label/$UF2_LABEL detected" >&2
      echo "hint: ls -l /dev/disk/by-label | grep -i rp" >&2
      return 1
    fi

    printf '\r[wait] %2ds remaining... ' "$remaining" >&2
    sleep "$poll_s"
  done
}

uf2_find_mountpoint() {
  if [[ -n "${UF2_MOUNT_DIR:-}" ]]; then
    [[ -d "$UF2_MOUNT_DIR" ]] || { echo "[uf2] UF2_MOUNT_DIR not found: $UF2_MOUNT_DIR" >&2; return 1; }
    echo "$UF2_MOUNT_DIR"
    return 0
  fi

  local dev mp
  dev="$(uf2_dev_path)"
  mp="$(findmnt -rn -S "$dev" -o TARGET 2>/dev/null | head -n 1 || true)"
  if [[ -n "${mp:-}" && -d "$mp" ]]; then
    echo "$mp"
    return 0
  fi

  if [[ -d "/run/media/$USER/$UF2_LABEL" ]]; then
    echo "/run/media/$USER/$UF2_LABEL"
    return 0
  fi

  return 1
}

uf2_mount_if_needed() {
  local dev mp
  dev="$(uf2_dev_path)"

  if mp="$(uf2_find_mountpoint 2>/dev/null)"; then
    echo "$mp"
    return 0
  fi

  if command -v udisksctl >/dev/null 2>&1; then
    udisksctl mount -b "$dev" >/dev/null
    mp="$(uf2_find_mountpoint)"
    [[ -n "${mp:-}" ]] || { echo "[uf2] mounted via udisksctl but couldn't locate mountpoint" >&2; return 1; }
    echo "$mp"
    return 0
  fi

  need sudo
  local tmp
  tmp="$(mktemp -d -t "uf2-${UF2_LABEL}.XXXXXX")"
  # vfat defaults to root-only; mount with uid/gid so user can write
  sudo mount -t vfat -o "uid=$(id -u),gid=$(id -g),umask=022" "$dev" "$tmp"
  echo "$tmp"
}

uf2_unmount_best_effort() {
  local dev mp
  dev="$(uf2_dev_path 2>/dev/null || true)"
  mp="${1:-}"

  if [[ -n "${dev:-}" ]] && command -v udisksctl >/dev/null 2>&1; then
    udisksctl unmount -b "$dev" >/dev/null 2>&1 || true
    return 0
  fi

  if [[ -n "${mp:-}" ]]; then
    sudo umount "$mp" >/dev/null 2>&1 || true
    case "$mp" in
      /tmp/uf2-*) rmdir "$mp" >/dev/null 2>&1 || true ;;
    esac
  fi
}

copy_latest_uf2() {
  local keyboard_filesafe="${KEYBOARD//\//_}"
  local out_name="ploopy_${keyboard_filesafe}_${KEYMAP}.uf2"

  local uf2
  uf2="$(find "$VENDOR_QMK" -type f -name "${keyboard_filesafe}_${KEYMAP}.uf2" -printf '%T@ %p\n' 2>/dev/null | sort -nr | head -n 1 | awk '{print $2}')" || true

  [[ -n "${uf2:-}" && -f "$uf2" ]] || { echo "[uf2] could not locate ${keyboard_filesafe}_${KEYMAP}.uf2 under $VENDOR_QMK" >&2; return 1; }

  cp -f "$uf2" "$PERSONAL_ROOT/$out_name"
  echo "$PERSONAL_ROOT/$out_name"
}

flash_uf2_auto() {
  local uf2 dev mp
  uf2="$(copy_latest_uf2)"
  echo "[uf2] local:  $uf2" >&2

  wait_for_uf2
  dev="$(uf2_dev_path)"
  echo "[uf2] device: $dev (label=$UF2_LABEL)" >&2

  mp="$(uf2_mount_if_needed)"
  echo "[uf2] mount:  $mp" >&2

  if [[ -w "$mp" ]]; then
    cp -f "$uf2" "$mp/$(basename "$uf2")"
  else
    sudo cp -f "$uf2" "$mp/$(basename "$uf2")"
  fi
  sync
  uf2_unmount_best_effort "$mp"
  echo "[uf2] flashed (copy + sync + unmount best-effort)" >&2
}

echo "Using KEYBOARD=$KEYBOARD KEYMAP=$KEYMAP"
echo "VENDOR_QMK=$VENDOR_QMK" >&2

case "${1:-compile}" in
  compile)
    qmkc compile -kb "$KEYBOARD" -km "$KEYMAP"
    if [[ "$KEYBOARD" == ploopyco/madromys* ]]; then
      out="$(copy_latest_uf2)"
      echo "[uf2] copied: $out" >&2
    fi
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

  flash-uf2)
    qmkc compile -kb "$KEYBOARD" -km "$KEYMAP"
    out="$(copy_latest_uf2)"
    cat >&2 <<MSG
[uf2] built: $out

Flash steps (RP2040 UF2):
- Unplug Adept
- Hold Bottom-Left button
- Plug in USB (drive appears, often as RPI-RP2)
- Drag-and-drop the .uf2 file onto that drive
MSG
    ;;

  flash-uf2-auto)
    qmkc compile -kb "$KEYBOARD" -km "$KEYMAP"
    flash_uf2_auto
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
  $0 flash-uf2        # compile + copy UF2 + print drag/drop instructions
  $0 flash-uf2-auto   # compile + wait for BOOTSEL drive + copy UF2 + unmount
  $0 list-dfu         # show DFU devices matching $DFU_VIDPID
  $0 clean

Env overrides:
  VENDOR_QMK=... KEYBOARD=... KEYMAP=...
  QMK_DFU_SERIAL=2054336B2034
  DFU_ALT=0 DFU_ADDR=0x08000000 DFU_LEAVE=1
  WAIT_DFU_SECS=25 WAIT_DFU_POLL_MS=250
  UF2_LABEL=RPI-RP2 WAIT_UF2_SECS=25 WAIT_UF2_POLL_MS=250 UF2_MOUNT_DIR=/run/media/$USER/RPI-RP2
USAGE
    exit 1
    ;;
esac
