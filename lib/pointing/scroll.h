#pragma once
#include <stdint.h>
#include <stdbool.h>

#ifndef SCROLL_SENSITIVITY
#define SCROLL_SENSITIVITY 0.9f
#endif

typedef struct {
    float accum_x;
    float accum_y;
    float sensitivity;
} scroll_state_t;

void scroll_init(scroll_state_t *state, float sensitivity);
void scroll_accumulate(scroll_state_t *state, int16_t dx, int16_t dy);
void scroll_consume(scroll_state_t *state, int8_t *h, int8_t *v);
void scroll_reset(scroll_state_t *state);
