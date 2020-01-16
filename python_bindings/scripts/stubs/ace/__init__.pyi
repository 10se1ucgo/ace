import ace
import ace.gl
import ace.glm
from typing import *
from typing import Iterable as iterable, Iterator as iterator
import enum

__all__  = [
"Button",
"Client",
"Key",
"Keymod",
"BuiltinImporter",
"draw",
"editor",
"font",
"gl",
"glm",
"map"
]
class Button(enum.IntEnum, int, enum.Enum):
    """An enumeration."""
    LEFT = 1
    MIDDLE = 2
    RIGHT = 3
    X1 = 4
    X2 = 5
    pass

class Client():
    def height(self) -> int: 
        """height(self: ace.Client) -> int
"""

    def quit(self) -> None: 
        """quit(self: ace.Client) -> None
"""

    def shader(self, arg0: str) -> ace.gl.ShaderProgram:
        """shader(self: ace.Client, arg0: str) -> ace.gl.ShaderProgram
"""

    def size(self) -> ace.glm.vec2:
        """size(self: ace.Client) -> ace.glm.vec2
"""

    def width(self) -> int: 
        """width(self: ace.Client) -> int
"""

    @property
    def exclusive_mouse(self) -> bool:
        """
:type: bool"""
    @exclusive_mouse.setter
    def exclusive_mouse(self, arg1: bool) -> None:
        ...

    @property
    def input_buffer(self) -> str:
        """
:type: str"""
    @input_buffer.setter
    def input_buffer(self, arg0: str) -> None:
        ...

    @property
    def input_cursor(self) -> int:
        """
:type: int"""
    @input_cursor.setter
    def input_cursor(self, arg0: int) -> None:
        ...

    @property
    def text_input_active(self) -> bool:
        """
:type: bool"""
    @text_input_active.setter
    def text_input_active(self, arg1: bool) -> None:
        ...

    pass

