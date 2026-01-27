#include QMK_KEYBOARD_H
#include "lib/ipc/lockstate.h"
#include "lib/pointing/gestures.h"
#include "lib/pointing/accumulators.h"
#include "lib/pointing/cursor.h"
#include "lib/pointing/scroll.h"

enum custom_keycodes {
    ZOOM_MODE = SAFE_RANGE,
    DPI_CYCLE
};

enum { TD_SCROLL_CLICK, TD_MR_CLICK, TD_MEDIA_CTRL, TD_NAV_OVERVIEW };
enum combos { COMBO_CONFIG_LAYER, COMBO_BOOTLOADER };
enum layers { _BASE, _NAV, _SCROLL, _MEDIA, _CONFIG };

static bool is_nav_mode = false;
static bool is_overview_mode = false;
static bool is_scroll_mode = false;
static bool is_zoom_mode = false;

static gesture_detector_t nav_gesture;
static gesture_detector_t media_gesture;
static scroll_state_t scroll_state;
static cursor_state_t cursor_state;

static uint16_t zoom_timer = 0;
static uint16_t boot_combo_timer = 0;

static uint8_t current_dpi_index = 1;
static const uint16_t dpi_levels[] = {400, 800, 1600};
static const uint8_t dpi_levels_count = 3;

static bool cursor_frozen = false;
static bool gestures_disabled = false;

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
    (void)state; (void)user_data;
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
            gesture_init(&media_gesture, 150, 0);
        }
    }
}

void media_ctrl_reset(tap_dance_state_t *state, void *user_data) {
    (void)state; (void)user_data;
    if (IS_LAYER_ON(_MEDIA)) layer_off(_MEDIA);
}

void nav_overview_finished(tap_dance_state_t *state, void *user_data) {
    (void)user_data;
    if (state->count == 1) {
        if (state->pressed) {
            is_nav_mode = true;
            layer_on(_NAV);
            gesture_init(&nav_gesture, 450, 300);
        }
    } else if (state->count == 2) {
        if (state->pressed) {
            is_overview_mode = true;
            layer_on(_NAV);
            gesture_init(&nav_gesture, 450, 300);
        }
    }
}

