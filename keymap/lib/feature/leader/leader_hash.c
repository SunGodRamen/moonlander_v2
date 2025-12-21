/**
 * @file leader_hash.c
 * @brief Hash-based leader key implementation
 */

#include "leader_hash.h"
#include "../../util/logger.h"

// ═══════════════════════════════════════════════════════════════════════════
// Internal State
// ═══════════════════════════════════════════════════════════════════════════

static bool     leader_active       = false;
static uint16_t leader_timer        = 0;
static uint32_t leader_hash         = 0;
static uint8_t  leader_index        = 0;

// ═══════════════════════════════════════════════════════════════════════════
// Weak Callbacks
// ═══════════════════════════════════════════════════════════════════════════

__attribute__((weak)) void leader_hash_start_user(void) {
    LOG_DEBUG("Leader sequence started");
}

__attribute__((weak)) void leader_hash_end_user(void) {
    LOG_DEBUG("Leader ended - hash: 0x%08lX, length: %d", leader_hash, leader_index);
}

// ═══════════════════════════════════════════════════════════════════════════
// Hash Function
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Rolling hash function
 * Combines keycode with position index for unique sequences
 */
static uint32_t hash_combine(uint16_t keycode, uint8_t index, uint32_t current_hash) {
    // Rotate left by 5, then XOR with keycode
    return ((current_hash << 5) | (current_hash >> 27)) ^ keycode;
}

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

void leader_hash_start(void) {
    if (leader_active) {
        return;  // Already in a sequence
    }
    
    leader_active = true;
    leader_timer  = timer_read();
    leader_hash   = 0;
    leader_index  = 0;
    
    leader_hash_start_user();
}

bool leader_hash_active(void) {
    return leader_active && leader_timer != 0;
}

bool leader_hash_add(uint16_t keycode) {
    if (!leader_active) {
        return false;
    }
    
#ifdef LEADER_HASH_NO_TIMEOUT
    // Reset timer only after first key
    if (leader_index == 0) {
        leader_hash_reset_timer();
    }
#endif

    // Add to hash
    leader_hash = hash_combine(keycode, leader_index, leader_hash);
    leader_index++;
    
    LOG_TRACE("Leader add: 0x%04X -> hash: 0x%08lX (len: %d)", 
              keycode, leader_hash, leader_index);
    
    return true;
}

void leader_hash_end(void) {
    if (!leader_active) {
        return;
    }
    
    LOG_INFO("Leader end - hash: 0x%08lX, length: %d", leader_hash, leader_index);
    
    leader_active = false;
    leader_timer  = 0;
    
    // Call user handler
    leader_hash_end_user();
    
    // Clear state after handler
    leader_hash  = 0;
    leader_index = 0;
}

void leader_hash_task(void) {
    if (leader_hash_active() && leader_hash_timed_out()) {
        leader_hash_end();
    }
}

void leader_hash_reset_timer(void) {
    leader_timer = timer_read();
}

bool leader_hash_timed_out(void) {
#ifdef LEADER_HASH_NO_TIMEOUT
    // Only timeout if we have keys in sequence
    return leader_index > 0 && timer_elapsed(leader_timer) > LEADER_HASH_TIMEOUT;
#else
    return timer_elapsed(leader_timer) > LEADER_HASH_TIMEOUT;
#endif
}

// ═══════════════════════════════════════════════════════════════════════════
// Hash Utilities
// ═══════════════════════════════════════════════════════════════════════════

uint32_t leader_hash_generate(const uint16_t keycodes[], uint8_t size) {
    uint32_t hash = 0;
    for (uint8_t i = 0; i < size; i++) {
        hash = hash_combine(keycodes[i], i, hash);
    }
    return hash;
}

bool leader_hash_is(const uint16_t keycodes[], uint8_t size) {
    if (leader_index != size) {
        return false;
    }
    return leader_hash == leader_hash_generate(keycodes, size);
}

uint32_t leader_hash_get(void) {
    return leader_hash;
}

uint8_t leader_hash_length(void) {
    return leader_index;
}
