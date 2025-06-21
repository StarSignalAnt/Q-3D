using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Vivid.Debug
{
    public class VividDebug
    {

        public static void Log(string msg)
        {
            NativeBridge.ConsoleLog(msg);
        }

    }
}
