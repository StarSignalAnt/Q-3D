#pragma once
#include "Component.h"
#include "MaterialTerrainDepth.h"
class TerrainDepthRenderer :
    public Component
{
public:
    TerrainDepthRenderer();
    void OnRenderDepth(GraphNode* camera) override;


private:

    MaterialTerrainDepth* m_TerrainDepth;

};

