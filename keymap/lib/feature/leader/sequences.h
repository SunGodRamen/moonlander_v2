/**
 * @file sequences.h
 * @brief Preprocessor for human-readable leader sequences
 *
 * This file processes sequences.def and generates:
 * 1. Compile-time hash values for each sequence
 * 2. Switch cases for leader_hash_end_user()
 *
 * Include this file in your keymap.c after defining aliases.
 */

#ifndef SEQUENCES_H
#define SEQUENCES_H

#include "leader_hash.h"

// ═══════════════════════════════════════════════════════════════════════════
// Hash Generation Macros
// ═══════════════════════════════════════════════════════════════════════════

// Helper to count variadic args (up to 8 keys per sequence)
#define SEQ_COUNT_ARGS(...) SEQ_COUNT_ARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define SEQ_COUNT_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, N, ...) N

// Generate hash check case for each sequence
#define SEQ_CASE_2(name, action, k1, k2) \
    { static const uint16_t seq_##name[] = {k1, k2}; \
      if (leader_hash_is(seq_##name, 2)) { SEND_STRING(action); return; } }

#define SEQ_CASE_3(name, action, k1, k2, k3) \
    { static const uint16_t seq_##name[] = {k1, k2, k3}; \
      if (leader_hash_is(seq_##name, 3)) { SEND_STRING(action); return; } }

#define SEQ_CASE_4(name, action, k1, k2, k3, k4) \
    { static const uint16_t seq_##name[] = {k1, k2, k3, k4}; \
      if (leader_hash_is(seq_##name, 4)) { SEND_STRING(action); return; } }

#define SEQ_CASE_5(name, action, k1, k2, k3, k4, k5) \
    { static const uint16_t seq_##name[] = {k1, k2, k3, k4, k5}; \
      if (leader_hash_is(seq_##name, 5)) { SEND_STRING(action); return; } }

// Helper to force expansion before token pasting
#define SEQ_CAT(a, b) SEQ_CAT_(a, b)
#define SEQ_CAT_(a, b) a##b

// Dispatch to appropriate case generator based on arg count
#define SEQ_DISPATCH(name, action, ...) \
    SEQ_CAT(SEQ_CASE_, SEQ_COUNT_ARGS(__VA_ARGS__))(name, action, __VA_ARGS__)

// Main SEQ macro
#define SEQ(name, action, ...) SEQ_DISPATCH(name, action, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════
// User Implementation
// ═══════════════════════════════════════════════════════════════════════════

/**
 * Process leader sequences
 * Call this from leader_hash_end_user() or include the inline version
 */
static inline void process_leader_sequences(void) {
    // Redefine SEQ for case generation
    #undef SEQ
    #define SEQ(name, action, ...) SEQ_DISPATCH(name, action, __VA_ARGS__)

    // Include all sequence definitions
    #include "sequences.def"

    // Restore SEQ macro
    #undef SEQ
}

#endif // SEQUENCES_H
