#pragma once
#include "SceneGraph.h"
#include "GraphNode.h"
class PPOutline;

class SceneSelectionOverlay
{
public:

	SceneSelectionOverlay(SceneGraph* graph);
	void SelectNode(GraphNode* node);
	void Render();

private:

	SceneGraph* m_Graph;
	GraphNode* m_Selected = nullptr;
	PPOutline* m_PP;

};

