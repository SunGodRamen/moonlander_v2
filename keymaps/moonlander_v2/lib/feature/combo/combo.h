/**
 * @file combo.h
 * @brief Custom combo implementation with COMB/SUBS/TOGG macros
 * 
 * Supports:
 * - COMB(name, keycode, key1, key2, ...) - Standard combo
 * - SUBS(name, string, key1, key2, ...) - String substitution combo
 * - TOGG(name, layer, key1, key2, ...) - Layer toggle combo
 * - COMBO_REF_LAYER(layer, ref) - Reference layer mapping
 * - DEFAULT_REF_LAYER(layer) - Default reference layer
 */

#pragma once

#include "quantum.h"
#include "../core/keycodes.h"

// ═══════════════════════════════════════════════════════════════════════════
// Macro Definitions for combos.def processing
// ═══════════════════════════════════════════════════════════════════════════

// Reference layer macros (processed separately)
#define REF_LAYER_FOR_LAYER(LAYER, REF_LAYER) case LAYER: return REF_LAYER;
#define DEF_REF_LAYER(LAYER) default: return LAYER;

// Generator macros for enums
#define K_ENUM(name, key, ...) name,
#define A_ENUM(name, string, ...) name,

// Generator macros for combo data arrays
#define K_DATA(name, key, ...) const uint16_t PROGMEM cmb_##name[] = {__VA_ARGS__, COMBO_END};
#define A_DATA(name, string, ...) const uint16_t PROGMEM cmb_##name[] = {__VA_ARGS__, COMBO_END};

// Generator macros for combo array initialization
#define K_COMB(name, key, ...) [name] = COMBO(cmb_##name, key),
#define A_COMB(name, string, ...) [name] = COMBO_ACTION(cmb_##name),

// Generator macros for combo actions
#define A_ACTI(name, string, ...) \
    case name: \
        if (pressed) SEND_STRING(string); \
        break;

#define A_TOGG(name, layer, ...) \
    case name: \
        if (pressed) layer_invert(layer); \
        break;

// Blank macro for selective processing
#define BLANK(...)

// ═══════════════════════════════════════════════════════════════════════════
// First pass: Setup reference layer macros as blank
// ═══════════════════════════════════════════════════════════════════════════
#undef COMBO_REF_LAYER
#undef DEFAULT_REF_LAYER
#define COMBO_REF_LAYER BLANK
#define DEFAULT_REF_LAYER BLANK

// ═══════════════════════════════════════════════════════════════════════════
// Generate combo enum
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#define COMB K_ENUM
#define SUBS A_ENUM
#define TOGG A_ENUM

enum combo_names {
#include "combos.def"
    COMBO_COUNT
};

// ═══════════════════════════════════════════════════════════════════════════
// Generate combo key arrays
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#define COMB K_DATA
#define SUBS A_DATA
#define TOGG A_DATA

#include "combos.def"

// ═══════════════════════════════════════════════════════════════════════════
// Generate combo_t array
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#define COMB K_COMB
#define SUBS A_COMB
#define TOGG A_COMB

combo_t key_combos[COMBO_COUNT] = {
#include "combos.def"
};

// ═══════════════════════════════════════════════════════════════════════════
// Generate process_combo_event handler
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#define COMB BLANK
#define SUBS A_ACTI
#define TOGG A_TOGG

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch (combo_index) {
#include "combos.def"
        default:
            break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Generate combo_ref_from_layer function
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#undef COMBO_REF_LAYER
#undef DEFAULT_REF_LAYER
#define COMB BLANK
#define SUBS BLANK
#define TOGG BLANK
#define COMBO_REF_LAYER REF_LAYER_FOR_LAYER
#define DEFAULT_REF_LAYER DEF_REF_LAYER

uint8_t combo_ref_from_layer(uint8_t current_layer) {
    switch (current_layer) {
#include "combos.def"
    }
    return current_layer;
}

// ═══════════════════════════════════════════════════════════════════════════
// Cleanup macros
// ═══════════════════════════════════════════════════════════════════════════
#undef COMB
#undef SUBS
#undef TOGG
#undef COMBO_REF_LAYER
#undef DEFAULT_REF_LAYER
