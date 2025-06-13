using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestLib
{
    public class Other
    {

        public int OtherInt = 250;

    }
    public class Test
    {
        public static string TestVal = "This is a C# string!";
        public int TestInt = 42;
        public float TestFloat = 3.0f;
        public double TestDouble = 4.5;
        public string TestString = "This is a C# string!";
        public bool TestBool1 = true;
        public Other Test1;



        public void Init(int a,int b)
        {
            System.Console.WriteLine("Check:" + Test1.OtherInt);
        }
        public int Add(int a, int b)
        {

            System.Console.WriteLine("Result" + (a + b));
            return a + b;
        }
    }
}
