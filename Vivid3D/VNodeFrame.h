#pragma once

#include <QWidget>
#include "ui_VNodeFrame.h"
#include "qframe.h"

class VNodeFrame : public QFrame
{
	Q_OBJECT

public:
	VNodeFrame(QWidget *parent = nullptr);
	~VNodeFrame();
protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
private:
	Ui::VNodeFrameClass ui;
	int nodes = 0;
};
