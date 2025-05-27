#include "pch.h"
#include "RendererBase.h"
#include "NodeCamera.h"
#include "SceneGraph.h"

RendererBase::RendererBase() {

}

void RendererBase::SetSceneGraph(SceneGraph* graph) {

	m_SceneGraph = graph;
	Engine::m_Camera = graph->GetCamera();

}