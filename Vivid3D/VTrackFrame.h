#pragma once

#include <QWidget>
#include "ui_VTrackFrame.h"
#include <qframe.h>

class Node;
class VTrackTimeLine;

class VTrackFrame : public QFrame
{
	Q_OBJECT

public:
	VTrackFrame(QWidget *parent = nullptr);
	~VTrackFrame();
	VTrackTimeLine* AddTimeLine(Node* node);

private:
	Ui::VTrackFrameClass ui;
};
