#pragma once
#include "BasicMath.hpp"
using namespace Diligent;

class SceneGraph;
class OctNode;

class SceneOctree
{
public:

	SceneOctree(SceneGraph* graph);
	int LeafCount();
	void RenderBF();
private:

	SceneGraph* m_Graph;
	OctNode* m_Root;
};

