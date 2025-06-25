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
            var rr = RenderRect;
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
            foreach (var control in Controls) control.OnRender();
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


        public virtual void UpdateChildren(float dt)
        {
            foreach (var c in Controls)
            {

                c.OnUpdate(dt);

            }
        }

        public virtual void RenderChildren()
        {

            foreach (var c in Controls) c.OnRender();

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
