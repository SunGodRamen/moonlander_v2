# Moonlander Dvorak Keymap

## Quick Start

```bash
# From qmk_firmware directory
cp -r /path/to/this/folder keyboards/zsa/moonlander/keymaps/joe
qmk compile -kb zsa/moonlander -km joe
qmk flash -kb zsa/moonlander -km joe
```

## Layout Overview

### Base Layer (Dvorak)
```
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐     ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ F1  │ F2  │ F3  │ F4  │ F5  │ F6  │     │     │     │ F7  │ F8  │ F9  │ F10 │ F11 │ F12 │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │  ;  │  ,  │  .  │  P  │  Y  │REC2 │     │REC1 │  F  │  G  │  C  │  R  │  L  │     │
├─────┼─────┼─────┼─────┼─────┼─────┼─────┤     ├─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │SH(A)│ A/O │ G/E │ C/U │  I  │PLY2 │     │PLY1 │  D  │  H  │  T  │  N  │SH(S)│     │
├─────┼─────┼─────┼─────┼─────┼─────┴─────┘     └─────┴─────┼─────┼─────┼─────┼─────┼─────┤
│     │  '  │  Q  │  J  │  K  │  X  │                 │  B  │  M  │  W  │  V  │  Z  │     │
├─────┼─────┼─────┼─────┼─────┼─────┤                 ├─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │     │     │     │     │                 │     │     │     │     │     │     │
└─────┴─────┴─────┴─────┴─────┼─────┤                 ├─────┼─────┴─────┴─────┴─────┴─────┘
                              │     │                 │     │
                  ┌───────────┼─────┼─────┐     ┌─────┼─────┼───────────┐
                  │    TAB    │LSFT │     │     │LEAD │NV/SP│   NUM     │
                  └───────────┴─────┴─────┘     └─────┴─────┴───────────┘
```

### Key Features

| Feature | Description |
|---------|-------------|
| **Home Row Mods** | O=Alt, E=GUI, U=Ctrl (left hand only) |
| **Swap Hands** | Hold A or S to mirror keyboard |
| **Nav Layer** | Hold Space for arrows, browser nav |
| **Num Layer** | Right thumb for calculator numpad |
| **Leader Key** | Right thumb outer for sequences |
| **Counter Keys** | INCR/DECR/TARE/VALU on num layer |
| **Dynamic Macros** | REC1/REC2, PLY1/PLY2 on base layer |

## Layers

| Layer | Name | Access |
|-------|------|--------|
| 0 | BASE | Default |
| 1 | NAV | Hold Space |
| 2 | NUM | Right thumb |
| 3 | FUNC | (Future) |
| 4 | MACRO | (Future) |
| 5 | MEDIA | (Future) |

## Combos

### Core Editing
| Output | Keys |
|--------|------|
| Backspace | H + T |
| Delete | T + N |
| Tab | H + N |
| Enter | M + W (or K + J) |
| Escape | R + C |
| F6 | H + N + T |

### Symbols
| Output | Keys |
|--------|------|
| / | M + T |
| \ | E + K |
| \| | T + C |
| - | W + V |
| = | M + V |
| ` | . + , |

### Editing Hotkeys
| Output | Keys |
|--------|------|
| Copy | U + O |
| Paste | U + A |
| Save | N + S |
| Find | F + G |

### IDE/Terminal
| Output | Keys |
|--------|------|
| Ctrl+Alt+Space | Nav + H |
| Ctrl+Shift+Space | Nav + T |
| Ctrl+Shift+P | Nav + N |
| Ctrl+` | Nav + W |
| Snipping Tool | Nav + M |

### Number Layer Brackets
| Output | Keys |
|--------|------|
| ( | 4 + 5 |
| ) | 5 + 6 |

## Leader Sequences

Press LEAD (right outer thumb), then type the sequence:

### Git
| Sequence | Output |
|----------|--------|
| G → S | `git status` |
| G → A | `git add .` |
| G → C | `git commit -m "` |
| G → P | `git push` |
| G → L | `git pull` |
| G → D | `git diff` |
| G → O | `git log --oneline -20` |

### Symbols
| Sequence | Output |
|----------|--------|
| S → R | `->` |
| S → L | `<-` |
| S → F | `=>` |
| S → N | `!=` |
| S → E | `==` |
| S → H | `<=` |
| S → S | `>=` |

### Window Management
| Sequence | Output |
|----------|--------|
| W → H | Win+Left |
| W → S | Win+Right |
| W → T | Win+Up |
| W → N | Win+Down |

## Counter Keys

Located on the NUM layer (right side):

| Key | Function |
|-----|----------|
| X_TARE | Reset counter to 1 |
| X_INCR | Increment by 1 (or hold + number for +N) |
| X_DECR | Decrement by 1 (or hold + number for -N) |
| X_VALU | Output current value |

**Usage with Dynamic Macros:**
1. Set counter to starting value (TARE or INCR/DECR)
2. Record macro that includes X_VALU
3. Play macro, which outputs current counter
4. Increment counter
5. Repeat

## RGB

- Per-key: Breathing red effect
- Indicator LEDs: Binary layer display

## Directory Structure

```
moonlander/
├── keymap.c          # Main keymap
├── config.h          # Configuration
├── rules.mk          # Build rules
├── aliases.h         # Key aliases
└── lib/
    ├── core/
    │   ├── layers.h      # Layer definitions
    │   └── keycodes.h    # Custom keycodes
    ├── feature/
    │   ├── combo/
    │   │   ├── combo.h       # Combo processor
    │   │   └── combos.def    # Combo definitions
    │   ├── leader/
    │   │   ├── leader_hash.c # Hash-based leader
    │   │   ├── leader_hash.h
    │   │   ├── sequences.def # Leader sequences
    │   │   └── sequences.h
    │   ├── counter/
    │   │   ├── counter_keys.c
    │   │   └── counter_keys.h
    │   └── rgb/
    │       ├── breathing.c
    │       └── breathing.h
    └── util/
        ├── logger.c
        ├── logger.h
        ├── send_integer.c
        └── send_integer.h
```

## Customization

### Adding Leader Sequences

Edit `lib/feature/leader/sequences.def`:

```c
SEQ(my_sequence, "output text", K_, E_, Y_, S_)
```

### Adding Combos

Edit `lib/feature/combo/combos.def`:

```c
COMB(CMB_NAME, KC_OUTPUT, K1_, K2_)
SUBS(CMB_NAME, "string output", K1_, K2_)
```

### Disabling Features

Edit `rules.mk`:

```makefile
LEADER_HASH_ENABLE = no
COUNTER_KEYS_ENABLE = no
LOGGING_ENABLE = no
```
