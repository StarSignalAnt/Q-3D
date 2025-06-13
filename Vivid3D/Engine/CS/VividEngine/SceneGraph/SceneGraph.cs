using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Scene;

namespace Vivid.Scene
{
    public class SceneGraph
    {
        public GraphNode RootNode
        {
            get; set;
        }

        

        public SceneGraph()
        {

            RootNode = new GraphNode();
            Console.WriteLine("Getting NodePTR");
            RootNode.NodePtr = NativeBridge.SceneGetRoot();
            Console.WriteLine("NodePTR: " + RootNode.NodePtr.ToString());

        }

    }
}
