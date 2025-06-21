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

        public CastResult RayCast(GlmNet.vec3 from,GlmNet.vec3 to)
        {

            VividDebug.Log("RC:");
            var res = NativeBridge.SceneRayCast(from, to);
            VividDebug.Log("RC2:");
            CastResult result = new CastResult();
            VividDebug.Log("RC3:");
            result.Hit = NativeBridge.CastResultHit(res);
            result.HitPoint = NativeBridge.CastResultPoint(res);
            result.HitNode = new GraphNode(NativeBridge.CastResultNode(res));
            VividDebug.Log("RC4:");

            VividDebug.Log("Hit:" + result.Hit);

            return result;




        }


    }
}
