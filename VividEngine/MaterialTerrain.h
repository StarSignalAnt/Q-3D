#pragma once
#include "MaterialBase.h"

class Texture2D;

class MaterialTerrain :
    public MaterialBase
{
public:

    MaterialTerrain();
    void Create();
    void Bind(bool sp);
    void SetLayerMap(Texture2D* layer);
    void SetLayerIndex(int index) {
        m_LayerIndex = index;
    }

private:

    int m_LayerIndex = 0;
    Texture2D* m_LayerMap = nullptr;

};

