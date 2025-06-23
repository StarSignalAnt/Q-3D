
#include "SceneGraph.h"
#include "GraphNode.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "CubeRenderer.h"
#include "QEngine.h"
#include "StaticMeshComponent.h"
#include "Intersections.h"
SceneGraph* SceneGraph::m_CurrentGraph = nullptr;
#include "ScriptHost.h"
#include "VFile.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "TerrainMeshComponent.h"
#include "TerrainMesh.h"
#include "TerrainLayer.h"
#include "Texture2D.h"
#include "PixelMap.h"
#include <cmath>   // For std::abs and std::floor
#include <iomanip> // For std::fixed and std::setprecision
#include "TerrainRendererComponent.h"
#include "TerrainDepthRenderer.h"
#include "SharpComponent.h"
bool addedGraphFuncs = false;



double floorIfCloseToZero(double number, double tolerance = 0.0001) {
	// Check if the absolute value of the number is less than the tolerance
	if (std::abs(number) < tolerance) {
		return 0.0;
		// If it is, return the floor of the number
		//return std::floor(number);
	}
	else {
		// Otherwise, return the number unchanged
		return number;
	}
}
SceneGraph* SceneGraph::m_Instance = nullptr;

SceneGraph::SceneGraph() {

	m_Instance = this;
	m_RootNode = new GraphNode;
	m_Camera = new GraphNode;
	m_Camera->AddComponent(new CameraComponent);
	m_RayTester = new Intersections();

	m_ShadowRenderer = new CubeRenderer(this, nullptr);
}

void SceneGraph::SetRootNode(GraphNode* node) {

	m_RootNode = node;
	

}

GraphNode* SceneGraph::GetRootNode() {

	return m_RootNode;

}

void SceneGraph::AddNode(GraphNode* node) {
	if (m_RootNode == nullptr) {
		m_RootNode = node;
	} else {
		m_RootNode->AddNode(node);
	}
}

void SceneGraph::RenderDepth() {

	
	m_CurrentGraph = this;
	if (m_Terrain) {
		m_Terrain->RenderDepth(m_Camera);
	}
	m_RootNode->RenderDepth(m_Camera);


}

void SceneGraph::Render() {

	m_CurrentGraph = this;
	
	if (m_Terrain) {
		m_Terrain->Render(m_Camera);
	}

	Ren_Count = 0;
	m_RootNode->Render(m_Camera);
	

//	for (auto sub : m_RootNode->GetNodes()) {
	
//	}

}

GraphNode* SceneGraph::GetCamera() {
	return m_Camera;
}

void SceneGraph::AddLight(GraphNode* light) {

	m_Lights.push_back(light);
	AddNode(light);
	light->SetName("Light" + std::to_string(m_Lights.size()));

}

void SceneGraph::SetCamera(GraphNode* camera) {
//	if (m_Camera != nullptr) {
//		delete m_Camera;
//	}
	m_Camera = camera;
}

void SceneGraph::RenderShadows() {

	for (auto light : m_Lights) {
		auto mat = light->GetWorldMatrix();
		glm::vec3 position = glm::vec3(mat[3]);


		//map_data[0].g_LightPosition = glm::vec4(position, 1.0f); // Li
		m_ShadowRenderer->RenderDepth(position, light->GetComponent<LightComponent>()->GetRange(),light->GetComponent<LightComponent>()->GetShadowMap());

	}

}

void SceneGraph::Reset() {

	m_RayTester = new Intersections();
}

void SceneGraph::Update(float dt) {


	m_CurrentGraph = this;
	m_RootNode->UpdatePhysics();
	m_RootNode->Update(dt);

}


