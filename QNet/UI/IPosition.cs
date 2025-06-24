using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet.UI
{
    public class IPosition
    {

        public int X
        {
            get;
            set;
        }

        public int Y
        {
            get;
            set;
        }

        public IPosition(int x,int y)
        {
            X = x;
            Y = y;
        }

        public IPosition()
        {

            X = Y = 0;

        }

    }
}
