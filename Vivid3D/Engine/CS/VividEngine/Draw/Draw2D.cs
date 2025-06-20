using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Texture;

namespace Vivid.Draw
{
    public class Draw2D
    {

        private IntPtr obj;

        public Draw2D()
        {
            obj = NativeBridge.CreateDraw2D();
        }

        public void Rect(Texture2D image,GlmNet.vec2 pos,GlmNet.vec2 size)
        {

            NativeBridge.DrawRect(obj, image.obj, pos, size);

        }

    }
}
