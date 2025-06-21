
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using GlmNet;

namespace Vivid.Input
{
    public class GameInput
    {

        public static vec2 MousePosition
        {
            get
            {
                int x = NativeBridge.GetMouseX();
                int y = NativeBridge.GetMouseY();
                var res = new vec2(x, y);
                return res;

            }
        }

    }
}
