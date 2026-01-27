#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "accumulators.h"

#ifndef GESTURE_THRESHOLD
#define GESTURE_THRESHOLD 450
#endif

#ifndef GESTURE_COOLDOWN
#define GESTURE_COOLDOWN 300
#endif

typedef enum {
    GESTURE_NONE,
    GESTURE_LEFT,
    GESTURE_RIGHT,
    GESTURE_UP,
    GESTURE_DOWN
} gesture_t;

typedef struct {
    accumulator_t acc;
    uint16_t last_trigger;
    int16_t threshold;
    uint16_t cooldown;
} gesture_detector_t;

void gesture_init(gesture_detector_t *det, int16_t threshold, uint16_t cooldown);
gesture_t gesture_detect(gesture_detector_t *det, int16_t dx, int16_t dy);
bool gesture_ready(gesture_detector_t *det);
void gesture_trigger(gesture_detector_t *det);
