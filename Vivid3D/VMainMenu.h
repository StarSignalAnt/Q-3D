#pragma once

#include <QWidget>
#include "ui_VMainMenu.h"
#include <qmenubar.h>
#include <qmenu.h>
#include <Qaction>

class VMainMenu : public QMenuBar
{
	Q_OBJECT

public:
	VMainMenu(QWidget *parent = nullptr);
	~VMainMenu();

private:
	Ui::VMainMenuClass ui;
};
