#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QAction>

class MainMenu : public QMenuBar
{
	Q_OBJECT

public:
	MainMenu(QWidget *parent = nullptr);
	~MainMenu();


private:
    void setupMenus();

private slots:
    void onNewFile();
    void onOpenFile();
    void onExit();
    void onSaveScene();

};

