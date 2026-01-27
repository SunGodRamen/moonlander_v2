#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    LOCK_STATE_IDLE      = 0b000,
    LOCK_STATE_ML_NAV    = 0b001,
    LOCK_STATE_ML_NUM    = 0b010,
    LOCK_STATE_ML_MACRO  = 0b011,
    LOCK_STATE_PA_SCROLL = 0b100,
    LOCK_STATE_PA_ZOOM   = 0b101,
    LOCK_STATE_PA_MEDIA  = 0b110,
    LOCK_STATE_SYNC_REQ  = 0b111
} lock_state_t;

typedef enum {
    LOCK_ROLE_PRIMARY,
    LOCK_ROLE_SECONDARY
} lock_role_t;

#ifndef LOCKSTATE_POLL_INTERVAL
#define LOCKSTATE_POLL_INTERVAL 50
#endif

#ifndef LOCKSTATE_TIMEOUT
#define LOCKSTATE_TIMEOUT 500
#endif

#ifndef LOCKSTATE_SYNC_HOLD
#define LOCKSTATE_SYNC_HOLD 1000
#endif

typedef struct {
    lock_state_t cached_state;
    lock_role_t role;
    uint16_t last_change_time;
    uint16_t last_poll_time;
    bool sync_requested;
} lockstate_state_t;

extern lockstate_state_t lockstate;

void lockstate_init(lock_role_t role);
void lockstate_set(lock_state_t state);
lock_state_t lockstate_get(void);
void lockstate_task(void);
lock_state_t lockstate_cached(void);
bool lockstate_is_owned(lock_state_t state);
void lockstate_sync_request(void);
const char* lockstate_name(lock_state_t state);
uint16_t lockstate_elapsed(void);

static inline bool lockstate_is_moonlander(lock_state_t s) {
    return s <= LOCK_STATE_ML_MACRO;
}

static inline bool lockstate_is_ploopy(lock_state_t s) {
    return s >= LOCK_STATE_PA_SCROLL && s < LOCK_STATE_SYNC_REQ;
}

void lockstate_on_remote_change(lock_state_t old_state, lock_state_t new_state);
void lockstate_on_sync_request(void);

#ifdef LOGGING_ENABLE
void lockstate_log_change(lock_state_t old_state, lock_state_t new_state);
void lockstate_debug_dump(void);
#endif
