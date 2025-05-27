#include "pch.h"
#include "SceneOctree.h"
#include "SceneGraph.h"
#include "Bounds.h"
#include "OctNode.h"


SceneOctree::SceneOctree(SceneGraph *graph) {

	auto size = graph->GetBounds();

	m_Root = new OctNode(graph,size.Centre, size.Size());
	//m_Root->subdivide();
	m_Root->Process(graph);
	m_Graph = graph;
	
	//m_Root->subdivide(0);


	m_Root->CreateLines(graph);

	int b = 0;
}

int countLeafs(OctNode* node,int c=0) {

	if (node == nullptr) {
		return c;
	}
	if (node->IsLeaf()) {
		c++;
	}

	for (auto sn : node->GetChildren())
	{
		c = countLeafs(sn, c);
	}

	return c;
}

int SceneOctree::LeafCount() {

	return countLeafs(m_Root);

}

void SceneOctree::RenderBF() {

	OctNode::m_NodesRendered = 0;



		m_Root->RenderBF(false,0);
	

	//printf("Nodes Rendered:%d\n", OctNode::m_NodesRendered);
}