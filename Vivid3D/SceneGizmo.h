#pragma once
#include "GraphNode.h"
#include "SceneGraph.h"

enum GizmoSpace {
	Local,World,Smart
};

class SceneGizmo
{
public:

	GraphNode* GetNode() {
		return m_Node;
	}
	void Render(GraphNode* camera);
	void FixNode();
	void SetGraph(SceneGraph* graph)
	{
		m_Graph = graph;
	}
	bool Click(int x, int y);
	virtual void Select(int index) = 0;
	virtual void Move(glm::vec2 pos) = 0;
	void SetNode(GraphNode* node)
	{
		m_Selected = node;
	}
	void AlignGizmo();
protected:

	int SelectedID = -1;
	GraphNode* m_Node;
	SceneGraph* m_Graph;
	GraphNode* m_Selected = nullptr;
	GizmoSpace m_Space = GizmoSpace::World;
	glm::mat4 prot;
};

