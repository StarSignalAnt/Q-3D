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
            GameUI.Draw.Rect(GameUI.Theme.Body, TitleRect, GameUI.Theme.ForeColor);

            TitleRect.X = TitleRect.X + 1;
            TitleRect.Y = TitleRect.Y + 1;
            TitleRect.Width = TitleRect.Width - 2;

            TitleRect.Height = TitleRect.Height - 2;
            GameUI.Draw.Rect(GameUI.Theme.Body, TitleRect,GameUI.Theme.WindowTitle);
          



            IPosition pos = new IPosition(RenderRect.X + 8, RenderRect.Y  + 8  +GameUI.Font.TextHeight(Text, 1.0f) / 2);

            GameUI.Font.DrawText(Text, pos, 1.0f);



            RenderChildren();

        }

        public override void Updated()
        {
            Body.Rect = new IRect(0,16, Rect.Width, Rect.Height - 16);
            //Rect.Height = 25;

            UpdatedChildren();
        }


    }
}
