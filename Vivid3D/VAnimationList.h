#pragma once

#include <QWidget>
#include <qlistwidget.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <qmimetype.h>
#include "qevent.h"
#include "ui_VAnimationList.h"

class Animation;

class VAnimationList : public QListWidget
{
	Q_OBJECT

public:
	VAnimationList(QWidget *parent = nullptr);
	~VAnimationList();
	void SetAnimations(std::vector<Animation*> anims);
protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;

private:
	Ui::VAnimationListClass ui;
};
