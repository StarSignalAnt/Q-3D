using System;
using System.Collections.Generic;
using QNet.Draw;
using QNet.Font;
using QNet.Input;

namespace QNet.UI
{
    public class GameUI
    {
        // --- Static and Public Properties ---
        public static Draw2D Draw { get; private set; }
        public static UITheme Theme { get; private set; }
        public static GameFont Font { get; private set; }
        public static GameUI Instance { get; private set; }

        public IControl Root { get; set; }
        public IControl OverControl { get; private set; }
        public IControl PressedControl { get; private set; }
        public IControl FocusedControl { get; private set; }

        // --- Private Fields ---
        private IPosition _previousMousePosition;
        private readonly Dictionary<GameKey, bool> _keyWasDown = new Dictionary<GameKey, bool>();
        // State tracking for mouse buttons to simulate press/release events
        private readonly Dictionary<int, bool> _mouseWasDown = new Dictionary<int, bool>();


        // --- Constructor ---
        public GameUI()
        {
            Instance = this;

            Theme = new Themes.ThemeGame();
            Draw = new Draw2D();
            Root = new IControl();
            Root.SetRect(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
            Font = new GameFont("Engine/system4.ttf", 12);
            Font.SetDraw(Draw);
            _previousMousePosition = new IPosition(GameInput.MousePosition.X, GameInput.MousePosition.Y);
        }

        /// <summary>
        /// Sets the keyboard focus to a specific control, notifying the old and new controls.
        /// </summary>
        public void SetFocus(IControl control)
        {
            if (FocusedControl == control) return;

            FocusedControl?.OnLostFocus();
            FocusedControl = control;
            FocusedControl?.OnGainedFocus();
        }

        /// <summary>
        /// Main update loop for the entire UI. Handles mouse, focus, and keyboard events.
        /// </summary>
        public void Update()
        {
            // --- Mouse Position Logic ---
            var mousePos = GameInput.MousePosition;
            var mouseDelta = new IPosition(mousePos.X - _previousMousePosition.X, mousePos.Y - _previousMousePosition.Y);
            _previousMousePosition = mousePos;

            // --- Mouse Over Logic ---
            List<IControl> controls = GetControlList();
            IControl currentOverControl = null;
            foreach (var con in controls)
            {
                if (con.InBounds(mousePos))
                {
                    currentOverControl = con;
                    break;
                }
            }

            if (OverControl != currentOverControl)
            {
                OverControl?.OnLeave();
                currentOverControl?.OnEnter();
                OverControl = currentOverControl;
            }

            // --- Mouse Button Logic (Corrected) ---
            bool isLeftMouseDown = GameInput.MouseButton(0);
            bool wasLeftMouseDown;
            _mouseWasDown.TryGetValue(0, out wasLeftMouseDown); // Safely gets value, defaults to false if not found.

            bool isLeftMouseJustPressed = isLeftMouseDown && !wasLeftMouseDown;
            bool isLeftMouseJustReleased = !isLeftMouseDown && wasLeftMouseDown;

            if (isLeftMouseJustPressed)
            {
                PressedControl = OverControl;
                PressedControl?.OnMouseDown(MouseButton.Left);
                SetFocus(PressedControl); // Set focus on the pressed control
            }

            if (isLeftMouseJustReleased)
            {
                // OnMouseUp should be called on the control that was initially pressed
                PressedControl?.OnMouseUp(MouseButton.Left);
                PressedControl = null;
            }

            // --- Mouse Move Logic ---
            (PressedControl ?? OverControl)?.OnMouseMove(mousePos, mouseDelta);

            // --- Keyboard Logic ---
            HandleKeyboard();

            // --- Control Updates ---
            Root.OnUpdate(0.0f);

            // --- Update State for Next Frame ---
            _mouseWasDown[0] = isLeftMouseDown;
        }

        /// <summary>
        /// Main render loop for the UI.
        /// </summary>
        public void Render()
        {
            Draw.Begin();
            Root.OnRender();
            Draw.Flush();
        }

        /// <summary>
        /// Handles global keyboard input and dispatches it to the focused control.
        /// </summary>
        private void HandleKeyboard()
        {
            if (FocusedControl == null) return;

            bool shift = GameInput.KeyDown(GameKey.LeftShift) || GameInput.KeyDown(GameKey.RightShift);
            bool ctrl = GameInput.KeyDown(GameKey.LeftControl) || GameInput.KeyDown(GameKey.RightControl);

            foreach (GameKey key in (GameKey[])Enum.GetValues(typeof(GameKey)))
            {
                bool isDown = GameInput.KeyDown(key);
                bool wasDown;
                _keyWasDown.TryGetValue(key, out wasDown); // Safely gets value, defaults to false if not found.

                if (isDown && !wasDown)
                {
                    FocusedControl.OnKeyDown(key, shift, ctrl);
                }
                else if (!isDown && wasDown)
                {
                    FocusedControl.OnKeyUp(key, shift, ctrl);
                }

                _keyWasDown[key] = isDown;
            }
        }

        /// <summary>
        /// Recursively gets a flat list of all controls, rendered from top to bottom.
        /// </summary>
        private List<IControl> GetControlList()
        {
            var list = new List<IControl>();

            void BuildList(IControl control)
            {
                list.Add(control);
                // Important: Iterate over a copy in case the collection is modified during iteration
                foreach (var child in new List<IControl>(control.Controls))
                {
                    BuildList(child);
                }
            }

            BuildList(Root);
            list.Reverse(); // Reverse so top-most controls are first
            return list;
        }
    }
}
