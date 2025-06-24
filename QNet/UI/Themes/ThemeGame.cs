using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using QNet.Debug;

namespace QNet.UI.Themes
{
    public class ThemeGame : UITheme
    {

        public ThemeGame()
        {

            this.Button = new Texture.Texture2D("Content/ThemeGame/button.png");
            this.Body = new Texture.Texture2D("Content/ThemeGame/White.png");
            WindowTitle = new IColor(0.13f, 0.13f, 0.13f, 0.85f);
            WindowBody = new IColor(0.1f, 0.1f, 0.1f, 0.85f);
            ForeColor = new IColor(0.7f, 0.7f, 0.7f, 0.85f);
            BackColor = new IColor(0.08f, 0.08f, 0.08f, 0.85f);
            VividDebug.Log("Button:" + Button.obj);


        }

    }
}
