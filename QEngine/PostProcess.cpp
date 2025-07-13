#include "pch.h"
#include "PostProcess.h"
#include "Draw2D.h"
#include "SceneGraph.h"
#include "RenderTarget2D.h"
#include "Texture2D.h"

PostProcess::PostProcess() {

//	m_Draw = new Draw2D(cam)

}

void PostProcess::CreateDraw(GraphNode* cam) {

	m_Draw = new Draw2D(cam);

}

void PostProcess::SetGraph(SceneGraph* graph) {

	m_Graph = graph;

}

void PostProcess::CreateTarget(int width, int height) {

	auto rt = new RenderTarget2D(width, height,false);
	m_Targets.push_back(rt);

}

void PostProcess::CreateTargets(int width, int height, int num) {


	for (int i = 0; i < num; i++) {

		CreateTarget(width, height);

	}

}

Q3D::Engine::Texture::Texture2D* PostProcess::Process(Q3D::Engine::Texture::Texture2D* frame) {

	return nullptr;

}

void PostProcess::BindRT(int i) {

	m_Targets[i]->Bind();

}

void PostProcess::ReleaseRT(int i) {

	m_Targets[i]->Release();

}

Q3D::Engine::Texture::Texture2D* PostProcess::GetTexture(int i) {

	return new Q3D::Engine::Texture::Texture2D(m_Targets[i]);

}