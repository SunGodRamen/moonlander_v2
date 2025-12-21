/**
 * @file logger.c
 * @brief Logging implementation
 */

#include "logger.h"

#ifdef LOGGING_ENABLE

log_level_t current_log_level = LOG_LEVEL_INFO;

void log_init(log_level_t level) {
    current_log_level = level;
    LOG_INFO("Logging initialized at level %d", level);
}

void log_set_level(log_level_t level) {
    current_log_level = level;
}

#endif // LOGGING_ENABLE
