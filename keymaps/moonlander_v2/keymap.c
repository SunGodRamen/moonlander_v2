/**
 * @file keymap.c
 * @brief Moonlander Dvorak Keymap
 *
 * Layers: BASE(0), NAV(1), NUM(2), FUNC(3), MACRO(4), MEDIA(5)
 */

#include QMK_KEYBOARD_H
#include "moonlander.h"
#include "aliases.h"

#ifdef COMBO_ENABLE
#include "lib/feature/combo/combo.h"
#endif

#ifdef LEADER_HASH_ENABLE
#include "lib/feature/leader/leader_hash.h"
#include "lib/feature/leader/sequences.h"
#endif

#ifdef COUNTER_KEYS_ENABLE
#include "lib/feature/counter/counter_keys.h"
#endif

#ifdef RGB_MATRIX_ENABLE
#include "lib/feature/rgb/breathing.h"
#endif

#include "lib/util/logger.h"

// ═══════════════════════════════════════════════════════════════════════════
// KEYMAPS
// ═══════════════════════════════════════════════════════════════════════════
// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*═══════════════════════════════════════════════════════════════════════════╗
║  BASE - Dvorak with F-keys on top row                                       ║
║                                                                             ║
║  Left Thumb:  [TAB] [LSFT] [___]                                           ║
║  Right Thumb: [LEAD] [NAV/SPC] [NUMBR]                                     ║
║                                                                             ║
║  Home Row Mods (left): O=Alt, E=GUI, U=Ctrl                                ║
║  Swap Hands: A and S are SH_T                                              ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_BASE] = LAYOUT_moonlander(
        _F1,  _F2,  _F3,  _F4,  _F5,  _F6,  ___,           ___,  _F7,  _F8,  _F9,  _F10, _F11, _F12,
        ___,  SCL_, CM_,  DT_,  P_,   Y_,   DM_REC2,       DM_REC1, F_,  G_,   C_,   R_,   L_,   ___,
        ___,  A_, AO_,  GE_,  CU_,  I_,   DM_PLY2,       DM_PLY1, D_,  H_,   T_,   N_,   S_, ___,
        ___,  QT_,  Q_,   J_,   K_,   X_,                          B_,  M_,   W_,   V_,   Z_,   ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,          ___,  ___,  ___,  ___,  ___,
                                TAB,  LS_,  SH_MON,           SH_MON, NV_SPC, NUMBR
    ),

/*═══════════════════════════════════════════════════════════════════════════╗
║  NAV - Navigation (activated by holding Space)                              ║
║  Right side: Arrows, Page navigation, Browser nav                          ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_NAV] = LAYOUT_moonlander(
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  ___,   ___,   ___,   ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  WBck,  P_TAB, N_TAB, WFwd, ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  ArL,   ArD,   ArU,   ArR,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,                       ___,  HOME,  PGDN,  PGUP,  END,  ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,        ___,   ___,   ___,   ___,  ___,
                                ___,  ___,  ___,           ___,  FROM, ___
    ),

/*═══════════════════════════════════════════════════════════════════════════╗
║  NUM - Number Pad with Counter Keys                                         ║
║  Right side: 789/456/123 layout, counter keys on outer column              ║
║  Counter: TARE=reset, INCR/DECR=+/-1 or hold+num for +/-N, VALU=output     ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_NUM] = LAYOUT_moonlander(
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,   ___,    ___,  ___,  ___,  ___,    ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,   X_TARE, _7,   _8,   _9,   X_INCR, ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,   _0,     _4,   _5,   _6,   X_VALU, ___,
        ___,  ___,  ___,  ___,  ___,  ___,                        X_TARE, _1,   _2,   _3,   X_DECR, ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,           ___,  ___,  ___,  ___,    ___,
                                ___,  ___,  ___,           ___,   ___,    FROM
    ),

/*═══════════════════════════════════════════════════════════════════════════╗
║  FUNC - Function Keys F1-F12                                                ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_FUNC] = LAYOUT_moonlander(
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  _F12, _F7,  _F8,  _F9,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  _F10, _F4,  _F5,  _F6,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,                       _F11, _F1,  _F2,  _F3,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,        ___,  ___,  ___,  ___,  ___,
                                FROM, ___,  ___,           ___,  ___,  FROM
    ),

/*═══════════════════════════════════════════════════════════════════════════╗
║  MACRO - Dynamic Macro Controls                                             ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_MACRO] = LAYOUT_moonlander(
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,     ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  FROM,          FROM,    ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  DM_PLY2, DM_PLY1, ___,  DM_REC2, DM_REC1, ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  DM_REC2, DM_REC1, ___,                    ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,           ___,  ___,  ___,  ___,  ___,
                                FROM, ___,  ___,           ___,  ___,  ___
    ),

/*═══════════════════════════════════════════════════════════════════════════╗
║  MEDIA - Media Controls                                                     ║
╚═════════════════════════════════════════════════════════════════════════════*/
    [_MEDIA] = LAYOUT_moonlander(
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  ___,  ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,  ___,           ___,  PLAY, PRV,  VDN,  VUP,  NXT,  ___,
        ___,  ___,  ___,  ___,  ___,  ___,                       MUTE, ___,  ___,  ___,  ___,  ___,
        ___,  ___,  ___,  ___,  ___,        ___,           ___,        ___,  ___,  ___,  ___,  ___,
                                ___,  ___,  ___,           FROM, ___,  ___
    ),
};
// clang-format on

