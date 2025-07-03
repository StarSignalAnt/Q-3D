using QNet.Debug;
using QNet.Input;
using System.Collections.Generic;
using System.Text;

namespace QNet.UI
{
    /// <summary>
    /// A control that renders text, automatically wrapping it to fit within its bounds.
    /// If the text exceeds the vertical space, it paginates, requiring a spacebar press to see the next page.
    /// Can also auto-size its height to fit all text.
    /// </summary>
    public class ISmartText : IControl
    {
        private List<string> _wrappedLines = new List<string>();
        private int _currentPage = 0;
        private int _linesPerPage = 0;
        private int _totalPages = 0;
        private string _previousText = "";
        private IRect _previousRect = new IRect(0, 0, 0, 0);
        private readonly bool _autoSize; // Field to store the auto-size setting

        /// <summary>
        /// Additional vertical space between lines of text.
        /// </summary>
        public int LineSpacing { get; set; } = 12;

        /// <summary>
        /// Initializes a new instance of the ISmartText class.
        /// </summary>
        /// <param name="text">The text to display.</param>
        /// <param name="autoSize">If true, the control's height will be automatically adjusted to fit the entire text.</param>
        public ISmartText(string text, bool autoSize = false)
        {
            Text = text;
            _autoSize = autoSize;
            WrapText();
        }

        /// <summary>
        /// Wraps the text to fit within the control's width. This version is more robust
        /// and correctly handles various newline scenarios.
        /// If auto-sizing is enabled, it also adjusts the control's height.
        /// </summary>
        private void WrapText()
        {
            // Only re-wrap if text or width has changed. For auto-sizing, we ignore height changes in this check.
            if (Text == _previousText && Rect.Width == _previousRect.Width && (!_autoSize || Rect.Height == _previousRect.Height))
                return;

            _wrappedLines.Clear();
            if (string.IsNullOrEmpty(Text) || Rect.Width <= 0)
            {
                if (_autoSize) Rect.Height = 0; // Collapse height if no text
                _previousText = Text;
                _previousRect = new IRect(Rect.X, Rect.Y, Rect.Width, Rect.Height);
                return;
            }

            // Use Replace to handle both Windows (\r\n) and Unix (\n) newlines
            string[] paragraphs = Text.Replace("\r\n", "\n").Split('\n');

            foreach (var paragraph in paragraphs)
            {
                // Handle empty lines created by multiple newlines (e.g., "para1\n\npara2")
                if (string.IsNullOrEmpty(paragraph))
                {
                    _wrappedLines.Add("");
                    continue;
                }

                var words = paragraph.Split(' ');
                var currentLine = new StringBuilder();

                foreach (var word in words)
                {
                    if (string.IsNullOrEmpty(word)) continue;

                    // Determine the width of the line if the new word were added
                    string testLine = currentLine.Length > 0 ? currentLine.ToString() + " " + word : word;
                    var lineWidth = GameUI.Font.TextWidth(testLine, 1.0f);

                    // If the line would be too long, wrap to the next line.
                    // The check for currentLine.Length > 0 prevents adding an empty line if a single word is wider than the rect.
                    if (lineWidth > Rect.Width && currentLine.Length > 0)
                    {
                        _wrappedLines.Add(currentLine.ToString());
                        currentLine.Clear();
                        currentLine.Append(word);
                    }
                    else // Otherwise, add the word to the current line
                    {
                        if (currentLine.Length > 0)
                        {
                            currentLine.Append(" ");
                        }
                        currentLine.Append(word);
                    }
                }

                // Add the remainder of the last line
                if (currentLine.Length > 0)
                {
                    _wrappedLines.Add(currentLine.ToString());
                }
            }

            // Adjust height if auto-sizing is enabled
            if (_autoSize)
            {
                float lineHeight = GameUI.Font.TextHeight("A", 1.0f) + LineSpacing;
                Rect.Height = (int)(_wrappedLines.Count * lineHeight);
                VividDebug.Log("RH:" + Rect.Height);
            }

            _previousText = Text;
            _previousRect = new IRect(Rect.X, Rect.Y, Rect.Width, Rect.Height);
        }

        public override void OnRender()
        {

            //base.OnRender();
            VividDebug.Log("RH:" + Rect.Height);
            WrapText();

            if (_wrappedLines.Count == 0) return;

            // Determine the height of a single line of text with user-defined spacing
            float initialLineHeight = GameUI.Font.TextHeight("A", 1.0f) + LineSpacing;
            if (initialLineHeight <= 0) return;

            // Calculate how many lines can fit on a page based on the initial height.
            // If auto-sized, Rect.Height is already adjusted, so this works for both cases.
            _linesPerPage = (int)(Rect.Height / initialLineHeight);
            if (_linesPerPage <= 0)
            {
                // If auto-sizing, we should have at least one line per wrapped line.
                if (_autoSize && _wrappedLines.Count > 0)
                {
                    _linesPerPage = _wrappedLines.Count;
                }
                else
                {
                    return; // Not enough height to render even one line.
                }
            }

            _totalPages = (_wrappedLines.Count + _linesPerPage - 1) / _linesPerPage;

            int startLine = _currentPage * _linesPerPage;
            int endLine = System.Math.Min(startLine + _linesPerPage, _wrappedLines.Count);
            int linesOnThisPage = endLine - startLine;

            float renderLineHeight;

            // If the page is full or auto-sized, dynamically adjust line height to fill the entire rect.
            // Otherwise, use the standard spacing for a cleaner look on partially-filled pages.
            // In auto-size mode, this condition is always true, and renderLineHeight equals initialLineHeight.
            if (linesOnThisPage >= _linesPerPage && linesOnThisPage > 0)
            {
                renderLineHeight = (float)Rect.Height / linesOnThisPage;
            }
            else
            {
                renderLineHeight = initialLineHeight;
            }

            for (int i = startLine; i < endLine; i++)
            {
                var line = _wrappedLines[i];
                // Calculate the Y position for the top of the line
                var position = new IPosition(RenderRect.X, RenderRect.Y + (i - startLine) * (int)renderLineHeight);

                // Add continuation indicator logic
                if (i == endLine - 1 && _currentPage < _totalPages - 1)
                {
                    string indicator = " (space to continue...)";
                    string lineWithIndicator = line + indicator;
                    float indicatorWidth = GameUI.Font.TextWidth(indicator, 1.0f);
                    float lineWidth = GameUI.Font.TextWidth(line, 1.0f);

                    if (lineWidth + indicatorWidth < Rect.Width)
                    {
                        line = lineWithIndicator;
                    }
                }

                GameUI.Font.DrawText(line, position, 1.0f);
            }
        }

        public override void OnKeyDown(GameKey key, bool shift, bool ctrl)
        {
            // Spacebar pagination is implicitly disabled in auto-size mode because totalPages will be 1.
            if (key == GameKey.Space)
            {
                if (_totalPages > 1)
                {
                    _currentPage++;
                    if (_currentPage >= _totalPages)
                    {
                        _currentPage = 0;
                    }
                }
            }
        }
    }
}
