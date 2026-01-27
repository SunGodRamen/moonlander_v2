#include "gestures.h"
#include QMK_KEYBOARD_H
#include <stdlib.h>

void gesture_init(gesture_detector_t *det, int16_t threshold, uint16_t cooldown) {
    accumulator_init(&det->acc);
    det->last_trigger = 0;
    det->threshold = threshold;
    det->cooldown = cooldown;
}

gesture_t gesture_detect(gesture_detector_t *det, int16_t dx, int16_t dy) {
    accumulator_add(&det->acc, dx, dy);
    
    if (!gesture_ready(det)) return GESTURE_NONE;
    
    if (det->acc.x > det->threshold) {
        gesture_trigger(det);
        return GESTURE_RIGHT;
    } else if (det->acc.x < -det->threshold) {
        gesture_trigger(det);
        return GESTURE_LEFT;
    } else if (det->acc.y > det->threshold) {
        gesture_trigger(det);
        return GESTURE_DOWN;
    } else if (det->acc.y < -det->threshold) {
        gesture_trigger(det);
        return GESTURE_UP;
    }
    
    return GESTURE_NONE;
}

bool gesture_ready(gesture_detector_t *det) {
    return timer_elapsed(det->last_trigger) > det->cooldown;
}

void gesture_trigger(gesture_detector_t *det) {
    det->last_trigger = timer_read();
    accumulator_reset(&det->acc);
}
