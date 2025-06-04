#include "SceneSelectionOverlay.h"
#include "PPOutline.h"

SceneSelectionOverlay::SceneSelectionOverlay(SceneGraph* graph) {

	m_Graph = graph;
	m_PP = new PPOutline;
	m_PP->SetCam(graph->GetCamera());
	

}

void SceneSelectionOverlay::SelectNode(GraphNode* node) {

	m_Selected = node;
	m_PP->SetNode(node);

}

void SceneSelectionOverlay::Render() {

	m_PP->Process(nullptr);

}