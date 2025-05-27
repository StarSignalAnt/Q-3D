#pragma once
#include "MaterialBase.h"
class MaterialTerrainDepth :
    public MaterialBase
{
public:

    MaterialTerrainDepth();
    void Create();
    void Bind(bool sp);
    void SetLayerMap(Texture2D* layer);

private:
};

