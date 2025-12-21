/**
 * @file leader_hash.h
 * @brief Hash-based leader key implementation
 * 
 * This implementation uses a rolling hash to detect leader sequences
 * without storing the full key history. Sequences are defined in
 * sequences.def using a human-readable SEQ() macro.
 */

#ifndef LEADER_HASH_H
#define LEADER_HASH_H

#include "quantum.h"

// ═══════════════════════════════════════════════════════════════════════════
// Configuration
// ═══════════════════════════════════════════════════════════════════════════

#ifndef LEADER_HASH_TIMEOUT
#define LEADER_HASH_TIMEOUT 500  // ms before sequence ends
#endif

#ifndef LEADER_HASH_NO_TIMEOUT
// If defined, timeout only starts after first key in sequence
#endif

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Start a leader sequence
 * Call this when the leader key is pressed
 */
void leader_hash_start(void);

/**
 * Check if leader sequence is currently active
 * @return true if in leader sequence
 */
bool leader_hash_active(void);

/**
 * Add a keycode to the current sequence
 * @param keycode The key that was pressed
 * @return true if key was consumed by leader system
 */
bool leader_hash_add(uint16_t keycode);

/**
 * End the leader sequence and execute matching action
 */
void leader_hash_end(void);

/**
 * Check for timeout and end sequence if needed
 * Call this from matrix_scan_user()
 */
void leader_hash_task(void);

/**
 * Reset the leader timeout timer
 * Used internally when keys are added
 */
void leader_hash_reset_timer(void);

/**
 * Check if the sequence has timed out
 * @return true if timed out
 */
bool leader_hash_timed_out(void);

// ═══════════════════════════════════════════════════════════════════════════
// Hash Utilities
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Generate hash for an array of keycodes at compile time
 * @param keycodes Array of keycodes
 * @param size Number of keycodes
 * @return 32-bit hash value
 */
uint32_t leader_hash_generate(const uint16_t keycodes[], uint8_t size);

/**
 * Check if current sequence matches the given keys
 * @param keycodes Array of keycodes to check
 * @param size Number of keycodes
 * @return true if sequence matches
 */
bool leader_hash_is(const uint16_t keycodes[], uint8_t size);

/**
 * Get current hash value (for debugging)
 */
uint32_t leader_hash_get(void);

/**
 * Get current sequence length
 */
uint8_t leader_hash_length(void);

// ═══════════════════════════════════════════════════════════════════════════
// User Callbacks (weak - override in keymap.c)
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Called when leader sequence starts
 */
void leader_hash_start_user(void);

/**
 * Called when leader sequence ends
 * Implement this to handle your sequences
 */
void leader_hash_end_user(void);

#endif // LEADER_HASH_H
