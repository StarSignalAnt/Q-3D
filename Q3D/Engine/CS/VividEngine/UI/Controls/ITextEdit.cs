using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Input;

namespace Vivid.UI
{
    public class ITextEdit : IControl
    {
        public int CursorPosition { get; set; }
        public int ScrollOffset { get; set; }
        private bool _hasFocus = false;
        private float _blinkTimer = 0f;
        private bool _showCursor = true;

        // The list of keys to check is now an instance field, not static.
        private List<GameKey> _inputKeys;

        public ITextEdit(string text = "")
        {
            Text = text;
            CursorPosition = 0;
            ScrollOffset = 0;
            Color = new IColor(0.2f, 0.2f, 0.2f, 1.0f);

            // The list is now initialized safely in the constructor, avoiding static initializer issues.
            _inputKeys = new List<GameKey>
            {
                // Alphanumeric
                GameKey.A, GameKey.B, GameKey.C, GameKey.D, GameKey.E, GameKey.F, GameKey.G, GameKey.H, GameKey.I, GameKey.J, GameKey.K, GameKey.L, GameKey.M,
                GameKey.N, GameKey.O, GameKey.P, GameKey.Q, GameKey.R, GameKey.S, GameKey.T, GameKey.U, GameKey.V, GameKey.W, GameKey.X, GameKey.Y, GameKey.Z,
                GameKey.D0, GameKey.D1, GameKey.D2, GameKey.D3, GameKey.D4, GameKey.D5, GameKey.D6, GameKey.D7, GameKey.D8, GameKey.D9,
                // Keypad
                GameKey.KeyPad0, GameKey.KeyPad1, GameKey.KeyPad2, GameKey.KeyPad3, GameKey.KeyPad4, GameKey.KeyPad5, GameKey.KeyPad6, GameKey.KeyPad7, GameKey.KeyPad8, GameKey.KeyPad9,
                // Symbols
                GameKey.Space, GameKey.Minus, GameKey.Equal, GameKey.LeftBracket, GameKey.RightBracket, GameKey.Backslash, GameKey.Semicolon,
                GameKey.Apostrophe, GameKey.Comma, GameKey.Period, GameKey.Slash, GameKey.GraveAccent,
                // Control keys
                GameKey.Backspace, GameKey.Left, GameKey.Right, GameKey.Delete, GameKey.Home, GameKey.End
            };
        }

        // Public method to allow the main UI logic to remove focus.
        public void LoseFocus()
        {
            _hasFocus = false;
            _showCursor = false;
        }

        public override void OnMouseDown(MouseButton button)
        {
            if (button != MouseButton.Left) return;

            // When this control is clicked, it gains focus.
            // Your main UI update loop should call LoseFocus() on any previously focused control.
            _hasFocus = true;

            // --- Improved Cursor Positioning ---
            // Calculate the precise cursor position based on the click location.
            int clickX = GameInput.MousePosition.X - RenderRect.X + ScrollOffset - 5; // 5 for padding
            float currentWidth = 0;
            int newCursorPos = 0;

            if (clickX > 0)
            {
                for (int i = 0; i < Text.Length; i++)
                {
                    float charWidth = GameUI.Font.TextWidth(Text[i].ToString(), 1.0f);
                    // Place cursor if click is in the first half of the character
                    if (clickX < currentWidth + charWidth / 2)
                    {
                        break;
                    }
                    currentWidth += charWidth;
                    newCursorPos++;
                }
            }

            CursorPosition = newCursorPos;
            UpdateScroll(); // Ensure view scrolls to the new cursor position
        }

        public override void OnUpdate(float dt)
        {
            base.OnUpdate(dt);

            if (_hasFocus)
            {
                _blinkTimer += dt;
                if (_blinkTimer >= 0.5f)
                {
                    _showCursor = !_showCursor;
                    _blinkTimer = 0f;
                }
                HandleKeyboardInput();
            }
            else
            {
                _showCursor = false;
            }
        }

        private void HandleKeyboardInput()
        {
            bool shift = GameInput.KeyDown(GameKey.LeftShift) || GameInput.KeyDown(GameKey.RightShift);

            // Iterate over our predefined list of keys instead of Enum.GetValues
            foreach (GameKey key in _inputKeys)
            {
                if (GameInput.KeyDown(key))
                {
                    // Basic debounce to prevent multiple triggers for a single press
                    if (IsKeyJustPressed(key))
                    {
                        ProcessKeyPress(key, shift);
                    }
                }
                else
                {
                    _pressedKeys.Remove(key);
                }
            }
        }

        private HashSet<GameKey> _pressedKeys = new HashSet<GameKey>();
        private bool IsKeyJustPressed(GameKey key)
        {
            if (!_pressedKeys.Contains(key))
            {
                _pressedKeys.Add(key);
                return true;
            }
            return false;
        }

