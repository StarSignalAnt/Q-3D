using System;
using System.Collections.Generic;
using QNet.Input;

namespace QNet.UI.Controls
{
    /// <summary>
    /// A delegate for notifying clients when the selected enum value changes.
    /// </summary>
    public delegate void OnEnumChanged(object sender, Enum newValue);

    /// <summary>
    /// A dropdown control for selecting a value from an enumeration.
    /// </summary>
    /// <typeparam name="T">The enum type to be displayed.</typeparam>
    public class IEnumSelector<T> : IControl where T : struct, IConvertible // Standard C# constraint for enums
    {
        public T SelectedValue { get; private set; }
        public event OnEnumChanged ValueChanged;

        private readonly IListView _listView;
        private bool _isOpen = false;
        private const int DropdownItemHeight = 22;

        public IEnumSelector()
        {
            if (!typeof(T).IsEnum)
            {
                throw new ArgumentException("T must be an enumerated type.");
            }

            // --- Setup the main selector box ---
            Rect = new IRect(0, 0, 150, 30); // Default size, height is fixed at 30.
            Color = new IColor(0.2f, 0.2f, 0.2f, 1.0f);

            // --- Setup the hidden dropdown list ---
            _listView = new IListView();

            // Populate the list with all values from the enum
            string[] names = Enum.GetNames(typeof(T));
            foreach (string name in names)
            {
                _listView.AddItem(new IListItem(name));
            }

            // Default to the first value in the enum
            if (names.Length > 0)
            {
                SelectedValue = (T)Enum.Parse(typeof(T), names[0]);
            }

            // Subscribe to the list's selection event
            _listView.OnItemSelect += OnListSelection;
        }

        // Handles the event when an item is clicked in the popup list
        private void OnListSelection(dynamic selected)
        {
            if (selected is IListItem listItem)
            {
                T newValue = (T)Enum.Parse(typeof(T), listItem.Text);
                if (!newValue.Equals(SelectedValue))
                {
                    SelectedValue = newValue;
                    ValueChanged?.Invoke(this, selected);
                }
            }
            // Close the dropdown after a selection is made
            CloseDropdown();
        }

        public override void OnMouseDown(MouseButton button)
        {
            if (button == MouseButton.Left)
            {
                // Toggle the dropdown's visibility
                if (_isOpen) CloseDropdown();
                else OpenDropdown();
            }
        }

        // When the selector loses focus (e.g., user clicks elsewhere), close the dropdown.
        public override void OnLostFocus()
        {
            base.OnLostFocus();
            CloseDropdown();
        }

        private void OpenDropdown()
        {
            if (_isOpen) return;
            _isOpen = true;

            // Position and size the dropdown list just below the selector
            int listHeight = _listView.Items.Count * DropdownItemHeight;
            const int maxListHeight = 200;
            if (listHeight > maxListHeight) listHeight = maxListHeight;

            // The popup list's rect is absolute, based on our final RenderRect
            _listView.SetRect(new IRect(RenderRect.X, RenderRect.Y + Rect.Height, Rect.Width, listHeight));

            // Add the list directly to the root of the UI to ensure it's drawn on top of everything.
            // This is a robust way to handle popups.
            GameUI.Instance.Root.AddControls(_listView);

            // Grab focus for this control so we can receive the OnLostFocus event.
            GameUI.Instance.SetFocus(this);
        }

        private void CloseDropdown()
        {
            if (!_isOpen) return;
            _isOpen = false;
            // Remove the list from the UI so it is no longer rendered or updated.
            GameUI.Instance.Root.Controls.Remove(_listView);
        }

        public override void OnRender()
        {
            // Draw the main selector box background
            GameUI.Draw.Rect(GameUI.Theme.Body, RenderRect, Color);

            // Draw a simple border around the box
            GameUI.Draw.Rect(GameUI.Theme.Body, new IRect(RenderRect.X, RenderRect.Y, RenderRect.Width, 1), GameUI.Theme.WindowTitle);
            GameUI.Draw.Rect(GameUI.Theme.Body, new IRect(RenderRect.X, RenderRect.Y + RenderRect.Height - 1, RenderRect.Width, 1), GameUI.Theme.WindowTitle);
            GameUI.Draw.Rect(GameUI.Theme.Body, new IRect(RenderRect.X, RenderRect.Y, 1, RenderRect.Height), GameUI.Theme.WindowTitle);
            GameUI.Draw.Rect(GameUI.Theme.Body, new IRect(RenderRect.X + RenderRect.Width - 1, RenderRect.Y, 1, RenderRect.Height), GameUI.Theme.WindowTitle);

            // Draw the text of the currently selected value
            string text = SelectedValue.ToString();
            float fontHeight = GameUI.Font.TextHeight(text, 1.0f);
            IPosition textPos = new IPosition(RenderRect.X + 5, RenderRect.Y + (RenderRect.Height + (int)fontHeight) / 2);
            GameUI.Font.DrawText(text, textPos, 1.0f);

            // Draw a dropdown arrow icon on the right
           // string arrow = _isOpen ? "▲" : "▼";
           // float arrowWidth = GameUI.Font.TextWidth(arrow, 1.0f);
           // IPosition arrowPos = new IPosition(RenderRect.X + RenderRect.Width - (int)arrowWidth - 5, RenderRect.Y + (RenderRect.Height + (int)fontHeight) / 2);
           // GameUI.Font.DrawText(arrow, arrowPos, 1.0f);


            // IMPORTANT: We do NOT call RenderChildren() here because the popup _listView
            // is handled at the top level of the UI tree when it's open.
        }
    }
}
