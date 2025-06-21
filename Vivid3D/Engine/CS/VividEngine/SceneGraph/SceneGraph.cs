using GlmNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Vivid.Debug;
using Vivid.Scene;

namespace Vivid.Scene
{

    public class CastResult
    {
        public bool Hit;
        public GlmNet.vec3 HitPoint;
        public float HitDistance;
        public GraphNode HitNode;
    }

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

        public CastResult MousePick(vec2 position)
        {

            var res = NativeBridge.SceneMousePick(position);

            CastResult result = new CastResult();

            result.Hit = NativeBridge.CastResultHit(res);
            result.HitPoint = NativeBridge.CastResultPoint(res);
            result.HitNode = new GraphNode(NativeBridge.CastResultNode(res));


            return result;

        }
        public CastResult RayCast(GlmNet.vec3 from,GlmNet.vec3 to)
        {

         
            var res = NativeBridge.SceneRayCast(from, to);
     
            CastResult result = new CastResult();
    
            result.Hit = NativeBridge.CastResultHit(res);
            result.HitPoint = NativeBridge.CastResultPoint(res);
            result.HitNode = new GraphNode(NativeBridge.CastResultNode(res));
       

            return result;




        }


    }
}
