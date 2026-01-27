/* ========================================
 * MOONLANDER COORDINATOR - IMPLEMENTATION
 * ========================================
 * Device behavior rules for Moonlander + Ploopy coordination
 * ======================================== */

#include "coordinator.h"
#include "lockstate.h"
#include "lib/core/layers.h"
#include QMK_KEYBOARD_H

#ifdef LOGGING_ENABLE
#include "lib/util/logger.h"
#endif

/* ========================================
 * INTERNAL STATE
 * ======================================== */

coordinator_state_t coordinator_state = {
    .ploopy_scroll_active = false,
    .ploopy_zoom_active = false,
    .ploopy_media_active = false,
    .macro_recording = false,
    .current_layer = _BASE
};

/* ========================================
 * INITIALIZATION
 * ======================================== */

void coordinator_init(void) {
    // Initialize lock state system as PRIMARY (Moonlander)
    lockstate_init(LOCK_ROLE_PRIMARY);
    
    // Reset coordinator state
    coordinator_state.ploopy_scroll_active = false;
    coordinator_state.ploopy_zoom_active = false;
    coordinator_state.ploopy_media_active = false;
    coordinator_state.macro_recording = false;
    coordinator_state.current_layer = _BASE;
    
#ifdef LOGGING_ENABLE
    LOG_INFO("Coordinator initialized - Moonlander PRIMARY");
#endif
}

/* ========================================
 * MOONLANDER STATE BROADCASTING
 * ======================================== */

void coordinator_on_layer_change(uint8_t layer) {
    coordinator_state.current_layer = layer;
    
    // Broadcast layer state to Ploopy
    lock_state_t new_state = LOCK_STATE_IDLE;
    
    switch (layer) {
        case _NAV:
#if COORDINATOR_NAV_ENABLE
            new_state = LOCK_STATE_ML_NAV;
#ifdef LOGGING_ENABLE
            LOG_INFO("NAV layer active - Ploopy precision mode");
#endif
#endif
            break;
        
        case _NUM:
#if COORDINATOR_NUM_ENABLE
            new_state = LOCK_STATE_ML_NUM;
#ifdef LOGGING_ENABLE
            LOG_INFO("NUM layer active - Ploopy cursor freeze");
#endif
#endif
            break;
        
        case _MEDIA:
            // Don't broadcast MEDIA layer unless it's user-initiated
            // (Ploopy-triggered MEDIA layer shouldn't cause feedback loop)
            if (!coordinator_state.ploopy_media_active) {
                // User manually activated MEDIA layer - no coordination needed
            }
            break;
        
        case _BASE:
        case _FUNC:
        default:
            // Return to IDLE for non-coordinated layers
            new_state = LOCK_STATE_IDLE;
            break;
    }
    
    // Write state if changed
    if (new_state != lockstate_cached()) {
        lockstate_set(new_state);
    }
}

void coordinator_on_macro_change(bool recording) {
    coordinator_state.macro_recording = recording;
    
#if COORDINATOR_MACRO_ENABLE
    if (recording) {
        lockstate_set(LOCK_STATE_ML_MACRO);
#ifdef LOGGING_ENABLE
        LOG_INFO("Macro recording - Ploopy safe mode");
#endif
    } else {
        lockstate_set(LOCK_STATE_IDLE);
#ifdef LOGGING_ENABLE
        LOG_INFO("Macro stopped - Ploopy normal mode");
#endif
    }
#endif
}

/* ========================================
 * PLOOPY STATE HANDLERS
 * ======================================== */

void coordinator_on_ploopy_scroll(bool active) {
    coordinator_state.ploopy_scroll_active = active;
    
#if COORDINATOR_SCROLL_ENABLE
    if (active) {
#ifdef LOGGING_ENABLE
        LOG_INFO("Ploopy scroll active - Arrow keys → Page nav");
#endif
        // Arrow key remap is handled in coordinator_process_key()
    } else {
#ifdef LOGGING_ENABLE
        LOG_INFO("Ploopy scroll inactive - Arrow keys normal");
#endif
    }
#endif
}

void coordinator_on_ploopy_zoom(bool active) {
    coordinator_state.ploopy_zoom_active = active;
    
    // Could implement Moonlander-side zoom shortcuts here
    // For now, just track state
#ifdef LOGGING_ENABLE
    LOG_INFO("Ploopy zoom %s", active ? "active" : "inactive");
#endif
}

