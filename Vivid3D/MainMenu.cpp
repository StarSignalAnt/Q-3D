#include "MainMenu.h"

MainMenu::MainMenu(QWidget *parent)
	: QMenuBar(parent)
{
	setupMenus();
	//ui.setupUi(this);
}

MainMenu::~MainMenu()
{}


void MainMenu::setupMenus()
{
    // File menu
    QMenu* fileMenu = addMenu("File");
    QMenu* editMenu = addMenu("Edit");
    QMenu* toolsMenu = addMenu("Tools");

    QAction* newAction = fileMenu->addAction("New Scene");
    QAction* openAction = fileMenu->addAction("Open Scene");
    fileMenu->addSeparator();
    QAction* saveAction = fileMenu->addAction("Save Scene");
    QAction* saveAsAction = fileMenu->addAction("Save Scene As..");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Exit Vivid3D");

    connect(newAction, &QAction::triggered, this, &MainMenu::onNewFile);
    connect(openAction, &QAction::triggered, this, &MainMenu::onOpenFile);
    connect(exitAction, &QAction::triggered, this, &MainMenu::onExit);

    // You can add more menus here: Edit, View, Help, etc.
}

void MainMenu::onNewFile() {
    qDebug() << "New file triggered";
}

void MainMenu::onOpenFile() {
    qDebug() << "Open file triggered";
}

void MainMenu::onExit() {
    qDebug() << "Exit triggered";
    //qApp->quit();
    exit(1);
}