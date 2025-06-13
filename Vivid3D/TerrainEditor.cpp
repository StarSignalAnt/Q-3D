#include "TerrainEditor.h"
#include "TerrainMeshComponent.h"
#include "TerrainLayer.h"
#include "PixelMap.h"
#include "TerrainMesh.h"
#include "Vivid.h"
void TerrainEditor::BeginPaint() {

	m_State = E_Paint;

}

void TerrainEditor::EndPaint()
{

	m_State = E_None;

}

void TerrainEditor::Update() {


    if (m_State == E_Paint) {

        Paint();

    }
    else if (m_State == E_Sculpt) {

        Sculpt();

    }
  

}

void TerrainEditor::Paint() {

    auto ter = m_Terrain;

    //var l1 = ter.Layers[TerrainEditorForm.ActiveLayer];
    auto layer = ter->GetComponent<TerrainMeshComponent>()->GetLayer(m_TerrainLayer);

    auto bb = ter->GetComponent<TerrainMeshComponent>()->GetTerrainBounds();

    float tw = bb.Size().x;
    float th = bb.Size().z;

    float tx = m_EditPos.x;
    float ty = m_EditPos.z;


    float tx1 = tx - bb.Min.x;
    float ty1 = ty - bb.Min.z;



    float mxi = ((layer->GetPixels()->GetWidth()) / tw);// * TerrainBrushSize;
    float myi = ((layer->GetPixels()->GetHeight()) / th);


    float w1 = m_TerrainBrushSize * mxi;
    float h1 = m_TerrainBrushSize * myi;



    float xi = tx1 / tw;
    float yi = ty1 / th;

    //                Console.WriteLine("XI:" + xi + " YI:" + yi);

    //return;


    //     xi = 1.0f - xi;
    //    yi = 1.0f - yi;

    layer->PlotBrush(xi, yi, (int)w1, (int)h1, m_TerrainStrength);

    //Lower other layers by a similar(Inverse?) amount.
    for (auto l : ter->GetComponent<TerrainMeshComponent>()->GetLayers())
    {
        if (l == layer) continue;
        l->PlotBrush(xi, yi, (int)w1, (int)h1, -m_TerrainStrength);
    }


}

void TerrainEditor::SetEditLayer(int layer) {

    m_TerrainLayer = layer;

}

void TerrainEditor::BeginSculpt() {

    m_State = E_Sculpt;

}

void TerrainEditor::EndSculpt() {

    m_State = E_None;

}

bool m_s = false;
void TerrainEditor::Sculpt() {


    if (m_s == true) return;
    m_s = true;
    auto ter = m_Terrain;


    //List<TerrainVertex> vertices = new List<TerrainVertex>();

    std::vector<TerrainVertex>& verts = ter->GetComponent<TerrainMeshComponent>()->GetMesh()->GetVertices();



    int v = 0;
    for (auto vert : verts)
    {

        float xd = (ter->GetPosition().x + vert.position.x) - m_EditPos.x;
        float zd = (ter->GetPosition().z + vert.position.z) - m_EditPos.z;

        float dis = (float)sqrt(xd * xd + zd * zd);

        if (dis < m_TerrainBrushSize)
        {

            dis = dis / m_TerrainBrushSize;
            if (dis > 1.0) dis = 1.0f;
            dis = 1.0f - dis;

            TerrainVertex nv = vert;

            //nv.position.y = nv.position.y + 0.02f * Editor::TerrainBrushStrength;
            verts[v].position.y = vert.position.y + 0.02f * m_TerrainStrength;

            //ter.Mesh.Vertices[v] = nv;



        }


        v++;
    }

    // ter->GetMesh()->SetVertices(verts);
    ter->GetComponent<TerrainMeshComponent>()->GetMesh()->CalculateNormals();
    ter->GetComponent<TerrainMeshComponent>()->GetMesh()->Build();
    Vivid::m_pImmediateContext->Flush();
    Vivid::m_pImmediateContext->WaitForIdle();

    m_s = false;
}