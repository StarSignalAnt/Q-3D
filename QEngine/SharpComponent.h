#pragma once
#include "Component.h"
#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"

struct Vec3 {
    float x, y, z;
    Vec3(float vx, float vy, float vz) {
        x = vx;
        y = vy;
        z = vz;
    }
    Vec3() {
        x = y = z = 0;
    }
};

struct Vec2 {

    float x, y;

};

struct Vec4 {
    float x, y, z, w;
};

//Node
extern "C" __declspec(dllexport) void NodeTurn(void* node,float x,float y,float z);
extern "C" __declspec(dllexport) void NodeSetPosition(void* node,Vec3 pos);
extern "C" __declspec(dllexport) Vec3 NodeGetPosition(void* node);
extern "C" __declspec(dllexport) void NodeGetRotation(void* node,float* out);
extern "C" __declspec(dllexport) void NodeGetWorldMatrix(void* node, float* out);
extern "C" __declspec(dllexport) void NodeSetRotation(void* node, float* out);
extern "C" __declspec(dllexport) void NodeSetScale(void* node,Vec3 scale);
extern "C" __declspec(dllexport) Vec3 NodeGetScale(void* node);
extern "C" __declspec(dllexport) void* SceneGetRoot();
extern "C" __declspec(dllexport) int NodeNodeCount(void* node);
extern "C" __declspec(dllexport) void* NodeGetNode(void* node,int index);
extern "C" __declspec(dllexport) void* NodeGetName(void* node);
extern "C" __declspec(dllexport) void* NodeGetLongName(void* node);
extern "C" __declspec(dllexport) void DebugLog(const char* str);
extern "C" __declspec(dllexport) void* CreateVideo(const char* path);
extern "C" __declspec(dllexport) void PlayVideo(void* video);
extern "C" __declspec(dllexport) void UpdateVideo(void* video);
extern "C" __declspec(dllexport) void* VideoGetFrame(void* video);
extern "C" __declspec(dllexport) void RenderVideo(void* video, Vec2 pos, Vec2 size);
extern "C" __declspec(dllexport) void* CreateDraw2D();
extern "C" __declspec(dllexport) void DrawRect(void* draw, void* tex, Vec2 pos, Vec2 size,Vec4 color);
extern "C" __declspec(dllexport) void DrawBegin(void* draw);
extern "C" __declspec(dllexport) void DrawFlush(void* draw);
extern "C" __declspec(dllexport) void* LoadTexture2D(const char* path);
extern "C" __declspec(dllexport) int EngineGetWidth();
extern "C" __declspec(dllexport) int EngineGetHeight();
extern "C" __declspec(dllexport) void* SceneRayCast(Vec3 from,Vec3 to);
extern "C" __declspec(dllexport) bool CastResultHit(void* res);
extern "C" __declspec(dllexport) Vec3 CastResultPoint(void* res);
extern "C" __declspec(dllexport) void* CastResultNode(void* res);
extern "C" __declspec(dllexport) void* SceneMousePick(Vec2 pos);
extern "C" __declspec(dllexport) int GetMouseX();
extern "C" __declspec(dllexport) int GetMouseY();
extern "C" __declspec(dllexport) bool GetKeyDown(int key);
extern "C" __declspec(dllexport) bool GetMouseDown(int id);
extern "C" __declspec(dllexport) void* LoadFont(char* path,float size);
extern "C" __declspec(dllexport) void SetFontDraw(void* font, void* draw);
extern "C" __declspec(dllexport) void FontDrawText(void* font, wchar_t* text,Vec2 pos, float size);
extern "C" __declspec(dllexport) int FontTextWidth(void* font, char* text,float size);
extern "C" __declspec(dllexport) int FontTextHeight(void* font, char* text, float size);
extern "C" __declspec(dllexport) void SetScissor(int x,int y,int w,int h);
extern "C" __declspec(dllexport) void* GetComponent(void* node,char* name);

class SharpComponent :
    public Component
{
public:
    SharpComponent();
    ~SharpComponent()
    {
        if (m_gcHandle) {
            mono_gchandle_free(m_gcHandle);
        }
        if (m_graphNode_gchandle) {
            mono_gchandle_free(m_graphNode_gchandle);
        }

        // 2. Now, delete the C++ wrapper objects that were allocated with 'new'.
        delete m_Instance;
        delete m_GraphClass;
        // The component takes ownership of the class template passed to it.
        delete m_StaticClass;
    }
    void SetScript(std::string file, std::string name);
    void OnPlay() override;
    void OnStop() override;
    void OnUpdate(float dt) override;
    void OnRender(GraphNode* camera) override;
    void SetClass(MClass* cls,MAsm* as,MAsm* vivid);
	void SetName(std::string name) { m_Name = name; }
	std::string GetName() { return m_Name; }
    MClass* GetClass() { return m_Instance; };
    void ReInit();
    MClass* CreateGraphNode();
    void SetTime(std::filesystem::file_time_type time) {
        edit_time = time;
    }
    std::filesystem::file_time_type GetTime() {
        return edit_time;
    }
  
    MonoObject* GetInstance() {
        // Get a fresh, guaranteed-valid pointer to the C# object.
        return mono_gchandle_get_target(m_gcHandle);
    }
    uint32_t GetHandle() {
        return m_gcHandle;
    }

    uint32_t m_gcHandle = 0;
    uint32_t m_graphNode_gchandle = 0;
private:

    MAsm* m_Assembly = nullptr;
    MAsm* m_Vivid;
    MClass* m_StaticClass = nullptr;
    MClass* m_GraphClass;
    MClass* m_Instance = nullptr;
    std::string m_Name;
    std::filesystem::file_time_type edit_time;
}
;

