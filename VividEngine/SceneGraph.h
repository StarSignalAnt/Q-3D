#pragma once
#include <vector>

class GraphNode;
class CubeRenderer;

class SceneGraph
{
public:

	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void AddNode(GraphNode* node);
	void Render();
	void RenderDepth();
	void RenderShadows();
	void SetCamera(GraphNode* cameraNode);
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
	CubeRenderer* m_ShadowRenderer = nullptr;

};