void nav_overview_reset(tap_dance_state_t *state, void *user_data) {
    (void)state; (void)user_data;
    is_nav_mode = false;
    is_overview_mode = false;
    if (IS_LAYER_ON(_NAV)) layer_off(_NAV);
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_SCROLL_CLICK]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, scroll_click_finished, scroll_click_reset),
    [TD_MR_CLICK]      = ACTION_TAP_DANCE_FN_ADVANCED(NULL, mr_click_finished, NULL),
    [TD_MEDIA_CTRL]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, media_ctrl_finished, media_ctrl_reset),
    [TD_NAV_OVERVIEW]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, nav_overview_finished, nav_overview_reset)
};

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
                if (timer_elapsed(boot_combo_timer) >= 2000) reset_keyboard();
                boot_combo_timer = 0;
            }
            break;
    }
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        ZOOM_MODE,              TD(TD_MEDIA_CTRL),
        TD(TD_SCROLL_CLICK),    TD(TD_MR_CLICK),
        QK_MOUSE_BUTTON_1,      TD(TD_NAV_OVERVIEW)
    ),
    [_NAV] = LAYOUT(_______, _______, _______, _______, _______, _______),
    [_SCROLL] = LAYOUT(_______, _______, _______, _______, _______, _______),
    [_MEDIA] = LAYOUT(_______, _______, _______, _______, _______, _______),
    [_CONFIG] = LAYOUT(DPI_CYCLE, DPI_CYCLE, _______, _______, _______, _______)
};

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
                if (timer_elapsed(zoom_timer) < 200) tap_code(QK_MOUSE_BUTTON_3);
            }
            return false;
        case DPI_CYCLE:
            if (record->event.pressed) {
                current_dpi_index = (current_dpi_index + 1) % dpi_levels_count;
                pointing_device_set_cpi(dpi_levels[current_dpi_index]);
            }
            return false;
    }
    return true;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    lockstate_task();
    
    lock_state_t s = lockstate_cached();
    if (lockstate_is_moonlander(s) || s == LOCK_STATE_IDLE) {
        cursor_frozen = (s == LOCK_STATE_ML_NUM);
        if (s == LOCK_STATE_IDLE) {
            gestures_disabled = false;
            pointing_device_set_cpi(dpi_levels[current_dpi_index]);
        } else if (s == LOCK_STATE_ML_NAV) {
            pointing_device_set_cpi(400);
        } else if (s == LOCK_STATE_ML_MACRO) {
            gestures_disabled = true;
        }
    }
    
    if (cursor_frozen) {
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.v = 0;
        mouse_report.h = 0;
        return mouse_report;
    }
    
    if (is_nav_mode) {
        gesture_t g = gesture_detect(&nav_gesture, mouse_report.x, mouse_report.y);
        if (g == GESTURE_RIGHT) tap_code16(LGUI(KC_N));
        else if (g == GESTURE_LEFT) tap_code16(LGUI(KC_P));
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (is_overview_mode) {
        gesture_t g = gesture_detect(&nav_gesture, mouse_report.x, mouse_report.y);
        if (g == GESTURE_UP) tap_code16(LGUI(KC_TAB));
        else if (g == GESTURE_DOWN) tap_code16(LGUI(LSFT(KC_TAB)));
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (is_scroll_mode) {
        scroll_accumulate(&scroll_state, mouse_report.x, mouse_report.y);
        scroll_consume(&scroll_state, &mouse_report.h, &mouse_report.v);
        mouse_report.x = 0;
        mouse_report.y = 0;
    } else if (IS_LAYER_ON(_MEDIA)) {
        gesture_t g = gesture_detect(&media_gesture, mouse_report.x, mouse_report.y);
        if (g == GESTURE_UP) tap_code(KC_VOLU);
        else if (g == GESTURE_DOWN) tap_code(KC_VOLD);
        else if (g == GESTURE_RIGHT) tap_code(KC_BRIU);
        else if (g == GESTURE_LEFT) tap_code(KC_BRID);
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.v = 0;
        mouse_report.h = 0;
    } else if (is_zoom_mode) {
        mouse_report.v = -mouse_report.y;
        mouse_report.x = 0;
        mouse_report.y = 0;
        mouse_report.h = 0;
    } else {
        int16_t ax = mouse_report.x;
        int16_t ay = mouse_report.y;
        cursor_apply_acceleration(&ax, &ay);
        if (ax > 127) ax = 127;
        if (ax < -127) ax = -127;
        if (ay > 127) ay = 127;
        if (ay < -127) ay = -127;
        mouse_report.x = (mouse_xy_report_t)ax;
        mouse_report.y = (mouse_xy_report_t)ay;
    }
    
    return mouse_report;
}

void keyboard_post_init_user(void) {
    pointing_device_set_cpi(dpi_levels[current_dpi_index]);
    gesture_init(&nav_gesture, 450, 300);
    gesture_init(&media_gesture, 150, 0);
    scroll_init(&scroll_state, 0.9f);
    cursor_init(&cursor_state, 400);
    lockstate_init(LOCK_ROLE_SECONDARY);
}

void lockstate_on_remote_change(lock_state_t old_state, lock_state_t new_state) {
    (void)old_state; (void)new_state;
}

void lockstate_on_sync_request(void) {
    cursor_frozen = false;
    gestures_disabled = false;
    is_scroll_mode = false;
    is_zoom_mode = false;
    if (IS_LAYER_ON(_MEDIA)) layer_off(_MEDIA);
    pointing_device_set_cpi(dpi_levels[current_dpi_index]);
}
