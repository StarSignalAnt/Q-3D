#include "pch.h"
#include "SceneGraph.h"
#include "GraphNode.h"

void SceneGraph::SetRootNode(GraphNode* node) {

	m_RootNode = node;

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