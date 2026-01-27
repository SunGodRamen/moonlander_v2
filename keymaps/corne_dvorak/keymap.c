#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x6_3(
        KC_SCLN, KC_COMM, KC_DOT,  KC_P,    KC_Y,    KC_F,        KC_G,    KC_C,    KC_R,    KC_L,    KC_SLSH, KC_BSPC,
        KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,        KC_H,    KC_T,    KC_N,    KC_S,    KC_MINS, KC_QUOT,
        KC_LSFT, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,        KC_M,    KC_W,    KC_V,    KC_Z,    KC_RSFT, KC_ENT,
                                   KC_TAB,  KC_LGUI, KC_SPC,      LT(1,KC_SPC), MO(2), KC_LALT
    ),
    [1] = LAYOUT_split_3x6_3(  // NAV layer
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, C(KC_LEFT), C(S(KC_TAB)), C(KC_TAB), C(KC_RGHT), KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_TRNS, KC_TRNS,
                                   KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS
    ),
    [2] = LAYOUT_split_3x6_3(  // NUM layer
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_7,    KC_8,    KC_9,    KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_0,    KC_4,    KC_5,    KC_6,    KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_1,    KC_2,    KC_3,    KC_TRNS, KC_TRNS,
                                   KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS
    ),
};
