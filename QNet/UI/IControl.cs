using QNet.Debug;
using QNet.Input;
using QNet.Texture;
using System.Collections.Generic;

namespace QNet.UI
{
    public delegate void OnClick(dynamic id);

    public enum MouseButton
    {
        Left, Right, Middle // etc.
    }

    public class IControl
    {
        // --- Properties ---
        public IControl Root { get; set; }
        public IRect Rect { get; set; } = new IRect(0, 0, 0, 0);
       
        public IPosition Position
        {
            get
            {
                return new IPosition(Rect.X, Rect.Y);
            }
            set
            {
                Rect.X = value.X;
                Rect.Y = value.Y;
            }
        }

        public string Text { get; set; }
        public IColor Color { get; set; } = new IColor(1, 1, 1, 1);
        public Texture2D Image { get; set; }
        public List<IControl> Controls { get; set; } = new List<IControl>();
        public OnClick Click { get; set; }
        public IPosition Offset { get; set; } = new IPosition();
        public bool UseOffset { get; set; } = true;
        public bool Static { get; set; } = false;
        public bool HasFocus { get; private set; }

        public IRect RenderRect
        {
            get
            {
                IRect rootRect = Root?.RenderRect ?? new IRect(0, 0, 0, 0);
                int offsetY = (Root?.UseOffset == true && UseOffset) ? Root.Offset.Y : 0;
                return new IRect(rootRect.X + Rect.X, rootRect.Y + Rect.Y - offsetY, Rect.Width, Rect.Height);
            }
        }

        // --- Public Methods ---
        public void SetRect(IRect rect)
        {
            Rect = rect;
            Updated();
        }

        public void SetPosition(IPosition position)
        {

            Position = position;


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

        public bool InBounds(IPosition position)
        {

            var rr = GetScissor();
            return position.X >= rr.X && position.X <= rr.X + rr.Width &&
                   position.Y >= rr.Y && position.Y <= rr.Y + rr.Height;
        }

        // --- Virtual Methods (for overriding in derived classes) ---
        public virtual void OnUpdate(float dt)
        {
            foreach (var con in Controls) con.OnUpdate(dt);
        }

        public virtual void OnRender()
        {
            RenderChildren();
        }

        public virtual void OnEnter() { }
        public virtual void OnLeave() { }
        public virtual void OnMouseDown(MouseButton button) { }
        public virtual void OnMouseUp(MouseButton button) { }
        public virtual void OnMouseMove(IPosition position, IPosition delta) { }

        public virtual void OnGainedFocus() => HasFocus = true;
        public virtual void OnLostFocus() => HasFocus = false;
        public virtual void OnKeyDown(GameKey key, bool shift, bool ctrl) { }
        public virtual void OnKeyUp(GameKey key, bool shift, bool ctrl) { }

        public bool CullNodes = false;
        public bool CullSelf = false;

        public virtual void UpdateChildren(float dt)
        {
            foreach (var c in Controls)
            {

                c.OnUpdate(dt);

            }
        }

        public IRect CullRect(IRect r)
        {
            IRect root = new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight);

            if (Root != null)
            {
                root = Root.GetScissor();
            }

            root = RenderRect;

            float x1, x2;
            float y1, y2;

            x1 = r.X;
            x2 = r.X + r.Width;
            y1 = r.Y;
            y2 = r.Y + r.Height;

            if (y1 < root.Y)
            {
                y1 = root.Y;
            }

            if (y1 > root.Y + root.Height)
            {

                y1 = root.Y + root.Height;

            }

            if (y2 < root.Y)
            {
                y2 = root.Y;
            }

            if (y2 > root.Y + root.Height)
            {

                y2 = root.Y + root.Height;

            }



            //IRect cur = RenderRect;


            return new IRect((int)x1, (int)y1, (int)x2 - (int)x1, (int)y2 - (int)y1);
        }

        public IRect GetScissor()
        {

            IRect root = new IRect(0,0,Engine.FrameWidth,Engine.FrameHeight);

            if (Root != null)
            {
                root = Root.GetScissor();
            }

            float x1, x2;
            float y1, y2;

            x1 = RenderRect.X;
            x2 = RenderRect.X+RenderRect.Width;
            y1 = RenderRect.Y;
            y2 = RenderRect.Y + RenderRect.Height;

            if (y1 < root.Y)
            {
                y1 = root.Y;
            }

            if (y1 > root.Y + root.Height)
            {

                y1 = root.Y + root.Height;

            }

            if (y2 < root.Y)
            {
                y2 = root.Y;
            }

            if (y2 > root.Y + root.Height)
            {

                y2 = root.Y + root.Height;

            }



            //IRect cur = RenderRect;


            return new IRect((int)x1, (int)y1, (int)x2 - (int)x1, (int)y2 - (int)y1);

            
        }

        public virtual void RenderChildren()
        {

            if (CullNodes)
            {
                IRect srect = GetScissor();

                GameUI.Draw.SetScissor(srect);

            }
            foreach (var c in Controls)
            { 
                if (c.CullSelf)
                {
                    var cc = c.GetScissor();
                    GameUI.Draw.SetScissor(c.RenderRect);
                    VividDebug.Log("CullSelf:" + c.ToString());
                }   
                
                c.OnRender();
                if (CullNodes)
                {
                    IRect srect = GetScissor();

                    GameUI.Draw.SetScissor(srect);

                }

                //  GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
            }
            GameUI.Draw.SetScissor(new IRect(0, 0, Engine.FrameWidth, Engine.FrameHeight));
        }

        // --- Update Propagation ---
        public void UpdatedChildren()
        {
            foreach (var con in Controls)
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
