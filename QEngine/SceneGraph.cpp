
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
#include "OctreeNode.h"
#include "OctreeScene.h"

bool addedGraphFuncs = false;

void CollectLightsRecursive(GraphNode* node, std::vector<GraphNode*>& lights_list) {
	if (!node) {
		return;
	}
	// Check if the current node has a light component
	if (node->GetComponent<LightComponent>()) {
		lights_list.push_back(node);
	}
	// Recurse for all children
	for (GraphNode* child : node->GetNodes()) {
		CollectLightsRecursive(child, lights_list);
	}
}


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
	m_RootNode->SetRenderType(NodeRenderType::RenderType_Static, false);
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

		// If the octree exists, use it for rendering.
	if (m_Octree)
	{
	m_Octree->RenderDepthCulled(m_Camera);
	}
	else
	{
		// Fallback to old rendering method if octree isn't built.
		Ren_Count = 0;
		m_RootNode->RenderDepth(m_Camera);
	}


	if (m_Terrain) {
		m_Terrain->RenderDepth(m_Camera);
	}
	//m_RootNode->RenderDepth(m_Camera);


}

void SceneGraph::Render() {

	m_CurrentGraph = this;

	m_CurrentGraph = this;





	// If the octree exists, use it for rendering.
	if (m_Octree)
	{
		m_Octree->CheckNodes();
		m_Octree->RenderCulled(m_Camera);
	
	}
	else
	{
		// Fallback to old rendering method if octree isn't built.
		Ren_Count = 0;
		m_RootNode->Render(m_Camera);
	}

	return;
	
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
namespace nlohmann {
	template <>
	struct adl_serializer<glm::vec4> {
		static void to_json(json& j, const glm::vec4& v) {
			j = { v.x, v.y, v.z, v.w };
		}

		static void from_json(const json& j, glm::vec4& v) {
			if (j.is_array() && j.size() == 4) {
				j.at(0).get_to(v.x);
				j.at(1).get_to(v.y);
				j.at(2).get_to(v.z);
				j.at(3).get_to(v.w);
			}
		}
	};

	template <>
	struct adl_serializer<TerrainVertex> {
		static void to_json(json& j, const TerrainVertex& v) {
			j = {
				{"pos", v.position}, {"tex", v.texture}, {"col", v.color},
				{"norm", v.normal}, {"binorm", v.binormal}, {"tan", v.tangent},
				{"layer", v.layercoord}
			};
		}

		static void from_json(const json& j, TerrainVertex& v) {
			j.at("pos").get_to(v.position);
			j.at("tex").get_to(v.texture);
			j.at("col").get_to(v.color);
			j.at("norm").get_to(v.normal);
			j.at("binorm").get_to(v.binormal);
			j.at("tan").get_to(v.tangent);
			j.at("layer").get_to(v.layercoord);
		}
	};

	template <>
	struct adl_serializer<Triangle> {
		static void to_json(json& j, const Triangle& t) {
			j = { t.v0, t.v1, t.v2 };
		}

		static void from_json(const json& j, Triangle& t) {
			if (j.is_array() && j.size() == 3) {
				j.at(0).get_to(t.v0);
				j.at(1).get_to(t.v1);
				j.at(2).get_to(t.v2);
			}
		}
	};
} // namespace nlohmann

void SceneGraph::JSaveScene(const std::string& path) {
	json scene_json;
	scene_json["version"] = 1.0;

	// Save camera
	json camera_json;
	m_Camera->JWrite(camera_json);
	scene_json["camera"] = camera_json;

	// Save root node hierarchy
	json root_json;
	m_RootNode->JWrite(root_json);
	scene_json["root"] = root_json;

	// Save scripts in a separate block for two-pass loading
	json scripts_json = json::object();
	m_RootNode->JWriteScripts(scripts_json);
	scene_json["scripts"] = scripts_json;

	// Save terrain if it exists
	if (m_Terrain) {
		json terrain_json;
		JWriteTerrain(terrain_json, m_Terrain);
		scene_json["terrain"] = terrain_json;
	}

	// Write to file
	std::ofstream o(path);
	o << std::setw(4) << scene_json << std::endl;
	o.close();
}

void SceneGraph::JLoadScene(const std::string& path) {
	std::ifstream i(path);
	if (!i.is_open()) {
		return;
	}
	json scene_json;
	i >> scene_json;
	i.close();

	// --- Clear current scene elements ---
	m_Lights.clear();
	if (m_RootNode) delete m_RootNode;
	m_RootNode = new GraphNode(); // Create a fresh root

	// --- PASS 1: Load node hierarchy, camera, and terrain ---
	if (scene_json.contains("camera")) {
		m_Camera->JRead(scene_json["camera"]);
	}

	if (scene_json.contains("root")) {
		m_RootNode->JRead(scene_json["root"]);
	}

	if (scene_json.contains("terrain")) {
		JReadTerrain(scene_json["terrain"]);
	}

	// --- PASS 2: Load scripts ---
	if (scene_json.contains("scripts")) {
		m_RootNode->JReadScripts(scene_json["scripts"]);
	}

	// --- PASS 3: Post-load setup ---
	SetOwners(m_RootNode);

	// Find all the lights in the newly loaded scene and populate the list
	CollectLightsRecursive(m_RootNode, m_Lights);
}

// ... rest of the SceneGraph.cpp file ...
void SceneGraph::JWriteTerrain(json& j, GraphNode* node)  {
	auto* mesh_comp = node->GetComponent<TerrainMeshComponent>();
	if (!mesh_comp) return;

	auto* mesh_buf = mesh_comp->GetMesh();
	if (!mesh_buf) return;

	j["vertices"] = mesh_buf->GetVertices();
	j["triangles"] = mesh_buf->GetTriangles();

	json layers_array = json::array();
	for (const auto& layer : mesh_comp->GetLayers()) {
		json layer_json;
		layer_json["color_path"] = layer->GetColor()->GetPath();
		layer_json["normal_path"] = layer->GetNormal()->GetPath();
		layer_json["specular_path"] = layer->GetSpec()->GetPath();
		layers_array.push_back(layer_json);
	}
	j["layers"] = layers_array;
}

void SceneGraph::JReadTerrain(const json& j) {
	if (m_Terrain) {
		if (m_RootNode) m_RootNode->RemoveNode(m_Terrain);
		delete m_Terrain;
		m_Terrain = nullptr;
	}

	m_Terrain = new GraphNode;
	m_Terrain->SetName("Terrain");
	m_Terrain->AddComponent(new TerrainMeshComponent);
	m_Terrain->AddComponent(new TerrainRendererComponent);
	m_Terrain->AddComponent(new TerrainDepthRenderer);
	AddNode(m_Terrain);

	auto* tcom = m_Terrain->GetComponent<TerrainMeshComponent>();
	TerrainMesh* mesh = tcom->GetMesh();

	if (j.contains("vertices")) {
		auto vertices = j["vertices"].get<std::vector<TerrainVertex>>();
		for (const auto& v : vertices) mesh->AddVertex(v);
	}

	if (j.contains("triangles")) {
		auto triangles = j["triangles"].get<std::vector<Triangle>>();
		for (const auto& t : triangles) mesh->AddTriangle(t);
	}
	mesh->Build();

	if (j.contains("layers")) {
		for (const auto& layer_json : j["layers"]) {
			auto* layer = new TerrainLayer();
			layer->SetColor(new Texture2D(layer_json.value("color_path", "")));
			layer->SetNormal(new Texture2D(layer_json.value("normal_path", "")));
			layer->SetSpecular(new Texture2D(layer_json.value("specular_path", "")));
			layer->SetPixels(new PixelMap(256, 256));
			layer->Create();
			tcom->AddLayer(layer);
		}
	}
}

void CollectStaticMeshesRecursive(GraphNode* node, std::vector<StaticMeshComponent*>& component_list)
{
	if (!node)
	{
		return;
	}

	// Get all StaticMeshComponents from the current node.
	// We use GetComponents (plural) in case a node could ever have more than one.
	auto components_on_this_node = node->GetComponents<StaticMeshComponent>();

	// Add them to our main list if any were found.
	if (!components_on_this_node.empty())
	{
		component_list.insert(component_list.end(), components_on_this_node.begin(), components_on_this_node.end());
	}

	// Recursively call this function for all children of the current node.
	for (GraphNode* child_node : node->GetNodes())
	{
		CollectStaticMeshesRecursive(child_node, component_list);
	}
}
void CalculateBoundsRecursive(GraphNode* node, Bounds& totalBounds, bool& foundFirstVertex)
{
	if (!node)
	{
		return;
	}

	// Process the meshes on the current node
	auto meshComponents = node->GetComponents<StaticMeshComponent>(); //
	if (!meshComponents.empty())
	{
		// Get the world matrix for THIS specific node.
		glm::mat4 worldMatrix = node->GetWorldMatrix(); //

		for (auto* meshComp : meshComponents)
		{
			for (const auto* subMesh : meshComp->GetSubMeshes()) //
			{
				for (const auto& vertex : subMesh->m_LODs[0]->m_Vertices) //
				{
					// Transform the local vertex position to world space.
					glm::vec3 worldPos = glm::vec3(worldMatrix * glm::vec4(vertex.position, 1.0f));

					if (!foundFirstVertex)
					{
						// Initialize the bounds with the very first vertex found.
						totalBounds.min = worldPos;
						totalBounds.max = worldPos;
						foundFirstVertex = true;
					}
					else
					{
						// Expand the bounds to include the new vertex.
						totalBounds.min = glm::min(totalBounds.min, worldPos);
						totalBounds.max = glm::max(totalBounds.max, worldPos);
					}
				}
			}
		}
	}

	// Recursively process all child nodes
	for (GraphNode* childNode : node->GetNodes()) //
	{
		CalculateBoundsRecursive(childNode, totalBounds, foundFirstVertex);
	}
}
void CollectMeshComponentsRecursive(GraphNode* node, std::vector<StaticMeshComponent*>& components)
{
	if (!node) return;

	auto meshComps = node->GetComponents<StaticMeshComponent>(); //
	if (!meshComps.empty()) {
		components.insert(components.end(), meshComps.begin(), meshComps.end());
	}

	for (auto* child : node->GetNodes()) { //
		CollectMeshComponentsRecursive(child, components);
	}
}



// --- PUBLIC FUNCTION IMPLEMENTATION (Add this to the end of SceneGraph.cpp) ---
Bounds SceneGraph::CalculateSceneBounds()
{
	std::vector<StaticMeshComponent*> allMeshComponents;
	if (m_RootNode) {
		CollectMeshComponentsRecursive(m_RootNode, allMeshComponents);
	}

	if (allMeshComponents.empty()) {
		return Bounds(); // Return an invalid bounds if no meshes exist
	}

	// 2. Create one large list of ALL vertices in WORLD SPACE.
	std::vector<glm::vec3> allWorldVertices;
	for (const auto meshComp : allMeshComponents)
	{
		GraphNode* owner = meshComp->GetOwner();
		if (!owner) continue;

		glm::mat4 worldMatrix = owner->GetWorldMatrix(); //

		for (const auto* subMesh : meshComp->GetSubMeshes()) //
		{
			for (const auto& vertex : subMesh->m_LODs[0]->m_Vertices) //
			{
				// Transform and add to our master list.
				allWorldVertices.push_back(glm::vec3(worldMatrix * glm::vec4(vertex.position, 1.0f)));
			}
		}
	}

	if (allWorldVertices.empty()) {
		return Bounds();
	}

	// 3. Find the min and max of the world-space points.
	// This simple loop is unambiguous and guaranteed to be correct.
	Bounds totalBounds;
	totalBounds.min = allWorldVertices[0];
	totalBounds.max = allWorldVertices[0];

	for (size_t i = 1; i < allWorldVertices.size(); ++i)
	{
		totalBounds.min = glm::min(totalBounds.min, allWorldVertices[i]);
		totalBounds.max = glm::max(totalBounds.max, allWorldVertices[i]);
	}

	// 4. Finalize the bounds calculation.
	totalBounds.CalculateDerivedValues(); //

	return totalBounds;
}

void SceneGraph::InitializeOctree()
{
	// Ensure we don't already have an octree.
	// Ensure we don't already have an octree.
	m_Octree.reset();

	// STEP 1: Calculate the total bounds of all static meshes in the scene.
	Bounds sceneBounds = CalculateSceneBounds(); //

	// Proceed only if we found valid geometry.
	if (!sceneBounds.IsValid()) //
	{
		// Optional: Log a warning that no geometry was found.
		return;
	}

	// --- NEW: Expand the bounds slightly to avoid floating point issues at the edges. ---
	constexpr float bounds_epsilon = 0.01f;
	sceneBounds.min -= glm::vec3(bounds_epsilon);
	sceneBounds.max += glm::vec3(bounds_epsilon);
	sceneBounds.CalculateDerivedValues(); // Recalculate size and center after expanding


	sceneBounds.Debug(); //
	// STEP 2: Instantiate the Octree with the new, slightly larger bounds.
	m_Octree = std::make_unique<Octree>(sceneBounds,m_Camera); //
	m_Octree->SetGraph(this);
	// STEP 3: Build the octree.
	std::cout << "Building octree" << std::endl;
	m_Octree->Build(m_RootNode); //
	std::cout << "Octree built" << std::endl;
	m_Octree->Optimize(); //
	m_Octree->BakeRenderCache(); //
	int b = 5;
	m_Octree->DebugLog(); //

}



void SceneGraph::ExportOctree(std::string path) {

	m_Octree->Export(path);

}

void SceneGraph::ImportOctree(std::string path) {

	m_Octree.reset(new Octree(path,m_Camera));
	m_Octree->SetGraph(this);
//	m_Octree->LoadAllNodes();

}

std::vector<GraphNode*> GetDynamicNodes(GraphNode* node, std::vector<GraphNode*> list) {

	if (node->GetRenderType() == NodeRenderType::RenderType_Dynamic) {
		list.push_back(node);
	}

	for (auto sub : node->GetNodes()) {

		list = GetDynamicNodes(sub, list);

	}

	return list;
}

std::vector<GraphNode*> SceneGraph::GetDynamics() {

	std::vector<GraphNode*> list;

	list = GetDynamicNodes(m_RootNode, list);

	return list;

}