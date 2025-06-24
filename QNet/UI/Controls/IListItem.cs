using QNet.Texture;
using System.Collections.Generic;

namespace QNet.UI.Controls
{
    /// <summary>
    /// Represents a single item within an IListView control.
    /// </summary>
    public class IListItem
    {
        /// <summary>
        /// The text label displayed for this list item.
        /// </summary>
        public string Text { get; set; }

        /// <summary>
        /// An optional icon to display to the left of the text.
        /// Can be null if no icon is desired.
        /// </summary>
        public Texture2D Icon { get; set; }

        /// <summary>
        /// A user-definable object to store extra data associated with this item.
        /// </summary>
        public object Tag { get; set; }

        /// <summary>
        /// Initializes a new instance of the IListItem class.
        /// </summary>
        /// <param name="text">The text to be displayed for the item.</param>
        /// <param name="icon">An optional icon for the item.</param>
        public IListItem(string text, Texture2D icon = null)
        {
            Text = text;
            Icon = icon;
            Tag = null;
        }
    }
}
