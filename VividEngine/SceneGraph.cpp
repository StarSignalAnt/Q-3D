
#include "SceneGraph.h"
#include "GraphNode.h"
#include "CameraComponent.h"

SceneGraph* SceneGraph::m_CurrentGraph = nullptr;

void SceneGraph::SetRootNode(GraphNode* node) {

	m_RootNode = node;
	m_Camera = new GraphNode;
	m_Camera->AddComponent(new CameraComponent);

}

GraphNode* SceneGraph::GetRootNode() {

	return m_RootNode;

}

void SceneGraph::AddNode(GraphNode* node) {
	if (m_RootNode == nullptr) {
		m_RootNode = node;
	} else {
		m_RootNode->AddNode(node);
	}
}

void SceneGraph::Render() {

	m_CurrentGraph = this;
	m_RootNode->Render(m_Camera);

//	for (auto sub : m_RootNode->GetNodes()) {
	
//	}

}

GraphNode* SceneGraph::GetCamera() {
	return m_Camera;
}

void SceneGraph::AddLight(GraphNode* light) {

	m_Lights.push_back(light);

}