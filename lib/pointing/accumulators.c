#include "accumulators.h"
#include QMK_KEYBOARD_H
#include <stdlib.h>

void accumulator_init(accumulator_t *acc) {
    acc->x = 0;
    acc->y = 0;
    acc->last_reset = timer_read();
}

void accumulator_add(accumulator_t *acc, int16_t dx, int16_t dy) {
    acc->x += dx;
    acc->y += dy;
}

void accumulator_reset(accumulator_t *acc) {
    acc->x = 0;
    acc->y = 0;
    acc->last_reset = timer_read();
}

bool accumulator_check_overflow(accumulator_t *acc) {
    return (abs(acc->x) > ACCUMULATOR_OVERFLOW_LIMIT || 
            abs(acc->y) > ACCUMULATOR_OVERFLOW_LIMIT);
}

void accumulator_clamp(accumulator_t *acc, int16_t limit) {
    if (acc->x > limit) acc->x = limit;
    if (acc->x < -limit) acc->x = -limit;
    if (acc->y > limit) acc->y = limit;
    if (acc->y < -limit) acc->y = -limit;
}
