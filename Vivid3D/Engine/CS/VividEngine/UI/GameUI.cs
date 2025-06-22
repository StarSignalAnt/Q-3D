using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Vivid.Draw;
using Vivid.Font;
using Vivid.Input;

namespace Vivid.UI
{
    public class GameUI
    {

        public static Draw2D Draw;
        public static UITheme Theme;
        public static GameFont Font;

        public IControl Root
        {
            get;
            set;
        }

        public IControl OverControl
        {
            get;
            set;
        }

        public IControl PressedControl
        {
            get;
            set;


        }

        private IPosition PreviousPosition = new IPosition(GameInput.MousePosition.X, GameInput.MousePosition.Y);

        public GameUI()
        {

            Theme = new Themes.ThemeGame();
            Draw = new Draw2D();
            Root = new IControl();
            Root.SetRect(new IRect(0,0,Engine.FrameWidth, Engine.FrameHeight));
            OverControl = null;
            PressedControl = null;
            Font = new GameFont("Engine/system2.ttf", 16);
            Font.SetDraw(Draw);
        }

        private List<IControl> GetList(IControl control,List<IControl> list)
        {

            list.Add(control);
            
            foreach(var con in control.Controls)
            {
                list = GetList(con, list);

            }

            return list;

        }

        public void Update()
        {



            var m_pos = GameInput.MousePosition;
            var delta = new IPosition(m_pos.X - PreviousPosition.X, m_pos.Y - PreviousPosition.Y);
            PreviousPosition = m_pos;


            var c_list = new List<IControl>();

            c_list = GetList(Root, c_list);

            c_list.Reverse();


            if (PressedControl == null)
            {
                foreach (var con in c_list)
                {

                    if (con.InBounds(m_pos))
                    {
                        if (OverControl != null)
                        {
                            if (OverControl != con)
                            {
                                OverControl.OnLeave();
                            }
                        }
                        if (con != OverControl)
                        {
                            con.OnEnter();
                        }
                        OverControl = con;
                        break;
                    }

                }
            }

            if (PressedControl == null)
            {

                if (OverControl != null)
                {

                    if (GameInput.MouseButton(0))
                    {
                        PressedControl = OverControl;
                        PressedControl.OnMouseDown(0);

                    }

                    OverControl.OnMouseMove(m_pos, delta);



                }

                

            }
            else
            {
                PressedControl.OnMouseMove(m_pos, delta);
                if (!GameInput.MouseButton(0))
                {
                    PressedControl.OnMouseUp(0);
                    PressedControl = null;
                }

            }


            Root.OnUpdate(0.0f);
        }

        public void Render()
        {


           

            Draw.Begin();
            Root.OnRender();
            Draw.Flush();
        }

    }
}
