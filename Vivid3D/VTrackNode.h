#pragma once

#include <QWidget>
#include "ui_VTrackNode.h"

class Node;
class VTrackTimeLine;

class VTrackNode : public QWidget
{
	Q_OBJECT

public:
	VTrackNode(QWidget *parent = nullptr);
	~VTrackNode();
	void SetNode(Node* node);
	void SetTimeLine(VTrackTimeLine* timeline) {
		m_TimeLine = timeline;
	}
protected:
	void paintEvent(QPaintEvent* event) override;

private:
	Ui::VTrackNodeClass ui;
	Node* m_Node = nullptr;
	VTrackTimeLine* m_TimeLine;
};
