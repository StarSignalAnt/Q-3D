#pragma once
#include <vector>
#include <glm/glm.hpp>
class Intersections;
class GraphNode;
class CubeRenderer;
class StaticMeshComponent;

struct HitResult {

	bool m_Hit = false;
	GraphNode* m_Node = nullptr;
	glm::vec3 m_Point;
	float m_Distance = 10000.0f;
	StaticMeshComponent* m_Mesh;
	int SubMeshIndex = -1;
	//NodeEntity* m_Entity = nullptr;


};


class SceneGraph
{
public:

	SceneGraph();
	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void AddNode(GraphNode* node);
	void Render();
	void RenderDepth();
	void RenderShadows();
	void Update(float dt);
	void SetCamera(GraphNode* cameraNode);
	GraphNode* GetCamera();
	void AddLight(GraphNode* lightNode);

	static SceneGraph* m_CurrentGraph;

	std::vector<GraphNode*> GetLights() {
		return m_Lights;
	}
	HitResult MousePick(int x, int y);
	HitResult RayCast(glm::vec3 pos, glm::vec3 end);
	HitResult RayCast(StaticMeshComponent* mesh,glm::vec3 pos, glm::vec3 end);
	HitResult MousePickSelect(int x, int y, StaticMeshComponent* mesh);
	void Play();
	void Stop();
	void Push();
	void Pop();
private:

	GraphNode* m_RootNode = nullptr;
	GraphNode* m_Camera;
	std::vector<GraphNode*> m_Lights;
	CubeRenderer* m_ShadowRenderer = nullptr;
	Intersections* m_RayTester;

};

