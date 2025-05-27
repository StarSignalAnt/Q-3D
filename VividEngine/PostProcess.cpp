#include "pch.h"
#include "PostProcess.h"
#include "Draw2D.h"
#include "SceneGraph.h"
#include "RenderTarget2D.h"
#include "Texture2D.h"

PostProcess::PostProcess() {

	m_Draw = new Draw2D;

}

void PostProcess::SetGraph(SceneGraph* graph) {

	m_Graph = graph;

}

void PostProcess::CreateTarget(int width, int height) {

	auto rt = new RenderTarget2D(width, height);
	m_Targets.push_back(rt);

}

void PostProcess::CreateTargets(int width, int height, int num) {


	for (int i = 0; i < num; i++) {

		CreateTarget(width, height);

	}

}

Texture2D* PostProcess::Process(Texture2D* frame) {

	return nullptr;

}

void PostProcess::BindRT(int i) {

	m_Targets[i]->Bind();

}

void PostProcess::ReleaseRT(int i) {

	m_Targets[i]->Release();

}

Texture2D* PostProcess::GetTexture(int i) {

	return new Texture2D(m_Targets[i]);

}