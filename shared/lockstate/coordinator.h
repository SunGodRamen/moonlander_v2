/* ========================================
 * MOONLANDER COORDINATOR - API HEADER
 * ========================================
 * High-level coordination logic for Moonlander + Ploopy
 * 
 * Responsibilities:
 * - Translate Moonlander layer changes to lock states
 * - React to Ploopy state changes
 * - Implement device-specific behavior rules
 * ======================================== */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "lockstate.h"

/* ========================================
 * CONFIGURATION
 * ======================================== */

/**
 * @brief Enable/disable specific coordination features
 * 
 * Set these in config.h to customize behavior
 */
#ifndef COORDINATOR_NAV_ENABLE
#define COORDINATOR_NAV_ENABLE true   // NAV layer → Ploopy precision mode
#endif

#ifndef COORDINATOR_NUM_ENABLE
#define COORDINATOR_NUM_ENABLE true   // NUM layer → Ploopy cursor freeze
#endif

#ifndef COORDINATOR_MACRO_ENABLE
#define COORDINATOR_MACRO_ENABLE true // Macro recording → Ploopy safe mode
#endif

#ifndef COORDINATOR_SCROLL_ENABLE
#define COORDINATOR_SCROLL_ENABLE true // Ploopy scroll → Arrow key remap
#endif

#ifndef COORDINATOR_MEDIA_ENABLE
#define COORDINATOR_MEDIA_ENABLE true // Ploopy media → Moonlander media layer
#endif

/* ========================================
 * CORE API
 * ======================================== */

/**
 * @brief Initialize coordinator system
 * 
 * Call once in keyboard_post_init_user()
 * Sets up lock state system and default behaviors
 */
void coordinator_init(void);

/**
 * @brief Handle Moonlander layer changes
 * 
 * Call in layer_state_set_user() hook
 * Broadcasts layer state to Ploopy via lock state
 * 
 * @param layer New active layer (0-5: BASE/NAV/NUM/FUNC/MACRO/MEDIA)
 */
void coordinator_on_layer_change(uint8_t layer);

/**
 * @brief Handle dynamic macro state changes
 * 
 * Call when dynamic macro recording starts/stops
 * Broadcasts macro state to Ploopy
 * 
 * @param recording true if recording started, false if stopped
 */
void coordinator_on_macro_change(bool recording);

/**
 * @brief Process coordinator tasks
 * 
 * Call in matrix_scan_user() for polling
 * Delegates to lockstate_task() and handles coordination logic
 */
void coordinator_task(void);

/**
 * @brief Check if coordination is active
 * 
 * @return true if any coordination feature is currently active
 */
bool coordinator_is_active(void);

/**
 * @brief Get current coordination state
 * 
 * @return Current lock state representing coordination status
 */
lock_state_t coordinator_get_state(void);

/* ========================================
 * PLOOPY STATE HANDLERS
 * ======================================== */

/**
 * @brief Handle Ploopy scroll mode activation
 * 
 * Automatically called when Ploopy broadcasts PA_SCROLL
 * Implements Moonlander-side behavior (e.g., arrow key remap)
 */
void coordinator_on_ploopy_scroll(bool active);

/**
 * @brief Handle Ploopy zoom mode activation
 * 
 * Automatically called when Ploopy broadcasts PA_ZOOM
 * Could implement Moonlander-side zoom shortcuts
 */
void coordinator_on_ploopy_zoom(bool active);

/**
 * @brief Handle Ploopy media mode activation
 * 
 * Automatically called when Ploopy broadcasts PA_MEDIA
 * Activates Moonlander MEDIA layer automatically
 */
void coordinator_on_ploopy_media(bool active);

/* ========================================
 * KEYCODE OVERRIDES (Optional)
 * ======================================== */

/**
 * @brief Process keycode with coordination awareness
 * 
 * Call in process_record_user() BEFORE normal keycode handling
 * Intercepts keys for coordination features (e.g., arrow key remap)
 * 
 * @param keycode Key being pressed/released
 * @param record Key record with event data
 * @return true if key should be processed normally, false if intercepted
 */
bool coordinator_process_key(uint16_t keycode, keyrecord_t *record);

/* ========================================
 * STATE QUERY API
 * ======================================== */

/**
 * @brief Check if Ploopy scroll mode is active
 * 
 * @return true if Ploopy is in scroll mode
 */
bool coordinator_ploopy_scrolling(void);

/**
 * @brief Check if Ploopy zoom mode is active
 * 
 * @return true if Ploopy is in zoom mode
 */
bool coordinator_ploopy_zooming(void);

/**
 * @brief Check if Ploopy media mode is active
 * 
 * @return true if Ploopy is in media mode
 */
bool coordinator_ploopy_media(void);

/**
 * @brief Check if coordination has overridden normal key behavior
 * 
 * Useful for conditional RGB effects or layer indicators
 * 
 * @return true if keys are remapped due to coordination
 */
bool coordinator_keys_overridden(void);

/* ========================================
 * DEBUG API
 * ======================================== */

#ifdef LOGGING_ENABLE
/**
 * @brief Dump coordinator state for debugging
 * 
 * Prints active coordination features, Ploopy state, layer state
 */
void coordinator_debug_dump(void);
#endif

/* ========================================
 * INTERNAL STATE
 * ======================================== */

// Implementation detail - use accessor functions
typedef struct {
    bool ploopy_scroll_active;
    bool ploopy_zoom_active;
    bool ploopy_media_active;
    bool macro_recording;
    uint8_t current_layer;
} coordinator_state_t;

extern coordinator_state_t coordinator_state;
