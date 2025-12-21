/**
 * @file send_integer.h
 * @brief Utility for sending integers as keystrokes
 */

#ifndef SEND_INTEGER_H
#define SEND_INTEGER_H

#include "quantum.h"

/**
 * Send an integer as a sequence of digit keypresses
 * Handles negative numbers by prefixing with minus
 * 
 * @param value The integer to send
 */
void send_integer_as_keycodes(int16_t value);

/**
 * Send an integer padded to a specific width
 * 
 * @param value The integer to send
 * @param width Minimum width (pads with leading zeros)
 */
void send_integer_padded(int16_t value, uint8_t width);

#endif // SEND_INTEGER_H
