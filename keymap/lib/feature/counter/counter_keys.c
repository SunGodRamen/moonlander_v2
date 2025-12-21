/**
 * @file counter_keys.c
 * @brief Counter key feature implementation
 */

#include "counter_keys.h"
#include "../../util/send_integer.h"
#include "../../util/logger.h"

// ═══════════════════════════════════════════════════════════════════════════
// Internal State
// ═══════════════════════════════════════════════════════════════════════════

static int16_t counter_value = COUNTER_INITIAL_VALUE;
static bool incr_held = false;
static bool decr_held = false;
static bool number_consumed = false;  // Track if number key was used as modifier

// ═══════════════════════════════════════════════════════════════════════════
// Internal Helpers
// ═══════════════════════════════════════════════════════════════════════════

static void clamp_counter(void) {
    if (counter_value > COUNTER_MAX_VALUE) {
        counter_value = COUNTER_MAX_VALUE;
    } else if (counter_value < COUNTER_MIN_VALUE) {
        counter_value = COUNTER_MIN_VALUE;
    }
}

static bool handle_incr_key(keyrecord_t *record) {
    if (record->event.pressed) {
        incr_held = true;
        number_consumed = false;
        LOG_DEBUG("Counter: INCR held");
    } else {
        incr_held = false;
        if (!number_consumed) {
            // No number was pressed while held - do simple increment
            counter_value++;
            clamp_counter();
            LOG_INFO("Counter: incremented to %d", counter_value);
        }
    }
    return false;  // Consume the key
}

static bool handle_decr_key(keyrecord_t *record) {
    if (record->event.pressed) {
        decr_held = true;
        number_consumed = false;
        LOG_DEBUG("Counter: DECR held");
    } else {
        decr_held = false;
        if (!number_consumed) {
            // No number was pressed while held - do simple decrement
            counter_value--;
            clamp_counter();
            LOG_INFO("Counter: decremented to %d", counter_value);
        }
    }
    return false;  // Consume the key
}

static bool handle_tare_key(keyrecord_t *record) {
    if (record->event.pressed) {
        counter_value = COUNTER_INITIAL_VALUE;
        LOG_INFO("Counter: reset to %d", counter_value);
    }
    return false;  // Consume the key
}

static bool handle_valu_key(keyrecord_t *record) {
    if (record->event.pressed) {
        counter_output();
        LOG_INFO("Counter: output value %d", counter_value);
    }
    return false;  // Consume the key
}

static bool handle_number_key(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;  // Only process on press
    }
    
    // Check if we're in modifier mode
    if (!incr_held && !decr_held) {
        return true;  // Not in modifier mode, pass through
    }
    
    // Extract number value (1-9, KC_0 handled separately)
    int16_t num_value;
    if (keycode == KC_0) {
        num_value = 10;  // KC_0 adds 10 when used as modifier
    } else {
        num_value = keycode - KC_1 + 1;  // KC_1 = 1, KC_9 = 9
    }
    
    // Apply modification
    if (incr_held) {
        counter_value += num_value;
        clamp_counter();
        number_consumed = true;
        LOG_INFO("Counter: +%d = %d", num_value, counter_value);
    } else if (decr_held) {
        counter_value -= num_value;
        clamp_counter();
        number_consumed = true;
        LOG_INFO("Counter: -%d = %d", num_value, counter_value);
    }
    
    return false;  // Consume the number key
}

// ═══════════════════════════════════════════════════════════════════════════
// Public API
// ═══════════════════════════════════════════════════════════════════════════

bool process_counter_key(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case X_INCR:
            return handle_incr_key(record);
            
        case X_DECR:
            return handle_decr_key(record);
            
        case X_TARE:
            return handle_tare_key(record);
            
        case X_VALU:
            return handle_valu_key(record);
            
        case KC_1:
        case KC_2:
        case KC_3:
        case KC_4:
        case KC_5:
        case KC_6:
        case KC_7:
        case KC_8:
        case KC_9:
        case KC_0:
            return handle_number_key(keycode, record);
            
        default:
            return true;  // Not a counter key
    }
}

int16_t counter_get_value(void) {
    return counter_value;
}

void counter_set_value(int16_t value) {
    counter_value = value;
    clamp_counter();
}

void counter_reset(void) {
    counter_value = COUNTER_INITIAL_VALUE;
}

void counter_increment(int16_t amount) {
    counter_value += amount;
    clamp_counter();
}

void counter_decrement(int16_t amount) {
    counter_value -= amount;
    clamp_counter();
}

void counter_output(void) {
    send_integer_as_keycodes(counter_value);
}

bool counter_incr_held(void) {
    return incr_held;
}

bool counter_decr_held(void) {
    return decr_held;
}
