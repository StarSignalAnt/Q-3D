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

        public virtual void OnUpdate(float dt) { }

    }
}