void coordinator_on_ploopy_media(bool active) {
    coordinator_state.ploopy_media_active = active;
    
#if COORDINATOR_MEDIA_ENABLE
    if (active) {
        // Ploopy activated media mode - sync Moonlander MEDIA layer
        layer_on(_MEDIA);
#ifdef LOGGING_ENABLE
        LOG_INFO("Ploopy media active - Moonlander MEDIA layer ON");
#endif
    } else {
        // Ploopy deactivated media mode - deactivate MEDIA layer
        layer_off(_MEDIA);
#ifdef LOGGING_ENABLE
        LOG_INFO("Ploopy media inactive - Moonlander MEDIA layer OFF");
#endif
    }
#endif
}

/* ========================================
 * LOCK STATE CALLBACK
 * ======================================== */

void lockstate_on_remote_change(lock_state_t old_state, lock_state_t new_state) {
    // Handle Ploopy state changes
    if (lockstate_is_ploopy(new_state) || 
        (lockstate_is_ploopy(old_state) && new_state == LOCK_STATE_IDLE)) {
        
        // Detect state transitions
        bool scroll_active = (new_state == LOCK_STATE_PA_SCROLL);
        bool zoom_active = (new_state == LOCK_STATE_PA_ZOOM);
        bool media_active = (new_state == LOCK_STATE_PA_MEDIA);
        
        // Handle state changes
        if (scroll_active != coordinator_state.ploopy_scroll_active) {
            coordinator_on_ploopy_scroll(scroll_active);
        }
        
        if (zoom_active != coordinator_state.ploopy_zoom_active) {
            coordinator_on_ploopy_zoom(zoom_active);
        }
        
        if (media_active != coordinator_state.ploopy_media_active) {
            coordinator_on_ploopy_media(media_active);
        }
    }
}

void lockstate_on_sync_request(void) {
    // Emergency reset - clear all coordinator state
#ifdef LOGGING_ENABLE
    LOG_WARN("SYNC_REQ - Resetting coordinator state");
#endif
    
    coordinator_state.ploopy_scroll_active = false;
    coordinator_state.ploopy_zoom_active = false;
    coordinator_state.ploopy_media_active = false;
    coordinator_state.macro_recording = false;
    
    // Return to BASE layer
    layer_clear();
    coordinator_state.current_layer = _BASE;
}

/* ========================================
 * TASK LOOP
 * ======================================== */

void coordinator_task(void) {
    // Delegate to lock state polling
    lockstate_task();
}

/* ========================================
 * KEYCODE OVERRIDES
 * ======================================== */

bool coordinator_process_key(uint16_t keycode, keyrecord_t *record) {
#if COORDINATOR_SCROLL_ENABLE
    // Arrow key remap when Ploopy scroll mode active
    if (coordinator_state.ploopy_scroll_active) {
        switch (keycode) {
            case KC_UP:
                if (record->event.pressed) {
                    tap_code(KC_PGUP);
                }
                return false;  // Intercept
            
            case KC_DOWN:
                if (record->event.pressed) {
                    tap_code(KC_PGDN);
                }
                return false;  // Intercept
            
            case KC_LEFT:
            case KC_RIGHT:
                // Let these pass through (horizontal scroll via Ploopy)
                return true;
            
            default:
                break;
        }
    }
#endif
    
    return true;  // Process normally
}

/* ========================================
 * STATE QUERY
 * ======================================== */

bool coordinator_is_active(void) {
    return (lockstate_cached() != LOCK_STATE_IDLE);
}

lock_state_t coordinator_get_state(void) {
    return lockstate_cached();
}

bool coordinator_ploopy_scrolling(void) {
    return coordinator_state.ploopy_scroll_active;
}

bool coordinator_ploopy_zooming(void) {
    return coordinator_state.ploopy_zoom_active;
}

bool coordinator_ploopy_media(void) {
    return coordinator_state.ploopy_media_active;
}

bool coordinator_keys_overridden(void) {
#if COORDINATOR_SCROLL_ENABLE
    return coordinator_state.ploopy_scroll_active;
#else
    return false;
#endif
}

/* ========================================
 * DEBUG
 * ======================================== */

#ifdef LOGGING_ENABLE
void coordinator_debug_dump(void) {
    LOG_INFO("=== Coordinator Debug ===");
    LOG_INFO("Layer:         %d", coordinator_state.current_layer);
    LOG_INFO("Ploopy Scroll: %s", coordinator_state.ploopy_scroll_active ? "YES" : "NO");
    LOG_INFO("Ploopy Zoom:   %s", coordinator_state.ploopy_zoom_active ? "YES" : "NO");
    LOG_INFO("Ploopy Media:  %s", coordinator_state.ploopy_media_active ? "YES" : "NO");
    LOG_INFO("Macro Rec:     %s", coordinator_state.macro_recording ? "YES" : "NO");
    LOG_INFO("Keys Override: %s", coordinator_keys_overridden() ? "YES" : "NO");
    LOG_INFO("========================");
    lockstate_debug_dump();
}
#endif
