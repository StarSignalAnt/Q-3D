using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using QNet.Debug;

namespace QNet.UI.Controls
{
    public class IWindowBody : IControl
    {
        public IVerticalScroller Scroller
        {
            get;
            set;
        }
        public IWindowBody()
        {

            Scroller = new IVerticalScroller();
            AddControls(Scroller);
            CullNodes = true;

        }
        public override void OnUpdate(float dt)
        {

            Offset.Y = (int)(((float)Scroller.ContentHeight) * Scroller.Value);
            UpdateChildren(dt);

        }



        public override void OnRender()
        {

            IRect BodyRect = new IRect(RenderRect.X, RenderRect.Y, Rect.Width, Rect.Height);
            GameUI.Draw.Rect(GameUI.Theme.Body, BodyRect, GameUI.Theme.ForeColor);
            BodyRect.X = BodyRect.X + 1;
            BodyRect.Y = BodyRect.Y + 1;
            BodyRect.Width = BodyRect.Width - 2;
            BodyRect.Height = BodyRect.Height - 2;
            GameUI.Draw.Rect(GameUI.Theme.Body, BodyRect, GameUI.Theme.WindowBody);


            //GameUI.Draw.SetScissor(BodyRect);
            // GameUI.Draw.SetScissor(BodyRect);
         

            //VividDebug.Log("SRECT X:" + srect.X + " Y:" + srect.Y + " W:" + srect.Width + " H:" + srect.Height);
            RenderChildren();
           // GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));

            GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));

        }

        public override void Updated()
        {

            int max_y = 0;
            foreach(var con in Controls)
            {
                int ty = con.Rect.Y + con.Rect.Height;
                if (ty > max_y) max_y = ty;
            }

            Scroller.ContentHeight = max_y - Rect.Height;
            VividDebug.Log("CH:" + max_y);
            Scroller.SetRect(new IRect(Rect.Width - 10,1, 10, Rect.Height-1));
            UpdatedChildren();





        }

    }
}
