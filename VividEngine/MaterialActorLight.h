#pragma once
#include "MaterialBase.h"


class MaterialActorLight :
    public MaterialBase
{
public:
    MaterialActorLight();
    void Create();
    void Bind(bool sp);
    void SetBones(std::vector<float4x4> bones) {
        //m_Bones = bones;
    
        m_Bones = bones;
    }
private:

    std::vector<float4x4> m_Bones;
    RefCntAutoPtr<ISampler> m_Sampler;


};

