#include "TerrainMeshComponent.h"
#include "TerrainMesh.h"
#include "TerrainLayer.h"
#include "Texture2D.h"
#include "PixelMap.h"
TerrainMeshComponent::TerrainMeshComponent(float width, float depth, float divisions, int layers)
{


	//m_Material = new MaterialTerrain;
	//m_DepthMaterial = new MaterialTerrainDepth;
	m_Width = width;
	m_Depth = depth;
	m_Divisions = divisions;
	CreateTerrain(layers);
//	m_Name = "Terrain";
}

TerrainMeshComponent::TerrainMeshComponent() {

    m_Mesh = new TerrainMesh;

}


void TerrainMeshComponent::CreateTerrain(int layers) {

    int widthSegments = (int)(m_Width * m_Divisions);
    int depthSegments = (int)(m_Depth * m_Divisions);
    float segmentWidth = m_Width / widthSegments;
    float segmentDepth = m_Depth / depthSegments;

    std::vector<TerrainVertex> vertices;
    std::vector<Triangle> triangles;

    for (int z = 0; z <= depthSegments; z++)
    {
        for (int x = 0; x <= widthSegments; x++)
        {
            float xPos = x * segmentWidth - m_Width / 2;
            float zPos = z * segmentDepth - m_Depth / 2;


            TerrainVertex vertex;

            vertex.position = glm::vec3(xPos, 0, zPos);
            vertex.texture = glm::vec3(((float)x / widthSegments) * 70, ((float)z / depthSegments) * 70, 0);
            vertex.color = glm::vec4(1, 1, 1, 1);
            vertex.layercoord = glm::vec3((float)x / (float)widthSegments, (float)z / (float)depthSegments, 0);


            vertices.push_back(vertex);

            if (x < widthSegments && z < depthSegments)
            {
                int currentIndex = x + z * (widthSegments + 1);
                int nextIndex = (x + 1) + z * (widthSegments + 1);
                int bottomIndex = x + (z + 1) * (widthSegments + 1);
                int bottomNextIndex = (x + 1) + (z + 1) * (widthSegments + 1);

                Triangle t1;

                t1.v0 = currentIndex;
                t1.v2 = nextIndex;
                t1.v1 = bottomNextIndex;

                triangles.push_back(t1);

                Triangle t2;
                t2.v0 = currentIndex;
                t2.v2 = bottomNextIndex;
                t2.v1 = bottomIndex;

                triangles.push_back(t2);
            }
        }
    }
    m_Mesh = new TerrainMesh;
    m_Mesh->SetVertices(vertices);
    m_Mesh->SetTriangles(triangles);

    for (int i = 0; i < layers; i++)
    {
        TerrainLayer* layer1 = new TerrainLayer;
        if (i == 0)
        {


            layer1->SetColor(new Texture2D("edit/blank_layer.png"));
        }
        else
        {

            layer1->SetColor(new Texture2D("engine\\white.png"));
        }
        layer1->SetNormal(new Texture2D("engine/norm.png"));
        layer1->SetSpecular(new Texture2D("engine/white.png"));
        layer1->SetSpecular(new Texture2D("engine/white.png"));
        //layer1.NormalMap = new Texture.Texture2D("engine\\maps\\blanknormal.jpg");
        //layer1.SpecularMap = new Texture.Texture2D("engine\\maps\\white.png");
        layer1->SetPixels(new PixelMap(1024, 1024,PixelMapDataType::FLOAT32,0));


        if (i == 0)
        {
            layer1->SetLayerMap(new Texture2D("engine\\white.png"));
            auto pixmap = new PixelMap(1024, 1024,PixelMapDataType::FLOAT32,1.0f);
           layer1->SetPixels(pixmap);
            layer1->SetLayerMap(new Texture2D(pixmap->GetWidth(), pixmap->GetHeight(), (float*)pixmap->GetData(), 4));
            //
            // layer1->SetPixels()

        }
        else
        {


           layer1->SetLayerMap(new Texture2D(1024, 1024, (float*)layer1->GetPixels()->GetData(), 4));
           //layer1.LayerMap = new Texture.Texture2D("engine\\terrain\\layer1.png");
        }
        m_Layers.push_back(layer1);
    }
    m_Mesh->CalculateNormals();
    m_Mesh->Build();
    //m_Mesh->SetOwner(this);



}

void TerrainMeshComponent::OnAttach(GraphNode* node) {
    m_Mesh->SetOwner(node);
    m_Owner = node;
}

TBounds TerrainMeshComponent::GetTerrainBounds() {

    auto verts = m_Mesh->GetVertices();

    float3 min, max;

    for (auto v : verts) {

        auto pos = float3(v.position.x, v.position.y, v.position.z);
        min = Diligent::min(min, pos);
        max = Diligent::max(max, pos);

    }

    TBounds res;
    res.Min = glm::vec3(min.x, min.y, min.z);
    res.Max = glm::vec3(max.x, max.y, max.z);
    auto r= min + ((max / min) / 2);
    res.Centre = glm::vec3(r.x, r.y, r.z);
    return res;
}
