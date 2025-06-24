using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace QNet.Texture
{
    public class Texture2D
    {
        public IntPtr obj;

        public Texture2D(string path)
        {

            obj = NativeBridge.LoadTexture2D(path);

        }
        public Texture2D(IntPtr tobj)
        {

            obj = tobj;

            //obj = NativeBridge.LoadTexture2D(path);

        }


    }
}
