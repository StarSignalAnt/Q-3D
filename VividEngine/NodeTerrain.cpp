#include "pch.h"
#include "NodeTerrain.h"
#include <vector>
#include "data.h"
#include "TerrainMesh.h"
#include "TerrainLayer.h"
#include "Texture2D.h"
#include "PixelMap.h"
#include "TerrainMeshBuffer.h"
#include "MaterialTerrain.h"
#include "MaterialTerrainDepth.h"
#include "VFile.h"

NodeTerrain::NodeTerrain() {

    m_Material = new MaterialTerrain;
    m_DepthMaterial = new MaterialTerrainDepth;
    m_Name = "Terrain";
}

NodeTerrain::NodeTerrain(float width, float depth, float divisions, int layers)
{

    m_Material = new MaterialTerrain;
    m_DepthMaterial = new MaterialTerrainDepth;
	m_Width = width;
	m_Depth = depth;
	m_Divisions = divisions;
	CreateTerrain(layers);
    m_Name = "Terrain";
}

void NodeTerrain::CreateTerrain(int layers) {

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

            vertex.position = float3(xPos, 0, zPos);
            vertex.texture = float3(((float)x / widthSegments) * 70, ((float)z / depthSegments) * 70,0);
            vertex.color = float4(1, 1, 1, 1);
            vertex.layercoord = float3((float)x / (float)widthSegments, (float)z / (float)depthSegments, 0);

          
            vertices.push_back(vertex);

            if (x < widthSegments && z < depthSegments)
            {
                int currentIndex = x + z * (widthSegments + 1);
                int nextIndex = (x + 1) + z * (widthSegments + 1);
                int bottomIndex = x + (z + 1) * (widthSegments + 1);
                int bottomNextIndex = (x + 1) + (z + 1) * (widthSegments + 1);

                Triangle t1;

                t1.v0 = currentIndex;
                t1.v1 = nextIndex;
                t1.v2 = bottomNextIndex;

                triangles.push_back(t1);

                Triangle t2;
                t2.v0 = currentIndex;
                t2.v1 = bottomNextIndex;
                t2.v2 = bottomIndex;

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


            layer1->SetColor(new Texture2D("edit/blank_layer.png",true));
        }
        else
        {

            layer1->SetColor(new Texture2D("engine\\white.png",true));
        }
        layer1->SetNormal(new Texture2D("engine/normal.png",true));
        layer1->SetSpecular(new Texture2D("engine/white.png", true));
        layer1->SetSpecular(new Texture2D("engine/white.png"));
        //layer1.NormalMap = new Texture.Texture2D("engine\\maps\\blanknormal.jpg");
        //layer1.SpecularMap = new Texture.Texture2D("engine\\maps\\white.png");
        layer1->SetPixels(new PixelMap(1024, 1024));


        if (i == 0)
        {
            //layer1->SetLayerMap(new Texture2D("engine\\white.png"));
            auto pixmap = new PixelMap(1024, 1024, float4(1, 1, 1, 1));
            layer1->SetPixels(pixmap);
            layer1->SetLayerMap(new Texture2D(pixmap->GetWidth(), pixmap->GetHeight(), pixmap->GetData(), 4));
            //
            // layer1->SetPixels()

        }
        else
        {

            layer1->SetLayerMap(new Texture2D(1024, 1024, layer1->GetPixels()->GetData(),4));
            //  layer1.LayerMap = new Texture.Texture2D("engine\\terrain\\layer1.png");
        }
        m_Layers.push_back(layer1);
    }
    m_Mesh->CalculateNormals();
    m_Mesh->Build();
    m_Mesh->SetOwner(this);
    

}

