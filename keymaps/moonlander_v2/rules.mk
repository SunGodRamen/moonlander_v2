# ═══════════════════════════════════════════════════════════════════════════
# rules.mk - QMK Build Configuration for Moonlander
# ═══════════════════════════════════════════════════════════════════════════

# ───────────────────────────────────────────────────────────────────────────
# CORE FEATURES
# ───────────────────────────────────────────────────────────────────────────

# Combos - Custom key combinations
COMBO_ENABLE = yes

# Dynamic Macros - Record and playback
DYNAMIC_MACRO_ENABLE = yes

# Swap Hands - Mirror keyboard
SWAP_HANDS_ENABLE = yes

# ───────────────────────────────────────────────────────────────────────────
# CUSTOM FEATURES (from lib/)
# ───────────────────────────────────────────────────────────────────────────

# Leader key hash implementation
LEADER_HASH_ENABLE = yes

# Counter keys feature
COUNTER_KEYS_ENABLE = yes

# Logging (comment out for production builds)
LOGGING_ENABLE = yes

# ───────────────────────────────────────────────────────────────────────────
# RGB MATRIX
# ───────────────────────────────────────────────────────────────────────────

RGB_MATRIX_ENABLE = yes

# ───────────────────────────────────────────────────────────────────────────
# CONSOLE / DEBUG
# ───────────────────────────────────────────────────────────────────────────

# Enable console output for logging (disable for production)
CONSOLE_ENABLE = yes

# ───────────────────────────────────────────────────────────────────────────
# SPACE OPTIMIZATION
# ───────────────────────────────────────────────────────────────────────────

# Disable features we don't use
MOUSEKEY_ENABLE = no
EXTRAKEY_ENABLE = yes       # Media keys
NKRO_ENABLE = yes           # N-Key Rollover
AUDIO_ENABLE = no
MUSIC_ENABLE = no

# Link time optimization (smaller firmware)
LTO_ENABLE = yes

# ───────────────────────────────────────────────────────────────────────────
# SOURCE FILES
# ───────────────────────────────────────────────────────────────────────────

# Utility files
SRC += lib/util/logger.c
SRC += lib/util/send_integer.c

# Feature: Leader hash
ifeq ($(strip $(LEADER_HASH_ENABLE)), yes)
    OPT_DEFS += -DLEADER_HASH_ENABLE
    SRC += lib/feature/leader/leader_hash.c
endif

# Feature: Counter keys
ifeq ($(strip $(COUNTER_KEYS_ENABLE)), yes)
    OPT_DEFS += -DCOUNTER_KEYS_ENABLE
    SRC += lib/feature/counter/counter_keys.c
endif

# Feature: Logging
ifeq ($(strip $(LOGGING_ENABLE)), yes)
    OPT_DEFS += -DLOGGING_ENABLE
endif

# Feature: RGB breathing
ifeq ($(strip $(RGB_MATRIX_ENABLE)), yes)
    SRC += lib/feature/rgb/breathing.c
endif

# ───────────────────────────────────────────────────────────────────────────
# VPATH for includes
# ───────────────────────────────────────────────────────────────────────────

VPATH += $(KEYMAP_PATH)/lib
VPATH += $(KEYMAP_PATH)/lib/core
VPATH += $(KEYMAP_PATH)/lib/feature
VPATH += $(KEYMAP_PATH)/lib/util
