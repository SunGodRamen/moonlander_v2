/* ========================================
 * PLOOPY ADEPT KEYMAP - v1.0.1
 * ========================================
 * Target: ploopyco/madromys/rev1_001
 * Notes:
 * - Fixes bootloader combo hold logic (implemented in process_combo_event)
 * - Uses 6-key LAYOUT for Adept
 * ======================================== */

#include QMK_KEYBOARD_H
#include <math.h>
#include <stdlib.h>

/* ========================================
 * CUSTOM KEYCODES
 * ======================================== */
enum custom_keycodes {
    ZOOM_MODE = SAFE_RANGE,
    DPI_CYCLE
};

/* ========================================
 * TAP DANCE ENUMERATION
 * ======================================== */
enum {
    TD_SCROLL_CLICK,
    TD_MR_CLICK,
    TD_MEDIA_CTRL,
    TD_NAV_OVERVIEW
};

/* ========================================
 * COMBO ENUMERATION
 * ======================================== */
enum combos {
    COMBO_CONFIG_LAYER,
    COMBO_BOOTLOADER
};

/* ========================================
 * LAYER DEFINITIONS
 * ======================================== */
enum layers {
    _BASE,
    _NAV,
    _SCROLL,
    _MEDIA,
    _CONFIG
};

/* ========================================
 * STATE VARIABLES
 * ======================================== */
static bool is_nav_mode = false;
static bool is_overview_mode = false;
static bool is_scroll_mode = false;
static bool is_zoom_mode = false;

static int16_t nav_acum_x = 0;
static int16_t nav_acum_y = 0;
static int16_t media_acum_x = 0;
static int16_t media_acum_y = 0;
static float scroll_accum_x = 0.0f;
static float scroll_accum_y = 0.0f;

static uint16_t last_nav_time = 0;
static uint16_t zoom_timer = 0;
static uint16_t boot_combo_timer = 0;

static uint8_t current_dpi_index = DEFAULT_DPI_INDEX;
static const uint16_t dpi_levels[] = {400, 800, 1600};
static const uint8_t dpi_levels_count = sizeof(dpi_levels) / sizeof(dpi_levels[0]);

/* ========================================
 * TAP DANCE IMPLEMENTATIONS
 * ======================================== */
void scroll_click_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;
    if (state->count == 1) {
        if (!state->pressed) {
            tap_code(QK_MOUSE_BUTTON_3);
        } else {
            is_scroll_mode = true;
        }
    } else if (state->count == 2) {
        tap_code(KC_HOME);
    }
}

void scroll_click_reset(tap_dance_state_t *state, void *user_data) {
    (void)state;
    (void)user_data;
    is_scroll_mode = false;
}

void mr_click_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;
    if (state->count == 1) {
        tap_code(QK_MOUSE_BUTTON_2);
    } else if (state->count == 2) {
        tap_code(KC_END);
    }
}

void media_ctrl_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;
    if (state->count == 1) {
        if (!state->pressed) {
            tap_code(QK_MOUSE_BUTTON_4);
        } else {
            layer_on(_MEDIA);
            media_acum_x = 0;
            media_acum_y = 0;
        }
    }
}

void media_ctrl_reset(tap_dance_state_t *state, void *user_data) {
    (void)state;
    (void)user_data;
    if (IS_LAYER_ON(_MEDIA)) {
        layer_off(_MEDIA);
    }
}

void nav_overview_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;
    if (state->count == 1) {
        if (state->pressed) {
            is_nav_mode = true;
            layer_on(_NAV);
            nav_acum_x = 0;
            nav_acum_y = 0;
        }
    } else if (state->count == 2) {
        if (state->pressed) {
            is_overview_mode = true;
            layer_on(_NAV);
            nav_acum_y = 0;
        }
    }
}

void nav_overview_reset(tap_dance_state_t *state, void *user_data) {
    (void)state;
    (void)user_data;
    is_nav_mode = false;
    is_overview_mode = false;
    if (IS_LAYER_ON(_NAV)) {
        layer_off(_NAV);
    }
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SCROLL_CLICK]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, scroll_click_finished, scroll_click_reset),
    [TD_MR_CLICK]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mr_click_finished, NULL),
    [TD_MEDIA_CTRL]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, media_ctrl_finished, media_ctrl_reset),
    [TD_NAV_OVERVIEW]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, nav_overview_finished, nav_overview_reset)
};

/* ========================================
 * COMBO DEFINITIONS
 * ======================================== */
const uint16_t PROGMEM combo_config[]     = {ZOOM_MODE, TD(TD_MEDIA_CTRL), COMBO_END};
const uint16_t PROGMEM combo_bootloader[] = {TD(TD_SCROLL_CLICK), TD(TD_MR_CLICK), COMBO_END};

combo_t key_combos[] = {
    [COMBO_CONFIG_LAYER] = COMBO_ACTION(combo_config),
    [COMBO_BOOTLOADER]   = COMBO_ACTION(combo_bootloader)
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch (combo_index) {
        case COMBO_CONFIG_LAYER:
            if (pressed) layer_on(_CONFIG);
            else layer_off(_CONFIG);
            break;

        case COMBO_BOOTLOADER:
            if (pressed) {
                boot_combo_timer = timer_read();
            } else {
                if (timer_elapsed(boot_combo_timer) >= COMBO_HOLD_TERM) {
                    reset_keyboard();
                }
                boot_combo_timer = 0;
            }
            break;
    }
}

