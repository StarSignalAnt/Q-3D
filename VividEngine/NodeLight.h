#pragma once
#include "Node.h"

class RenderTargetCube;

class NodeLight :
    public Node
{
public:

    NodeLight();
    float3 GetDiffuse();
    float3 GetSpecular();
    void SetDiffuse(float3 diff) {
        m_Diffuse = diff;
    }
    void SetSpecular(float3 spec) {
        m_Specular = spec;
    }
    void SetRange(float range) {
        m_Range = range;
    }
    float GetRange();
    RenderTargetCube* GetShadowMap() {
        return m_ShadowMap;
    }
    void WriteNode(VFile* file);
    void ReadNode(VFile* file);

private:

    float3 m_Diffuse;
    float3 m_Specular;
    float m_Range = 2.0f;
    RenderTargetCube* m_ShadowMap;


};

