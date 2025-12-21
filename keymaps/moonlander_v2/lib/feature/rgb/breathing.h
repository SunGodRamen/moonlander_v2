/**
 * @file breathing.h
 * @brief Breathing RGB effect for Moonlander
 * 
 * Implements a red breathing effect on per-key RGB.
 * Indicator LEDs (top 3 per side) show layer state.
 */

#ifndef BREATHING_H
#define BREATHING_H

#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE

// ═══════════════════════════════════════════════════════════════════════════
// Configuration
// ═══════════════════════════════════════════════════════════════════════════

#ifndef BREATHING_MIN_VAL
#define BREATHING_MIN_VAL 20    // Minimum brightness (0-255)
#endif

#ifndef BREATHING_MAX_VAL
#define BREATHING_MAX_VAL 180   // Maximum brightness (0-255)
#endif

#ifndef BREATHING_PERIOD_MS
#define BREATHING_PERIOD_MS 3000  // Full cycle time in ms
#endif

// Color (HSV)
#ifndef BREATHING_HUE
#define BREATHING_HUE 0          // Red
#endif

#ifndef BREATHING_SAT
#define BREATHING_SAT 255        // Full saturation
#endif

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Initialize breathing effect
 * Call from keyboard_post_init_user()
 */
void breathing_init(void);

/**
 * Update breathing effect
 * Call from rgb_matrix_indicators_user() or similar
 */
void breathing_update(void);

/**
 * Get current breathing brightness value
 * @return Brightness 0-255
 */
uint8_t breathing_get_val(void);

#endif // RGB_MATRIX_ENABLE

#endif // BREATHING_H
