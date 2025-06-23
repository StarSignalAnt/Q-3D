#pragma once
#include <vector>

class RenderTarget2D;
class Draw2D;
class SceneGraph;
class Texture2D;
class GraphNode;

class PostProcess
{
public:

	PostProcess();
	void SetGraph(SceneGraph* graph);
	void CreateTarget(int width, int height);
	void CreateTargets(int width, int height, int num);
	virtual Texture2D* Process(Texture2D* frame);
	void BindRT(int i);
	void ReleaseRT(int i);
	Texture2D* GetTexture(int i);
	SceneGraph* GetGraph() {
		return m_Graph;
	}
	void CreateDraw(GraphNode* cam);

protected:

	std::vector<RenderTarget2D*> m_Targets;
	Draw2D* m_Draw;
	SceneGraph* m_Graph;

};

