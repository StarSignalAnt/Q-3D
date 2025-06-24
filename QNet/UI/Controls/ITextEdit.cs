using QNet.Input;
using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace QNet.UI
{
    public class ITextEdit : IControl
    {
        #region Properties
        public int CursorPosition { get; private set; }
        public int SelectionStart { get; private set; }
        #endregion

        #region Private Fields
        private int _selectionEnd;
        private int _scrollOffset;
        private float _blinkTimer;
        private bool _showCursor = true;
        private bool _isDragging;
        private readonly Dictionary<GameKey, float> _heldKeys = new Dictionary<GameKey, float>();
        private static string _clipboard = "";

        private readonly Stopwatch _stopwatch = new Stopwatch();
        private long _lastFrameTime;
        #endregion

        #region Constants
        private const float BlinkRate = 0.53f;
        private const int Padding = 4;
        private const float KeyRepeatDelay = 0.4f;
        private const float KeyRepeatRate = 0.03f;
        #endregion

        #region Constructor
        public ITextEdit(string text = "")
        {
            Text = text;
            Color = new IColor(0.2f, 0.2f, 0.2f, 1.0f);
            ClearSelection();
            _stopwatch.Start();
            _lastFrameTime = _stopwatch.ElapsedMilliseconds;
        }
        #endregion

        #region Focus and Input Handling
        public override void OnGainedFocus()
        {
            base.OnGainedFocus();
            _blinkTimer = 0;
            _showCursor = true;
        }

        public override void OnLostFocus()
        {
            base.OnLostFocus();
            _isDragging = false;
            _heldKeys.Clear();
            ClearSelection();
        }

        public override void OnMouseDown(MouseButton button)
        {
            if (button != MouseButton.Left) return;
            _isDragging = true;
            int newCursorPos = GetPositionFromMouse(GameInput.MousePosition);
            CursorPosition = newCursorPos;
            SelectionStart = newCursorPos;
            _selectionEnd = newCursorPos;
            UpdateScroll();
        }

        public override void OnMouseMove(IPosition position, IPosition delta)
        {
            if (!_isDragging || !HasFocus) return;
            CursorPosition = GetPositionFromMouse(position);
            _selectionEnd = CursorPosition;
            UpdateScroll();
        }

        public override void OnMouseUp(MouseButton button)
        {
            if (button == MouseButton.Left) _isDragging = false;
        }

        public override void OnKeyDown(GameKey key, bool shift, bool ctrl)
        {
            ProcessKeyPress(key, shift, ctrl);
            _heldKeys[key] = 0f;
        }

        public override void OnKeyUp(GameKey key, bool shift, bool ctrl)
        {
            _heldKeys.Remove(key);
        }

        #endregion

        #region Update and Render
        public override void OnUpdate(float dt)
        {
            long currentFrameTime = _stopwatch.ElapsedMilliseconds;
            float reliableDt = (currentFrameTime - _lastFrameTime) / 1000.0f;
            _lastFrameTime = currentFrameTime;

            if (HasFocus)
            {
                _blinkTimer += reliableDt;
                if (_blinkTimer >= BlinkRate)
                {
                    _showCursor = !_showCursor;
                    _blinkTimer = 0f;
                }
                HandleKeyRepeat(reliableDt);
            }

            base.OnUpdate(reliableDt);
        }

        public override void OnRender()
        {
            IRect renderRect = RenderRect;
            var theme = GameUI.Theme;
            var font = GameUI.Font;

            if (theme?.Body == null) return;

            GameUI.Draw.Rect(theme.Body, renderRect, new IColor(0.8f, 0.8f, 0.8f, 1.0f));
            IRect bgRect = new IRect(renderRect.X + 1, renderRect.Y + 1, renderRect.Width - 2, renderRect.Height - 2);
            GameUI.Draw.Rect(theme.Body, bgRect, new IColor(0.01f,0.01f,0.01f,1.0f));

            if (HasSelection())
            {
                int min = Math.Min(SelectionStart, _selectionEnd);
                int max = Math.Max(SelectionStart, _selectionEnd);
                float startX = font.TextWidth(Text.Substring(0, min), 1.0f);
                float endX = font.TextWidth(Text.Substring(0, max), 1.0f);
                IRect selectionRect = new IRect(bgRect.X + Padding + (int)startX - _scrollOffset, bgRect.Y + 2, (int)(endX - startX), bgRect.Height - 4);
                GameUI.Draw.Rect(theme.Body, selectionRect, new IColor(0.2f, 0.4f, 0.8f, 0.5f));
            }

            GameUI.Draw.SetScissor(bgRect);

            float textHeight = font.TextHeight(" ", 1.0f);
            IPosition textPos = new IPosition(bgRect.X + Padding - _scrollOffset, bgRect.Y + (bgRect.Height + (int)textHeight) / 2+3);
            font.DrawText(Text, textPos, 1.0f);


            if (HasFocus && _showCursor)
            {
                float cursorX = font.TextWidth(Text.Substring(0, CursorPosition), 1.0f);
                IRect cursorRect = new IRect(bgRect.X + Padding + (int)cursorX - _scrollOffset, bgRect.Y + 2, 1, bgRect.Height - 4);
                GameUI.Draw.Rect(theme.Body, cursorRect, new IColor(1, 1, 1, 1));
            }

            GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
            RenderChildren();
        }
        #endregion

        #region Core Logic
        private void HandleKeyRepeat(float dt)
        {
            bool shift = GameInput.KeyDown(GameKey.LeftShift) || GameInput.KeyDown(GameKey.RightShift);
            bool ctrl = GameInput.KeyDown(GameKey.LeftControl) || GameInput.KeyDown(GameKey.RightControl);

            var keysToRepeat = new List<GameKey>(_heldKeys.Keys);
            foreach (var key in keysToRepeat)
            {
                // Do not repeat modifier keys themselves
                if (key == GameKey.LeftShift || key == GameKey.RightShift || key == GameKey.LeftControl || key == GameKey.RightControl || key == GameKey.LeftAlt || key == GameKey.RightAlt)
                {
                    continue;
                }

                float newTime = _heldKeys[key] + dt;
                if (newTime > KeyRepeatDelay)
                {
                    float repeatTime = newTime - KeyRepeatDelay;
                    int currentRepeatCount = (int)(repeatTime / KeyRepeatRate);
                    if (currentRepeatCount < 0) currentRepeatCount = 0;

                    float oldTime = newTime - dt;
                    int previousRepeatCount = (int)((oldTime - KeyRepeatDelay) / KeyRepeatRate);
                    if (previousRepeatCount < 0) previousRepeatCount = 0;

                    for (int i = 0; i < currentRepeatCount - previousRepeatCount; i++)
                    {
                        ProcessKeyPress(key, shift, ctrl, true);
                    }
                }
                _heldKeys[key] = newTime;
            }
        }

        private void ProcessKeyPress(GameKey key, bool shift, bool ctrl, bool isRepeat = false)
        {
            if (ctrl && !isRepeat)
            {
                switch (key)
                {
                    case GameKey.C: if (HasSelection()) _clipboard = GetSelectedText(); return;
                    case GameKey.X: if (HasSelection()) { _clipboard = GetSelectedText(); DeleteSelection(); } return;
                    case GameKey.V: if (!string.IsNullOrEmpty(_clipboard)) InsertText(_clipboard); return;
                    case GameKey.A: SelectionStart = 0; _selectionEnd = Text.Length; CursorPosition = Text.Length; return;
                }
            }

            char c = GetCharFromKey(key, shift);
            if (c != '\0')
            {
                InsertText(c.ToString());
            }
            else
            {
                switch (key)
                {
                    case GameKey.Left:
                        if (CursorPosition > 0) CursorPosition--;
                        if (!shift) ClearSelection(); else _selectionEnd = CursorPosition;
                        break;
                    case GameKey.Right:
                        if (CursorPosition < Text.Length) CursorPosition++;
                        if (!shift) ClearSelection(); else _selectionEnd = CursorPosition;
                        break;
                    case GameKey.Home:
                        CursorPosition = 0;
                        if (!shift) ClearSelection(); else _selectionEnd = CursorPosition;
                        break;
                    case GameKey.End:
                        CursorPosition = Text.Length;
                        if (!shift) ClearSelection(); else _selectionEnd = CursorPosition;
                        break;
                    case GameKey.Backspace:
                        if (HasSelection()) DeleteSelection();
                        else if (CursorPosition > 0) { Text = Text.Remove(CursorPosition - 1, 1); CursorPosition--; }
                        break;
                    case GameKey.Delete:
                        if (HasSelection()) DeleteSelection();
                        else if (CursorPosition < Text.Length) { Text = Text.Remove(CursorPosition, 1); }
                        break;
                    default:
                        return;
                }
            }

            _showCursor = true;
            _blinkTimer = 0f;
            UpdateScroll();
        }

        private char GetCharFromKey(GameKey key, bool shift)
        {
            // The 'shift' parameter is now only needed for letters, as symbols
            // are sent as unique key codes from the C++ side.
            switch (key)
            {
                // Letters (Shift dependent)
                case GameKey.A: return shift ? 'A' : 'a';
                case GameKey.B: return shift ? 'B' : 'b';
                case GameKey.C: return shift ? 'C' : 'c';
                case GameKey.D: return shift ? 'D' : 'd';
                case GameKey.E: return shift ? 'E' : 'e';
                case GameKey.F: return shift ? 'F' : 'f';
                case GameKey.G: return shift ? 'G' : 'g';
                case GameKey.H: return shift ? 'H' : 'h';
                case GameKey.I: return shift ? 'I' : 'i';
                case GameKey.J: return shift ? 'J' : 'j';
                case GameKey.K: return shift ? 'K' : 'k';
                case GameKey.L: return shift ? 'L' : 'l';
                case GameKey.M: return shift ? 'M' : 'm';
                case GameKey.N: return shift ? 'N' : 'n';
                case GameKey.O: return shift ? 'O' : 'o';
                case GameKey.P: return shift ? 'P' : 'p';
                case GameKey.Q: return shift ? 'Q' : 'q';
                case GameKey.R: return shift ? 'R' : 'r';
                case GameKey.S: return shift ? 'S' : 's';
                case GameKey.T: return shift ? 'T' : 't';
                case GameKey.U: return shift ? 'U' : 'u';
                case GameKey.V: return shift ? 'V' : 'v';
                case GameKey.W: return shift ? 'W' : 'w';
                case GameKey.X: return shift ? 'X' : 'x';
                case GameKey.Y: return shift ? 'Y' : 'y';
                case GameKey.Z: return shift ? 'Z' : 'z';

                // Unshifted Symbols & Numbers
                case GameKey.D0: return '0';
                case GameKey.D1: return '1';
                case GameKey.D2: return '2';
                case GameKey.D3: return '3';
                case GameKey.D4: return '4';
                case GameKey.D5: return '5';
                case GameKey.D6: return '6';
                case GameKey.D7: return '7';
                case GameKey.D8: return '8';
                case GameKey.D9: return '9';
                case GameKey.GraveAccent: return '`';
                case GameKey.Minus: return '-';
                case GameKey.Equal: return '=';
                case GameKey.LeftBracket: return '[';
                case GameKey.RightBracket: return ']';
                case GameKey.Backslash: return '\\';
                case GameKey.Semicolon: return ';';
                case GameKey.Apostrophe: return '\'';
                case GameKey.Comma: return ',';
                case GameKey.Period: return '.';
                case GameKey.Slash: return '/';
                case GameKey.Space: return ' ';

                // Shifted Symbols (now unique keys)
                case GameKey.Exclam: return '!';
                case GameKey.At: return '@';
                case GameKey.Hash: return '#';
                case GameKey.Dollar: return '$';
                case GameKey.Percent: return '%';
                case GameKey.Caret: return '^';
                case GameKey.Ampersand: return '&';
                case GameKey.Asterisk: return '*';
                case GameKey.Lparen: return '(';
                case GameKey.Rparen: return ')';
                case GameKey.Underscore: return '_';
                case GameKey.Plus: return '+';
                case GameKey.Lcurly: return '{';
                case GameKey.Rcurly: return '}';
                case GameKey.Pipe: return '|';
                case GameKey.Colon: return ':';
                case GameKey.Dquote: return '"';
                case GameKey.Langle: return '<';
                case GameKey.Rangle: return '>';
                case GameKey.Question: return '?';
                case GameKey.Tilde: return '~';

                // Numpad (assuming NumLock is on)
                case GameKey.KeyPad0: return '0';
                case GameKey.KeyPad1: return '1';
                case GameKey.KeyPad2: return '2';
                case GameKey.KeyPad3: return '3';
                case GameKey.KeyPad4: return '4';
                case GameKey.KeyPad5: return '5';
                case GameKey.KeyPad6: return '6';
                case GameKey.KeyPad7: return '7';
                case GameKey.KeyPad8: return '8';
                case GameKey.KeyPad9: return '9';
                case GameKey.KeyPadDecimal: return '.';
                case GameKey.KeyPadDivide: return '/';
                case GameKey.KeyPadMultiply: return '*';
                case GameKey.KeyPadSubtract: return '-';
                case GameKey.KeyPadAdd: return '+';
            }
            return '\0'; // Return null if no match was found
        }

        private int GetPositionFromMouse(IPosition mousePos)
        {
            int clickX = mousePos.X - RenderRect.X - Padding + _scrollOffset;
            float currentWidth = 0;
            if (clickX <= 0) return 0;
            for (int i = 0; i < Text.Length; i++)
            {
                float charWidth = GameUI.Font.TextWidth(Text[i].ToString(), 1.0f);
                if (clickX < currentWidth + charWidth / 2) return i;
                currentWidth += charWidth;
            }
            return Text.Length;
        }

        private void ClearSelection()
        {
            SelectionStart = CursorPosition;
            _selectionEnd = CursorPosition;
        }

        private bool HasSelection() => SelectionStart != _selectionEnd;

        private string GetSelectedText()
        {
            if (!HasSelection()) return "";
            int min = Math.Min(SelectionStart, _selectionEnd);
            int max = Math.Max(SelectionStart, _selectionEnd);
            return Text.Substring(min, max - min);
        }

        private void DeleteSelection()
        {
            if (!HasSelection()) return;
            int min = Math.Min(SelectionStart, _selectionEnd);
            int max = Math.Max(SelectionStart, _selectionEnd);
            Text = Text.Remove(min, max - min);
            CursorPosition = min;
            ClearSelection();
        }

        private void InsertText(string textToInsert)
        {
            if (HasSelection()) DeleteSelection();
            Text = Text.Insert(CursorPosition, textToInsert);
            CursorPosition += textToInsert.Length;
            ClearSelection();
        }

        private void UpdateScroll()
        {
            float cursorX = GameUI.Font.TextWidth(Text.Substring(0, CursorPosition), 1.0f);
            int visibleWidth = Rect.Width - Padding * 2;
            if (visibleWidth < 0) visibleWidth = 0;
            if (cursorX - _scrollOffset >= visibleWidth) _scrollOffset = (int)(cursorX - visibleWidth);
            else if (cursorX - _scrollOffset < 0) _scrollOffset = (int)cursorX - 40;
            if (_scrollOffset < 0) _scrollOffset = 0;
        }
        #endregion
    }
}