        private void ProcessKeyPress(GameKey key, bool shift)
        {
            if (key == GameKey.Backspace && CursorPosition > 0)
            {
                Text = Text.Remove(CursorPosition - 1, 1);
                CursorPosition--;
            }
            else if (key == GameKey.Delete && CursorPosition < Text.Length)
            {
                Text = Text.Remove(CursorPosition, 1);
            }
            else if (key == GameKey.Left && CursorPosition > 0)
            {
                CursorPosition--;
            }
            else if (key == GameKey.Right && CursorPosition < Text.Length)
            {
                CursorPosition++;
            }
            else if (key == GameKey.Home)
            {
                CursorPosition = 0;
            }
            else if (key == GameKey.End)
            {
                CursorPosition = Text.Length;
            }
            else
            {
                char c = GetCharFromKey(key, shift);
                if (c != '\0')
                {
                    Text = Text.Insert(CursorPosition, c.ToString());
                    CursorPosition++;
                }
            }
            UpdateScroll();
        }

        private void UpdateScroll()
        {
            float cursorX = GameUI.Font.TextWidth(Text.Substring(0, CursorPosition), 1.0f);

            // Scroll right if cursor goes past the edge
            if (cursorX - ScrollOffset > Rect.Width - 10) // 10 is a right margin
            {
                ScrollOffset = (int)(cursorX - Rect.Width + 10);
            }
            // Scroll left if cursor moves back off screen
            else if (cursorX - ScrollOffset < 5) // 5 is a left margin
            {
                ScrollOffset = (int)cursorX - 40; // Scroll a bit more to show context
            }

            if (ScrollOffset < 0) ScrollOffset = 0;
        }

        public override void OnRender()
        {
            // Draw the background
            GameUI.Draw.Rect(GameUI.Theme.Body, RenderRect, Color);

            // Set scissor to clip text that goes outside the control's bounds
            GameUI.Draw.SetScissor(RenderRect);

            // Calculate text position, accounting for the scroll offset and a small padding
            IPosition textPos = new IPosition(RenderRect.X + 5 - ScrollOffset, RenderRect.Y + Rect.Height / 2 - GameUI.Font.TextHeight(" ", 1.0f) / 2);

            // Draw the text
            GameUI.Font.DrawText(Text, textPos, 1.0f);

            // Draw the cursor if the control has focus
            if (_showCursor)
            {
                float cursorX = GameUI.Font.TextWidth(Text.Substring(0, CursorPosition), 1.0f);
                IRect cursorRect = new IRect(RenderRect.X + 5 + (int)cursorX - ScrollOffset, RenderRect.Y + 2, 1, Rect.Height - 4);
                GameUI.Draw.Rect(null, cursorRect, new IColor(1, 1, 1, 1)); // White cursor
            }

            // Reset scissor to not affect other UI elements
            GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));

            RenderChildren();
        }

        private char GetCharFromKey(GameKey key, bool shift)
        {
            if (key >= GameKey.A && key <= GameKey.Z)
            {
                return (char)(key + (shift ? 0 : 32)); // Add 32 for lowercase
            }
            if (key >= GameKey.D0 && key <= GameKey.D9)
            {
                if (shift)
                {
                    switch (key)
                    {
                        case GameKey.D1: return '!';
                        case GameKey.D2: return '@';
                        case GameKey.D3: return '#';
                        case GameKey.D4: return '$';
                        case GameKey.D5: return '%';
                        case GameKey.D6: return '^';
                        case GameKey.D7: return '&';
                        case GameKey.D8: return '*';
                        case GameKey.D9: return '(';
                        case GameKey.D0: return ')';
                    }
                }
                return (char)key;
            }
            if (key >= GameKey.KeyPad0 && key <= GameKey.KeyPad9)
            {
                return (char)('0' + (key - GameKey.KeyPad0));
            }
            switch (key)
            {
                case GameKey.Space: return ' ';
                case GameKey.Minus: return shift ? '_' : '-';
                case GameKey.Equal: return shift ? '+' : '=';
                case GameKey.LeftBracket: return shift ? '{' : '[';
                case GameKey.RightBracket: return shift ? '}' : ']';
                case GameKey.Backslash: return shift ? '|' : '\\';
                case GameKey.Semicolon: return shift ? ':' : ';';
                case GameKey.Apostrophe: return shift ? '"' : '\'';
                case GameKey.Comma: return shift ? '<' : ',';
                case GameKey.Period: return shift ? '>' : '.';
                case GameKey.Slash: return shift ? '?' : '/';
                case GameKey.GraveAccent: return shift ? '~' : '`';
            }
            return '\0'; // No character representation
        }
    }
}
