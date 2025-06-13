#pragma once
#include "Component.h"
#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"

struct Vec3 {
    float x, y, z;
};

//Node
extern "C" __declspec(dllexport) void NodeTurn(void* node,float x,float y,float z);
extern "C" __declspec(dllexport) void NodeSetPosition(void* node,Vec3 pos);
extern "C" __declspec(dllexport) Vec3 NodeGetPosition(void* node);
extern "C" __declspec(dllexport) void NodeGetRotation(void* node,float* out);
extern "C" __declspec(dllexport) void NodeSetRotation(void* node, float* out);
extern "C" __declspec(dllexport) void NodeSetScale(void* node,Vec3 scale);
extern "C" __declspec(dllexport) Vec3 NodeGetScale(void* node);
extern "C" __declspec(dllexport) void* SceneGetRoot();
extern "C" __declspec(dllexport) int NodeNodeCount(void* node);
extern "C" __declspec(dllexport) void* NodeGetNode(void* node,int index);
extern "C" __declspec(dllexport) void* NodeGetName(void* node);


class SharpComponent :
    public Component
{
public:
    void SetScript(std::string file, std::string name);
    void OnUpdate(float dt) override;
private:

    MAsm* m_Assembly = nullptr;
    MAsm* m_Vivid;
    MClass* m_StaticClass = nullptr;
    MClass* m_GraphClass;
    MClass* m_Instance = nullptr;
};

