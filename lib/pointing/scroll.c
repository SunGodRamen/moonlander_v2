#include "scroll.h"

void scroll_init(scroll_state_t *state, float sensitivity) {
    state->accum_x = 0.0f;
    state->accum_y = 0.0f;
    state->sensitivity = sensitivity;
}

void scroll_accumulate(scroll_state_t *state, int16_t dx, int16_t dy) {
    state->accum_x += (float)dx * state->sensitivity;
    state->accum_y += (float)dy * state->sensitivity;
}

void scroll_consume(scroll_state_t *state, int8_t *h, int8_t *v) {
    *v = (int8_t)(state->accum_y * -1.0f);
    *h = (int8_t)(state->accum_x);
    
    if (*v != 0) state->accum_y -= (float)(*v * -1);
    if (*h != 0) state->accum_x -= (float)(*h);
}

void scroll_reset(scroll_state_t *state) {
    state->accum_x = 0.0f;
    state->accum_y = 0.0f;
}
