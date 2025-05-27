#pragma once

#include <QWidget>
#include "ui_VTrackTimeLine.h"

class Node;
class CineTrack;

class VTrackTimeLine : public QWidget
{
	Q_OBJECT

public:
	VTrackTimeLine(QWidget *parent = nullptr);
	~VTrackTimeLine();
protected:
	void paintEvent(QPaintEvent* event) override;
public:
	void SetNode(Node* node)
	{
		m_Node = node;
	}
	void SetTrack(CineTrack* track) {
		m_Track = track;
	}
	CineTrack* GetCineTrack() {
		return m_Track;
	}
private:
	Ui::VTrackTimeLineClass ui;
	Node* m_Node = nullptr;
	CineTrack* m_Track = nullptr;
};
