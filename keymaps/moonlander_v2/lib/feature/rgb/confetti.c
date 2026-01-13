/**
 * @file confetti.c
 * @brief RGB confetti particle effect implementation
 * 
 * Physics-based confetti that launches from the right hand
 * and arcs across to land on the left side with realistic motion.
 */

#include "confetti.h"

#ifdef RGB_MATRIX_ENABLE

#include <stdlib.h>

// ═══════════════════════════════════════════════════════════════════════════
// MOONLANDER LED LAYOUT
// ═══════════════════════════════════════════════════════════════════════════

// Moonlander has 72 LEDs total
// Left hand: LEDs 0-35 (6 rows × 6 cols)
// Right hand: LEDs 36-71 (6 rows × 6 cols)

#define LEFT_HAND_START  0
#define LEFT_HAND_END    35
#define RIGHT_HAND_START 36
#define RIGHT_HAND_END   71

#define HAND_WIDTH  6   // 6 columns per hand
#define HAND_HEIGHT 6   // 6 rows per hand

// Virtual coordinate system spans both hands:
// X: 0-11 (0-5 = left hand, 6-11 = right hand)
// Y: 0-5 (rows, 0 = top)

// ═══════════════════════════════════════════════════════════════════════════
// PARTICLE DEFINITION
// ═══════════════════════════════════════════════════════════════════════════

typedef struct {
    // Position (fixed-point: multiply by 16 for sub-LED precision)
    int16_t x;          // Horizontal position (0-95: 0-5 = right hand, 6-11 = left hand)
    int16_t y;          // Vertical position (0-95: 0-5 rows × 16)
    
    // Velocity (fixed-point)
    int16_t vx;         // Horizontal velocity (negative = leftward)
    int16_t vy;         // Vertical velocity (positive = downward initially)
    
    // Visual
    uint8_t hue;        // Color hue (0-255)
    uint8_t sat;        // Saturation (0-255)
    
    // State
    bool active;        // Is particle alive?
    uint8_t brightness; // Current brightness (fades on landing)
} confetti_particle_t;

// ═══════════════════════════════════════════════════════════════════════════
// PHYSICS CONSTANTS (all fixed-point: multiply by 16)
// ═══════════════════════════════════════════════════════════════════════════

#define GRAVITY         2       // Downward acceleration per frame
#define INITIAL_VX_MIN  -12     // Minimum leftward velocity
#define INITIAL_VX_MAX  -6      // Maximum leftward velocity
#define INITIAL_VY_MIN  -20     // Minimum upward velocity (negative = up)
#define INITIAL_VY_MAX  -8      // Maximum upward velocity
#define FRICTION        1       // Horizontal drag (divide vx by this on landing)
#define BOUNCE_DAMP     2       // Vertical bounce damping (divide vy by this)

#define FIXED_SHIFT     4       // Fixed-point shift (16 = 2^4)
#define FIXED_ONE       (1 << FIXED_SHIFT)  // 16 in fixed-point

// ═══════════════════════════════════════════════════════════════════════════
// INTERNAL STATE
// ═══════════════════════════════════════════════════════════════════════════

static confetti_particle_t particles[CONFETTI_PARTICLES];
static bool confetti_is_active = false;
static bool confetti_initialized = false;

// ═══════════════════════════════════════════════════════════════════════════
// RANDOM NUMBER GENERATION
// ═══════════════════════════════════════════════════════════════════════════

static uint16_t rng_seed = 0;

static void confetti_seed_rng(void) {
    rng_seed = timer_read();
}

static uint8_t confetti_rand(uint8_t max) {
    if (max == 0) return 0;
    rng_seed = (rng_seed * 1103515245 + 12345) & 0x7FFFFFFF;
    return (uint8_t)((rng_seed >> 16) % max);
}

static int16_t confetti_rand_range(int16_t min, int16_t max) {
    if (min >= max) return min;
    int16_t range = max - min;
    return min + (int16_t)confetti_rand((uint8_t)range);
}

// ═══════════════════════════════════════════════════════════════════════════
// LED MAPPING
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Convert (x, y) position to LED index
 * x: 0-5 = left hand, 6-11 = right hand
 * y: 0-5 = rows (0 = top)
 */
