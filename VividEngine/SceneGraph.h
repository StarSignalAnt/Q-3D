#pragma once

class GraphNode;

class SceneGraph
{
public:

	void SetRootNode(GraphNode* node);
	GraphNode* GetRootNode();
	void AddNode(GraphNode* node);

private:

	GraphNode* m_RootNode = nullptr;

};

