using GlmNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Threading.Tasks;

internal class NativeBridge
{

    [DllImport("__Internal")]
    public static extern void NodeTurn(IntPtr node, float p, float y, float r, bool local);

    [DllImport("__Internal")]
    public static extern void NodeSetPosition(IntPtr node, GlmNet.vec3 pos);

    [DllImport("__Internal")]
    public static extern GlmNet.vec3 NodeGetPosition(IntPtr node);
    [DllImport("__Internal")]
    public static extern void NodeGetRotation(IntPtr node, float[] mat);

    [DllImport("__Internal")]
    public static extern void NodeSetRotation(IntPtr node, float[] mat);
    [DllImport("__Internal")]
    public static extern void NodeGetWorldMatrix(IntPtr node, float[] mat);
    [DllImport("__Internal")]
    public static extern GlmNet.vec3 NodeGetScale(IntPtr node);
    [DllImport("__Internal")]
    public static extern void NodeSetScale(IntPtr node, GlmNet.vec3 scale);

    [DllImport("__Internal")]
    public static extern IntPtr SceneGetRoot();
    [DllImport("__Internal")]
    public static extern int NodeNodeCount(IntPtr node);
    [DllImport("__Internal")]
    public static extern IntPtr NodeGetNode(IntPtr node, int index);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern IntPtr NodeGetName(IntPtr node);

    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern void ConsoleLog(string msg);
    [DllImport("__Internal")]
    public static extern IntPtr CreateDraw2D();
    [DllImport("__Internal")]
    public static extern void DrawRect(IntPtr draw, IntPtr tex, GlmNet.vec2 pos, GlmNet.vec2 size, GlmNet.vec4 color);
    [DllImport("__Internal")]
    public static extern void DrawBegin(IntPtr draw);

    [DllImport("__Internal")]
    public static extern void DrawFlush(IntPtr draw);
    [DllImport("__Internal")]
    public static extern void SetFontDraw(IntPtr font,IntPtr draw);


    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern IntPtr LoadTexture2D(string path);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern IntPtr CreateVideo(string path);
    [DllImport("__Internal")]
    public static extern void UpdateVideo(IntPtr video);
    [DllImport("__Internal")]
    public static extern IntPtr VideoGetFrame(IntPtr video);
    [DllImport("__Internal")]
    public static extern void PlayVideo(IntPtr video);
    [DllImport("__Internal")]
    public static extern int EngineGetWidth();
    [DllImport("__Internal")]
    public static extern int EngineGetHeight();
    [DllImport("__Internal")]
    public static extern IntPtr SceneRayCast(GlmNet.vec3 from, GlmNet.vec3 to);
    [DllImport("__Internal")]
    public static extern bool CastResultHit(IntPtr result);
    [DllImport("__Internal")]
    public static extern GlmNet.vec3 CastResultPoint(IntPtr result);
    [DllImport("__Internal")]
    public static extern IntPtr CastResultNode(IntPtr result);
    [DllImport("__Internal")]
    public static extern IntPtr SceneMousePick(GlmNet.vec2 position);

    [DllImport("__Internal")]
    public static extern int GetMouseX();
    [DllImport("__Internal")]
    public static extern int GetMouseY();
    [DllImport("__Internal")]
    public static extern bool GetMouseDown(int id);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern IntPtr LoadFont(string id,float size);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern void FontDrawText(IntPtr font,string text,vec2 pos,float size);
    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern int FontTextWidth(IntPtr font, string text, float size);


    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern int FontTextHeight(IntPtr font, string text, float size);

    [DllImport("__Internal", CharSet = CharSet.Ansi)]
    public static extern void SetScissor(int x,int y,int w,int h);


}