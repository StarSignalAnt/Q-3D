#pragma once
#include "MaterialBase.h"
class MaterialMeshPBR :
    public MaterialBase
{
public:

    MaterialMeshPBR();
    void Create();
    void Bind(bool sp);
    float GetRoughOverdrive() {
        return m_RoughnessOverdrive;
    }
    float GetMetalOverdrive() {
        return m_MetalOverdrive;
    }
    void SetRoughOverdrive(float drive) {
        m_RoughnessOverdrive = drive;
    }
    void SetMetalOverdrive(float drive) {
        m_MetalOverdrive = drive;
    }

private:

    float m_RoughnessOverdrive = 0.0f;
    float m_MetalOverdrive = 0.0f;

};

