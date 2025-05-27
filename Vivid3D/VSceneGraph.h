#pragma once

#include <QWidget>
#include "ui_VSceneGraph.h"
#include <string>
#include <vector>
#include <map>

class VTreeView;
class Node;
class TreeItem;


class VSceneGraph : public QWidget
{
	Q_OBJECT

public:
	VSceneGraph(QWidget *parent = nullptr);
	~VSceneGraph();
	void UpdateGraph();
	void SetNode(Node* node);
	void AddNode(TreeItem* item, Node* node);
	

private:
	Ui::VSceneGraphClass ui;
	VTreeView* m_Tree;
	Node* m_CurrentNode = nullptr;
	std::map<Node*, TreeItem*> m_NodeMap;
	bool m_IsPlaying = false;

};