HitResult SceneGraph::MousePick(int x, int y)
{
	float mx = -1.0f + 2.0f * (float)(x) / (float)QEngine::GetFrameWidth();
	float my = 1.0f - 2.0f * (float)(y) / (float)QEngine::GetFrameHeight();

	// Create points on near and far planes in NDC space
	glm::vec4 near_ndc = glm::vec4(mx, my, -1.0f, 1.0f);  // Near plane (z = -1)
	glm::vec4 far_ndc = glm::vec4(mx, my, 1.0f, 1.0f);   // Far plane (z = 1)

	// Get view and projection matrices
	glm::mat4 view = glm::inverse(m_Camera->GetWorldMatrix());
	glm::mat4 proj = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix();
	glm::mat4 invViewProj = glm::inverse(proj * view);

	// Transform NDC points to world space
	glm::vec4 near_world = invViewProj * near_ndc;
	glm::vec4 far_world = invViewProj * far_ndc;

	// Perform perspective divide
	if (fabs(near_world.w) > 1e-6f) near_world /= near_world.w;
	if (fabs(far_world.w) > 1e-6f) far_world /= far_world.w;

	// Extract 3D points
	glm::vec3 ray_start = glm::vec3(near_world);
	glm::vec3 ray_end = glm::vec3(far_world);

	// Cast ray from near plane to far plane
	HitResult result = RayCast(ray_start, ray_end);
	return result;
	/*
	float mx = -1.0f + 2.0f * (float)(x) / (float)Vivid::GetFrameWidth();
	float my = 1.0f - 2.0f * (float)(y) / (float)Vivid::GetFrameHeight();

	glm::vec4 near_ndc = glm::vec4(mx, my, -1.0f, 1.0f);
	glm::vec4 far_ndc = glm::vec4(mx, my, 1.0f, 1.0f);

	// Correct view matrix: inverse of the camera's world matrix
	glm::mat4 view = glm::inverse(m_Camera->GetWorldMatrix());
	glm::mat4 proj = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix();
	glm::mat4 invViewProj = glm::inverse(proj * view);

	glm::vec4 near_world = invViewProj * near_ndc;
	glm::vec4 far_world = invViewProj * far_ndc;

	if (fabs(near_world.w) > 1e-6f) near_world /= near_world.w;
	if (fabs(far_world.w) > 1e-6f)  far_world /= far_world.w;

	glm::vec3 ray_origin = glm::vec3(near_world);
	glm::vec3 ray_dir = glm::normalize(glm::vec3(far_world) - ray_origin);

	return RayCast(ray_origin, ray_dir);
	*/
}


std::vector<TerrainMeshComponent*> GetTerrainMeshes(GraphNode* node, std::vector<TerrainMeshComponent*> meshes)
{

	//NodeEntity* pEntity = dynamic_cast<NodeEntity*>(node);

	auto sm = node->GetComponent<TerrainMeshComponent>();

	if (sm != nullptr) {
		meshes.push_back(sm);
	}



	for (auto sub : node->GetNodes()) {
		meshes = GetTerrainMeshes(sub, meshes);

	}

	return meshes;

}

std::vector<StaticMeshComponent*> GetMeshes(GraphNode* node, std::vector<StaticMeshComponent*> meshes)
{

	//NodeEntity* pEntity = dynamic_cast<NodeEntity*>(node);

	auto sm = node->GetComponent<StaticMeshComponent>();

	if (sm != nullptr) {
		meshes.push_back(sm);
	}



	for (auto sub : node->GetNodes()) {
		meshes = GetMeshes(sub, meshes);

	}

	return meshes;

}constexpr float EPSILON = 1e-6f;

HitResult RayToTri(glm::vec3 point1, glm::vec3 point2, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	HitResult res = HitResult();

	// Calculate ray direction from point1 to point2
	glm::vec3 rayDir = point2 - point1;

	edge1 = v1 - v0;
	edge2 = v2 - v0;
	h = glm::cross(rayDir, edge2);

	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return res;    // This ray is parallel to this triangle.

	f = 1.0f / a;
	s = point1 - v0;
	u = f * glm::dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return res;

	q = glm::cross(s, edge1);
	v = f * glm::dot(rayDir, q);
	if (v < 0.0f || u + v > 1.0f)
		return res;

	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * glm::dot(edge2, q);

	// Check if intersection is within the line segment (between point1 and point2)
	if (t > EPSILON && t <= 1.0f) // t must be between 0 and 1 for line segment
	{
		res.m_Hit = true;
		res.m_Point = point1 + rayDir * t;
		res.m_Distance = glm::length(res.m_Point - point1);
		return res;
	}
	else // No intersection within the line segment
	{
		res.m_Hit = false;
		return res;
	}
}

