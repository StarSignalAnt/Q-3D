using QNet.UI;
using QNet.UI.Controls;
using System;
using System.Collections.Generic;
using System.Net.Http.Headers;
using System.Text;

namespace QNet.Converse
{
    public class Conversation
    {

        public ConvNode Root
        {
            get;
            set;
        }

        private IWindow MainWindow
        {
            get;
            set;
        }

        public ISmartText Text
        {
            get;
            set;
        }

        public Conversation()
        {

            Root = null;
            MainWindow = null;
        }

        public void Begin()
        {

            if (Root == null)
            {
                return;
            }

            MainWindow = new IWindow("");

            GameUI.Instance.Root.AddControls(MainWindow);

            MainWindow.SetRect(new IRect(100, Engine.FrameHeight - 350, Engine.FrameWidth - 200, 200));
            Text = new ISmartText(Root.Full,true);
            Text.SetRect(new IRect(20, 30, MainWindow.Body.Rect.Width - 40, MainWindow.Body.Rect.Height - 50));
            Text.LineSpacing = 20;



            MainWindow.Body.AddControls(Text);
            MainWindow.Body.Updated();

        }

    }
}
