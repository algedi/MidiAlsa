# kui_midi_alsa

byte1       byte2                     byte3                     Command name

0x80-0x8F   Key # (0-127)             Off Velocity (0-127)      Note OFF
0x90-0x90   Key # (0-127)             On Velocity (0-127)       Note ON
0xA0-0xA0   Key # (0-127)             Pressure (0-127)          Poly Key Pressure
0xB0-0xB0   Control # (0-127)         Control Value (0-127)     Control Change
0xC0-0xC0   Program # (0-127)         Not Used (send 0)         Program Change
0xD0-0xD0   Pressure Value (0-127)    Not Used (send 0)         Mono Key Pressure (Channel Pressure)
0xE0-0xE0   Range LSB (0-127)         Range MSB (0-127)         Pitch Bend
