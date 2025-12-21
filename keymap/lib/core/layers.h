/**
 * @file layers.h
 * @brief Shared layer definitions across all keyboards
 *
 * This file defines the layer enum used across all keymap configurations.
 * Portable between Moonlander, ErgoDox, Ferris Sweep, etc.
 */

#ifndef LAYERS_H
#define LAYERS_H

enum layers {
    _BASE = 0,    // Dvorak base layer
    _NAV,         // Navigation layer (arrows, page up/down, browser nav)
    _NUM,         // Number pad layer
    _FUNC,        // Function keys F1-F12
    _MACRO,       // Dynamic macro layer
    _MEDIA,       // Media controls
    // Reserved for future use
    _LAYER_COUNT  // Always last - used for array sizing
};

// Layer access helpers
#define RA_IS_LAYER_ON_STATE(state, layer) ((state) & (1UL << (layer)))
#define RA_LAYER_MASK(layer) (1UL << (layer))

#endif // LAYERS_H
