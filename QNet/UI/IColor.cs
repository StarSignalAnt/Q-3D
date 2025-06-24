using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet.UI
{
    public class IColor
    {

        public float R
        {
            get;
            set;
        }

        public float G
        {
            get;

            set;
        }

        public float B
        {
            get;
            set;
        }

        public float Alpha
        {
            get;
            set;
        }

        public IColor(float r,float g,float b,float alpha = 1.0f)
        {
            R = r;
            G = g;
            B = b;
            Alpha = alpha;
        }


    }
}
