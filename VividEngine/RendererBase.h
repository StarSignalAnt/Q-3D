#pragma once
#include "Engine.h"
#include <vector>

class SceneGraph;
class PostProcessing;

class RendererBase
{
public:

	RendererBase();
	void SetSceneGraph(SceneGraph* graph);
	virtual void PreRender() = 0;
	virtual void Render(bool no_render) = 0;
	SceneGraph* GetSceneGraph() {
		return m_SceneGraph;
	}

private:

	SceneGraph* m_SceneGraph;

};

