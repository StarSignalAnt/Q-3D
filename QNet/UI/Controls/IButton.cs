using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet.UI
{
    public class IButton : IControl
    {

        public IButton()
        {

            Color = new IColor(0.7f, 0.7f, 0.7f, 1.0f);

        }

        public override void OnEnter()
        {
            Color = new IColor(0.8f,0.8f,0.8f,1);
            //base.OnEnter();
        }

        public override void OnLeave()
        {

            Color = new IColor(0.7f, 0.7f, 0.7f, 1.0f);

        }

        public override void OnMouseDown(MouseButton button)
        {
            Color = new IColor(1, 1, 1, 1);
            Click?.Invoke(this);
        }

        public override void OnMouseUp(MouseButton button)
        {

            Color = new IColor(0.8f, 0.8f, 0.8f);

        }

        public override void OnRender()
        {

            IPosition textPos = new IPosition();
            textPos.X = RenderRect.X + Rect.Width / 2;
            textPos.Y = RenderRect.Y + Rect.Height / 2;
            textPos.X -= GameUI.Font.TextWidth(Text, 1.0f) / 2;
            textPos.Y += GameUI.Font.TextHeight(Text, 1.0f) / 2;
            GameUI.Draw.Rect(GameUI.Theme.Button, RenderRect,Color);
            GameUI.Font.DrawText(Text, textPos, 1.0f);



            RenderChildren();
        
        }

    }
}
