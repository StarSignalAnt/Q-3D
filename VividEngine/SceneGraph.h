#pragma once

class GraphNode;

class SceneGraph
{
public:

	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void AddNode(GraphNode* node);
	void Render();
	GraphNode* GetCamera();


private:

	GraphNode* m_RootNode = nullptr;
	GraphNode* m_Camera;

};

