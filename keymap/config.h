/**
 * @file config.h
 * @brief Moonlander-specific configuration
 */

#pragma once

// ═══════════════════════════════════════════════════════════════════════════
// TAPPING BEHAVIOR
// ═══════════════════════════════════════════════════════════════════════════

// Tap-hold timing
#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY

// Permit hold on tap-hold keys even if another key is pressed
#define PERMISSIVE_HOLD

// Immediately select hold action when another key is pressed
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY

// Retro tapping - if hold key is released without any other key press, treat as tap
// #define RETRO_TAPPING

// ═══════════════════════════════════════════════════════════════════════════
// COMBO CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#define COMBO_COUNT COMBO_COUNT        // Defined by combo.h
#define COMBO_TERM 50                  // Max time between combo keys (ms)
#define COMBO_MUST_HOLD_MODS           // Hold mods must be held, not tapped
#define COMBO_HOLD_TERM 150            // Time to hold for combo-hold behavior
#define COMBO_MUST_TAP_PER_COMBO       // Allow per-combo tap/hold config
#define COMBO_STRICT_TIMER             // Combo timer resets on each keypress

// ═══════════════════════════════════════════════════════════════════════════
// LEADER KEY CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#define LEADER_HASH_TIMEOUT 500        // Leader sequence timeout (ms)
#define LEADER_HASH_NO_TIMEOUT         // Only timeout after first key

// ═══════════════════════════════════════════════════════════════════════════
// DYNAMIC MACRO CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#define DYNAMIC_MACRO_SIZE 256         // Bytes per macro slot

// ═══════════════════════════════════════════════════════════════════════════
// COUNTER CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#define COUNTER_INITIAL_VALUE 1
#define COUNTER_MIN_VALUE -9999
#define COUNTER_MAX_VALUE 9999

// ═══════════════════════════════════════════════════════════════════════════
// RGB CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#ifdef RGB_MATRIX_ENABLE
    // Breathing effect settings
    #define BREATHING_MIN_VAL 100
    #define BREATHING_MAX_VAL 180
    #define BREATHING_PERIOD_MS 9000
    #define BREATHING_HUE 0            // Red
    #define BREATHING_SAT 255

    // Disable default RGB modes we don't use
    #define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_SOLID_COLOR
    #define RGB_MATRIX_STARTUP_HUE 0
    #define RGB_MATRIX_STARTUP_SAT 255
    #define RGB_MATRIX_STARTUP_VAL 100
#endif

// ═══════════════════════════════════════════════════════════════════════════
// DEBUG / LOGGING
// ═══════════════════════════════════════════════════════════════════════════

#ifdef LOGGING_ENABLE
    #define DEBUG_MATRIX_SCAN_RATE     // Show matrix scan rate in debug
#endif

// ═══════════════════════════════════════════════════════════════════════════
// MEMORY OPTIMIZATION
// ═══════════════════════════════════════════════════════════════════════════

// Reduce firmware size by disabling unused features
#define NO_ACTION_ONESHOT              // Disable oneshot if not using
// #define NO_ACTION_TAPPING           // Don't disable - we use tap-hold

// Layer count (reduces memory if fewer than 16)
#define LAYER_STATE_8BIT               // Only 8 layers max
