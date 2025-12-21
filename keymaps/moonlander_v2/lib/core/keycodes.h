/**
 * @file keycodes.h
 * @brief Custom keycode definitions
 *
 * Defines all custom keycodes used across the keymap.
 * Keycodes are partitioned by feature for easy extension.
 */

#ifndef KEYCODES_H
#define KEYCODES_H

#include "quantum.h"

// Base custom keycode range
enum custom_keycodes {
    // ═══════════════════════════════════════════════════════════════
    // LEADER KEYS (0x7E00 - 0x7E0F)
    // ═══════════════════════════════════════════════════════════════
    LEAD_KEY = QK_USER_0,   // Single leader key trigger

    // ═══════════════════════════════════════════════════════════════
    // COUNTER KEYS (0x7E10 - 0x7E1F)
    // ═══════════════════════════════════════════════════════════════
    X_INCR,     // Increment counter
    X_DECR,     // Decrement counter
    X_TARE,     // Reset counter to 1
    X_VALU,     // Print current counter value

    // ═══════════════════════════════════════════════════════════════
    // BRACKET MODIFIERS (0x7E20 - 0x7E2F)
    // ═══════════════════════════════════════════════════════════════
    BR_MOD,     // Bracket modifier key (A held)

    // ═══════════════════════════════════════════════════════════════
    // DYNAMIC MACRO HELPERS (0x7E30 - 0x7E3F)
    // ═══════════════════════════════════════════════════════════════
    DMP,        // Dynamic macro play (context-aware)

    // ═══════════════════════════════════════════════════════════════
    // END MARKER
    // ═══════════════════════════════════════════════════════════════
    CUSTOM_KEYCODE_END
};

// Verify we haven't exceeded QMK's user keycode range
_Static_assert((uint16_t)CUSTOM_KEYCODE_END <= (uint16_t)QK_USER_MAX,
               "Too many custom keycodes defined");

#endif // KEYCODES_H
