#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifndef ACCUMULATOR_OVERFLOW_LIMIT
#define ACCUMULATOR_OVERFLOW_LIMIT 10000
#endif

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t last_reset;
} accumulator_t;

void accumulator_init(accumulator_t *acc);
void accumulator_add(accumulator_t *acc, int16_t dx, int16_t dy);
void accumulator_reset(accumulator_t *acc);
bool accumulator_check_overflow(accumulator_t *acc);
void accumulator_clamp(accumulator_t *acc, int16_t limit);
