using System;
using GlmNet;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Texture;
using Vivid.UI;

namespace Vivid.Draw
{
    public class Draw2D
    {

        public IntPtr obj;

        public Draw2D()
        {
            obj = NativeBridge.CreateDraw2D();
        }

        public void Rect(Texture2D image, GlmNet.vec2 pos, GlmNet.vec2 size, GlmNet.vec4 color)
        {

            NativeBridge.DrawRect(obj, image.obj, pos, size,color);

        }

        public void Rect(Texture2D image,IRect rect,IColor color)
        {
            NativeBridge.DrawRect(obj, image.obj, new vec2(rect.X, rect.Y), new vec2(rect.Width, rect.Height),new vec4(color.R,color.G,color.B,color.Alpha));
        }
        public void Begin()
        {
            NativeBridge.DrawBegin(obj);
        }

        public void Flush()
        {
            NativeBridge.DrawFlush(obj);
        }

        public void SetScissor(IRect rect)
        {
            NativeBridge.SetScissor(rect.X, rect.Y, rect.Width, rect.Height);
        }

    }
}
