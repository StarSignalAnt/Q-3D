
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using GlmNet;
using Vivid.UI;

namespace Vivid.Input
{
    public class GameInput
    {

        public static IPosition MousePosition
        {
            get
            {
                int x = NativeBridge.GetMouseX();
                int y = NativeBridge.GetMouseY();
               
                return new IPosition(x, y);

            }
        }

        public static bool MouseButton(MouseButton button)
        {
            int bid = (int)button;

            return NativeBridge.GetMouseDown(0);
        }

    }
}
