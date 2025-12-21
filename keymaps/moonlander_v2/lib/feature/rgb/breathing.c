/**
 * @file breathing.c
 * @brief Breathing RGB effect implementation
 */

#include "breathing.h"

#ifdef RGB_MATRIX_ENABLE

#include <math.h>

// ═══════════════════════════════════════════════════════════════════════════
// Internal State
// ═══════════════════════════════════════════════════════════════════════════

static uint16_t breathing_timer = 0;
static bool breathing_initialized = false;

// ═══════════════════════════════════════════════════════════════════════════
// Implementation
// ═══════════════════════════════════════════════════════════════════════════

void breathing_init(void) {
    breathing_timer = timer_read();
    breathing_initialized = true;
}

uint8_t breathing_get_val(void) {
    if (!breathing_initialized) {
        return BREATHING_MAX_VAL;
    }
    
    // Calculate position in breathing cycle (0.0 to 1.0)
    uint16_t elapsed = timer_elapsed(breathing_timer) % BREATHING_PERIOD_MS;
    float phase = (float)elapsed / (float)BREATHING_PERIOD_MS;
    
    // Use sine wave for smooth breathing (0 to 2π)
    // sin returns -1 to 1, we need 0 to 1
    float sine_val = (sinf(phase * 2.0f * 3.14159f) + 1.0f) / 2.0f;
    
    // Map to brightness range
    uint8_t range = BREATHING_MAX_VAL - BREATHING_MIN_VAL;
    return BREATHING_MIN_VAL + (uint8_t)(sine_val * range);
}

void breathing_update(void) {
    if (!breathing_initialized) {
        breathing_init();
    }
    
    // Get current breathing value
    uint8_t val = breathing_get_val();
    
    // Create HSV color
    HSV hsv = {
        .h = BREATHING_HUE,
        .s = BREATHING_SAT,
        .v = val
    };
    
    // Convert to RGB
    RGB rgb = hsv_to_rgb(hsv);
    
    // Apply to all keys
    // Note: This should be called from rgb_matrix_indicators_user()
    // which handles the actual LED updates
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
}

#endif // RGB_MATRIX_ENABLE
