#include "VSceneGraph.h"
#include "VTreeView.h"
#include <QVBoxLayout>
#include <qscrollarea.h>
#include "Node.h"
#include "Editor.h"
#include "SceneGraph.h"
VSceneGraph::VSceneGraph(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	Editor::m_SceneGraph = this;

	VTreeView* tree = new VTreeView(this);

	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(tree);
	scrollArea->setWidgetResizable(true);

	auto lo = new QVBoxLayout(this);



	lo->addWidget(scrollArea);

	setLayout(lo);
	m_Tree = tree;

	UpdateGraph();


}

void VSceneGraph::AddNode(TreeItem* item, Node* node) {

	item->m_Text = node->GetName();
	item->m_Data = (void*)node;
	
	m_NodeMap[node] = item;
	
	for (auto sub : node->GetNodes()) {

		TreeItem* new_item = new TreeItem;
		AddNode(new_item, sub);
		item->m_Items.push_back(new_item);

	}

}

void VSceneGraph::UpdateGraph() {

	m_NodeMap.clear();

	auto item = new TreeItem;
	item->m_Text = "Scene Root";

	AddNode(item, Editor::m_Graph->GetRoot());

	m_Tree->SetRoot(item);
	update();
	m_Tree->update();

}

void VSceneGraph::SetNode(Node* node) {

	m_CurrentNode = node;
	//Up
	if (node == nullptr) {
		m_Tree->SetActive(nullptr);
		//repaint();
		update();
		m_Tree->update();
		return;
	}
	auto item = m_NodeMap[node];
	m_Tree->SetActive(item);
//	repaint();
	update();
	m_Tree->update();

}

VSceneGraph::~VSceneGraph()
{}
