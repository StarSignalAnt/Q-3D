#pragma once
#include "Component.h"
#include "MaterialTerrain.h"

class GraphNode;

class TerrainRendererComponent :
    public Component
{
public:

    TerrainRendererComponent();
    void OnRender(GraphNode* camera) override;

private:

    MaterialTerrain* m_Terrain;
};

