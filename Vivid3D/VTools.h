#pragma once

#include <QWidget>
#include "ui_VTools.h"
#include <qtoolbar.h>
#include <qcombobox.h>

class VTools : public QToolBar
{
	Q_OBJECT

public:
	VTools(QWidget *parent = nullptr);
	~VTools();
	void Update();
	static VTools* m_This;

private:
	Ui::VToolsClass ui;
	QComboBox* m_GameCamera;
};
