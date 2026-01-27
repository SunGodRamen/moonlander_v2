/* ========================================
 * LOCK STATE IPC - API HEADER
 * ========================================
 * Cross-device coordination via keyboard LED states
 * 
 * Protocol: 3-bit encoding using Num/Caps/Scroll locks
 * Devices: Moonlander (primary) ↔ Ploopy Adept (secondary)
 * Latency: ~50ms (poll-based, configurable)
 * ======================================== */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* ========================================
 * PROTOCOL DEFINITIONS
 * ======================================== */

/**
 * @brief Lock state encoding (3-bit message space)
 * 
 * Bit layout: [Scroll(2)][Caps(1)][Num(0)]
 * 
 * States 0-3: Moonlander ownership (keyboard states)
 * States 4-6: Ploopy ownership (trackball states)
 * State 7: Emergency sync request (either device)
 */
typedef enum {
    LOCK_STATE_IDLE      = 0b000,  // Default state, no coordination
    LOCK_STATE_ML_NAV    = 0b001,  // Moonlander in NAV layer
    LOCK_STATE_ML_NUM    = 0b010,  // Moonlander in NUM layer
    LOCK_STATE_ML_MACRO  = 0b011,  // Moonlander recording/playing macro
    LOCK_STATE_PA_SCROLL = 0b100,  // Ploopy in drag scroll mode
    LOCK_STATE_PA_ZOOM   = 0b101,  // Ploopy in zoom mode
    LOCK_STATE_PA_MEDIA  = 0b110,  // Ploopy in media control mode
    LOCK_STATE_SYNC_REQ  = 0b111   // Emergency reset request
} lock_state_t;

/**
 * @brief Device role in coordination protocol
 */
typedef enum {
    LOCK_ROLE_PRIMARY,   // Moonlander (keyboard) - owns states 0-3
    LOCK_ROLE_SECONDARY  // Ploopy (trackball) - owns states 4-6
} lock_role_t;

/* ========================================
 * CONFIGURATION
 * ======================================== */

#ifndef LOCKSTATE_POLL_INTERVAL
#define LOCKSTATE_POLL_INTERVAL 50  // Poll lock state every 50ms (20 Hz)
#endif

#ifndef LOCKSTATE_TIMEOUT
#define LOCKSTATE_TIMEOUT 500       // Stale state timeout (500ms)
#endif

#ifndef LOCKSTATE_SYNC_HOLD
#define LOCKSTATE_SYNC_HOLD 1000    // Hold SYNC_REQ for 1 second
#endif

/* ========================================
 * CORE API
 * ======================================== */

/**
 * @brief Initialize lock state system
 * 
 * Call once in keyboard_post_init_user()
 * Sets initial role and state to IDLE
 * 
 * @param role Device role (PRIMARY or SECONDARY)
 */
void lockstate_init(lock_role_t role);

/**
 * @brief Set lock state (write to OS)
 * 
 * Encodes 3-bit state into Num/Caps/Scroll lock LEDs
 * Updates cached state and timestamp
 * 
 * @param state Lock state to write (0-7)
 */
void lockstate_set(lock_state_t state);

/**
 * @brief Get current lock state (read from OS)
 * 
 * Decodes Num/Caps/Scroll lock LEDs into 3-bit state
 * Does NOT update cache (use lockstate_task for polling)
 * 
 * @return Current lock state (0-7)
 */
lock_state_t lockstate_get(void);

/**
 * @brief Poll lock state and handle changes
 * 
 * Call in matrix_scan_user() for continuous polling
 * Detects remote state changes and invokes callbacks
 * Implements timeout and conflict resolution
 */
void lockstate_task(void);

/**
 * @brief Get cached lock state (no OS read)
 * 
 * Returns last known state from cache
 * Fast but may be stale (use after lockstate_task)
 * 
 * @return Cached lock state (0-7)
 */
lock_state_t lockstate_cached(void);

/**
 * @brief Check if lock state is owned by this device
 * 
 * Primary owns states 0-3, Secondary owns states 4-6
 * State 7 (SYNC_REQ) is ownable by either device
 * 
 * @param state Lock state to check
 * @return true if this device can write this state
 */
bool lockstate_is_owned(lock_state_t state);

/**
 * @brief Trigger emergency synchronization
 * 
 * Writes SYNC_REQ (111) and holds for LOCKSTATE_SYNC_HOLD
 * All devices reset to IDLE when detected
 * Use when devices are out of sync or stuck
 */
void lockstate_sync_request(void);

/* ========================================
 * CALLBACK HOOKS
 * ======================================== */

/**
 * @brief Callback when remote device changes state
 * 
 * Implement this in keymap.c to react to remote state changes
 * Called by lockstate_task() when remote write detected
 * 
 * @param old_state Previous lock state
 * @param new_state New lock state
 */
void lockstate_on_remote_change(lock_state_t old_state, lock_state_t new_state);

/**
 * @brief Callback when SYNC_REQ detected
 * 
 * Implement this in keymap.c to handle emergency reset
 * Called by lockstate_task() when SYNC_REQ (111) detected
 * Should reset all device state to default
 */
void lockstate_on_sync_request(void);

/* ========================================
 * UTILITY FUNCTIONS
 * ======================================== */

/**
 * @brief Get human-readable state name
 * 
 * Useful for logging and debugging
 * 
 * @param state Lock state (0-7)
 * @return String name (e.g., "ML_NAV", "PA_SCROLL")
 */
const char* lockstate_name(lock_state_t state);

/**
 * @brief Check if state represents Moonlander mode
 * 
 * @param state Lock state to check
 * @return true if state is 0-3 (Moonlander)
 */
static inline bool lockstate_is_moonlander(lock_state_t state) {
    return state <= LOCK_STATE_ML_MACRO;
}

/**
 * @brief Check if state represents Ploopy mode
 * 
 * @param state Lock state to check
 * @return true if state is 4-6 (Ploopy)
 */
static inline bool lockstate_is_ploopy(lock_state_t state) {
    return state >= LOCK_STATE_PA_SCROLL && state < LOCK_STATE_SYNC_REQ;
}

/**
 * @brief Get time since last state change
 * 
 * @return Elapsed milliseconds since last lockstate_set()
 */
uint16_t lockstate_elapsed(void);

/* ========================================
 * DEBUG API (LOGGING_ENABLE)
 * ======================================== */

#ifdef LOGGING_ENABLE
/**
 * @brief Log lock state change
 * 
 * Automatically called by lockstate_set() when logging enabled
 * Format: "[LOCKSTATE] OLD_STATE -> NEW_STATE (XXms)"
 */
void lockstate_log_change(lock_state_t old_state, lock_state_t new_state);

/**
 * @brief Dump current lock state info
 * 
 * Prints current state, cached state, elapsed time, role
 * Useful for debugging coordination issues
 */
void lockstate_debug_dump(void);
#endif

/* ========================================
 * INTERNAL STATE (DO NOT ACCESS DIRECTLY)
 * ======================================== */

// Implementation detail - use accessor functions instead
typedef struct {
    lock_state_t cached_state;
    lock_role_t role;
    uint16_t last_change_time;
    uint16_t last_poll_time;
    bool sync_requested;
} lockstate_state_t;

// Extern declaration (defined in lockstate.c)
extern lockstate_state_t lockstate;
