#pragma once
#include <vector>

class GraphNode;

class SceneGraph
{
public:

	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void AddNode(GraphNode* node);
	void Render();
	GraphNode* GetCamera();
	void AddLight(GraphNode* lightNode);

	static SceneGraph* m_CurrentGraph;

	std::vector<GraphNode*> GetLights() {
		return m_Lights;
	}

private:

	GraphNode* m_RootNode = nullptr;
	GraphNode* m_Camera;
	std::vector<GraphNode*> m_Lights;

};