static uint8_t pos_to_led(int16_t x, int16_t y) {
    // Clamp to valid range
    if (x < 0) x = 0;
    if (x >= (HAND_WIDTH * 2)) x = (HAND_WIDTH * 2) - 1;
    if (y < 0) y = 0;
    if (y >= HAND_HEIGHT) y = HAND_HEIGHT - 1;
    
    // Determine which hand
    if (x < HAND_WIDTH) {
        // Left hand (LEDs 0-35)
        // x: 0-5 maps directly to columns
        return LEFT_HAND_START + (y * HAND_WIDTH) + x;
    } else {
        // Right hand (LEDs 36-71)
        // x: 6-11 maps to local x: 0-5
        uint8_t local_x = x - HAND_WIDTH;
        return RIGHT_HAND_START + (y * HAND_WIDTH) + local_x;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// PUBLIC FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void confetti_init(void) {
    if (confetti_initialized) {
        return;
    }
    
    confetti_seed_rng();
    
    // Clear particle array
    for (uint8_t i = 0; i < CONFETTI_PARTICLES; i++) {
        particles[i].active = false;
        particles[i].x = 0;
        particles[i].y = 0;
        particles[i].vx = 0;
        particles[i].vy = 0;
        particles[i].hue = 0;
        particles[i].sat = 0;
        particles[i].brightness = 0;
    }
    
    confetti_is_active = false;
    confetti_initialized = true;
}

void confetti_trigger(void) {
    if (!confetti_initialized) {
        confetti_init();
    }
    
    confetti_seed_rng();
    confetti_is_active = true;
    
    // Launch particles from random positions on the RIGHT hand (x: 6-11, y: 0-5)
    for (uint8_t i = 0; i < CONFETTI_PARTICLES; i++) {
        // Start position: somewhere on right hand
        // X: 6-11 (right hand), use fixed-point
        particles[i].x = (HAND_WIDTH + confetti_rand(HAND_WIDTH)) * FIXED_ONE;
        // Y: 0-5 (any row)
        particles[i].y = confetti_rand(HAND_HEIGHT) * FIXED_ONE;
        
        // Random leftward and upward velocity
        particles[i].vx = confetti_rand_range(INITIAL_VX_MIN, INITIAL_VX_MAX);
        particles[i].vy = confetti_rand_range(INITIAL_VY_MIN, INITIAL_VY_MAX);
        
        // Random bright color
        particles[i].hue = confetti_rand(255);
        particles[i].sat = 200 + confetti_rand(55);  // 200-255
        particles[i].brightness = 255;
        
        particles[i].active = true;
        
#ifdef LOGGING_ENABLE
        if (i < 3) {  // Only log first 3 particles
            dprintf("Particle %d: x=%d y=%d vx=%d vy=%d hue=%d\n", 
                    i, particles[i].x >> FIXED_SHIFT, particles[i].y >> FIXED_SHIFT,
                    particles[i].vx, particles[i].vy, particles[i].hue);
        }
#endif
    }
}

void confetti_update(void) {
    if (!confetti_is_active) {
        return;
    }
    
    // CRITICAL: Clear all LEDs first so only confetti particles are visible
    for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        rgb_matrix_set_color(i, 0, 0, 0);
    }
    
    bool any_alive = false;
    
    // Update physics for each particle
    for (uint8_t i = 0; i < CONFETTI_PARTICLES; i++) {
        if (!particles[i].active) {
            continue;
        }
        
        any_alive = true;
        
        // Apply gravity (downward acceleration)
        particles[i].vy += GRAVITY;
        
        // Update position
        particles[i].x += particles[i].vx;
        particles[i].y += particles[i].vy;
        
        // Convert to LED coordinates (divide by FIXED_ONE)
        int16_t led_x = particles[i].x >> FIXED_SHIFT;
        int16_t led_y = particles[i].y >> FIXED_SHIFT;
        
        // Boundary checking and bouncing
        
        // Bottom boundary (floor)
        if (particles[i].y >= ((HAND_HEIGHT - 1) * FIXED_ONE)) {
            particles[i].y = (HAND_HEIGHT - 1) * FIXED_ONE;
            
            // Bounce with damping
            particles[i].vy = -particles[i].vy / BOUNCE_DAMP;
            
            // Apply friction to horizontal velocity
            particles[i].vx = particles[i].vx * (FRICTION) / (FRICTION + 1);
            
            // Fade out on each bounce
            if (particles[i].brightness > 30) {
                particles[i].brightness -= 30;
            } else {
                particles[i].active = false;
                continue;
            }
        }
        
        // Left boundary (stop at left edge of left hand)
        if (particles[i].x < 0) {
            particles[i].x = 0;
            particles[i].vx = 0;
            
            // Fade out when hitting left wall
            if (particles[i].brightness > 40) {
                particles[i].brightness -= 40;
            } else {
                particles[i].active = false;
                continue;
            }
        }
        
        // Right boundary (stop at right edge of right hand)
        if (particles[i].x >= ((HAND_WIDTH * 2 - 1) * FIXED_ONE)) {
            particles[i].x = (HAND_WIDTH * 2 - 1) * FIXED_ONE;
            particles[i].vx = -particles[i].vx / 2;
        }
        
        // Top boundary (ceiling)
        if (particles[i].y < 0) {
            particles[i].y = 0;
            particles[i].vy = -particles[i].vy / BOUNCE_DAMP;
        }
        
        // Kill particles that are barely moving and dim
        if (particles[i].brightness < 20 || 
            (abs(particles[i].vx) < 2 && abs(particles[i].vy) < 2)) {
            particles[i].active = false;
            continue;
        }
        
        // Render particle at current position
        uint8_t led = pos_to_led(led_x, led_y);
        
        HSV hsv = {
            .h = particles[i].hue,
            .s = particles[i].sat,
            .v = particles[i].brightness
        };
        
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
    }
    
    // Deactivate when all particles are dead
    if (!any_alive) {
        confetti_is_active = false;
    }
}

bool confetti_active(void) {
    return confetti_is_active;
}

#endif // RGB_MATRIX_ENABLE
