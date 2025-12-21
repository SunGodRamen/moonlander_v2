/**
 * @file aliases.h
 * @brief Key aliases for Moonlander keymap
 *
 * Provides shorthand names for all keycodes used in the layout.
 * Board-specific - other keyboards may need different aliases.
 */

#ifndef ALIASES_H
#define ALIASES_H

#include "quantum.h"
#include "lib/core/layers.h"
#include "lib/core/keycodes.h"

// ═══════════════════════════════════════════════════════════════════════════
// TRANSPARENT / NO-OP
// ═══════════════════════════════════════════════════════════════════════════

#define ___ KC_TRANSPARENT
#define XXX KC_NO
#define FROM KC_TRANSPARENT

// ═══════════════════════════════════════════════════════════════════════════
// LETTERS (Plain)
// ═══════════════════════════════════════════════════════════════════════════

#define A_  KC_A
#define B_  KC_B
#define C_  KC_C
#define D_  KC_D
#define E_  KC_E
#define F_  KC_F
#define G_  KC_G
#define H_  KC_H
#define I_  KC_I
#define J_  KC_J
#define K_  KC_K
#define L_  KC_L
#define M_  KC_M
#define N_  KC_N
#define O_  KC_O
#define P_  KC_P
#define Q_  KC_Q
#define R_  KC_R
#define S_  KC_S
#define T_  KC_T
#define U_  KC_U
#define V_  KC_V
#define W_  KC_W
#define X_  KC_X
#define Y_  KC_Y
#define Z_  KC_Z

// ═══════════════════════════════════════════════════════════════════════════
// NUMBERS
// ═══════════════════════════════════════════════════════════════════════════

#define _0  KC_0
#define _1  KC_1
#define _2  KC_2
#define _3  KC_3
#define _4  KC_4
#define _5  KC_5
#define _6  KC_6
#define _7  KC_7
#define _8  KC_8
#define _9  KC_9

// ═══════════════════════════════════════════════════════════════════════════
// FUNCTION KEYS
// ═══════════════════════════════════════════════════════════════════════════

#define _F1   KC_F1
#define _F2   KC_F2
#define _F3   KC_F3
#define _F4   KC_F4
#define _F5   KC_F5
#define _F6   KC_F6
#define _F7   KC_F7
#define _F8   KC_F8
#define _F9   KC_F9
#define _F10  KC_F10
#define _F11  KC_F11
#define _F12  KC_F12
#define _F13  KC_F13
#define _F14  KC_F14
#define _F15  KC_F15
#define _F16  KC_F16
#define _F17  KC_F17
#define _F18  KC_F18
#define _F19  KC_F19
#define _F20  KC_F20

// ═══════════════════════════════════════════════════════════════════════════
// PUNCTUATION
// ═══════════════════════════════════════════════════════════════════════════

#define SCL_  KC_SCLN      // ;
#define CLN_  KC_COLN      // :
#define CM_   KC_COMMA     // ,
#define DT_   KC_DOT       // .
#define QT_   KC_QUOT      // '
#define DQ_   KC_DQUO      // "
#define GRV_  KC_GRV       // `
#define TILD_ KC_TILD      // ~

// ═══════════════════════════════════════════════════════════════════════════
// HOME ROW MODS (Left hand)
// ═══════════════════════════════════════════════════════════════════════════

#define AO_   LALT_T(KC_O)   // O with Alt on hold
#define GE_   LGUI_T(KC_E)   // E with GUI on hold
#define CU_   LCTL_T(KC_U)   // U with Ctrl on hold

// ═══════════════════════════════════════════════════════════════════════════
// SWAP HANDS KEYS
// Tap for letter, hold for swap hands
// ═══════════════════════════════════════════════════════════════════════════

#define SH_A  SH_T(KC_A)     // A with swap hands on hold
#define SH_S  SH_T(KC_S)     // S with swap hands on hold

// For convenience in combos that need the mod-tap version
//#define SH_T(kc) SH_T(kc)

// ═══════════════════════════════════════════════════════════════════════════
// LAYER ACCESS
// ═══════════════════════════════════════════════════════════════════════════

#define NV_SPC  LT(_NAV, KC_SPC)     // Space, Nav on hold
#define NUMBR   MO(_NUM)             // Numbers layer (momentary)
#define FUNCL   MO(_FUNC)            // Function layer (momentary)
#define MACRO   MO(_MACRO)           // Macro layer (momentary)
#define MEDIA   MO(_MEDIA)           // Media layer (momentary)

// ═══════════════════════════════════════════════════════════════════════════
// NAVIGATION KEYS
// ═══════════════════════════════════════════════════════════════════════════

#define ArL   KC_LEFT
#define ArD   KC_DOWN
#define ArU   KC_UP
#define ArR   KC_RIGHT
#define HOME  KC_HOME
#define END   KC_END
#define P_UP  KC_PAGE_UP
#define P_DN  KC_PAGE_DOWN
#define PGUP  KC_PGUP
#define PGDN  KC_PGDN

// Tab navigation
#define N_TAB  LCTL(KC_TAB)           // Next tab
#define P_TAB  LSFT(LCTL(KC_TAB))     // Previous tab

// Browser navigation
#define WFwd   KC_WWW_FORWARD
#define WBck   KC_WWW_BACK

// ═══════════════════════════════════════════════════════════════════════════
// MODIFIERS
// ═══════════════════════════════════════════════════════════════════════════

#define LS_   KC_LSFT
#define RS_   KC_RSFT
#define LC_   KC_LCTL
#define RC_   KC_RCTL
#define LA_   KC_LALT
#define RA_   KC_RALT
#define LG_   KC_LGUI
#define RG_   KC_RGUI

// ═══════════════════════════════════════════════════════════════════════════
// DYNAMIC MACROS
// ═══════════════════════════════════════════════════════════════════════════

#define DM_REC1   QK_DYNAMIC_MACRO_RECORD_START_1
#define DM_REC2   QK_DYNAMIC_MACRO_RECORD_START_2
#define DM_PLY1   QK_DYNAMIC_MACRO_PLAY_1
#define DM_PLY2   QK_DYNAMIC_MACRO_PLAY_2
#define DM_STOP   QK_DYNAMIC_MACRO_RECORD_STOP

// ═══════════════════════════════════════════════════════════════════════════
// MEDIA KEYS
// ═══════════════════════════════════════════════════════════════════════════

#define PLAY  KC_MEDIA_PLAY_PAUSE
#define NXT   KC_MEDIA_NEXT_TRACK
#define PRV   KC_MEDIA_PREV_TRACK
#define VDN   KC_AUDIO_VOL_DOWN
#define VUP   KC_AUDIO_VOL_UP
#define MUTE  KC_AUDIO_MUTE

// ═══════════════════════════════════════════════════════════════════════════
// MOUSE KEYS
// ═══════════════════════════════════════════════════════════════════════════

#define BTN1  KC_MS_BTN1
#define BTN2  KC_MS_BTN2
#define BTN3  KC_MS_BTN3
#define BTN4  KC_MS_BTN4
#define BTN5  KC_MS_BTN5

// ═══════════════════════════════════════════════════════════════════════════
// LEADER KEY
// ═══════════════════════════════════════════════════════════════════════════

#define LEAD_0  LEAD_KEY   // Single leader key

// ═══════════════════════════════════════════════════════════════════════════
// MISC
// ═══════════════════════════════════════════════════════════════════════════

#define ENTR  KC_ENT
#define BSPC  KC_BSPC
#define DEL   KC_DEL
#define TAB   KC_TAB
#define ESC   KC_ESC
#define SPC   KC_SPC

#endif // ALIASES_H
