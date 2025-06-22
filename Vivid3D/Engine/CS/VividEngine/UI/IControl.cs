using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Texture;

namespace Vivid.UI
{

    public delegate void OnClick(dynamic id);

    public enum MouseButton
    {
        Left,Right,Middle,Back,Forward,Aux1,Aux2,Aux3,Aux4,Aux5,Aux6
    }
    public class IControl
    {

        public IControl Root
        {
            get;
            set;
        }

        public IRect Rect
        {
            get
            {
                return _Rect;
            }
            set
            {
                _Rect = value;
              
            }
        }
        IRect _Rect = new IRect(0, 0, 0, 0);

        public IRect RenderRect
        {
            get
            {
                IRect root = new IRect(0, 0, 0, 0);
                if (Root != null)
                {
                    root.X = Root.RenderRect.X;
                    root.Y = Root.RenderRect.Y;
                }


                if (Root != null)
                {
                    if (Root.UseOffset && UseOffset)
                    {

                        return new IRect(root.X + Rect.X, (root.Y + Rect.Y) - Root.Offset.Y, Rect.Width, Rect.Height);
                    }
                }
                //lse
                {
                    return new IRect(root.X + Rect.X, (root.Y + Rect.Y), Rect.Width, Rect.Height);
                }
            }
        }

        public string Text
        {
            get;
            set;
        }

        public IColor Color
        {
            get;
            set;
        }

        public Texture2D Image
        {
            get;
            set;
        }

        public List<IControl> Controls
        {
            get;
            set;
        }

        public OnClick Click
        {
            get;
            set;
        }

        public IPosition Offset
        {
            get;
            set;
        }

        public bool UseOffset
        {
            get;
            set;
        }

        public bool Static
        {
            get;
            set;
        }

        public IControl()
        {


            Static = false;
            UseOffset = true;
            Controls = new List<IControl>();
            Root = null;
            Rect = new IRect(0, 0, 0, 0);
            Color = new IColor(1, 1, 1, 1);
            Click = null;
            Offset = new IPosition();
        }

        public void SetRect(IRect rect)
        {

            Rect = rect;
            Updated();

        }

        public void AddControls(params IControl[] controls)
        {
            foreach (var control in controls)
            {

                Controls.Add(control);
                control.Root = this;
            }
            Updated();
        }

        public virtual void OnUpdate(float dt)
        {

            UpdateChildren(dt);
        }

        public virtual void UpdateChildren(float dt)
        {
            foreach(var con in Controls)
            {
                con.OnUpdate(dt);
            }
        }

        public virtual void OnRender()
        {
           
            RenderChildren();
        }

        public virtual void RenderChildren()
        {

            foreach (var control in Controls)
            {

                control.OnRender();

            }

        }

        public virtual void OnEnter()
        {

        }

        public virtual void OnLeave()
        {

        }

        public virtual void OnMouseDown(MouseButton button)
        {

        }

        public virtual void OnMouseUp(MouseButton button)
        {

        }

        public virtual void OnMouseMove(IPosition position,IPosition delta)
        {


        }

        public virtual bool InBounds(IPosition position)
        {
            var rr = RenderRect;
            if(position.X>=rr.X && position.X<=rr.X+rr.Width)
            {
                if(position.Y>=rr.Y && position.Y <= rr.Y + rr.Height)
                {
                    return true;
                }
            }
            return false;

        }

        public void UpdatedChildren()
        {
            foreach(var con in Controls)
            {
                con.Updated();
            }
        }
        public virtual void Updated()
        {

            UpdatedChildren();
        }

    }
}
