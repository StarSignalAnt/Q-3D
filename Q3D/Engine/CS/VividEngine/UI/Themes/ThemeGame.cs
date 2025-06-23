using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Debug;

namespace Vivid.UI.Themes
{
    public class ThemeGame : UITheme
    {

        public ThemeGame()
        {

            this.Button = new Texture.Texture2D("Content/ThemeGame/button.png");
            this.Body = new Texture.Texture2D("Content/ThemeGame/White.png");
            WindowTitle = new IColor(0, 0.25f, 0.25f, 0.85f);
            WindowBody = new IColor(0, 0.1f, 0.1f, 0.85f);
            VividDebug.Log("Button:" + Button.obj);

        }

    }
}
