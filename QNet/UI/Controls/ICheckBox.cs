using System;

namespace QNet.UI
{
    /// <summary>
    /// Represents a checkbox control with a text label.
    /// </summary>
    public class ICheckBox : IControl
    {
        private bool _checked;
        private const int CheckBoxSize = 20;
        private const int Padding = 5;

        /// <summary>
        /// Gets or sets a value indicating whether this <see cref="ICheckBox"/> is checked.
        /// </summary>
        public bool Checked
        {
            get => _checked;
            set
            {
                if (_checked != value)
                {
                    _checked = value;
                    OnCheckChanged?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        /// <summary>
        /// Occurs when the value of the Checked property changes.
        /// </summary>
        public event EventHandler OnCheckChanged;

        /// <summary>
        /// Hides the base Text property to automatically recalculate the control's size when the text changes.
        /// </summary>
        public new string Text
        {
            get => base.Text;
            set
            {
                base.Text = value;
                UpdateRect();
            }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ICheckBox"/> class.
        /// </summary>
        /// <param name="text">The text to be displayed next to the checkbox.</param>
        public ICheckBox(string text = "CheckBox")
        {
            // Set the text via the base property to avoid calling UpdateRect before GameUI is ready.
            base.Text = text;
            Color = new IColor(0.9f, 0.9f, 0.9f, 1.0f);

            // The size is calculated after the control is created and GameUI is available.
            // You might need to call UpdateRect() manually after initialization if GameUI.Font is not yet available.
        }

        /// <summary>
        /// Recalculates the control's rectangle based on the checkbox size and text length.
        /// This should be called if the text is changed after initialization.
        /// </summary>
        public void UpdateRect()
        {
            if (GameUI.Font == null) return; // Don't crash if font isn't loaded yet

            var textWidth = GameUI.Font.TextWidth(Text, 1.0f);
            var textHeight = GameUI.Font.TextHeight(Text, 1.0f);

            Rect.Width = CheckBoxSize + Padding + textWidth;
            Rect.Height = Math.Max(CheckBoxSize, textHeight);
        }

        public override void OnMouseDown(MouseButton button)
        {
            if (button == MouseButton.Left)
            {
                Checked = !Checked;
            }
        }

        public override void OnEnter()
        {
            Color = new IColor(1.0f, 1.0f, 1.0f, 1.0f);
        }

        public override void OnLeave()
        {
            Color = new IColor(0.9f, 0.9f, 0.9f, 1.0f);
        }

        public override void OnRender()
        {
            // If the rect hasn't been calculated yet (e.g. font wasn't ready), try to calculate it now.
            if (Rect.Width == 0)
            {
                UpdateRect();
            }

            var r = RenderRect;

            // --- Draw the checkbox box ---
            // Vertically center the checkbox within the control's total height
            var checkRect = new IRect(r.X, r.Y + (r.Height - CheckBoxSize) / 2, CheckBoxSize, CheckBoxSize);
            var innerRect = new IRect(checkRect.X + 2, checkRect.Y + 2, checkRect.Width - 4, checkRect.Height - 4);
            GameUI.Draw.Rect(GameUI.Theme.Button, checkRect, Color);
            GameUI.Draw.Rect(GameUI.Theme.Body, innerRect, new IColor(0.15f, 0.15f, 0.15f, 1.0f));


            // --- Draw the check mark if checked ---
            if (Checked)
            {
                var checkMarkRect = new IRect(checkRect.X + 4, checkRect.Y + 4, CheckBoxSize - 8, CheckBoxSize - 8);
                GameUI.Draw.Rect(GameUI.Theme.Button, checkMarkRect, new IColor(0.8f, 0.8f, 0.8f, 1.0f));
            }

            // --- Draw the text label ---
            // Calculate text position to be vertically centered, consistent with IButton's rendering logic.
            IPosition textPos = new IPosition
            {
                X = r.X + CheckBoxSize + Padding,
                Y = r.Y + r.Height / 2 + GameUI.Font.TextHeight(Text, 1.0f) / 2
            };

            GameUI.Font.DrawText(Text, textPos, 1.0f);

            RenderChildren();
        }
    }
}
