using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Vivid.Scene;

namespace Vivid
{
    public class Other
    {

        public int OtherInt = 250;

    }

    public class Test : Vivid.Component.SharpComponent
    {
        GraphNode root;
        Test()
        {

            Console.WriteLine("Test Component Created!");

            var scene = new SceneGraph();
            root = scene.RootNode;


            //var nodes = root.Nodes;

            root.Scale = new GlmNet.vec3(5, 5, 5);

        }

        public override void OnUpdate(float dt)
        {
     
           // Node.Turn(new GlmNet.vec3(0, 1, 0), Scene.NodeSpace.World);

   
            foreach(var c in root.Nodes)
            {
                c.Scale = c.Scale + new GlmNet.vec3(0.04f, 0.03f, 0.02f);
            }

            //Node.Position = Node.Position + new GlmNet.vec3(0, 0.1f, 0);

            var rot = Node.Rotation * GlmNet.glm.rotate(0.05f,new GlmNet.vec3(0,1,0));
            Console.WriteLine("Rotation: " + rot.ToString());
            Node.Scale = Node.Scale + new GlmNet.vec3(0.01f, 0.01f, 0.01f);
            Node.Rotation = rot;

        }

    }

    public class Test2
    {
        public static string TestVal = "This is a C# string!";
        public int TestInt = 42;
        public float TestFloat = 3.0f;
        public double TestDouble = 4.5;
        public string TestString = "This is a C# string!";
        public bool TestBool1 = true;
        public IntPtr pt1;
        public Other Test1;


        public IntPtr TestPtr(IntPtr p1,int b)
        {
            pt1 = p1;
            System.Console.WriteLine("PT1:"+ pt1.ToString()+" B:"+b.ToString());
            return p1;
        }
        public void Init(int a,int b)
        {
            System.Console.WriteLine("Ptr:"+pt1.ToString());
        }
        public int Add(int a, int b)
        {

            System.Console.WriteLine("Result" + (a + b));
            return a + b;
        }
    }
}
