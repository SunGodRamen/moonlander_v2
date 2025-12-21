/**
 * @file counter_keys.h
 * @brief Counter key feature for dynamic numbering
 * 
 * Provides a persistent counter that can be:
 * - Incremented/decremented with dedicated keys
 * - Modified by holding incr/decr and tapping number keys
 * - Reset to 1 (tare)
 * - Output as keystrokes
 * 
 * Useful with dynamic macros for repetitive numbered tasks.
 */

#ifndef COUNTER_KEYS_H
#define COUNTER_KEYS_H

#include "quantum.h"
#include "../core/keycodes.h"

// ═══════════════════════════════════════════════════════════════════════════
// Configuration
// ═══════════════════════════════════════════════════════════════════════════

#ifndef COUNTER_INITIAL_VALUE
#define COUNTER_INITIAL_VALUE 1
#endif

#ifndef COUNTER_MIN_VALUE
#define COUNTER_MIN_VALUE INT16_MIN
#endif

#ifndef COUNTER_MAX_VALUE
#define COUNTER_MAX_VALUE INT16_MAX
#endif

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Process counter-related keycodes
 * @param keycode The keycode to process
 * @param record Key event record
 * @return false if keycode was handled, true to continue processing
 */
bool process_counter_key(uint16_t keycode, keyrecord_t *record);

/**
 * Get current counter value
 */
int16_t counter_get_value(void);

/**
 * Set counter to specific value
 */
void counter_set_value(int16_t value);

/**
 * Reset counter to initial value
 */
void counter_reset(void);

/**
 * Increment counter by amount
 */
void counter_increment(int16_t amount);

/**
 * Decrement counter by amount
 */
void counter_decrement(int16_t amount);

/**
 * Output current counter value as keystrokes
 */
void counter_output(void);

/**
 * Check if increment key is currently held
 */
bool counter_incr_held(void);

/**
 * Check if decrement key is currently held
 */
bool counter_decr_held(void);

#endif // COUNTER_KEYS_H
