#!/usr/bin/env bash
set -euo pipefail

KEYBOARD="${KEYBOARD:-zsa/moonlander}"
KEYMAP="${KEYMAP:-moonlander_v2}"

if [[ "$KEYBOARD" == keyclicks/* ]]; then
  # Use vial-qmk for keyclicks boards
  export QMK_HOME="$HOME/code/vendor/Split-Keyboard/firmware/vial-qmk"
else
  # Use mainline QMK for everything else
  export QMK_HOME="$HOME/code/vendor/qmk_firmware"
fi

cd "$QMK_HOME"

# Ensure your keymap is linked
KEYMAP_SRC="$HOME/code/personal/qmk/keymaps/$KEYMAP"
KEYMAP_DST="keyboards/$KEYBOARD/keymaps/$KEYMAP"

if [[ -d "$KEYMAP_SRC" ]] && [[ ! -e "$KEYMAP_DST" ]]; then
  ln -sf "$KEYMAP_SRC" "$KEYMAP_DST"
fi

qmk compile -kb "$KEYBOARD" -km "$KEYMAP"
