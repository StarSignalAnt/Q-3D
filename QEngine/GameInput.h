// GameInput.h

#pragma once
#include <glm/glm.hpp>
enum Key {
    // Standard ASCII keys
    KEY_SPACE = 32,
    KEY_APOSTROPHE = 39,
    KEY_COMMA = 44,
    KEY_MINUS = 45,
    KEY_PERIOD = 46,
    KEY_SLASH = 47,
    KEY_0 = 48, KEY_1 = 49, KEY_2 = 50, KEY_3 = 51, KEY_4 = 52,
    KEY_5 = 53, KEY_6 = 54, KEY_7 = 55, KEY_8 = 56, KEY_9 = 57,
    KEY_SEMICOLON = 59,
    KEY_EQUAL = 61,
    KEY_A = 65, KEY_B = 66, KEY_C = 67, KEY_D = 68, KEY_E = 69,
    KEY_F = 70, KEY_G = 71, KEY_H = 72, KEY_I = 73, KEY_J = 74,
    KEY_K = 75, KEY_L = 76, KEY_M = 77, KEY_N = 78, KEY_O = 79,
    KEY_P = 80, KEY_Q = 81, KEY_R = 82, KEY_S = 83, KEY_T = 84,
    KEY_U = 85, KEY_V = 86, KEY_W = 87, KEY_X = 88, KEY_Y = 89,
    KEY_Z = 90,
    KEY_LBRACKET = 91,
    KEY_BACKSLASH = 92,
    KEY_RBRACKET = 93,
    KEY_GRAVE = 96,

    // Custom mapping for special non-printable keys
    KEY_SHIFT = 200,
    KEY_CTRL = 201,
    KEY_ALT = 202,
    KEY_UP = 203,
    KEY_DOWN = 204,
    KEY_LEFT = 205,
    KEY_RIGHT = 206,
    KEY_ESC = 207,
    KEY_BACKSPACE = 208,
    KEY_DELETE = 209,
    KEY_ENTER = 210,
    KEY_TAB = 211,

    // Custom mapping for Qt's special shifted symbol keys
    KEY_EXCLAM = 150, // !
    KEY_AT = 151, // @
    KEY_HASH = 152, // #
    KEY_DOLLAR = 153, // $
    KEY_PERCENT = 154, // %
    KEY_CARET = 155, // ^
    KEY_AMPERSAND = 156, // &
    KEY_ASTERISK = 157, // *
    KEY_LPAREN = 158, // (
    KEY_RPAREN = 159, // )
    KEY_UNDERSCORE = 160, // _
    KEY_PLUS = 161, // +
    KEY_LCURLY = 162, // {
    KEY_RCURLY = 163, // }
    KEY_PIPE = 164, // |
    KEY_COLON = 165, // :
    KEY_DQUOTE = 166, // "
    KEY_LANGLE = 167, // <
    KEY_RANGLE = 168, // >
    KEY_QUESTION = 169, // ?
    KEY_TILDE = 170  // ~
};


class GameInput
{
public:
    static glm::vec2 MousePosition;
    static bool m_Key[1024]; // Array to hold the state of all keys.
    static bool m_Button[16];

private:
};