#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
class Intersections;
class GraphNode;
class CubeRenderer;
class StaticMeshComponent;
class TerrainMeshComponent;
class TerrainMesh;
class VFile;

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
	void SetTerrain(GraphNode* node);


	HitResult MousePick(int x, int y);
	HitResult RayCast(glm::vec3 pos, glm::vec3 end);
	HitResult RayCast(StaticMeshComponent* mesh,glm::vec3 pos, glm::vec3 end);
	HitResult RayCast(TerrainMesh* mesh, glm::vec3 pos, glm::vec3 end);
	HitResult MousePickSelect(int x, int y, StaticMeshComponent* mesh);
	HitResult MousePickTerrain(int x, int y, TerrainMeshComponent* mesh);
	void Play();
	void Stop();
	void Push();
	void Pop();
	
	void SaveScene(std::string path);
	void LoadScene(std::string path);
	GraphNode* FindNode(std::string node);
	void WriteTerrain(VFile* f,GraphNode* node);
	void ReadTerrain(VFile* f);

	static SceneGraph* m_Instance;

	void Reset();
	void SetOwners(GraphNode* node);
private:

	GraphNode* m_RootNode = nullptr;
	GraphNode* m_Camera;
	std::vector<GraphNode*> m_Lights;
	CubeRenderer* m_ShadowRenderer = nullptr;
	Intersections* m_RayTester;
	float m_TerrainX = 0;
	float m_TerrainZ = 0;
	GraphNode* m_Terrain = nullptr;
};

