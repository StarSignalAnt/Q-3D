#include "EditHelp.h"
#include "MathsHelp.h"
#include "NodeEntity.h"
#include "Mesh3D.h"
#include "Editor.h"
#include "SceneGraph.h"

NodeEntity* EditHelp::CreateTerrainBrush(float mx, float h, float my, float size, float strength) {

    h = h + 0.1f;


    //var ent = new Entity();
    NodeEntity* ent = new NodeEntity;


    // if (BMESH == null)
   //  {
         //   Vivid.Meshes.Mesh bmesh = new Mesh(ent);
      //   BMESH = new Mesh(ent);
 //    }

    Mesh3D* bmesh = new Mesh3D;
    bmesh->SetOwner(ent);

    //Vivid.Meshes.Mesh bmesh = BMESH;
    //BMESH.Owner = ent;


    ent->SetPosition(float3(mx, h, my));


    //ent.Position = new System.Numerics.Vector3(mx, 0, my);


    Vertex cv;
    cv.color = float4(1, 1, 1, 0.85f * strength);
    cv.texture = float3(0, 0, 0);
    cv.position = float3(0, h, 0);
    bmesh->AddVertex(cv, false);
    bool first = true;
    int vi = 1;


    //bmesh->GetVertices.Clear();
    for (int ang = 0; ang <= 360; ang += 20)
    {

        float ax = cos(MathsHelp::Deg2Rad(ang)) * size;
        float ay = sin(MathsHelp::Deg2Rad(ang)) * size;


        //Ray ray = new Ray();
       // ray.Pos = new System.Numerics.Vector3(mx + ax, 30.0f, my + ay);
       // ray.Dir = new System.Numerics.Vector3(0, -35, 0);

        //var res = Vivid.Scene.Scene.CurrentScene.Raycast(ray);

        auto pos = float3(mx + ax, 30.0f, my + ay);
        auto dir = float3(0, -35, 0);

       // auto res = Editor::m_Graph->RayCast(pos, dir);


        float hy = 0;

        //if (res.m_Hit)
        {

            //     int b = 5;
          //  hy = res.m_Point.y;

           // h = hy + 0.1f;
            //    Random r = new Random(Environment.TickCount);
                // Console.WriteLine("Hitting Terrain:"+r.Next(20,2000));
                 //hy = hy + 0.225f;




        }
        //else
        {
         //   h = -5.0f;
        }

        //                hy = 0;

                      //  float y1 = GetHeight(ax, ay);

        Vertex v1, v2, v3;

        // v1 =new Vertex();

        v1.color = float4(1, 1, 1, 0);

        v1.texture = float3(0, 1, 0);

        v1.position = float3(ax, h, ay);

        bmesh->AddVertex(v1, false);



        if (!first)
        {

            Triangle nt;// = new Triangle();
            nt.v0 = vi - 1;
            nt.v2 = vi;
            nt.v1 = 0;
            bmesh->AddTri(nt);

        }
        first = false;
        vi += 1;

        //bmesh.AddTriangle(nt);

    }
    //ent.Meshes.Add(bmesh);
    ent->AddMesh(bmesh);
    //bmesh.CreateBuffers()
    bmesh->Build();
    //if (BM == null)
   // {
    //    BM = new Vivid.Materials.MaterialBasic(false, false, true);
     //   CM = new Vivid.Texture.Texture2D("engine\\terrain\\brush1.png");
        //   bmesh.BasicMaterial = new Vivid.Materials.MaterialBasic(false, false, true);
      //     bmesh.LightMaterial.ColorMap = CM;// new Vivid.Texture.Texture2D("engine\\terrain\\brush1.png");

    //}
   // else
    {


    }
    //bmesh.BasicMaterial = BM;
    //bmesh.LightMaterial.ColorMap = CM;



    return ent;

}
