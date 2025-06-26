using QNet.Texture;
using QNet.Input;
using System.Collections.Generic;

namespace QNet.UI.Controls
{
    /// <summary>
    /// A UI control that displays a hierarchical collection of items (a tree).
    /// Supports expanding/collapsing nodes, icons, and vertical scrolling.
    /// </summary>
    public class ITreeView : IControl
    {
        public List<ITreeItem> RootItems { get; private set; }
        public ITreeItem SelectedItem { get; private set; }
        public OnClick OnItemSelect { get; set; }

        private readonly IVerticalScroller _scroller;
        private float _contentHeight = 0;
        private IPosition _mouseOverPos = null;

        // --- Constants for layout ---
        private const int ItemHeight = 22;
        private const int IndentWidth = 18;
        private const int ExpanderSize = 8;
        private const int IconSize = 16;
        private const int TextPadding = 4;

        public ITreeView()
        {
            RootItems = new List<ITreeItem>();
            Color = new IColor(0.25f, 0.25f, 0.25f, 1.0f); // Default background

            _scroller = new IVerticalScroller();
            CullSelf = true;
            AddControls(_scroller); // Add scroller as a child control
        }

        public void AddItem(ITreeItem item)
        {
            item.Parent = null;
            RootItems.Add(item);
        }

        public void Clear()
        {
            RootItems.Clear();
            SelectedItem = null;
        }

        public override void OnUpdate(float dt)
        {
            _mouseOverPos = GameInput.MousePosition;
            base.OnUpdate(dt);
        }

        public override void OnMouseDown(MouseButton button)
        {
            // This logic is only triggered if the main TreeView area is clicked,
            // not its child scroller (which is handled by the GameUI dispatcher).
            if (button == MouseButton.Left)
            {
                var mousePos = GameInput.MousePosition;
                float scrollOffset = _scroller.Value * (_contentHeight - RenderRect.Height);
                if (_contentHeight <= RenderRect.Height) scrollOffset = 0;

                float currentY = RenderRect.Y - scrollOffset;

                ITreeItem newlySelectedItem = null;

                // Recursively find which item, if any, was clicked
                foreach (var item in RootItems)
                {
                    newlySelectedItem = FindClickedItem(item, mousePos, ref currentY);
                    if (newlySelectedItem != null)
                    {
                        break; // Stop searching once we've found the clicked item
                    }
                }

                // Update selection state. If nothing was clicked, newlySelectedItem will be null,
                // correctly deselecting any previously selected item.
                SelectedItem = newlySelectedItem;
                OnItemSelect?.Invoke(SelectedItem);
            }
        }

        /// <summary>
        /// Recursively checks items to find which one was clicked by the user.
        /// Also handles toggling the open/closed state of parent items.
        /// </summary>
        /// <returns>The clicked ITreeItem, or null if no item was clicked.</returns>
        private ITreeItem FindClickedItem(ITreeItem item, IPosition mousePos, ref float currentY)
        {
            int indent = GetItemIndentLevel(item);
            IRect itemRect = new IRect(RenderRect.X, (int)currentY, RenderRect.Width, ItemHeight);

            // Stop searching if we're past the visible area of the control
            if (itemRect.Y > RenderRect.Y + RenderRect.Height) return null;

            ITreeItem clickedItem = null;

            // Only process items that are at least partially visible
            if (itemRect.Y + ItemHeight > RenderRect.Y)
            {
                // Check if the mouse is within the vertical bounds of this item
                if (mousePos.Y >= itemRect.Y && mousePos.Y < itemRect.Y + itemRect.Height)
                {
                    // Check if the expander icon was clicked
                    int expanderX = RenderRect.X + indent * IndentWidth;
                    IRect expanderRect = new IRect(expanderX, itemRect.Y + (ItemHeight - ExpanderSize) / 2, ExpanderSize, ExpanderSize);

                    if (item.Children.Count > 0 && mousePos.X >= expanderRect.X && mousePos.X < expanderRect.X + expanderRect.Width)
                    {
                        item.IsOpen = !item.IsOpen;
                        // Clicking the expander should not select the item, so we return null.
                        return null;
                    }
                    else
                    {
                        // The item itself was clicked, so this is our potential selection.
                        clickedItem = item;
                    }
                }
            }

            // Move to the next vertical position
            currentY += ItemHeight;

            // If this item is open, recurse into its children
            if (item.IsOpen)
            {
                foreach (var child in item.Children)
                {
                    // Check if a child was clicked
                    ITreeItem childClick = FindClickedItem(child, mousePos, ref currentY);
                    if (childClick != null)
                    {
                        // A child was clicked, so it takes precedence over the parent.
                        return childClick;
                    }
                }
            }

            // Return the item that was clicked at this level, or null if nothing was clicked.
            return clickedItem;
        }


