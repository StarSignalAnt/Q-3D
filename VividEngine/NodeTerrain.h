#pragma once
#include "Node.h"
#include <vector>

class TerrainLayer;
class TerrainMesh;
class MaterialTerrain;
class MaterialTerrainDepth;
class MaterialBase;

class NodeTerrain :
    public Node
{
public:

    NodeTerrain(float width, float depth, float divisions, int layers);
    NodeTerrain();
    void CreateTerrain(int layers);
    void Render(bool sp);
    void RenderDepth();
    TerrainMesh* GetMesh();
    TerrainLayer* GetLayer(int i)
    {
        return m_Layers[i];
    }
    std::vector<TerrainLayer*> GetLayers() {
        return m_Layers;
    }
    Bounds GetTerrainBounds();
    void WriteNode(VFile* file);
    void ReadNode(VFile* file);
    void SaveTerrain(std::string path);
    void LoadTerrain(std::string path);
private:


    MaterialTerrain* m_Material;
    MaterialTerrainDepth* m_DepthMaterial;
    MaterialBase* m_InitialMat;
    float m_Width;
    float m_Depth;
    TerrainMesh* m_Mesh;
    float m_Divisions;
    std::vector<TerrainLayer*> m_Layers;
   

};

