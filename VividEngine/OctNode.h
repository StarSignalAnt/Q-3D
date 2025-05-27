#pragma once
#include <vector>
#include "data.h"
#include "MeshLines.h"
#include "BasicMath.hpp"
using namespace Diligent;
#include "SceneGraph.h"
#include "Mesh3D.h"
#include "MaterialBase.h"
#include "Engine.h"


class OctNode
{
public:
	OctNode(SceneGraph* graph,float3 center, float3 size)
		: m_Graph(graph), m_center(center), m_size(size), m_children(8, nullptr) {}

    void subdivide() {
        float3 subSize = m_size / 2.0f;
        float3 childCenter[8] = {
            { m_center.x - subSize.x / 2, m_center.y - subSize.y / 2, m_center.z - subSize.z / 2 },
            { m_center.x + subSize.x / 2, m_center.y - subSize.y / 2, m_center.z - subSize.z / 2 },
            { m_center.x - subSize.x / 2, m_center.y + subSize.y / 2, m_center.z - subSize.z / 2 },
            { m_center.x + subSize.x / 2, m_center.y + subSize.y / 2, m_center.z - subSize.z / 2 },
            { m_center.x - subSize.x / 2, m_center.y - subSize.y / 2, m_center.z + subSize.z / 2 },
            { m_center.x + subSize.x / 2, m_center.y - subSize.y / 2, m_center.z + subSize.z / 2 },
            { m_center.x - subSize.x / 2, m_center.y + subSize.y / 2, m_center.z + subSize.z / 2 },
            { m_center.x + subSize.x / 2, m_center.y + subSize.y / 2, m_center.z + subSize.z / 2 }
        };


      

        for (int i = 0; i < 8; ++i) {
            //if (level < 2) {

            auto info = m_Graph->GetInfo(childCenter[i], subSize,false);
            if (info.m_TriCount > 0) {
                m_children[i] = new OctNode(m_Graph, childCenter[i], subSize);
                m_children[i]->Process(m_Graph);
            }


             //   m_children[i] = new OctNode(m_Graph,childCenter[i], subSize);
            //    m_children[i]->Process(m_Graph);
             //   m_children[i]->subdivide(level + 1);
            
        }
    }

    void Process(SceneGraph* graph) {

        auto info = graph->GetInfo(m_center, m_size);

        if (info.m_TriCount > m_TriTH) {
            subdivide();

        }
        else {
            m_Leaf = true;
            auto info = m_Graph->GetInfo(m_center, m_size,true);
            for (auto rm : info.m_LocalMeshes) {
                m_renderMeshes.push_back(rm);
                rm->Build();
            }
            m_NodeInfo = info;

             MeshLines* line = new MeshLines;
            line->CreateBox(m_center, m_size);
            line->CreateBuffer();
            graph->AddLines(line);
        }

        int b = 5;

    }

    void CreateLines(SceneGraph* graph) {

     
        return;

        /*
        for (auto ch : m_children) {
            if (ch == nullptr) return;
           
            ch->CreateLines(graph);


        }
        */



        // Create or update the buffer
        //line->CreateBuffer();
        //graph->AddLines(line);


    }

    bool IsLeaf() {
        return m_Leaf;
    }

    std::vector<OctNode*> GetChildren() {
        return m_children;
    }

    void RenderBF(bool sp,int in) {
        auto cam = Engine::m_Camera;

        //in = 0;

        if (in == 2) {
//            int b = 5;
        }
        else {
            in = cam->InView(m_center, m_size);
        }
        if(in>0)
        {
            if (m_Leaf) {

             

 
                Engine::m_Lights = m_Graph->GetLights();
                Engine::m_Camera = m_Graph->GetCamera();

                bool sp2 = false;

                Engine::m_Lights = m_Graph->GetLights();
                //bool sp = false;
                for (int l = 0; l < m_Graph->GetLights().size(); l++)
                {
                    auto al = m_Graph->GetLights()[l];

                    Engine::m_Light = al;
                    for (auto mesh : m_renderMeshes) {
                        Engine::m_Node = (Node*)mesh->GetOwner();

                        mesh->GetMaterial()->Bind(sp2);
                        mesh->Render(sp2);
                    }
                    m_NodesRendered++;
                    sp2 = true;
                }

            }
            else {


             
                for (int i = 0; i < 8; i++) {
                    if (m_children[i] != nullptr) {

                        
                        m_children[i]->RenderBF(sp,in);
                    }

                }
            }

        }
    }

    static int m_NodesRendered;


private:

	float3 m_center;  // Center of this octree node
	float3 m_size;     // Size of this octree node
	std::vector<OctNode*> m_children;  // Children nodes
    std::vector<Mesh3D*> m_renderMeshes;
    int m_totalTris;
    int m_TriTH = 90000;
    SceneGraph* m_Graph;
    bool m_Leaf = false;
    SceneInfo m_NodeInfo;
    
};

