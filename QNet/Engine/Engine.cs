using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet
{
    public class Engine
    {

        public static int FrameWidth
        {
            get
            {
                return NativeBridge.EngineGetWidth();
            }
        }

        public static int FrameHeight
        {
            get
            {
                return NativeBridge.EngineGetHeight();
            }
        }

    }
}
