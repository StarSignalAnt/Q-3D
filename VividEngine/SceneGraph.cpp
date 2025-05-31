
#include "SceneGraph.h"
#include "GraphNode.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include "CubeRenderer.h"

SceneGraph* SceneGraph::m_CurrentGraph = nullptr;

void SceneGraph::SetRootNode(GraphNode* node) {

	m_RootNode = node;
	m_Camera = new GraphNode;
	m_Camera->AddComponent(new CameraComponent);
	m_ShadowRenderer = new CubeRenderer(this, nullptr);

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

void SceneGraph::RenderDepth() {

	m_CurrentGraph = this;
	m_RootNode->RenderDepth(m_Camera);

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

void SceneGraph::SetCamera(GraphNode* camera) {
//	if (m_Camera != nullptr) {
//		delete m_Camera;
//	}
	m_Camera = camera;
}

void SceneGraph::RenderShadows() {

	for (auto light : m_Lights) {

		m_ShadowRenderer->RenderDepth(light->GetPosition(), light->GetComponent<LightComponent>()->GetRange(),light->GetComponent<LightComponent>()->GetShadowMap());

	}

}

void SceneGraph::Update(float dt) {


	m_CurrentGraph = this;
	m_RootNode->Update(dt);

}