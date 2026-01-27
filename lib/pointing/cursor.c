#include "cursor.h"
#include QMK_KEYBOARD_H
#include <math.h>

void cursor_init(cursor_state_t *state, uint16_t precision_dpi) {
    state->frozen = false;
    state->precision_mode = false;
    state->saved_dpi = 0;
    state->precision_dpi = precision_dpi;
}

void cursor_apply_acceleration(int16_t *x, int16_t *y) {
    float speed = sqrtf((float)(*x) * (float)(*x) + (float)(*y) * (float)(*y));
    if (speed > CURSOR_ACCEL_OFFSET) {
        float factor = 1.0f + powf(speed - CURSOR_ACCEL_OFFSET, 2) * 0.001f * CURSOR_ACCEL_SLOPE;
        if (factor > CURSOR_ACCEL_LIMIT) factor = CURSOR_ACCEL_LIMIT;
        *x = (int16_t)((float)(*x) * factor);
        *y = (int16_t)((float)(*y) * factor);
    }
}

void cursor_freeze(cursor_state_t *state) {
    state->frozen = true;
}

void cursor_unfreeze(cursor_state_t *state) {
    state->frozen = false;
}

void cursor_set_precision(cursor_state_t *state, bool enable, uint16_t current_dpi) {
    if (enable && !state->precision_mode) {
        state->saved_dpi = current_dpi;
        pointing_device_set_cpi(state->precision_dpi);
        state->precision_mode = true;
    } else if (!enable && state->precision_mode) {
        pointing_device_set_cpi(state->saved_dpi);
        state->precision_mode = false;
    }
}

bool cursor_is_frozen(cursor_state_t *state) {
    return state->frozen;
}

bool cursor_is_precision(cursor_state_t *state) {
    return state->precision_mode;
}
