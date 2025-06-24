using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using QNet.Input;

namespace QNet.UI.Controls
{
    /// <summary>
    /// Represents a vertical scroller control.
    /// The Value (0-1) determines the scrolled position.
    /// The thumb size is calculated based on ContentHeight versus the control's height.
    /// </summary>
    public class IVerticalScroller : IControl
    {
        /// <summary>
        /// The current scroll value, from 0.0 (top) to 1.0 (bottom).
        /// </summary>
        public float Value { get; set; } = 0.0f;

        /// <summary>
        /// The total height of the content that this scroller is meant to scroll through.
        /// </summary>
        public float ContentHeight { get; set; } = 0.0f;

        private bool _isDragging = false;
        private IRect _thumbRect = new IRect(0, 0, 0, 0);

        // Stores the mouse's Y offset from the top of the thumb when a drag begins.
        private int _dragOffsetY = 0;

        public IVerticalScroller()
        {
            // Set a default background color for the scroller track.
            Color = new IColor(0.2f, 0.2f, 0.2f, 1.0f);
            UseOffset = false;
        }

        /// <summary>
        /// Handles mouse down events on the control.
        /// </summary>
        public override void OnMouseDown(MouseButton button)
        {
            if (button == MouseButton.Left)
            {
                var mousePos = GameInput.MousePosition;

                // Check if the mouse click is within the thumb's bounds.
                bool isThumbHovered = mousePos.X >= _thumbRect.X && mousePos.X <= _thumbRect.X + _thumbRect.Width &&
                                      mousePos.Y >= _thumbRect.Y && mousePos.Y <= _thumbRect.Y + _thumbRect.Height;

                if (isThumbHovered)
                {
                    _isDragging = true;
                    // Record where on the thumb the click occurred to prevent it from jumping.
                    _dragOffsetY = mousePos.Y - _thumbRect.Y;
                }
            }
        }

        /// <summary>
        /// Handles mouse up events to stop dragging.
        /// </summary>
        public override void OnMouseUp(MouseButton button)
        {
            if (button == MouseButton.Left)
            {
                _isDragging = false;
            }
        }

        /// <summary>
        /// Handles mouse movement for dragging the thumb.
        /// </summary>
        public override void OnMouseMove(IPosition position, IPosition delta)
        {
            if (_isDragging)
            {
                // The total height of the track the thumb can move along.
                float trackHeight = Rect.Height - _thumbRect.Height;
                if (trackHeight <= 0)
                {
                    // No room to scroll, so do nothing.
                    return;
                }

                // Calculate the new thumb Y position based on the absolute mouse position,
                // the control's top, and the initial drag offset.
                float newThumbY = position.Y - RenderRect.Y - _dragOffsetY;

                // Calculate the new scroll Value based on the thumb's position on the track.
                Value = newThumbY / trackHeight;

                // Clamp the value between 0.0 and 1.0.
                if (Value < 0.0f) Value = 0.0f;
                if (Value > 1.0f) Value = 1.0f;
            }
        }

        /// <summary>
        /// Renders the scroller track and thumb.
        /// </summary>
        public override void OnRender()
        {
            var r = RenderRect;

            // 1. Draw the scroller background/track.
            GameUI.Draw.Rect(GameUI.Theme.Body, r, Color);

            // 2. Calculate the thumb's height. It's proportional to the amount of content visible.
            float viewRatio = (float)Rect.Height / ContentHeight;
            if (float.IsNaN(viewRatio) || float.IsInfinity(viewRatio) || viewRatio > 1.0f)
            {
                viewRatio = 1.0f;
            }

            int thumbHeight = (int)(Rect.Height * viewRatio);

            // Ensure the thumb has a minimum clickable size.
            int minThumbHeight = 20;
            if (thumbHeight < minThumbHeight) thumbHeight = minThumbHeight;
            if (thumbHeight > Rect.Height) thumbHeight = Rect.Height;

            // 3. Calculate the thumb's Y position based on the current Value.
            int trackHeight = Rect.Height - thumbHeight;
            int thumbY = (int)(trackHeight * Value);

            // 4. Update the thumb's rectangle for input checking and rendering.
            _thumbRect.X = r.X;
            _thumbRect.Y = r.Y + thumbY;
            _thumbRect.Width = r.Width;
            _thumbRect.Height = thumbHeight;

            // 5. Draw the thumb using a contrasting color from the theme.
            GameUI.Draw.Rect(GameUI.Theme.Body, _thumbRect, GameUI.Theme.ForeColor);

            // 6. Render any child controls (though a scroller usually won't have any).
            RenderChildren();
        }
    }
}
