using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

internal class NativeBridge
{

    [DllImport("__Internal")]
    public static extern void NodeTurn(IntPtr node,float p,float y,float r,bool local);

    [DllImport("__Internal")]
    public static extern void NodeSetPosition(IntPtr node,GlmNet.vec3 pos);

    [DllImport("__Internal")]
    public static extern GlmNet.vec3 NodeGetPosition(IntPtr node);
    [DllImport("__Internal")]
    public static extern void NodeGetRotation(IntPtr node, float[] mat);

    [DllImport("__Internal")]
    public static extern void NodeSetRotation(IntPtr node, float[] mat);
    [DllImport("__Internal")]
    public static extern GlmNet.vec3 NodeGetScale(IntPtr node);
    [DllImport("__Internal")]
    public static extern void NodeSetScale(IntPtr node,GlmNet.vec3 scale);

    [DllImport("__Internal")]
    public static extern IntPtr SceneGetRoot();
    [DllImport("__Internal")]
    public static extern int NodeNodeCount(IntPtr node);
    [DllImport("__Internal")]
    public static extern IntPtr NodeGetNode(IntPtr node,int index);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern IntPtr NodeGetName(IntPtr node);

    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern void ConsoleLog(string msg);

}