HitResult SceneGraph::RayCast(glm::vec3 pos, glm::vec3 end) {


	HitResult result;

	std::vector<StaticMeshComponent*> meshes;
	float cd = 10000;
	HitResult close;
	close.m_Hit = false;

	meshes = GetMeshes(m_RootNode, meshes);

	for (auto mesh : meshes) {

		//CastResult res = m_RayTester->CastMesh(pos, end, mesh);

		for (auto sm : mesh->GetSubMeshes()) {

			CastResult res = m_RayTester->CastMesh(float3(pos.x, pos.y, pos.z), float3(end.x, end.y, end.z), sm);


			//auto res = RayToTri(pos, end, v0, v1, v2);


			if (res.Hit) {



				if (res.Distance < cd) {
					HitResult nres;
					nres.m_Hit = true;
					nres.m_Distance = res.Distance;
					nres.m_Mesh = mesh;
					nres.m_Node = mesh->GetOwner();
					nres.m_Point = glm::vec3(res.HitPoint.x, res.HitPoint.y, res.HitPoint.z);
					//nres.m_Node = mesh->GetOwner();

					//nres.m_Entity = (NodeEntity*)mesh->GetOwner();
					cd = res.Distance;
					close = nres;

				}
			}



		}

	}

	std::vector<TerrainMeshComponent*> tmeshes;

	tmeshes = GetTerrainMeshes(m_RootNode, tmeshes);

	for (auto mesh : tmeshes) {

		//CastResult res = m_RayTester->CastMesh(pos, end, mesh);

		auto sm = mesh->GetMesh();

		CastResult res = m_RayTester->CastTerrainMesh(float3(pos.x, pos.y, pos.z), float3(end.x, end.y, end.z), sm);


		//auto res = RayToTri(pos, end, v0, v1, v2);


		if (res.Hit) {



			if (res.Distance < cd) {
				HitResult nres;
				nres.m_Hit = true;
				nres.m_Distance = res.Distance;
				//nres.m_Mesh = mesh;
				nres.m_Node = mesh->GetOwner();
				nres.m_Point = glm::vec3(res.HitPoint.x, res.HitPoint.y, res.HitPoint.z);
				//nres.m_Node = mesh->GetOwner();

				//nres.m_Entity = (NodeEntity*)mesh->GetOwner();
				cd = res.Distance;
				close = nres;

			}
		}



	}



	return close;
}

HitResult SceneGraph::RayCast(TerrainMesh* mesh, glm::vec3 pos, glm::vec3 end) {


	HitResult result;


	float cd = 10000;
	HitResult close;
	close.m_Hit = false;

	auto sm = mesh;

	CastResult res = m_RayTester->CastTerrainMesh(float3(pos.x, pos.y, pos.z), float3(end.x, end.y, end.z), sm);


	//auto res = RayToTri(pos, end, v0, v1, v2);


	if (res.Hit) {



		if (res.Distance < cd) {
			HitResult nres;
			nres.m_Hit = true;
			nres.m_Distance = res.Distance;
			//nres.m_Mesh = mesh;
			nres.m_Node = mesh->GetOwner();
			nres.m_Point = glm::vec3(res.HitPoint.x, res.HitPoint.y, res.HitPoint.z);

			//nres.m_Node = mesh->GetOwner();

			//nres.m_Entity = (NodeEntity*)mesh->GetOwner();
			cd = res.Distance;
			close = nres;

		}
	}

	return close;

}

HitResult SceneGraph::RayCast(StaticMeshComponent* mesh, glm::vec3 pos, glm::vec3 end) {


	HitResult result;

	std::vector<StaticMeshComponent*> meshes;
	float cd = 10000;
	HitResult close;
	close.m_Hit = false;





	//CastResult res = m_RayTester->CastMesh(pos, end, mesh);

	int ii = 0;
	for (auto sm : mesh->GetSubMeshes()) {

		CastResult res = m_RayTester->CastMesh(float3(pos.x, pos.y, pos.z), float3(end.x, end.y, end.z), sm);


		//auto res = RayToTri(pos, end, v0, v1, v2);


		if (res.Hit) {



			if (res.Distance < cd) {
				HitResult nres;
				nres.SubMeshIndex = ii;
				nres.m_Hit = true;
				nres.m_Distance = res.Distance;
				nres.m_Mesh = mesh;
				//nres.m_Node = mesh->GetOwner();

				//nres.m_Entity = (NodeEntity*)mesh->GetOwner();
				cd = res.Distance;
				close = nres;

			}
		}


		ii++;
	}


	return close;
}
HitResult SceneGraph::MousePickTerrain(int x, int y, TerrainMeshComponent* terrain) {
	float mx = -1.0f + 2.0f * (float)(x) / (float)QEngine::GetFrameWidth();
	float my = 1.0f - 2.0f * (float)(y) / (float)QEngine::GetFrameHeight();

	// --- Get Matrices and Camera Position ---
	glm::mat4 cameraWorldMatrix = m_Camera->GetWorldMatrix();
	glm::mat4 projMatrix = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix();

	// The ray should always originate from the camera's position for a perspective view
	glm::vec3 ray_origin = glm::vec3(cameraWorldMatrix[3]);

	// --- Calculate Ray Direction ---
	// Start with the mouse click in Normalized Device Coordinates (NDC)
	glm::vec4 ray_clip = glm::vec4(mx, my, -1.0, 1.0);

	// Transform from clip space to eye (view) space
	glm::mat4 invProjMatrix = glm::inverse(projMatrix);
	glm::vec4 ray_eye = invProjMatrix * ray_clip;

	// In view space, the direction is forward. Set z to -1 and w to 0 to make it a direction vector.
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// Transform the direction vector from eye (view) space to world space
	// Note: We use the camera's world matrix, NOT the view matrix (which is the inverse)
	glm::vec3 ray_direction = glm::normalize(glm::vec3(cameraWorldMatrix * ray_eye));

	// --- Cast the Ray ---
	// Your RayCast function may need an origin and direction, or two points.
	// If it needs two points, create a distant point along the ray.
	glm::vec3 ray_end = ray_origin + ray_direction * 100.0f; // 10000.0f is an arbitrary large distance

	HitResult result = RayCast(terrain->GetMesh(), ray_origin, ray_end);


	return result;
}