// ═══════════════════════════════════════════════════════════════════════════
// INITIALIZATION
// ═══════════════════════════════════════════════════════════════════════════

void keyboard_post_init_user(void) {
#ifdef LOGGING_ENABLE
    log_init(LOG_LEVEL_INFO);
    LOG_INFO("Moonlander initialized");
#endif

#ifdef RGB_MATRIX_ENABLE
    breathing_init();
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
// KEY PROCESSING
// ═══════════════════════════════════════════════════════════════════════════

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    LOG_KEY(keycode, record->event.pressed);

#ifdef LEADER_HASH_ENABLE
    if (keycode == LEAD_KEY && record->event.pressed) {
        leader_hash_start();
        return false;
    }

    if (leader_hash_active()) {
        if (record->event.pressed) {
            leader_hash_add(keycode);
            leader_hash_reset_timer();
        }
        return false;
    }
#endif

#ifdef COUNTER_KEYS_ENABLE
    if (!process_counter_key(keycode, record)) {
        return false;
    }
#endif

    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// MATRIX SCAN
// ═══════════════════════════════════════════════════════════════════════════

void matrix_scan_user(void) {
#ifdef LEADER_HASH_ENABLE
    leader_hash_task();
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
// LEADER SEQUENCE HANDLER
// ═══════════════════════════════════════════════════════════════════════════

#ifdef LEADER_HASH_ENABLE
void leader_hash_end_user(void) {
    LOG_INFO("Leader end - hash: 0x%08lX, len: %d",
             leader_hash_get(), leader_hash_length());
    process_leader_sequences();
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// RGB MATRIX
// ═══════════════════════════════════════════════════════════════════════════

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_user(void) {
    breathing_update();
    return false;
}
#endif

// ═══════════════════════════════════════════════════════════════════════════
// TAPPING TERM PER KEY
// ═══════════════════════════════════════════════════════════════════════════

#ifdef TAPPING_TERM_PER_KEY
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AO_: case GE_: case CU_:
            return TAPPING_TERM + 30;
        case NV_SPC:
            return TAPPING_TERM - 20;
        case SH_T(KC_A): case SH_T(KC_S):
            return TAPPING_TERM + 20;
        default:
            return TAPPING_TERM;
    }
}
#endif

#ifdef HOLD_ON_OTHER_KEY_PRESS_PER_KEY
bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case NV_SPC: return true;
        case AO_: case GE_: case CU_: return false;
        default: return false;
    }
}
#endif
