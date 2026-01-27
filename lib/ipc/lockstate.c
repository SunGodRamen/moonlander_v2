#include "lockstate.h"
#include QMK_KEYBOARD_H

#ifdef LOGGING_ENABLE
#include "lib/util/logger.h"
#endif

lockstate_state_t lockstate = {
    .cached_state = LOCK_STATE_IDLE,
    .role = LOCK_ROLE_PRIMARY,
    .last_change_time = 0,
    .last_poll_time = 0,
    .sync_requested = false
};

void lockstate_init(lock_role_t role) {
    lockstate.role = role;
    lockstate.cached_state = LOCK_STATE_IDLE;
    lockstate.last_change_time = timer_read();
    lockstate.last_poll_time = timer_read();
    lockstate.sync_requested = false;
    lockstate_set(LOCK_STATE_IDLE);
#ifdef LOGGING_ENABLE
    LOG_INFO("Lock state init: role=%s", 
             role == LOCK_ROLE_PRIMARY ? "PRIMARY" : "SECONDARY");
#endif
}

void lockstate_set(lock_state_t state) {
    if (state != LOCK_STATE_SYNC_REQ && !lockstate_is_owned(state)) {
#ifdef LOGGING_ENABLE
        LOG_WARN("Attempted to set unowned state: %s", lockstate_name(state));
#endif
        return;
    }
    
    lock_state_t old_state = lockstate.cached_state;
    
    led_t led_state = {
        .num_lock    = (state & 0b001) ? 1 : 0,
        .caps_lock   = (state & 0b010) ? 1 : 0,
        .scroll_lock = (state & 0b100) ? 1 : 0
    };
    
    host_keyboard_leds(led_state);
    lockstate.cached_state = state;
    lockstate.last_change_time = timer_read();
    
#ifdef LOGGING_ENABLE
    lockstate_log_change(old_state, state);
#endif
}

lock_state_t lockstate_get(void) {
    led_t led_state = host_keyboard_led_state();
    uint8_t state = 0;
    state |= led_state.num_lock    ? 0b001 : 0;
    state |= led_state.caps_lock   ? 0b010 : 0;
    state |= led_state.scroll_lock ? 0b100 : 0;
    return (lock_state_t)state;
}

lock_state_t lockstate_cached(void) {
    return lockstate.cached_state;
}

bool lockstate_is_owned(lock_state_t state) {
    if (state == LOCK_STATE_SYNC_REQ) return true;
    if (lockstate.role == LOCK_ROLE_PRIMARY) {
        return state <= LOCK_STATE_ML_MACRO;
    } else {
        return state >= LOCK_STATE_PA_SCROLL && state < LOCK_STATE_SYNC_REQ;
    }
}

void lockstate_task(void) {
    if (timer_elapsed(lockstate.last_poll_time) < LOCKSTATE_POLL_INTERVAL) return;
    lockstate.last_poll_time = timer_read();
    
    lock_state_t current_state = lockstate_get();
    lock_state_t cached_state = lockstate.cached_state;
    
    if (current_state == LOCK_STATE_SYNC_REQ) {
        if (!lockstate.sync_requested) {
#ifdef LOGGING_ENABLE
            LOG_WARN("SYNC_REQ detected - resetting to IDLE");
#endif
            lockstate_on_sync_request();
            lockstate_set(LOCK_STATE_IDLE);
        } else {
            if (timer_elapsed(lockstate.last_change_time) >= LOCKSTATE_SYNC_HOLD) {
                lockstate_set(LOCK_STATE_IDLE);
                lockstate.sync_requested = false;
            }
        }
        return;
    }
    
    if (current_state != cached_state) {
        bool is_remote_change = !lockstate_is_owned(current_state);
        bool is_timeout = timer_elapsed(lockstate.last_change_time) > LOCKSTATE_TIMEOUT;
        
        if (is_remote_change || is_timeout) {
#ifdef LOGGING_ENABLE
            LOG_INFO("Remote state change: %s -> %s", 
                     lockstate_name(cached_state), 
                     lockstate_name(current_state));
#endif
            lockstate.cached_state = current_state;
            lockstate_on_remote_change(cached_state, current_state);
        } else {
#ifdef LOGGING_ENABLE
            LOG_WARN("State conflict detected - rewriting %s", 
                     lockstate_name(cached_state));
#endif
            lockstate_set(cached_state);
        }
    }
}

void lockstate_sync_request(void) {
#ifdef LOGGING_ENABLE
    LOG_WARN("Requesting emergency sync");
#endif
    lockstate.sync_requested = true;
    lockstate_set(LOCK_STATE_SYNC_REQ);
}

const char* lockstate_name(lock_state_t state) {
    switch (state) {
        case LOCK_STATE_IDLE:      return "IDLE";
        case LOCK_STATE_ML_NAV:    return "ML_NAV";
        case LOCK_STATE_ML_NUM:    return "ML_NUM";
        case LOCK_STATE_ML_MACRO:  return "ML_MACRO";
        case LOCK_STATE_PA_SCROLL: return "PA_SCROLL";
        case LOCK_STATE_PA_ZOOM:   return "PA_ZOOM";
        case LOCK_STATE_PA_MEDIA:  return "PA_MEDIA";
        case LOCK_STATE_SYNC_REQ:  return "SYNC_REQ";
        default:                   return "UNKNOWN";
    }
}

uint16_t lockstate_elapsed(void) {
    return timer_elapsed(lockstate.last_change_time);
}

__attribute__((weak)) void lockstate_on_remote_change(lock_state_t old_state, lock_state_t new_state) {
    (void)old_state; (void)new_state;
}

__attribute__((weak)) void lockstate_on_sync_request(void) {}

#ifdef LOGGING_ENABLE
void lockstate_log_change(lock_state_t old_state, lock_state_t new_state) {
    if (old_state != new_state) {
        LOG_INFO("[LOCKSTATE] %s -> %s (%ums)", 
                 lockstate_name(old_state), 
                 lockstate_name(new_state),
                 lockstate_elapsed());
    }
}

void lockstate_debug_dump(void) {
    lock_state_t current = lockstate_get();
    LOG_INFO("=== Lock State Debug ===");
    LOG_INFO("Current:  %s (0x%02X)", lockstate_name(current), current);
    LOG_INFO("Cached:   %s (0x%02X)", lockstate_name(lockstate.cached_state), lockstate.cached_state);
    LOG_INFO("Role:     %s", lockstate.role == LOCK_ROLE_PRIMARY ? "PRIMARY" : "SECONDARY");
    LOG_INFO("Elapsed:  %ums", lockstate_elapsed());
    LOG_INFO("=======================");
}
#endif
