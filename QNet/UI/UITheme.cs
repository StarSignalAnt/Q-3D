using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using QNet.Texture;

namespace QNet.UI
{
    public class UITheme
    {

        public Texture2D Button
        {
            get;
            set;
        }

        public IColor ForeColor
        {
            get;
            set;
        }

        public IColor BackColor
        {
            get;
            set;
        }

        public IColor WindowTitle
        {
            get;
            set;
        }

        public IColor WindowBody
        {
            get;
            set;
        }

        public Texture2D Body
        {
            get;
            set;
        }

         public Texture2D White
        {
            get;
            set;
        }

    }
}
