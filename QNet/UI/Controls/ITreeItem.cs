using QNet.Texture;
using System.Collections.Generic;

namespace QNet.UI.Controls
{
    /// <summary>
    /// Represents a single node or item within an ITreeView control.
    /// It can have child items to form a hierarchical structure.
    /// </summary>
    public class ITreeItem
    {
        /// <summary>
        /// The text label displayed for this tree item.
        /// </summary>
        public string Text { get; set; }

        /// <summary>
        /// An optional icon to display to the left of the text.
        /// Can be null if no icon is desired.
        /// </summary>
        public Texture2D Icon { get; set; }

        /// <summary>
        /// A list of child items nested under this item.
        /// </summary>
        public List<ITreeItem> Children { get; private set; }

        /// <summary>
        /// A reference to the parent of this item. Will be null for top-level items.
        /// </summary>
        public ITreeItem Parent { get; internal set; }

        /// <summary>
        /// Determines if the item is expanded to show its children.
        /// </summary>
        public bool IsOpen { get; set; }

        /// <summary>
        /// A user-definable object to store extra data associated with this item.
        /// </summary>
        public object Tag { get; set; }

        /// <summary>
        /// Initializes a new instance of the ITreeItem class.
        /// </summary>
        /// <param name="text">The text to be displayed for the item.</param>
        /// <param name="icon">An optional icon for the item.</param>
        public ITreeItem(string text, Texture2D icon = null)
        {
            Text = text;
            Icon = icon;
            Children = new List<ITreeItem>();
            IsOpen = false;
        }

        /// <summary>
        /// Adds a child item to this item.
        /// </summary>
        /// <param name="child">The ITreeItem to add as a child.</param>
        public void AddChild(ITreeItem child)
        {
            child.Parent = this;
            Children.Add(child);
        }
    }
}