void NodeTerrain::Render(bool sp) {

    Engine::m_Node = this;
    


    auto m_Buffer = m_Mesh->GetBuffer();

    int ii = 0;
    for (auto layer : m_Layers) {

        m_Material->SetDiffuse(layer->GetColor());
        m_Material->SetNormals(layer->GetNormal());
        m_Material->SetSpecular(layer->GetSpec());
        m_Material->SetLayerMap(layer->GetLayerMap());
        m_Material->SetLayerIndex(ii);
        m_Material->Bind(sp);
       
        ii++;
        IBuffer* pBuffs[] = { m_Buffer->GetVertexBuffer() };
        Uint64 offsets = 0;
        Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        Engine::m_pImmediateContext->SetIndexBuffer(m_Buffer->GetTriangleBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        if (sp) {
            Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSecondPassSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
        else {
            Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        }
        DrawIndexedAttribs attrib;
        attrib.IndexType = VALUE_TYPE::VT_UINT32;
        attrib.NumIndices = m_Mesh->GetTriangles().size() * 3;
        attrib.Flags = DRAW_FLAG_VERIFY_ALL;
        Engine::m_pImmediateContext->DrawIndexed(attrib);


    }

}

TerrainMesh* NodeTerrain::GetMesh() {

    return m_Mesh;

}

Bounds NodeTerrain::GetTerrainBounds() {

    auto verts = m_Mesh->GetVertices();

    float3 min, max;

    for (auto v : verts) {

        auto pos = v.position;
        min = Diligent::min(min, pos);
        max = Diligent::max(max, pos);

    }

    Bounds res;
    res.Min = min;
    res.Max = max;
    res.Centre = min + ((max / min) / 2);
    return res;
}

void NodeTerrain::RenderDepth() {
    //return;

    //m_DepthMaterial->Bind(false);
    //m_Material->SetDiffuse(layer->GetColor());
    //m_Material->SetLayerMap(layer->GetLayerMap());
    Engine::m_Node = this;
    
    m_DepthMaterial->Bind(false);




    IBuffer* pBuffs[] = { m_Mesh->GetBuffer()->GetVertexBuffer() };
    Uint64 offsets = 0;
    Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
    Engine::m_pImmediateContext->SetIndexBuffer(m_Mesh->GetBuffer()->GetTriangleBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    //if (sp) {

     //   Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSecondPassSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
   // }
   // else {
        Engine::m_pImmediateContext->CommitShaderResources(m_DepthMaterial->GetSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    //}
    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;
    attrib.NumIndices = m_Mesh->GetTriangles().size() * 3;
    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    Engine::m_pImmediateContext->DrawIndexed(attrib);



}

void NodeTerrain::WriteNode(VFile* file) {

    std::string t_path = Engine::m_ContentPath+"terrains\\"+ m_Name + ".terrain";
    file->WriteInt(4);
    file->WriteString(t_path.c_str());
    file->WriteString(m_Name.c_str());
    SaveTerrain(t_path);




}

void NodeTerrain::ReadNode(VFile* file) {

    auto path = file->ReadString();
    m_Name = file->ReadString();
    LoadTerrain(path);
   // m_Name = "Terrain";


}

void NodeTerrain::LoadTerrain(std::string path) {

    VFile* file = new VFile(path.c_str(), FileMode::Read);

    m_Width = file->ReadFloat();
    m_Depth = file->ReadFloat();
    m_Divisions = file->ReadFloat();

    m_Mesh = new TerrainMesh;

    int verts = file->ReadInt();

    std::vector<TerrainVertex> tverts;

    for (int i = 0; i < verts; i++) {

        TerrainVertex v;
        v.position = file->ReadVec3();
        v.color = file->ReadVec4();
        v.normal = file->ReadVec3();
        v.binormal = file->ReadVec3();
        v.tangent = file->ReadVec3();
        v.texture = file->ReadVec3();
        v.layercoord = file->ReadVec3();
        tverts.push_back(v);
    }

    int tris = file->ReadInt();

    std::vector<Triangle> ttris;

    for (int i = 0; i < tris; i++) {

        Triangle t;
        t.v0 = file->ReadInt();
        t.v1 = file->ReadInt();
        t.v2 = file->ReadInt();
        ttris.push_back(t);
    }

    m_Mesh->SetVertices(tverts);
    m_Mesh->SetTriangles(ttris);
    m_Mesh->SetOwner(this);
    m_Mesh->Build();

    int lc = file->ReadInt();

    for (int l = 0; l < lc; l++) {

        auto layer = new TerrainLayer;
        int w = file->ReadInt();
        int h = file->ReadInt();
        int bpp = file->ReadInt();
        auto pix = new PixelMap(w, h);
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {

                float r = file->ReadFloat();
                float g = file->ReadFloat();
                float b = file->ReadFloat();
                float a = file->ReadFloat();

                pix->SetColor(x, y, float4(r, g, b, a));

            }
        }

        auto col = new Texture2D(file->ReadString());
        auto norm = new Texture2D(file->ReadString());
        auto spec = new Texture2D(file->ReadString());
        layer->SetColor(col);
        layer->SetNormal(norm);
        layer->SetSpecular(spec);
        layer->SetPixels(pix);
        m_Layers.push_back(layer);
        


    }

    file->Close();

}

void NodeTerrain::SaveTerrain(std::string path) {

    VFile* file = new VFile(path.c_str(), FileMode::Write);

    
    file->WriteFloat(m_Width);
    file->WriteFloat(m_Depth);
    file->WriteFloat(m_Divisions);
 

    auto verts = m_Mesh->GetVertices();
    auto tris = m_Mesh->GetTriangles();

    file->WriteInt(verts.size());
    for (auto v : verts) {

        file->WriteVec3(v.position);
        file->WriteVec4(v.color);
        file->WriteVec3(v.normal);
        file->WriteVec3(v.binormal);
        file->WriteVec3(v.tangent);
        file->WriteVec3(v.texture);
        file->WriteVec3(v.layercoord);

    }
    file->WriteInt(tris.size());
    for (auto t : tris) {

        file->WriteInt(t.v0);
        file->WriteInt(t.v1);
        file->WriteInt(t.v2);

    }
    file->WriteInt(m_Layers.size());
    for (auto l : m_Layers) {

        auto pix = l->GetPixels();
        file->WriteInt(pix->GetWidth());
        file->WriteInt(pix->GetHeight());
        file->WriteInt(pix->GetBPP());

        for (int y = 0; y < pix->GetHeight(); y++) {
            for (int x = 0; x < pix->GetWidth(); x++) {

                auto col = pix->GetColor(x, y);

                file->WriteFloat(col.r);
                file->WriteFloat(col.g);
                file->WriteFloat(col.b);
                file->WriteFloat(col.a);

            }
        }

        file->WriteString(l->GetColor()->GetPath().c_str());
        file->WriteString(l->GetNormal()->GetPath().c_str());
        file->WriteString(l->GetSpec()->GetPath().c_str());

    }
      

    file->Close();

}