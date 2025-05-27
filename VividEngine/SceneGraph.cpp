#include "pch.h"
#include "SceneGraph.h"
#include "Node.h"
#include "NodeEntity.h"
#include "NodeCamera.h"
#include "Engine.h"
#include "Mesh3D.h"
#include "CubeRenderer.h"
#include "MeshLines.h"
#include "Intersections.h"
#include "Bounds.h"
#include "NodeTerrain.h"
#include "TerrainMesh.h"
#include "MathsHelp.h"
#include "VFile.h"
#include "RTMesh.h"
#include "NodeTerrain.h"
#include "NodeActor.h"
#include "ScriptHost.h"
#include "VClass.h"
#include "VVar.h"
#include "VScope.h"
#include "EngineSettings.h"



std::vector<TerrainMesh*> GetTerrainMeshes(Node* node, std::vector<TerrainMesh*> meshes)
{

       
    NodeTerrain* pTerrain = dynamic_cast<NodeTerrain*>(node);

    if (pTerrain) {

        meshes.push_back(pTerrain->GetMesh());

    }
   

    for (auto sub : node->GetNodes()) {
        meshes = GetTerrainMeshes(sub, meshes);

    }

    return meshes;

}
std::vector<Mesh3D*> GetMeshes(Node* node, std::vector<Mesh3D*> meshes)
{

    NodeEntity* pEntity = dynamic_cast<NodeEntity*>(node);
  

    if (pEntity) {
        // Casting successful, pNode is a nodeEntity
        //pEntity->entityFunction();

        for (auto mesh : pEntity->GetMeshes()) {

            meshes.push_back(mesh);

        }
    }

    for (auto sub : node->GetNodes()) {
        meshes = GetMeshes(sub, meshes);

    }

    return meshes;

}


SceneGraph::SceneGraph() {

	m_Camera = new NodeCamera;
	m_RootNode = new Node();
    m_RayTester = new Intersections();
	m_RootNode->SetName("");
//    m_CubeRenderer = new CubeRenderer()

}

void SceneGraph::AddNode(Node* node) {
    
    if (dynamic_cast<NodeTerrain*>(node))
    {
       m_ActiveTerrain  = (NodeTerrain*)node;
    }
	m_RootNode->AddNode(node);

}

void SceneGraph::AddLight(NodeLight* light) {

	m_Lights.push_back(light);
    m_RootNode->AddNode(light);

}

void SceneGraph::RenderShadows() {


    for (auto light : m_Lights) {

        CubeRenderer* renderer = new CubeRenderer(this, light->GetShadowMap());
        renderer->RenderDepth(light->GetPosition(), light->GetRange());

    }

}

void SceneGraph::RenderDepth() {

    Engine::m_Camera = m_Camera;
    Engine::m_Light = m_Lights[0];
    Engine::m_Lights = m_Lights;
    m_RootNode->RenderDepth();

}

void SceneGraph::RenderLines() {
    Engine::m_Camera = m_Camera;
    Engine::m_Lights = m_Lights;
    Engine::m_Light = m_Lights[0];

    if (m_LinesOn) {
        for (auto lines : m_Lines) {

            lines->Render();

        }
    }
}

void SceneGraph::Render() {

	Engine::m_Camera = m_Camera;
	Engine::m_Lights = m_Lights;
    Engine::m_Light = m_Lights[0];
	
    if (m_LinesOn) {
        for (auto lines : m_Lines) {

            lines->Render();

        }
    }
    
    bool sp = false;
    for (auto l : m_Lights) {
        Engine::m_Light = l;
        m_RootNode->Render(sp);
        sp = true;
    }

	

}

NodeCamera* SceneGraph::GetCamera() {

	return m_Camera;

}

