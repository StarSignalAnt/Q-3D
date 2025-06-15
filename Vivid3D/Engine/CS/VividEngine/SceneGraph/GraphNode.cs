using GlmNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;


namespace Vivid.Scene
{

    public enum NodeSpace
    {
        Local,
        World
    }
    public class GraphNode
    {

        public IntPtr EngineNode = IntPtr.Zero;
        public GlmNet.vec3 Position
        {
            get
            {
                return NativeBridge.NodeGetPosition(NodePtr);
            }
            set
            {
                NativeBridge.NodeSetPosition(NodePtr,value);
                //     NativeBridge.NodeSetPosition(NodePtr, value.x, value.y, value.z);
            }
        }
        public GlmNet.vec3 Scale
        {
            get
            {
                return NativeBridge.NodeGetScale(NodePtr);
            }
            set
            {
                NativeBridge.NodeSetScale(NodePtr, value);
                //     NativeBridge.NodeSetPosition(NodePtr, value.x, value.y, value.z);
            }
        }
        public GlmNet.mat4 Rotation
        {
            get
            {
                float[] raw = new float[16];
                NativeBridge.NodeGetRotation(NodePtr, raw);
                _Rotation= new mat4(
       new vec4(raw[0], raw[1], raw[2], raw[3]),     // Column 0
       new vec4(raw[4], raw[5], raw[6], raw[7]),     // Column 1
       new vec4(raw[8], raw[9], raw[10], raw[11]),   // Column 2
       new vec4(raw[12], raw[13], raw[14], raw[15])  // Column 3
       );
                return _Rotation;

            }
            set
            {
                var matrix = value;
                float[] raw = new float[16];
                raw[0] = matrix[0, 0]; raw[1] = matrix[0, 1]; raw[2] = matrix[0, 2]; raw[3] = matrix[0, 3];
                raw[4] = matrix[1, 0]; raw[5] = matrix[1, 1]; raw[6] = matrix[1, 2]; raw[7] = matrix[1, 3];
                raw[8] = matrix[2, 0]; raw[9] = matrix[2, 1]; raw[10] = matrix[2, 2]; raw[11] = matrix[2, 3];
                raw[12] = matrix[3, 0]; raw[13] = matrix[3, 1]; raw[14] = matrix[3, 2]; raw[15] = matrix[3, 3];
                NativeBridge.NodeSetRotation(NodePtr, raw);
                _Rotation = value;
            }
        }
        public GlmNet.mat4 _Rotation = new GlmNet.mat4(1.0f);
        public IntPtr NodePtr = IntPtr.Zero;

        public List<GraphNode> Nodes
        {
            get
            {

                var r = new List<GraphNode>();
                int c = NativeBridge.NodeNodeCount(NodePtr);
                System.Console.WriteLine("Nodes:"+c);
                for(int i = 0; i < c; i++)
                {
                    var nn = new GraphNode() { NodePtr = NativeBridge.NodeGetNode(NodePtr, i) };
                    r.Add(nn);

                    System.Console.WriteLine("Node:" + i + " Name:" + nn.Name);
                }
                return r;
            }
        }

        public string Name
        {
            get
            {
                if(NodePtr == IntPtr.Zero)
                {
                    return "NodePtr is null!";
                }
                var r = NativeBridge.NodeGetName(NodePtr);
                string msg = Marshal.PtrToStringAnsi(r);
                return msg;
            }
        }

        public string GetName()
        {
            return Name;

        }

        public GraphNode()
        {

            System.Console.WriteLine("Graphnode created.");
         

        }

        public void Turn(GlmNet.vec3 rotation, NodeSpace space)
        {
            //    Turn(rotation.x, rotation.y, rotation.z);

            switch (space) {
                case NodeSpace.Local:
                    NativeBridge.NodeTurn(NodePtr, rotation.x, rotation.y, rotation.z,true);
                    break;
                case NodeSpace.World:
                    NativeBridge.NodeTurn(NodePtr, rotation.x, rotation.y, rotation.z,false);
                    break;
            }
            //Console.WriteLine("Node Turned: " + x + " " + y + " " + z);
        }

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(this, obj))
                return true;

            if (obj is null || obj.GetType() != GetType())
                return false;


            GraphNode other = (GraphNode)obj;
            return NodePtr == other.NodePtr;//  Id == other.Id && Name == other.Name;
        }

        public override int GetHashCode()
        {
            unchecked
            {
                int hash = 17;
                hash = hash * 23 + NodePtr.GetHashCode();

                return hash;
            }
        }

        public static bool operator ==(GraphNode left, GraphNode right)
        {
            if (left is null)
                return right is null;

            return left.Equals(right);
        }

        public static bool operator !=(GraphNode left, GraphNode right)
        {
            return !(left == right);
        }


    }
}
