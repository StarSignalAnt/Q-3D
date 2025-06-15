﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Vivid.Scene;
using Vivid.Debug;

namespace Vivid
{
    public class Other
    {

        public int OtherInt = 250;

    }

    public class Test : Vivid.Component.SharpComponent
    {
 

        public float TurnSpeed = 0.1f;
        public int TurnFactor = 1;
        public GraphNode TestNode;// = new GraphNode();

        Test()
        {
            
            Console.WriteLine("Test Component Created!");

        
        }

        public override void OnUpdate(float dt)
        {
     
           // Node.Turn(new GlmNet.vec3(0, 1, 0), Scene.NodeSpace.World);

   
            //Node.Position = Node.Position + new GlmNet.vec3(0, 0.1f, 0);

            var rot = Node.Rotation * GlmNet.glm.rotate(TurnFactor,new GlmNet.vec3(0,1,0));
            Console.WriteLine("Rotation: " + rot.ToString());
        //    Node.Scale = Node.Scale + new GlmNet.vec3(0.01f, 0.01f, 0.01f);
       //     Node.Rotation = rot;
           // TestNode.Position = TestNode.Position + new GlmNet.vec3(0.1f,0, 0);
            VividDebug.Log("It is working!");

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