HitResult SceneGraph::MousePickSelect(int x, int y, StaticMeshComponent* mesh)
{
	float mx = -1.0f + 2.0f * (float)(x) / (float)QEngine::GetFrameWidth();
	float my = 1.0f - 2.0f * (float)(y) / (float)QEngine::GetFrameHeight();

	// Create points on near and far planes in NDC space
	glm::vec4 near_ndc = glm::vec4(mx, my, -1.0f, 1.0f);  // Near plane (z = -1)
	glm::vec4 far_ndc = glm::vec4(mx, my, 1.0f, 1.0f);   // Far plane (z = 1)

	// Get view and projection matrices
	glm::mat4 view = glm::inverse(m_Camera->GetWorldMatrix());
	glm::mat4 proj = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix();
	glm::mat4 invViewProj = glm::inverse(proj * view);

	// Transform NDC points to world space
	glm::vec4 near_world = invViewProj * near_ndc;
	glm::vec4 far_world = invViewProj * far_ndc;

	// Perform perspective divide
	if (fabs(near_world.w) > 1e-6f) near_world /= near_world.w;
	if (fabs(far_world.w) > 1e-6f) far_world /= far_world.w;

	// Extract 3D points
	glm::vec3 ray_start = glm::vec3(near_world);
	glm::vec3 ray_end = glm::vec3(far_world);
	HitResult result = RayCast(mesh,m_Camera->GetPosition(), ray_end);
	//RayCast(ray_start, ray_end);




	return result;

}

void SceneGraph::Push() {

	m_RootNode->Push();

}

void SceneGraph::Pop()
{

	m_RootNode->Pop();

}

void SceneGraph::Play() {

	m_RootNode->Play();

}
void SceneGraph::Stop() {

	m_RootNode->Stop();

}

void SceneGraph::SaveScene(std::string path) {

	VFile* f = new VFile(path.c_str(), FileMode::Write);

	m_Camera->Write(f);

	m_RootNode->Write(f);
	
	m_RootNode->WriteScripts(f);

	if (m_Terrain != nullptr) {
		f->WriteBool(true);
		WriteTerrain(f,m_Terrain);
	}
	else {
		f->WriteBool(false);
	}

	f->Close();



}

void SceneGraph::LoadScene(std::string path) {

	VFile* f = new VFile(path.c_str(), FileMode::Read);

	auto cam = m_Camera;
	m_Camera = new GraphNode;
	m_Camera->AddComponent(new CameraComponent);
	m_Camera->Read(f);
	m_Camera = cam;


	m_RootNode = new GraphNode;
	m_RootNode->Read(f);

	m_RootNode->ReadScripts(f);

	if(f->ReadBool()) {
		ReadTerrain(f);
	}
	else {
		m_Terrain = nullptr;
	}

	f->Close();

	SetOwners(m_RootNode);

}

void SceneGraph::SetOwners(GraphNode* node) {

	for (auto c : node->GetAllComponents()) {
		c->OnAttach(node);

	}

	for (auto n : node->GetNodes()) {
		SetOwners(n);
	}

}

GraphNode* SceneGraph::FindNode(std::string name) {

	return m_RootNode->FindNode(name);

}


