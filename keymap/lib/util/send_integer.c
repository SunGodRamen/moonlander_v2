/**
 * @file send_integer.c
 * @brief Implementation for sending integers as keystrokes
 */

#include "send_integer.h"

void send_integer_as_keycodes(int16_t value) {
    // Handle negative numbers
    if (value < 0) {
        tap_code(KC_MINUS);
        value = -value;
    }
    
    // Handle zero specially
    if (value == 0) {
        tap_code(KC_0);
        return;
    }
    
    // Convert to string (max 5 digits for int16_t)
    char buffer[7];
    int8_t i = 0;
    
    // Build digits in reverse
    while (value > 0 && i < 6) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    
    // Send in correct order
    while (i > 0) {
        char c = buffer[--i];
        if (c == '0') {
            tap_code(KC_0);
        } else {
            tap_code(KC_1 + (c - '1'));
        }
    }
}

void send_integer_padded(int16_t value, uint8_t width) {
    // Handle negative
    bool negative = value < 0;
    if (negative) {
        value = -value;
        width--; // Account for minus sign
    }
    
    // Count digits
    int16_t temp = value;
    uint8_t digits = (temp == 0) ? 1 : 0;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    
    // Send minus if needed
    if (negative) {
        tap_code(KC_MINUS);
    }
    
    // Send leading zeros
    while (digits < width) {
        tap_code(KC_0);
        width--;
    }
    
    // Send the actual number
    send_integer_as_keycodes(value);
}
