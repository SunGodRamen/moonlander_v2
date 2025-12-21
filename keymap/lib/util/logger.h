/**
 * @file logger.h
 * @brief Logging utilities for QMK debugging
 * 
 * Provides conditional logging that compiles out when disabled.
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "quantum.h"
#include "print.h"

// Log levels
typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} log_level_t;

#ifdef LOGGING_ENABLE

// Current log level (can be changed at runtime)
extern log_level_t current_log_level;

// Initialize logging system
void log_init(log_level_t level);

// Set log level
void log_set_level(log_level_t level);

// Logging macros
#define LOG_ERROR(fmt, ...) \
    do { if (current_log_level >= LOG_LEVEL_ERROR) uprintf("[ERR] " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_WARN(fmt, ...) \
    do { if (current_log_level >= LOG_LEVEL_WARN) uprintf("[WRN] " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_INFO(fmt, ...) \
    do { if (current_log_level >= LOG_LEVEL_INFO) uprintf("[INF] " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_DEBUG(fmt, ...) \
    do { if (current_log_level >= LOG_LEVEL_DEBUG) uprintf("[DBG] " fmt "\n", ##__VA_ARGS__); } while(0)

#define LOG_TRACE(fmt, ...) \
    do { if (current_log_level >= LOG_LEVEL_TRACE) uprintf("[TRC] " fmt "\n", ##__VA_ARGS__); } while(0)

// Log keycode event
#define LOG_KEY(keycode, pressed) \
    LOG_DEBUG("Key 0x%04X %s", keycode, pressed ? "pressed" : "released")

#else // LOGGING_ENABLE not defined

// No-op implementations
#define log_init(level) ((void)0)
#define log_set_level(level) ((void)0)
#define LOG_ERROR(fmt, ...) ((void)0)
#define LOG_WARN(fmt, ...) ((void)0)
#define LOG_INFO(fmt, ...) ((void)0)
#define LOG_DEBUG(fmt, ...) ((void)0)
#define LOG_TRACE(fmt, ...) ((void)0)
#define LOG_KEY(keycode, pressed) ((void)0)

#endif // LOGGING_ENABLE

#endif // LOGGER_H