void SceneGraph::ReadTerrain(VFile* f) {

	m_Terrain = new GraphNode;
	m_Terrain->AddComponent(new TerrainMeshComponent);
	m_Terrain->AddComponent(new TerrainRendererComponent);
	m_Terrain->AddComponent(new TerrainDepthRenderer);

	AddNode(m_Terrain);

	auto tcom = m_Terrain->GetComponent<TerrainMeshComponent>();

	TerrainMesh* mesh = tcom->GetMesh();

	tcom->SetMesh(mesh);

	int vc = f->ReadInt();
	for (int i = 0; i < vc; i++) {
		TerrainVertex v;
		v.position = f->ReadVec3();
		v.texture = f->ReadVec3();
		v.color = f->ReadVec4();
		v.normal = f->ReadVec3();
		v.binormal = f->ReadVec3();
		v.tangent = f->ReadVec3();
		v.layercoord = f->ReadVec3();
		mesh->AddVertex(v);
	}

	int tc = f->ReadInt();
	for (int i = 0; i < tc; i++) {
		Triangle t;
		t.v0 = f->ReadInt();
		t.v1 = f->ReadInt();
		t.v2 = f->ReadInt();
		mesh->AddTriangle(t);
	}
	mesh->Build();

	int lc = f->ReadInt();
	for (int i = 0; i < lc; i++) {
		std::string colorPath = f->ReadString();
		std::string normalPath = f->ReadString();
		std::string specPath = f->ReadString();
		Texture2D* colorTex = new Texture2D(colorPath);
		Texture2D* normalTex = new Texture2D(normalPath);
		Texture2D* specTex = new Texture2D(specPath);
		TerrainLayer* layer = new TerrainLayer;
		PixelMap* pix = new PixelMap(1, 1);
//		pix->Read(f);
		layer->SetColor(colorTex);
		layer->SetNormal(normalTex);
		layer->SetSpecular(specTex);
		layer->SetPixels(pix);
		layer->Create();
		tcom->AddLayer(layer);

		//layer->SetPixels(new PixelMap(256, 256));

		//mesh->AddLayer(layer);
	}
}

void SceneGraph::WriteTerrain(VFile* f,GraphNode* node) {

	auto mesh = node->GetComponent<TerrainMeshComponent>();

	auto meshBuf = mesh->GetMesh();

	f->WriteInt(meshBuf->GetVertices().size());
	for(auto v : meshBuf->GetVertices()) {
		f->WriteVec3(v.position);
		f->WriteVec3(v.texture);
		f->WriteVec4(v.color);
		f->WriteVec3(v.normal);
		f->WriteVec3(v.binormal);
		f->WriteVec3(v.tangent);
		f->WriteVec3(v.layercoord);
	}
	f->WriteInt(meshBuf->GetTriangles().size());
	for(auto t : meshBuf->GetTriangles()) {
		f->WriteInt(t.v0);
		f->WriteInt(t.v1);
		f->WriteInt(t.v2);
	}

	f->WriteInt(mesh->GetLayers().size());
	for(auto layer : mesh->GetLayers()) {
		f->WriteString(layer->GetColor()->GetPath().c_str());
		f->WriteString(layer->GetNormal()->GetPath().c_str());
		f->WriteString(layer->GetSpec()->GetPath().c_str());
		auto pix = layer->GetPixels();
		//pix->Write(f);


		//f->WriteString(layer.name);
	//	f->WriteString(layer.texture);
//		f->WriteFloat(layer.strength);
	//	f->WriteFloat(layer.height);
	}



}

void SceneGraph::SetTerrain(GraphNode* node) {
	m_Terrain = node;
	m_RootNode->AddNode(node);
}

int SceneGraph::Ren_Count = 0;

// This is a private helper function to recursively traverse the scene graph.
// It is not part of the SceneGraph class itself, but is a utility for it.
void CollectSharpComponentsRecursive(GraphNode* node, std::vector<SharpComponent*>& component_list)
{
	if (!node)
	{
		return;
	}

	// Get all SharpComponents from the current node.
	auto components_on_this_node = node->GetComponents<SharpComponent>();

	// Add them to our main list if any were found.
	if (!components_on_this_node.empty())
	{
		component_list.insert(component_list.end(), components_on_this_node.begin(), components_on_this_node.end());
	}

	// Recursively call this function for all children of the current node.
	for (GraphNode* child_node : node->GetNodes())
	{
		CollectSharpComponentsRecursive(child_node, component_list);
	}
}


std::vector<SharpComponent*> SceneGraph::GetAllSharpComponents()
{
	std::vector<SharpComponent*> result_list;
	if (m_RootNode)
	{
		// Start the recursive collection from the root node.
		CollectSharpComponentsRecursive(m_RootNode, result_list);
	}
	return result_list;
}