float dot(const float3& a, const float3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length(const float3& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
float3 cross(const float3& a, const float3& b)
{
    return float3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

constexpr float EPSILON = 1e-6f;

HitResult RayToTri(float3 pos,float3 end, float3 v0, float3 v1, float3 v2)
{
    return HitResult();
    float3 edge1, edge2, h, s, q;
    float a, f, u, v;
    HitResult res = HitResult();

    edge1 = v1 - v0;// vertex1 - vertex0;
    edge2 = v2 - v0;
    h = cross(end, edge2);


    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return res;    // This ray is parallel to this triangle.
    f = 1.0f / a;
    s =pos - v0;
    u = f *dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return res;
    q = cross(s, edge1);
    v = f * dot(end, q);
    if (v < 0.0f || u + v > 1.0f)
        return res;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);

    if (t > EPSILON) // ray intersection
    {
        //outIntersectionPoint = rayOrigin + rayVector * t;
        res.m_Hit = true;
        res.m_Point =pos + end * t;
        res.m_Distance = length(res.m_Point - pos);
        return res;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return res;

    res.m_Hit = false;
    return res;

    return res;
}


HitResult RayCastMesh(float3 pos, float3 end, Mesh3D* mesh) {

    float cd = 100000;
    HitResult close;

    auto world = mesh->GetOwner()->GetWorldMatrix();

    for (auto t : mesh->GetTris())
    {

        float3 v0, v1, v2;



        v0 = mesh->GetVertices()[t.v0].position * world;
        v1 = mesh->GetVertices()[t.v1].position * world;
        v2 = mesh->GetVertices()[t.v2].position * world;

        HitResult res = RayToTri(pos, end, v0, v1, v2);
        if (res.m_Hit) {
            if (res.m_Distance < cd) {
                cd = res.m_Distance;
                close = res;
                close.m_Mesh = mesh;
                close.m_Node = (Node*)mesh->GetOwner();
                close.m_Entity = mesh->GetOwner();
            }
        }
    }

 //   printf("CD:%f\n", cd);

    return close;

}


HitResult SceneGraph::MousePick(int x, int y, NodeTerrain* entity) {

    float fx = (float)x;
    float fy = (float)y;
    float mx = -1 + (float)(x) / (float)(Engine::GetFrameWidth()) * 2;
    float my = 1 - (float)(y) / (float)(Engine::GetFrameHeight()) * 2;

    float3 origin = float3(mx, my, 0);
    float3 dest = float3(mx, my, 1.0f);



    //Matrix4x4 viewProj = RenderGlobals.CurrentCamera.WorldMatrix * RenderGlobals.CurrentCamera.ProjectionMatrix;
    float4x4 viewProj = Engine::m_Camera->GetWorldMatrix() * Engine::m_Camera->GetProjection();

    // Matrix4x4 vp;
    // Matrix4x4.Invert(viewProj, out vp);
    // Matrix4x4 inverseProj = vp;


    float4x4 inverseProj = viewProj.Inverse();


    //****Ve
    //Vector3 ray_origin = Vector3.Transform (origin, inverseProj);
    //Vector3 ray_end = Vector3.TransformNormal(dest, inverseProj);
    //Vector4 ro = Vector4.Transform(origin, inverseProj);
    //Vector4 rd = Vector4.Transform(dest, inverseProj);

    float4 ro = float4(origin, 1.0f) * inverseProj;
    float4 rd = float4(dest, 1.0) * inverseProj;


    float3 ray_origin = float3(ro.x / ro.w, ro.y / ro.w, ro.z / ro.w);
    float3 ray_end = float3(rd.x / rd.w, rd.y / rd.w, rd.z / rd.w);
    float3 ray_dir = ray_end - ray_origin;

    //Vector3 ray_origin = new Vector3(ro.X / ro.W, ro.Y / ro.W, ro.Z / ro.W);
    //Vector3 ray_end = new Vector3(rd.X / rd.W, rd.Y / rd.W, rd.Z / rd.W);
    //Vector3 ray_dir = ray_end - ray_origin;

    ray_dir = normalize(ray_dir);


    float cd = 10000.0f;
    HitResult close;
    close.m_Hit = false;
    std::vector<TerrainMesh*> meshes;
    meshes = GetTerrainMeshes(entity, meshes);

 

    for (auto mesh : meshes) {

        CastResult res = m_RayTester->CastTerrainMesh(m_Camera->GetPosition(), ray_dir, mesh);
        if (res.Hit) {



            if (res.Distance < cd) {
                HitResult nres;
                nres.m_Hit = true;
                nres.m_Distance = res.Distance;
                nres.m_Point = m_Camera->GetPosition() + Diligent::normalize(ray_dir) * res.Distance;
                //nres.m_Mesh = mesh;

                nres.m_Node = mesh->GetOwner();
               // nres.m_Entity = (NodeEntity*)mesh->GetOwner();
                close = nres;

            }
        }

        /*
        HitResult res = RayCastMesh(m_Camera->GetPosition(), ray_dir,mesh);
        if (res.m_Hit) {
            if (res.m_Distance < cd) {
                cd = res.m_Distance;
                close = res;
            }
        }
        */
    }



    return close;

}

HitResult SceneGraph::MousePick(int x, int y, NodeEntity* entity) {

    float fx = (float)x;
    float fy = (float)y;
    float mx = -1 + (float)(x) / (float)(Engine::GetFrameWidth()) * 2;
    float my = 1 - (float)(y) / (float)(Engine::GetFrameHeight()) * 2;

    float3 origin = float3(mx, my, 0);
    float3 dest = float3(mx, my, 1.0f);



    //Matrix4x4 viewProj = RenderGlobals.CurrentCamera.WorldMatrix * RenderGlobals.CurrentCamera.ProjectionMatrix;
    float4x4 viewProj = Engine::m_Camera->GetWorldMatrix() * Engine::m_Camera->GetProjection();

    // Matrix4x4 vp;
    // Matrix4x4.Invert(viewProj, out vp);
    // Matrix4x4 inverseProj = vp;


    float4x4 inverseProj = viewProj.Inverse();


    //****Ve
    //Vector3 ray_origin = Vector3.Transform (origin, inverseProj);
    //Vector3 ray_end = Vector3.TransformNormal(dest, inverseProj);
    //Vector4 ro = Vector4.Transform(origin, inverseProj);
    //Vector4 rd = Vector4.Transform(dest, inverseProj);

    float4 ro = float4(origin, 1.0f) * inverseProj;
    float4 rd = float4(dest, 1.0) * inverseProj;


    float3 ray_origin = float3(ro.x / ro.w, ro.y / ro.w, ro.z / ro.w);
    float3 ray_end = float3(rd.x / rd.w, rd.y / rd.w, rd.z / rd.w);
    float3 ray_dir = ray_end - ray_origin;

    //Vector3 ray_origin = new Vector3(ro.X / ro.W, ro.Y / ro.W, ro.Z / ro.W);
    //Vector3 ray_end = new Vector3(rd.X / rd.W, rd.Y / rd.W, rd.Z / rd.W);
    //Vector3 ray_dir = ray_end - ray_origin;

    ray_dir = normalize(ray_dir);


    float cd = 10000.0f;
    HitResult close;
    close.m_Hit = false;
    std::vector<Mesh3D*> meshes;
    meshes = GetMeshes(entity, meshes);


    for (auto mesh : meshes) {

        CastResult res = m_RayTester->CastMesh(m_Camera->GetPosition(), ray_dir, mesh);
        if (res.Hit) {



            if (res.Distance < cd) {
                HitResult nres;
                nres.m_Hit = true;
                nres.m_Distance = res.Distance;
                nres.m_Mesh = mesh;
                nres.m_Node = mesh->GetOwner();
                nres.m_Entity = (NodeEntity*)mesh->GetOwner();
                cd = res.Distance;
                close = nres;

            }
        }

        /*
        HitResult res = RayCastMesh(m_Camera->GetPosition(), ray_dir,mesh);
        if (res.m_Hit) {
            if (res.m_Distance < cd) {
                cd = res.m_Distance;
                close = res;
            }
        }
        */
    }

   

    return close;

}

HitResult SceneGraph::MousePick(int x, int y)
{
	float fx = (float)x;
	float fy = (float)y;
    float mx = -1 + (float)(x) / (float)(Engine::GetFrameWidth()) * 2;
    float my = 1 - (float)(y) / (float)(Engine::GetFrameHeight()) * 2;

    float3 origin = float3(mx, my, 0);
    float3 dest = float3(mx, my, 1.0f);



    //Matrix4x4 viewProj = RenderGlobals.CurrentCamera.WorldMatrix * RenderGlobals.CurrentCamera.ProjectionMatrix;
    float4x4 viewProj = Engine::m_Camera->GetWorldMatrix() * Engine::m_Camera->GetProjection();

   // Matrix4x4 vp;
   // Matrix4x4.Invert(viewProj, out vp);
   // Matrix4x4 inverseProj = vp;


    float4x4 inverseProj = viewProj.Inverse();


    //****Ve
    //Vector3 ray_origin = Vector3.Transform (origin, inverseProj);
    //Vector3 ray_end = Vector3.TransformNormal(dest, inverseProj);
    //Vector4 ro = Vector4.Transform(origin, inverseProj);
    //Vector4 rd = Vector4.Transform(dest, inverseProj);

    float4 ro = float4(origin,1.0f) * inverseProj;
    float4 rd = float4(dest, 1.0) * inverseProj;


    float3 ray_origin = float3(ro.x / ro.w, ro.y / ro.w, ro.z / ro.w);
    float3 ray_end = float3(rd.x / rd.w, rd.y / rd.w, rd.z / rd.w);
    float3 ray_dir = ray_end - ray_origin;

    //Vector3 ray_origin = new Vector3(ro.X / ro.W, ro.Y / ro.W, ro.Z / ro.W);
    //Vector3 ray_end = new Vector3(rd.X / rd.W, rd.Y / rd.W, rd.Z / rd.W);
    //Vector3 ray_dir = ray_end - ray_origin;

    ray_dir = normalize(ray_dir);

    //Ray ray = new Ray();


   // ray.Pos = RenderGlobals.CurrentCamera.Position;
   // ray.Dir = ray_dir;
   // return ray;
	HitResult result = RayCast(m_Camera->GetPosition(),ray_dir);


	return result;
}

HitResult RayCastNode(Node* node) {

    
    return HitResult();
}

HitResult SceneGraph::RayCast(float3 pos, float3 end) {


    HitResult result;

    std::vector<Mesh3D*> meshes;
    float cd = 10000;
    HitResult close;
    close.m_Hit = false;

    meshes = GetMeshes(m_RootNode,meshes);

    for (auto mesh : meshes) {

        CastResult res = m_RayTester->CastMesh(pos, end, mesh);
        if (res.Hit) {



            if (res.Distance < cd) {
                HitResult nres;
                nres.m_Hit = true;
                nres.m_Distance = res.Distance;
                nres.m_Mesh = mesh;
                nres.m_Node = mesh->GetOwner();
                nres.m_Entity = (NodeEntity*)mesh->GetOwner();
                cd = res.Distance;
                close = nres;
          
            }
        }
        


    }
    std::vector<TerrainMesh*> tmeshes;

    tmeshes = GetTerrainMeshes(m_RootNode, tmeshes);
    for (auto mesh : tmeshes) {

        CastResult res = m_RayTester->CastTerrainMesh(m_Camera->GetPosition(), end, mesh);
        if (res.Hit) {



            if (res.Distance < cd) {
                HitResult nres;
                nres.m_Hit = true;
                nres.m_Distance = res.Distance;
                nres.m_Node = mesh->GetOwner();
                // nres.m_Mesh = mesh;
                // nres.m_Node = mesh->GetOwner();
               //  nres.m_Entity = (NodeEntity*)mesh->GetOwner();
                close = nres;
                cd = nres.m_Distance;

            }
        }

    }
    //result = RayCastNode(m_RootNode);


    return close;

}

float2 SceneGraph::ToScreenSpace(float3 position) {


    float4x4 view = m_Camera->GetWorldMatrix();

    float4x4 proj = m_Camera->GetProjection();

    float4x4 mvp = view * proj;

    float4 pos = float4(position, 1.0) * mvp;

    pos.x = pos.x / pos.w;
    pos.y = pos.y / pos.w;

    pos.x = (0.5f + pos.x * 0.5f) * Engine::GetFrameWidth();
    pos.y = (0.5f - pos.y * 0.5f) * Engine::GetFrameHeight();

    return float2(pos.x, pos.y);

   
    


}

void SceneGraph::AddLines(MeshLines* lines) {

    m_Lines.push_back(lines);

}

void SceneGraph::Update(float delta) {

    m_RootNode->Update(delta);

}

void SceneGraph::BeginPlay() {

    m_IsPlaying = true;
    m_RootNode->BeginPlay();

}

void SceneGraph::Stop() {

    m_IsPlaying = false;
    m_RootNode->Stop();

}

Bounds SceneGraph::GetBounds() {

    Bounds res;
    res.Min = float3(10000, 10000, 10000);
    res.Max = float3(-10000, -10000, -10000);



    std::vector<Mesh3D*> meshes;

    meshes = GetMeshes(m_RootNode, meshes);

    for (auto m : meshes) {

        if (m->GetOwner()->GetStatic() == false) continue;
        auto world = m->GetOwner()->GetWorldMatrix();

        auto tris = m->GetTris();
        auto verts = m->GetVertices();

        for (auto v : verts) {

            float3 npos = v.position * world;
            if (npos.x < res.Min.x) res.Min.x = npos.x;
            if (npos.x > res.Max.x) res.Max.x = npos.x;
            if (npos.y < res.Min.y) res.Min.y = npos.y;
            if (npos.y > res.Max.y) res.Max.y = npos.y;
            if (npos.z < res.Min.z) res.Min.z = npos.z;
            if (npos.z > res.Max.z) res.Max.z = npos.z;

        }


    }

    res.Centre.x = res.Min.x + (res.Max.x - res.Min.x) / 2.0f;
    res.Centre.y = res.Min.y + (res.Max.y - res.Min.y) / 2.0f;
    res.Centre.z = res.Min.z + (res.Max.z - res.Min.z) / 2.0f;

    return res;

}




SceneInfo InfoProcessNode(Node* node, float3 centre, float3 size, SceneInfo info)
{

    if (node->GetStatic()) {
        auto pos = node->GetPosition();

        if (MathsHelp::IsPositionInsideBounds(centre, size, pos)) {
            info.m_Nodes.push_back(node);
        }

        if (dynamic_cast<NodeEntity*>(node) != nullptr) {

            auto ent = (NodeEntity*)node;
            bool any = false;
            for (auto mesh : ent->GetMeshes()) {

                auto tris = mesh->GetTris();
                auto verts = mesh->GetVertices();

                for (auto t : tris) {

                    auto v0 = verts[t.v0];
                    auto v1 = verts[t.v1];
                    auto v2 = verts[t.v2];

                    if (MathsHelp::IsPositionInsideBounds(centre, size, v0.position) || MathsHelp::IsPositionInsideBounds(centre, size, v1.position) || MathsHelp::IsPositionInsideBounds(centre, size, v2.position))
                    {

                        if (std::find(info.m_Meshes.begin(), info.m_Meshes.end(), mesh) != info.m_Meshes.end()) {

                        }
                        else {
                            info.m_Meshes.push_back(mesh);
                        }


                        info.m_TriCount++;
                        info.m_VertCount += 3;

                    };



                };

            }


        }
    }
    else {

        int bb = 5;
    }
    for (auto sub : node->GetNodes()) {

        info = InfoProcessNode(sub, centre, size, info);

    }

    return info;
}

SceneInfo SceneGraph::GetInfo(float3 centre, float3 size,bool inc_local) {

    SceneInfo info;

    info = InfoProcessNode(m_RootNode, centre, size, info);

    if (inc_local) {
        for (auto mesh : info.m_Meshes) {

            auto tris = mesh->GetTris();
            auto verts = mesh->GetVertices();

            Mesh3D* new_mesh = nullptr;
            int vi = 0;

            for (auto t : tris) {

                auto v0 = verts[t.v0];
                auto v1 = verts[t.v1];
                auto v2 = verts[t.v2];

                if (MathsHelp::IsPositionInsideBounds(centre, size, v0.position) || MathsHelp::IsPositionInsideBounds(centre, size, v1.position) || MathsHelp::IsPositionInsideBounds(centre, size, v2.position))
                {

              
                        if (new_mesh == nullptr) {

                            new_mesh = new Mesh3D;
                            info.m_LocalMeshes.push_back(new_mesh);
                            new_mesh->SetMaterial(mesh->GetMaterial());
                            new_mesh->SetOwner(mesh->GetOwner());
                            vi = 0;
                        }

                        new_mesh->AddVertex(v0, false);
                        new_mesh->AddVertex(v1, false);
                        new_mesh->AddVertex(v2, false);
                        Triangle t1, t2;
                        t1.v0 = vi;
                        t1.v1 = vi + 1;
                        t1.v2 = vi + 2;
                        new_mesh->AddTri(t1);
                        new_mesh->AddTri(t2);
                        vi = vi + 3;

                    
                    //info.m_Meshes.push_back(mesh);
                }
                //info.m_TriCount++;
                //info.m_VertCount += 3;


            };



        };
    }


    return info;

}

Node* _FindNode(Node* node, std::string url) {

    if (node->GetFullName() == url) {
        return node;
    }
    for (auto sub : node->GetNodes()) {

        auto res = _FindNode(sub, url);
        if (res != nullptr) {
            return res;
        }

    }

    return nullptr;

}


Node* SceneGraph::FindNode(std::string url) {

    return _FindNode(m_RootNode, url);

}

void SceneGraph::ConnectNodes(Node* node) {

    for (auto s : node->GetScripts()) {

        for (auto v : s->GetScope()->GetVars()) {

            if (v->GetDataName() != "")
            {

                auto name = v->GetDataName();
                auto onode = FindNode(name);
                auto cls = onode->FindClass(v->GetClassType());
                v->SetClassValue(cls);
               // v->SetName(name);
                cls->SetDataName(v->GetDataName());

                int b = 5;
            }

        }

    }

    for (auto sub : node->GetNodes()) {

        ConnectNodes(sub);

    }

}

void SceneGraph::SaveScene(std::string path) {

    VFile* file = new VFile(path.c_str(), FileMode::Write);

    file->WriteInt((int)EngineSettings::UsedRenderer);

    ScriptHost::m_This->WriteContext(file);
    m_RootNode->WriteNode(file);




    file->Close();

}

Node* SceneGraph::ReadNode(VFile* file) {

    
    int type = file->ReadInt();

    Node* res = nullptr;

    switch (type) {
    case 1:
    {
        auto n = new Node;

        n->ReadNode(file);
        res = n;
    }
        break;
    case 2:

    {
        auto ne = new NodeEntity;
        ne->ReadNode(file);
        res = (Node*)ne;
    }
        break;
    case 3:
    {
        auto le = new NodeLight;
        le->ReadNode(file);
        res = (Node*)le;
        m_Lights.push_back(le);
       
    }
        break;
    case 4:
    {
        auto te = new NodeTerrain;
        te->ReadNode(file);
        res = (Node*)te;



    }

    break;
    case 5:
    {
        auto ce = new NodeCamera;
        ce->ReadNode(file);
        res = (Node*)ce;

    }
    }


    res->ReadScripts(file);

    int nc = file->ReadInt();

    for (int i = 0; i < nc; i++) {

        res->AddNode(ReadNode(file));

    }

    return res;

}

void SceneGraph::LoadScene(std::string path) {

    VFile* file = new VFile(path.c_str(), FileMode::Read);

    EngineSettings::UsedRenderer = (RendererSetting)file->ReadInt();

    ScriptHost::m_This->ReadContext(file);

    m_RootNode = ReadNode(file);

    ConnectNodes(m_RootNode);

    file->Close();

}

std::vector<RTMesh*> SceneGraph::DynRTGetMeshes(Node* node, std::vector<RTMesh*> meshes) {

   
    if (dynamic_cast<NodeActor*>(node)) {

        auto ent = (NodeActor*)node;
        if (ent->GetRTEnable()) {
            for (auto mesh : ent->GetMeshes()) {
                if (m_DynamicCache.count(mesh) > 0)
                {
                    continue;
                }
                auto rm = new RTMesh(mesh);
                meshes.push_back(rm);
                m_DynamicCache[mesh] = rm;
            }
        }

    }

    for (auto node : node->GetNodes()) {

        meshes = DynRTGetMeshes(node, meshes);


    }

    return meshes;

}


std::vector<RTMesh*> SceneGraph::RTGetMeshes(Node* node, std::vector<RTMesh*> meshes) {
    if (dynamic_cast<NodeActor*>(node) == nullptr) {


        if (dynamic_cast<NodeEntity*>(node)) {

            
            auto ent = (NodeEntity*)node;
            if (ent->GetRTEnable()) {
                for (auto mesh : ent->GetMeshes()) {
                    if (m_StaticCache.count(mesh) > 0) {
                        continue;
                    }
                    auto rm = new RTMesh(mesh);
                    meshes.push_back(rm);
                    m_StaticCache[mesh] = rm;
                }
            }

        }
    }

    

    for (auto node : node->GetNodes()) {

        meshes = RTGetMeshes(node, meshes);


    }

    return meshes;

}

std::vector<RTMesh*> SceneGraph::GetRTMeshes() {

    std::vector<RTMesh*> meshes;



    return RTGetMeshes(m_RootNode, meshes);


}

std::vector<RTMesh*> SceneGraph::GetDynRTMeshes() {

    std::vector<RTMesh*> meshes;

    return DynRTGetMeshes(m_RootNode, meshes);


}


void SceneGraph::RenderForcedMaterial(MaterialBase* material)
{
    Engine::m_Camera = m_Camera;
    Engine::m_Lights = m_Lights;
    Engine::m_Light = m_Lights[0];
    m_RootNode->RenderForcedMaterial(material);

}


NodeTerrain* FindTerrain(Node* node) {

    if (dynamic_cast<NodeTerrain*>(node)) {
        return (NodeTerrain*)node;
    }

    for (auto n : node->GetNodes()) {

        auto res = FindTerrain(n);
        if (res != nullptr) {
            return res;
        }

    }
    return nullptr;
}

NodeTerrain* SceneGraph::GetTerrain() {

    return FindTerrain(m_RootNode);

}

std::vector<NodeEntity*> GGetEntities(Node* node, std::vector<NodeEntity*> entities)
{

    if (dynamic_cast<NodeActor*>(node)) {

    }
    else {
        if (dynamic_cast<NodeEntity*>(node)) {

            entities.push_back((NodeEntity*)node);

        }
    }

    for (auto sub : node->GetNodes()) {

        entities = GGetEntities(sub, entities);

    }

    return entities;

}

std::vector<NodeEntity*> SceneGraph::GetEntities() {

    std::vector<NodeEntity*> entities;

    entities = GGetEntities(m_RootNode, entities);

    return entities;

}

std::vector<NodeActor*> GGetActors(Node* node, std::vector<NodeActor*> actors) {

    if (dynamic_cast<NodeActor*>(node)) {

        actors.push_back((NodeActor*)node);

    }

    for (auto sub : node->GetNodes()) {

        actors = GGetActors(sub, actors);

    }

    return actors;

}


std::vector<NodeActor*> SceneGraph::GetActors() {


    std::vector<NodeActor*> actors;

    actors = GGetActors(m_RootNode, actors);

    return actors;

}

std::vector<NodeCamera*> GGetCameras(Node* node, std::vector<NodeCamera*> cams) {

    if (dynamic_cast<NodeCamera*>(node)) {
        cams.push_back((NodeCamera*)node);
    }

    for (auto sub : node->GetNodes()) {

        cams = GGetCameras(sub, cams);

    }

    return cams;

}

std::vector<NodeCamera*> SceneGraph::GetCameras() {


    std::vector<NodeCamera*> cams;

    cams = GGetCameras(m_RootNode, cams);

    return cams;
}