class Key(enum.IntEnum, int, enum.Enum):
    """An enumeration."""
    A = 4
    AC_BACK = 270
    AC_BOOKMARKS = 274
    AC_FORWARD = 271
    AC_HOME = 269
    AC_REFRESH = 273
    AC_SEARCH = 268
    AC_STOP = 272
    AGAIN = 121
    ALTERASE = 153
    APOSTROPHE = 52
    APP1 = 283
    APP2 = 284
    APPLICATION = 101
    AUDIOFASTFORWARD = 286
    AUDIOMUTE = 262
    AUDIONEXT = 258
    AUDIOPLAY = 261
    AUDIOPREV = 259
    AUDIOREWIND = 285
    AUDIOSTOP = 260
    B = 5
    BACKSLASH = 49
    BACKSPACE = 42
    BRIGHTNESSDOWN = 275
    BRIGHTNESSUP = 276
    C = 6
    CALCULATOR = 266
    CANCEL = 155
    CAPSLOCK = 57
    CLEAR = 156
    CLEARAGAIN = 162
    COMMA = 54
    COMPUTER = 267
    COPY = 124
    CRSEL = 163
    CURRENCYSUBUNIT = 181
    CURRENCYUNIT = 180
    CUT = 123
    D = 7
    DECIMALSEPARATOR = 179
    DELETE = 76
    DISPLAYSWITCH = 277
    DOWN = 81
    E = 8
    EJECT = 281
    END = 77
    EQUALS = 46
    ESCAPE = 41
    EXECUTE = 116
    EXSEL = 164
    F = 9
    F1 = 58
    F10 = 67
    F11 = 68
    F12 = 69
    F13 = 104
    F14 = 105
    F15 = 106
    F16 = 107
    F17 = 108
    F18 = 109
    F19 = 110
    F2 = 59
    F20 = 111
    F21 = 112
    F22 = 113
    F23 = 114
    F24 = 115
    F3 = 60
    F4 = 61
    F5 = 62
    F6 = 63
    F7 = 64
    F8 = 65
    F9 = 66
    FIND = 126
    G = 10
    GRAVE = 53
    H = 11
    HELP = 117
    HOME = 74
    I = 12
    INSERT = 73
    INTERNATIONAL1 = 135
    INTERNATIONAL2 = 136
    INTERNATIONAL3 = 137
    INTERNATIONAL4 = 138
    INTERNATIONAL5 = 139
    INTERNATIONAL6 = 140
    INTERNATIONAL7 = 141
    INTERNATIONAL8 = 142
    INTERNATIONAL9 = 143
    J = 13
    K = 14
    KBDILLUMDOWN = 279
    KBDILLUMTOGGLE = 278
    KBDILLUMUP = 280
    KP_0 = 98
    KP_00 = 176
    KP_000 = 177
    KP_1 = 89
    KP_2 = 90
    KP_3 = 91
    KP_4 = 92
    KP_5 = 93
    KP_6 = 94
    KP_7 = 95
    KP_8 = 96
    KP_9 = 97
    KP_A = 188
    KP_AMPERSAND = 199
    KP_AT = 206
    KP_B = 189
    KP_BACKSPACE = 187
    KP_BINARY = 218
    KP_C = 190
    KP_CLEAR = 216
    KP_CLEARENTRY = 217
    KP_COLON = 203
    KP_COMMA = 133
    KP_D = 191
    KP_DBLAMPERSAND = 200
    KP_DBLVERTICALBAR = 202
    KP_DECIMAL = 220
    KP_DIVIDE = 84
    KP_E = 192
    KP_ENTER = 88
    KP_EQUALS = 103
    KP_EQUALSAS400 = 134
    KP_EXCLAM = 207
    KP_F = 193
    KP_GREATER = 198
    KP_HASH = 204
    KP_HEXADECIMAL = 221
    KP_LEFTBRACE = 184
    KP_LEFTPAREN = 182
    KP_LESS = 197
    KP_MEMADD = 211
    KP_MEMCLEAR = 210
    KP_MEMDIVIDE = 214
    KP_MEMMULTIPLY = 213
    KP_MEMRECALL = 209
    KP_MEMSTORE = 208
    KP_MEMSUBTRACT = 212
    KP_MINUS = 86
    KP_MULTIPLY = 85
    KP_OCTAL = 219
    KP_PERCENT = 196
    KP_PERIOD = 99
    KP_PLUS = 87
    KP_PLUSMINUS = 215
    KP_POWER = 195
    KP_RIGHTBRACE = 185
    KP_RIGHTPAREN = 183
    KP_SPACE = 205
    KP_TAB = 186
    KP_VERTICALBAR = 201
    KP_XOR = 194
    L = 15
    LALT = 226
    LANG1 = 144
    LANG2 = 145
    LANG3 = 146
    LANG4 = 147
    LANG5 = 148
    LANG6 = 149
    LANG7 = 150
    LANG8 = 151
    LANG9 = 152
    LCTRL = 224
    LEFT = 80
    LEFTBRACKET = 47
    LGUI = 227
    LSHIFT = 225
    M = 16
    MAIL = 265
    MEDIASELECT = 263
    MENU = 118
    MINUS = 45
    MODE = 257
    MUTE = 127
    N = 17
    NONUSBACKSLASH = 100
    NONUSHASH = 50
    NUMLOCKCLEAR = 83
    NUM_KEYS = 512
    O = 18
    OPER = 161
    OUT = 160
    P = 19
    PAGEDOWN = 78
    PAGEUP = 75
    PASTE = 125
    PAUSE = 72
    PERIOD = 55
    POWER = 102
    PRINTSCREEN = 70
    PRIOR = 157
    Q = 20
    R = 21
    RALT = 230
    RCTRL = 228
    RETURN = 40
    RETURN2 = 158
    RGUI = 231
    RIGHT = 79
    RIGHTBRACKET = 48
    RSHIFT = 229
    S = 22
    SCROLLLOCK = 71
    SELECT = 119
    SEMICOLON = 51
    SEPARATOR = 159
    SLASH = 56
    SLEEP = 282
    SPACE = 44
    STOP = 120
    SYSREQ = 154
    T = 23
    TAB = 43
    THOUSANDSSEPARATOR = 178
    U = 24
    UNDO = 122
    UNKNOWN = 0
    UP = 82
    V = 25
    VOLUMEDOWN = 129
    VOLUMEUP = 128
    W = 26
    WWW = 264
    X = 27
    Y = 28
    Z = 29
    _0 = 39
    _1 = 30
    _2 = 31
    _3 = 32
    _4 = 33
    _5 = 34
    _6 = 35
    _7 = 36
    _8 = 37
    _9 = 38
    pass

class Keymod(enum.IntFlag, int, enum.Flag, enum.Enum):
    """An enumeration."""
    CAPS = 8192
    LALT = 256
    LCTRL = 64
    LGUI = 1024
    LSHIFT = 1
    MODE = 16384
    NONE = 0
    NUM = 4096
    RALT = 512
    RCTRL = 128
    RESERVED = 32768
    RGUI = 2048
    RSHIFT = 2
    pass

class BuiltinImporter():
    """Meta path import for built-in modules.

    All methods are either class or static methods to avoid the need to
    instantiate the class.

    """
    pass
