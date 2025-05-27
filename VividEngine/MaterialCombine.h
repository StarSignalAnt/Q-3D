#pragma once
#include "Material2D.h"

class MaterialCombine : public Material2D
{
public:

    MaterialCombine();
    void Create();
    void Bind(bool sp) override;
    void SetCombine(float i1,float i2) {
        m_C1 = i1;
        m_C2 = i2;
    }
    //float GetLimit() {
       
    //}

private:
    float m_C1 = 0.5f;
    float m_C2 = 0.55f;
};

