#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifndef CURSOR_ACCEL_OFFSET
#define CURSOR_ACCEL_OFFSET 10
#endif

#ifndef CURSOR_ACCEL_SLOPE
#define CURSOR_ACCEL_SLOPE 1.5
#endif

#ifndef CURSOR_ACCEL_LIMIT
#define CURSOR_ACCEL_LIMIT 4.0
#endif

typedef struct {
    bool frozen;
    bool precision_mode;
    uint16_t saved_dpi;
    uint16_t precision_dpi;
} cursor_state_t;

void cursor_init(cursor_state_t *state, uint16_t precision_dpi);
void cursor_apply_acceleration(int16_t *x, int16_t *y);
void cursor_freeze(cursor_state_t *state);
void cursor_unfreeze(cursor_state_t *state);
void cursor_set_precision(cursor_state_t *state, bool enable, uint16_t current_dpi);
bool cursor_is_frozen(cursor_state_t *state);
bool cursor_is_precision(cursor_state_t *state);
