﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Vivid.UI
{
    public class IRect
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

        public int Width
        {
            get;
            set;
        }

        public int Height
        {
            get;
            set;
        }

        public IRect(int x,int y,int width,int height)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
        }

        public override string ToString()
        {

            return "X:" + X + " Y:" + Y + " W:" + Width + " H:" + Height;

        }


    }
}
