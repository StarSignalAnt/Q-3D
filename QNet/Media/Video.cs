using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using QNet.Texture;

namespace QNet
{
    public class Video
    {

        IntPtr obj;

        public Video(string path)
        {

            obj = NativeBridge.CreateVideo(path);

        }

        public void Play()
        {

            NativeBridge.PlayVideo(obj);
        }

        public void Update()
        {
            for(int i = 0; i < 5; i++)
            {

                NativeBridge.UpdateVideo(obj);

            }
        }

        public Texture2D GetFrame()
        {

            return new Texture2D(NativeBridge.VideoGetFrame(obj));

        }


    }
}
