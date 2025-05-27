#pragma once
#include "MaterialBase.h"
#include "Material2D.h"

class MaterialColorLimit :
    public Material2D
{
public:

    MaterialColorLimit();
    void Create();
    void Bind(bool sp) override;
    void SetLimit(float limit) {
        m_ColorLimit = limit;
    }
    float GetLimit() {
        return m_ColorLimit;
    }

private:
    float m_ColorLimit = 0.85f;

};

