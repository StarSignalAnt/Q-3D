#pragma once
#include "Component.h"
#include <vector>

class TerrainLayer;
class TerrainMesh;


class TerrainMeshComponent :
    public Component
{
public:
    TerrainMeshComponent(float width, float depth, float divisions, int layers);
    void OnAttach(GraphNode* node) override;

    void CreateTerrain(int layers);
    TerrainMesh* GetMesh() {
        return m_Mesh;
    }
    TerrainLayer* GetLayer(int i)
    {
        return m_Layers[i];
    }
    std::vector<TerrainLayer*> GetLayers() {
        return m_Layers;
    }
  //  Bounds GetTerrainBounds();
private:
    float m_Width;
    float m_Depth;
    TerrainMesh* m_Mesh;
    float m_Divisions;
    std::vector<TerrainLayer*> m_Layers;

};