/* ========================================
 * LAYOUT
 * ======================================== */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        ZOOM_MODE,              TD(TD_MEDIA_CTRL),
        TD(TD_SCROLL_CLICK),    TD(TD_MR_CLICK),
        QK_MOUSE_BUTTON_1,      TD(TD_NAV_OVERVIEW)
    ),

    [_NAV] = LAYOUT(
        _______, _______,
        _______, _______,
        _______, _______
    ),

    [_SCROLL] = LAYOUT(
        _______, _______,
        _______, _______,
        _______, _______
    ),

    [_MEDIA] = LAYOUT(
        _______, _______,
        _______, _______,
        _______, _______
    ),

    [_CONFIG] = LAYOUT(
        DPI_CYCLE, DPI_CYCLE,
        _______,  _______,
        _______,  _______
    )
};

/* ========================================
 * KEYCODES
 * ======================================== */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ZOOM_MODE:
            if (record->event.pressed) {
                is_zoom_mode = true;
                zoom_timer = timer_read();
                register_code(KC_LCTL);
                register_code(KC_LSFT);
            } else {
                is_zoom_mode = false;
                unregister_code(KC_LSFT);
                unregister_code(KC_LCTL);
                if (timer_elapsed(zoom_timer) < TAPPING_TERM) {
                    tap_code(QK_MOUSE_BUTTON_3);
                }
            }
            return false;

        case DPI_CYCLE:
            if (record->event.pressed) {
                current_dpi_index = (current_dpi_index + 1) % dpi_levels_count;
                pointing_device_set_cpi(dpi_levels[current_dpi_index]);
            }
            return false;

        default:
            return true;
    }
}

/* ========================================
 * POINTING DEVICE TASK
 * ======================================== */
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(nav_acum_x) > ACCUMULATOR_OVERFLOW_LIMIT) nav_acum_x = 0;
    if (abs(nav_acum_y) > ACCUMULATOR_OVERFLOW_LIMIT) nav_acum_y = 0;
    if (abs(media_acum_x) > ACCUMULATOR_OVERFLOW_LIMIT) media_acum_x = 0;
    if (abs(media_acum_y) > ACCUMULATOR_OVERFLOW_LIMIT) media_acum_y = 0;

    if (timer_elapsed(last_nav_time) > TIMER_ROLLOVER_RESET) {
        last_nav_time = timer_read();
    }

    if (!is_nav_mode && !is_overview_mode && !is_scroll_mode && !IS_LAYER_ON(_MEDIA) && !is_zoom_mode) {
        float speed = sqrtf((float)mouse_report.x * (float)mouse_report.x + (float)mouse_report.y * (float)mouse_report.y);
        if (speed > ACCEL_OFFSET) {
            float factor = 1.0f + powf(speed - ACCEL_OFFSET, 2) * 0.001f * ACCEL_SLOPE;
            if (factor > ACCEL_LIMIT) factor = ACCEL_LIMIT;
            mouse_report.x = (int16_t)((float)mouse_report.x * factor);
            mouse_report.y = (int16_t)((float)mouse_report.y * factor);
        }
    }

    if (is_nav_mode) {
        nav_acum_x += mouse_report.x;
        nav_acum_y += mouse_report.y;

        if (timer_elapsed(last_nav_time) > NAV_COOLDOWN) {
            if (nav_acum_x > NAV_THRESHOLD) {
                tap_code16(LGUI(KC_N));
                last_nav_time = timer_read();
                nav_acum_x = 0;
                nav_acum_y = 0;
            } else if (nav_acum_x < -NAV_THRESHOLD) {
                tap_code16(LGUI(KC_P));
                last_nav_time = timer_read();
                nav_acum_x = 0;
                nav_acum_y = 0;
            }
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (is_overview_mode) {
        nav_acum_y += mouse_report.y;

        if (timer_elapsed(last_nav_time) > NAV_COOLDOWN) {
            if (nav_acum_y < -NAV_THRESHOLD) {
                tap_code16(LGUI(KC_TAB));
                last_nav_time = timer_read();
                nav_acum_y = 0;
            } else if (nav_acum_y > NAV_THRESHOLD) {
                tap_code16(LGUI(LSFT(KC_TAB)));
                last_nav_time = timer_read();
                nav_acum_y = 0;
            }
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (is_scroll_mode) {
        scroll_accum_x += (float)mouse_report.x * SCROLL_SENSITIVITY;
        scroll_accum_y += (float)mouse_report.y * SCROLL_SENSITIVITY;

        int8_t v_scroll = (int8_t)(scroll_accum_y * -1.0f);
        int8_t h_scroll = (int8_t)(scroll_accum_x);

        mouse_report.v = v_scroll;
        mouse_report.h = h_scroll;

        if (mouse_report.v != 0) scroll_accum_y -= (float)(mouse_report.v * -1);
        if (mouse_report.h != 0) scroll_accum_x -= (float)(mouse_report.h);

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (IS_LAYER_ON(_MEDIA)) {
        media_acum_x += mouse_report.x;
        media_acum_y += mouse_report.y;

        if (media_acum_y > MEDIA_THRESHOLD) {
            tap_code(KC_VOLD);
            media_acum_y = 0;
        } else if (media_acum_y < -MEDIA_THRESHOLD) {
            tap_code(KC_VOLU);
            media_acum_y = 0;
        }

        if (media_acum_x > MEDIA_THRESHOLD) {
            tap_code(KC_BRIU);
            media_acum_x = 0;
        } else if (media_acum_x < -MEDIA_THRESHOLD) {
            tap_code(KC_BRID);
            media_acum_x = 0;
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.v = 0;
        mouse_report.h = 0;
    } else if (is_zoom_mode) {
        mouse_report.v = -mouse_report.y;
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.h = 0;
    }

    return mouse_report;
}

void keyboard_post_init_user(void) {
    pointing_device_set_cpi(dpi_levels[DEFAULT_DPI_INDEX]);
}
