#pragma once
#include <QMenuBar>
#include <QMenu>
#include <QAction>

class LogicGraphMenu : public QMenuBar
{
	Q_OBJECT

public:
	LogicGraphMenu(QWidget* parent = nullptr);
	~LogicGraphMenu();


private:
	void setupMenus();
};

