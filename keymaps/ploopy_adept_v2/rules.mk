POINTING_DEVICE_ENABLE = yes
MOUSEKEY_ENABLE = yes
MOUSE_EXTENDED_REPORT = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
VIA_ENABLE = no
PLOOPY_DRAGSCROLL_ENABLE = no
LTO_ENABLE = yes

SRC += lib/ipc/lockstate.c
SRC += lib/pointing/gestures.c
SRC += lib/pointing/accumulators.c
SRC += lib/pointing/cursor.c
SRC += lib/pointing/scroll.c
