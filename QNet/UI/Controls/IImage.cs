using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using QNet.Debug;

namespace QNet.UI
{
    public class IImage : IControl
    {

        public override void OnRender()
        {

            var rect = RenderRect;

            GameUI.Draw.Rect(Image, rect,Color);
            VividDebug.Log("Rendering Image:" + Image.obj);
            VividDebug.Log("Rect:" + rect);

            RenderChildren();
        }

    }
}
