#pragma once
#include "LinesRendererComponent.h"
#include "SceneGraph.h"
#include "GraphNode.h"

class SceneGrid
{
public:

	SceneGrid(SceneGraph* graph);
	void CreateGrid();
	void Update();

private:

	SceneGraph* m_Graph;
	GraphNode* m_Grid;
	float m_XSize = 80;
	float m_YSize = 80;
	float m_UnitSize = 1.0f;

	glm::vec4 m_MainColor;

};

