using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet.UI.Controls
{
    public class IWindow : IControl
    {

        public IWindowBody Body
        {
            get;
            set;
        }
        private bool InTitle = false;
        private bool Dragging = false;

        public IWindow(string title)
        {

            Text = title;
            Body = new IWindowBody();
            AddControls(Body);


        }

        public override void OnMouseMove(IPosition position, IPosition delta)
        {
            InTitle = false;
            if (position.X > RenderRect.X && position.Y > RenderRect.Y && position.X<RenderRect.X + RenderRect.Width && position.Y < RenderRect.Y + 25)
            {
                InTitle = true;
            }

            if (Dragging && !Static)
            {
                Rect.X = Rect.X + delta.X;
                Rect.Y = Rect.Y + delta.Y;
            }

        }

        public override void OnMouseDown(MouseButton button)
        {
            if (InTitle)
            {
                if (button == MouseButton.Left)
                {
                    Dragging = true;
                }
            }

        }

        public override void OnMouseUp(MouseButton button)
        {
           
                if (button == MouseButton.Left)
                {
                    Dragging = false;
                }
            

        }

        public override void OnRender()
        {

            IRect TitleRect = new IRect(RenderRect.X, RenderRect.Y, Rect.Width, 20);

            IRect outLine = new IRect(TitleRect.X, TitleRect.Y, Rect.Width, 20);
            GameUI.Draw.Rect(GameUI.Theme.White, outLine, new IColor(0.4f,0.4f,0.4f,0.85f));

            TitleRect.X = TitleRect.X + 2;
            TitleRect.Y = TitleRect.Y + 2;
            TitleRect.Width = TitleRect.Width - 4;

            TitleRect.Height = TitleRect.Height - 6;
            GameUI.Draw.Rect(GameUI.Theme.Body, TitleRect,GameUI.Theme.WindowTitle);
          




            IPosition pos = new IPosition(RenderRect.X + 8, RenderRect.Y  + 8  +GameUI.Font.TextHeight(Text, 1.0f) / 2);

            GameUI.Font.DrawText(Text, pos, 1.0f);



            RenderChildren();

        }

        public override void Updated()
        {
            Body.Rect = new IRect(0,17, Rect.Width, Rect.Height - 17);
            //Rect.Height = 25;

            UpdatedChildren();
        }


    }
}
