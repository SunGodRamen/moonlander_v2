/**
 * @file confetti.h
 * @brief RGB confetti particle effect
 * 
 * Physics-based confetti that launches from the right hand
 * and arcs across the keyboard with realistic motion.
 */

#ifndef CONFETTI_H
#define CONFETTI_H

#include "quantum.h"

#ifdef RGB_MATRIX_ENABLE

// ═══════════════════════════════════════════════════════════════════════════
// CONFIGURATION
// ═══════════════════════════════════════════════════════════════════════════

#ifndef CONFETTI_PARTICLES
#define CONFETTI_PARTICLES 18           // Number of simultaneous particles
#endif

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC API
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Initialize confetti system
 * Call from keyboard_post_init_user()
 */
void confetti_init(void);

/**
 * Trigger a confetti burst
 * Launches particles from the right hand that arc across to the left
 * Call when user presses confetti key
 */
void confetti_trigger(void);

/**
 * Update confetti animation
 * Handles physics simulation and rendering
 * Call from rgb_matrix_indicators_user()
 */
void confetti_update(void);

/**
 * Check if confetti effect is currently active
 * @return true if confetti is animating
 */
bool confetti_active(void);

#endif // RGB_MATRIX_ENABLE

#endif // CONFETTI_H
