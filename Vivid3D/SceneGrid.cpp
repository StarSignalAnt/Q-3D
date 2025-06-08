#include "SceneGrid.h"

SceneGrid::SceneGrid(SceneGraph* graph) {

	m_Graph = graph;
	m_Grid = new GraphNode;
	m_Grid->SetHideFromEditor(true);
	m_Graph->AddNode(m_Grid);
	m_MainColor = glm::vec4(1, 1, 1, 1);
	CreateGrid();

}

void SceneGrid::CreateGrid() {

	auto com = new LinesRendererComponent;

	auto col2 = m_MainColor;
	col2.x *= 0.45;
	col2.y *= 0.45;
	col2.z *= 0.45;
	col2.a = 1.0;

	int ix = 0;
	for (float x = -m_XSize;x < m_XSize;x += m_UnitSize)
	{
		
		LineVertex v1, v2;

		v1.position = glm::vec3(x, 0, -m_YSize);
		v2.position = glm::vec3(x, 0, m_YSize);
		
		v1.color = col2;
		v2.color = col2;
		Line line;
		line.v0 = ix++;
		line.v1 = ix++;

		com->AddVertex(v1);
		com->AddVertex(v2);

		com->AddLine(line);

	}
	
	for (float z = -m_YSize;z < m_YSize;z += m_UnitSize) {

		LineVertex v1, v2;
		v1.position = glm::vec3(-m_XSize, 0, z);
		v2.position = glm::vec3(m_XSize, 0, z);

		v1.color = col2;
		v2.color = col2;
		Line line;
		line.v0 = ix++;
		line.v1 = ix++;

		com->AddVertex(v1);
		com->AddVertex(v2);
		com->AddLine(line);

	}

	

	for (float x = -m_XSize;x < m_XSize;x += m_UnitSize*4)
	{

		LineVertex v1, v2;

		v1.position = glm::vec3(x, 0, -m_YSize);
		v2.position = glm::vec3(x, 0, m_YSize);
		v1.color = m_MainColor;
		v2.color = m_MainColor;

		Line line;
		line.v0 = ix++;
		line.v1 = ix++;

		com->AddVertex(v1);
		com->AddVertex(v2);

		com->AddLine(line);

	}

	for (float z = -m_YSize;z < m_YSize;z += m_UnitSize*4) {

		LineVertex v1, v2;
		v1.position = glm::vec3(-m_XSize, 0, z);
		v2.position = glm::vec3(m_XSize, 0, z);
		v1.color = m_MainColor;
		v2.color = m_MainColor;

		Line line;
		line.v0 = ix++;
		line.v1 = ix++;

		com->AddVertex(v1);
		com->AddVertex(v2);
		com->AddLine(line);

	}


	com->Finalize();

	m_Grid->AddComponent(com);

}