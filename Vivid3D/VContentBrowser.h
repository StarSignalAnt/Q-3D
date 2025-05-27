#pragma once

#include <QWidget>
#include "ui_VContentBrowser.h"

class VContentView;

class VContentBrowser : public QWidget
{
	Q_OBJECT

public:
	VContentBrowser(QWidget *parent = nullptr);
	~VContentBrowser();

private:
	Ui::VContentBrowserClass ui;
	VContentView* m_View = nullptr;
};
