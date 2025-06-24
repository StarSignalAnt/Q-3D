
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using GlmNet;
using QNet.UI;

namespace QNet.Input
{
    public enum GameKey
    {
        // Standard ASCII keys
        Space = 32,
        Apostrophe = 39,
        Comma = 44,
        Minus = 45,
        Period = 46,
        Slash = 47,
        D0 = 48, D1 = 49, D2 = 50, D3 = 51, D4 = 52,
        D5 = 53, D6 = 54, D7 = 55, D8 = 56, D9 = 57,
        Semicolon = 59,
        Equal = 61,
        A = 65, B = 66, C = 67, D = 68, E = 69,
        F = 70, G = 71, H = 72, I = 73, J = 74,
        K = 75, L = 76, M = 77, N = 78, O = 79,
        P = 80, Q = 81, R = 82, S = 83, T = 84,
        U = 85, V = 86, W = 87, X = 88, Y = 89,
        Z = 90,
        LeftBracket = 91,
        Backslash = 92,
        RightBracket = 93,
        GraveAccent = 96,

        // Custom mapping for special non-printable keys
        LeftShift = 200, RightShift = 200, // Both map to the same code
        LeftControl = 201, RightControl = 201,
        LeftAlt = 202, RightAlt = 202,
        Up = 203,
        Down = 204,
        Left = 205,
        Right = 206,
        Escape = 207,
        Backspace = 208,
        Delete = 209,
        Enter = 210,
        Tab = 211,

        // Custom mapping for Qt's special shifted symbol keys
        Exclam = 150,     // !
        At = 151,         // @
        Hash = 152,       // #
        Dollar = 153,     // $
        Percent = 154,    // %
        Caret = 155,      // ^
        Ampersand = 156,  // &
        Asterisk = 157,   // *
        Lparen = 158,     // (
        Rparen = 159,     // )
        Underscore = 160, // _
        Plus = 161,       // +
        Lcurly = 162,     // {
        Rcurly = 163,     // }
        Pipe = 164,       // |
        Colon = 165,      // :
        Dquote = 166,     // "
        Langle = 167,     // <
        Rangle = 168,     // >
        Question = 169,   // ?
        Tilde = 170,      // ~

        // You might want to add other non-printable keys here if needed
        // For example, function keys, PageUp/Down, etc.
        Home = 268,
        End = 269,

        // Numpad (example values, ensure they don't clash)
        KeyPad0 = 320, KeyPad1 = 321, KeyPad2 = 322,
        KeyPad3 = 323, KeyPad4 = 324, KeyPad5 = 325,
        KeyPad6 = 326, KeyPad7 = 327, KeyPad8 = 328,
        KeyPad9 = 329,
        KeyPadDecimal = 330,
        KeyPadDivide = 331,
        KeyPadMultiply = 332,
        KeyPadSubtract = 333,
        KeyPadAdd = 334,
        KeyPadEnter = 335,
        KeyPadEqual = 336
    };

    public class GameInput
    {

        public static IPosition MousePosition
        {
            get
            {
                int x = NativeBridge.GetMouseX();
                int y = NativeBridge.GetMouseY();
               
                return new IPosition(x, y);

            }
        }

        public static bool MouseButton(MouseButton button)
        {
            int bid = (int)button;

            return NativeBridge.GetMouseDown(0);
        }

        public static bool KeyDown(GameKey key)
        {
            return NativeBridge.GetKeyDown((int)key);
        }

    }
}
