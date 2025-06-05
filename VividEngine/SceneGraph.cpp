
#include "SceneGraph.h"
#include "GraphNode.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "CubeRenderer.h"
#include "Vivid.h"
#include "StaticMeshComponent.h"
#include "Intersections.h"
SceneGraph* SceneGraph::m_CurrentGraph = nullptr;
#include "ScriptHost.h"

bool addedGraphFuncs = false;


SceneGraph::SceneGraph() {

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
	m_RootNode->RenderDepth(m_Camera);

}

void SceneGraph::Render() {

	m_CurrentGraph = this;
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

void SceneGraph::Update(float dt) {


	m_CurrentGraph = this;
	m_RootNode->Update(dt);

}


HitResult SceneGraph::MousePick(int x, int y)
{
	float fx = (float)x;
	float fy = (float)y;
	float mx = -1 + (float)(x) / (float)(Vivid::GetFrameWidth()) * 2;
	float my = 1 - (float)(y) / (float)(Vivid::GetFrameHeight()) * 2;

	glm::vec3 origin = glm::vec3(mx, my, 0);
	glm::vec3 dest = glm::vec3(mx, my, 1.0f);



	//Matrix4x4 viewProj = RenderGlobals.CurrentCamera.WorldMatrix * RenderGlobals.CurrentCamera.ProjectionMatrix;
	glm::mat4 viewProj = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix() * glm::inverse(m_Camera->GetWorldMatrix());

	// Matrix4x4 vp;
	// Matrix4x4.Invert(viewProj, out vp);
	// Matrix4x4 inverseProj = vp;


	glm::mat4 inverseProj = glm::inverse(viewProj);



	//****Ve
	//Vector3 ray_origin = Vector3.Transform (origin, inverseProj);
	//Vector3 ray_end = Vector3.TransformNormal(dest, inverseProj);
	//Vector4 ro = Vector4.Transform(origin, inverseProj);
	//Vector4 rd = Vector4.Transform(dest, inverseProj);

	glm::vec4 ro = inverseProj * glm::vec4(origin, 1.0f);
		glm::vec4 rd = inverseProj * glm::vec4(dest, 1.0);



	glm::vec3 ray_origin = glm::vec3(ro.x / ro.w, ro.y / ro.w, ro.z / ro.w);
	glm::vec3 ray_end = glm::vec3(rd.x / rd.w, rd.y / rd.w, rd.z / rd.w);
	glm::vec3 ray_dir = ray_end - ray_origin;

	//Vector3 ray_origin = new Vector3(ro.X / ro.W, ro.Y / ro.W, ro.Z / ro.W);
	//Vector3 ray_end = new Vector3(rd.X / rd.W, rd.Y / rd.W, rd.Z / rd.W);
	//Vector3 ray_dir = ray_end - ray_origin;

	ray_dir = normalize(ray_dir);

	//Ray ray = new Ray();


   // ray.Pos = RenderGlobals.CurrentCamera.Position;
   // ray.Dir = ray_dir;
   // return ray;
	HitResult result = RayCast(m_Camera->GetPosition(), ray_dir);


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

}
constexpr float EPSILON = 1e-6f;
HitResult RayToTri(glm::vec3 pos, glm::vec3 end, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{

	//return HitResult();
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	HitResult res = HitResult();

	edge1 = v1 - v0;// vertex1 - vertex0;
	edge2 = v2 - v0;
	h = glm::cross(end, edge2);



	a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return res;    // This ray is parallel to this triangle.
	f = 1.0f / a;
	s = pos - v0;
	u = f * dot(s, h);
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
		res.m_Point = pos + end * t;
		res.m_Distance = glm::length(res.m_Point - pos);
		return res;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return res;

	res.m_Hit = false;
	return res;

	return res;
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

			CastResult res = m_RayTester->CastMesh(float3(pos.x,pos.y,pos.z), float3(end.x,end.y,end.z), sm);


			//auto res = RayToTri(pos, end, v0, v1, v2);


			if (res.Hit) {



				if (res.Distance < cd) {
					HitResult nres;
					nres.m_Hit = true;
					nres.m_Distance = res.Distance;
					nres.m_Mesh = mesh;
					nres.m_Node = mesh->GetOwner();
					//nres.m_Node = mesh->GetOwner();

					//nres.m_Entity = (NodeEntity*)mesh->GetOwner();
					cd = res.Distance;
					close = nres;

				}
			}



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

HitResult SceneGraph::MousePickSelect(int x, int y, StaticMeshComponent* mesh)
{
	float fx = (float)x;
	float fy = (float)y;
	float mx = -1 + (float)(x) / (float)(Vivid::GetFrameWidth()) * 2;
	float my = 1 - (float)(y) / (float)(Vivid::GetFrameHeight()) * 2;

	glm::vec3 origin = glm::vec3(mx, my, 0);
	glm::vec3 dest = glm::vec3(mx, my, 1.0f);



	//Matrix4x4 viewProj = RenderGlobals.CurrentCamera.WorldMatrix * RenderGlobals.CurrentCamera.ProjectionMatrix;
	glm::mat4 viewProj = m_Camera->GetComponent<CameraComponent>()->GetProjectionMatrix() * glm::inverse(m_Camera->GetWorldMatrix());

	// Matrix4x4 vp;
	// Matrix4x4.Invert(viewProj, out vp);
	// Matrix4x4 inverseProj = vp;


	glm::mat4 inverseProj = glm::inverse(viewProj);



	//****Ve
	//Vector3 ray_origin = Vector3.Transform (origin, inverseProj);
	//Vector3 ray_end = Vector3.TransformNormal(dest, inverseProj);
	//Vector4 ro = Vector4.Transform(origin, inverseProj);
	//Vector4 rd = Vector4.Transform(dest, inverseProj);

	glm::vec4 ro = inverseProj * glm::vec4(origin, 1.0f);
	glm::vec4 rd = inverseProj * glm::vec4(dest, 1.0);



	glm::vec3 ray_origin = glm::vec3(ro.x / ro.w, ro.y / ro.w, ro.z / ro.w);
	glm::vec3 ray_end = glm::vec3(rd.x / rd.w, rd.y / rd.w, rd.z / rd.w);
	glm::vec3 ray_dir = ray_end - ray_origin;

	//Vector3 ray_origin = new Vector3(ro.X / ro.W, ro.Y / ro.W, ro.Z / ro.W);
	//Vector3 ray_end = new Vector3(rd.X / rd.W, rd.Y / rd.W, rd.Z / rd.W);
	//Vector3 ray_dir = ray_end - ray_origin;

	ray_dir = normalize(ray_dir);

	//Ray ray = new Ray();


   // ray.Pos = RenderGlobals.CurrentCamera.Position;
   // ray.Dir = ray_dir;
   // return ray;
	HitResult result = RayCast(mesh,m_Camera->GetPosition(), ray_dir);


	return result;

}

void SceneGraph::Push() {

	m_RootNode->Push();

}

void SceneGraph::Pop()
{

	m_RootNode->Pop();

}