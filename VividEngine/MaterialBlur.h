#pragma once
#include "Material2D.h"
class MaterialBlur :
    public Material2D
{
public:

    MaterialBlur();
    void Create();
    void Bind(bool sp) override;
    void SetLimit(float blur) {
        m_Blur = blur;
    }
    float GetLimit() {
        return m_Blur;
    }

private:
    float m_Blur = 0.525f;
};

