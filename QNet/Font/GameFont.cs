using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using QNet.Draw;
using QNet.UI;

namespace QNet.Font
{
    public class GameFont
    {
        private IntPtr obj;
        public GameFont(string font,float size)
        {
            obj = NativeBridge.LoadFont(font, size);
        }

        public void DrawText(string text,IPosition position,float scale=1.0f)
        {
            NativeBridge.FontDrawText(obj, text, new GlmNet.vec2(position.X, position.Y), scale);
        }

        public int TextWidth(string text,float scale)
        {
            return NativeBridge.FontTextWidth(obj, text, scale);
        }

        public int TextHeight(string text, float scale)
        {
            return NativeBridge.FontTextHeight(obj, text, scale);
        }
        
        public void SetDraw(Draw2D draw)
        {
            NativeBridge.SetFontDraw(obj, draw.obj);
        }

    }
}
