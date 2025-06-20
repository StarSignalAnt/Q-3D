using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Scene;

namespace Vivid.Component
{
    public class SharpComponent
    {

        public GraphNode Node = null;

        public virtual void OnPlay() { }
        public virtual void OnStop() { }
        public virtual void OnUpdate(float dt) { }
        public virtual void OnRender(GraphNode camera) { }

    }
}