        public override void OnRender()
        {
            // 1. Draw background

            var rr = RenderRect;
            rr.Width = rr.Width + 1;
            rr.Height = rr.Height + 1;
            GameUI.Draw.Rect(GameUI.Theme.Body, rr, GameUI.Theme.ForeColor);
            GameUI.Draw.Rect(GameUI.Theme.Body,new IRect(RenderRect.X+1,RenderRect.Y+1,Rect.Width-2,Rect.Height-2), GameUI.Theme.BackColor);


            // 2. Calculate content height and update scroller
            _contentHeight = CalculateContentHeight();
            _scroller.ContentHeight = _contentHeight;
            _scroller.SetRect(new IRect(Rect.Width - 12, 1, 12, Rect.Height-2));

            if (_contentHeight > RenderRect.Height)
            {
                _scroller.Color.Alpha = 1.0f; // Make scroller visible
            }
            else
            {
                _scroller.Color.Alpha = 0.0f; // Hide scroller
            }

            // 3. Set a scissor rectangle to clip drawing to the control's bounds
     //       GameUI.Draw.SetScissor(RenderRect);

            // 4. Recursively render all visible items
            float scrollOffset = _scroller.Value * (_contentHeight - RenderRect.Height);
            if (scrollOffset < 0) scrollOffset = 0;
            float currentY = RenderRect.Y - scrollOffset;

            foreach (var item in RootItems)
            {
                RenderItem(item, ref currentY);
            }

            // 5. Render children (the scroller), then reset scissor
            RenderChildren();
       //     GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
        }

        private void RenderItem(ITreeItem item, ref float currentY)
        {
            int indent = GetItemIndentLevel(item);
            int itemX = RenderRect.X + (indent * IndentWidth);

            // Culling: If the item is entirely outside the view, skip it
            if (currentY + ItemHeight < RenderRect.Y)
            {
                currentY += ItemHeight;
                // Still need to account for children if open
                if (item.IsOpen)
                {
                    foreach (var child in item.Children)
                    {
                        SkipRender(child, ref currentY);
                    }
                }
                return;
            }
            if (currentY > RenderRect.Y + RenderRect.Height) return;

            // --- Draw Item ---
            IRect itemRect = new IRect(RenderRect.X, (int)currentY, RenderRect.Width-12, ItemHeight);

            itemRect = CullRect(itemRect);


            // Draw highlight for selected or mouse-over item
            if (SelectedItem == item)
            {
                GameUI.Draw.Rect(GameUI.Theme.Body, itemRect, new IColor(0.4f, 0.4f, 0.6f, 1.0f));
            }
            else if (_mouseOverPos != null && _mouseOverPos.Y >= itemRect.Y && _mouseOverPos.Y < itemRect.Y + itemRect.Height && _mouseOverPos.X >= itemRect.X && _mouseOverPos.X < itemRect.X + itemRect.Width)
            {
                GameUI.Draw.Rect(GameUI.Theme.Body, itemRect, new IColor(0.3f, 0.3f, 0.35f, 1.0f));
            }

            // Draw Expander [+]/[-]
            if (item.Children.Count > 0)
            {
                IRect expanderRect = new IRect(itemX, (int)currentY + (ItemHeight - ExpanderSize) / 2, ExpanderSize, ExpanderSize);
                GameUI.Draw.Rect(GameUI.Theme.Body, expanderRect, GameUI.Theme.WindowTitle); // Box
                IPosition textPos = new IPosition(expanderRect.X + 2, expanderRect.Y + (ExpanderSize / 2) + (GameUI.Font.TextHeight("-", 1.0f) / 2));
                GameUI.Font.DrawText(item.IsOpen ? "-" : "+", textPos, 1.0f);
            }

            itemX += ExpanderSize + TextPadding;

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

            currentY += ItemHeight;

            // Render children if open
            if (item.IsOpen)
            {
                foreach (var child in item.Children)
                {
                    RenderItem(child, ref currentY);
                }
            }
        }

        // Used to correctly calculate Y offsets for culled items
        private void SkipRender(ITreeItem item, ref float currentY)
        {
            currentY += ItemHeight;
            if (item.IsOpen)
            {
                foreach (var child in item.Children)
                {
                    SkipRender(child, ref currentY);
                }
            }
        }

        private float CalculateContentHeight()
        {
            float height = 0;
            foreach (var item in RootItems)
            {
                height += GetItemSubtreeHeight(item);
            }
            return height;
        }

        private float GetItemSubtreeHeight(ITreeItem item)
        {
            float height = ItemHeight;
            if (item.IsOpen)
            {
                foreach (var child in item.Children)
                {
                    height += GetItemSubtreeHeight(child);
                }
            }
            return height;
        }

        private int GetItemIndentLevel(ITreeItem item)
        {
            int level = 0;
            var current = item.Parent;
            while (current != null)
            {
                level++;
                current = current.Parent;
            }
            return level;
        }
    }
}
