#include "LogicGraphMenu.h"
LogicGraphMenu::LogicGraphMenu(QWidget* parent)
	: QMenuBar(parent)
{

	setupMenus();
	addAction("Testing");
	addMenu("File");
	//ui.setupUi(this);
}

LogicGraphMenu::~LogicGraphMenu()
{
}


void LogicGraphMenu::setupMenus()
{
}