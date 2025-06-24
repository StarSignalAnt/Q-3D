using QNet.Texture;
using QNet.Input;
using System.Collections.Generic;

namespace QNet.UI.Controls
{
    /// <summary>
    /// A UI control that displays a vertical list of items.
    /// Supports optional icons, selection, and vertical scrolling.
    /// </summary>
    public class IListView : IControl
    {
        public List<IListItem> Items { get; private set; }
        public IListItem SelectedItem { get; private set; }
        public OnClick OnItemSelect { get; set; }

        private readonly IVerticalScroller _scroller;
        private float _contentHeight = 0;
        private IPosition _mouseOverPos = null;

        // --- Constants for layout ---
        private const int ItemHeight = 22;
        private const int IconSize = 16;
        private const int TextPadding = 4;

        public IListView()
        {
            Items = new List<IListItem>();
            Color = new IColor(0.25f, 0.25f, 0.25f, 1.0f); // Default background

            _scroller = new IVerticalScroller();
            AddControls(_scroller);
        }

        public void AddItem(IListItem item)
        {
            Items.Add(item);
        }

        public void Clear()
        {
            Items.Clear();
            SelectedItem = null;
        }

        public override void OnUpdate(float dt)
        {
            _mouseOverPos = GameInput.MousePosition;
            base.OnUpdate(dt);
        }

        public override void OnMouseDown(MouseButton button)
        {
            if (button == MouseButton.Left)
            {
                // Let the GameUI dispatcher handle clicks on the scrollbar.
                // This logic will only run if the main list area is clicked.

                var mousePos = GameInput.MousePosition;

                float scrollOffset = _scroller.Value * (_contentHeight - RenderRect.Height);
                if (_contentHeight <= RenderRect.Height) scrollOffset = 0;

                // Calculate which item index was clicked based on its Y position.
                int clickedIndex = (int)((mousePos.Y - RenderRect.Y + scrollOffset) / ItemHeight);

                if (clickedIndex >= 0 && clickedIndex < Items.Count)
                {
                    // A valid item was clicked.
                    SelectedItem = Items[clickedIndex];
                    OnItemSelect?.Invoke(SelectedItem);
                }
                else
                {
                    // The empty space was clicked.
                    SelectedItem = null;
                    OnItemSelect?.Invoke(null);
                }
            }
        }

        public override void OnRender()
        {
            // 1. Draw background
            GameUI.Draw.Rect(GameUI.Theme.Body, RenderRect, GameUI.Theme.BackColor);

            // 2. Calculate content height and update scroller
            _contentHeight = Items.Count * ItemHeight;
            _scroller.ContentHeight = _contentHeight;
            _scroller.SetRect(new IRect(Rect.Width - 12, 0, 12, Rect.Height));

            if (_contentHeight > RenderRect.Height)
            {
                _scroller.Color.Alpha = 1.0f; // Show scroller
            }
            else
            {
                _scroller.Color.Alpha = 0.0f; // Hide scroller
            }

            // 3. Set a scissor rectangle to clip drawing
            GameUI.Draw.SetScissor(RenderRect);

            // 4. Render all visible items
            float scrollOffset = _scroller.Value * (_contentHeight - RenderRect.Height);
            if (scrollOffset < 0) scrollOffset = 0;

            int startIndex = (int)(scrollOffset / ItemHeight);
            if (startIndex < 0) startIndex = 0;

            for (int i = startIndex; i < Items.Count; i++)
            {
                IListItem item = Items[i];
                float currentY = RenderRect.Y + (i * ItemHeight) - scrollOffset;

                // Culling: Stop drawing once we're past the bottom of the control
                if (currentY > RenderRect.Y + RenderRect.Height) break;
                // Culling: Don't draw items that are completely above the top
                if (currentY + ItemHeight < RenderRect.Y) continue;

                // --- Draw Item ---
                int itemX = RenderRect.X + TextPadding;
                IRect itemRect = new IRect(RenderRect.X, (int)currentY, Rect.Width, ItemHeight);

                // Draw highlight for selected or hovered item
                if (SelectedItem == item)
                {
                    GameUI.Draw.Rect(GameUI.Theme.Body, itemRect, new IColor(0.4f, 0.4f, 0.6f, 1.0f));
                }
                else if (_mouseOverPos != null && itemRect.InBounds(_mouseOverPos))
                {
                    GameUI.Draw.Rect(GameUI.Theme.Body, itemRect, new IColor(0.3f, 0.3f, 0.35f, 1.0f));
                }

                // Draw Icon
                if (item.Icon != null)
                {
                    IRect iconRect = new IRect(itemX, (int)currentY + (ItemHeight - IconSize) / 2, IconSize, IconSize);
                    GameUI.Draw.Rect(item.Icon, iconRect, new IColor(1, 1, 1, 1));
                    itemX += IconSize + TextPadding;
                }

                // Draw Text
                float fontHeight = GameUI.Font.TextHeight(item.Text, 1.0f);
                IPosition mainTextPos = new IPosition(itemX, (int)currentY + (ItemHeight + (int)fontHeight) / 2);
                GameUI.Font.DrawText(item.Text, mainTextPos, 1.0f);
            }

            // 5. Render children (the scroller), then reset scissor
            RenderChildren();
            GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
        }
    }
}
