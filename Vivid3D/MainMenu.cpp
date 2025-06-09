#include "MainMenu.h"
#include <QFileDialog>
#include <QString>
#include "Content.h"
#include "SceneView.h"
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
    QMenu* fileMenu = addMenu("Project");
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
    connect(saveAction, &QAction::triggered, this, &MainMenu::onSaveScene);

    // You can add more menus here: Edit, View, Help, etc.
}

void MainMenu::onNewFile() {
    qDebug() << "New file triggered";
}

void MainMenu::onOpenFile() {

    QString filePath = QFileDialog::getOpenFileName(
        this,                               // parent widget (e.g., QMainWindow*)
        "Open Scene",                       // dialog title
        Content::m_Instance->GetPath().c_str(),                   // default path
        "Scene Files (*.scene);;All Files (*)"  // filter
    );

    if (!filePath.isEmpty()) {
        // use filePath
        SceneGraph* new_Graph = new SceneGraph;
        new_Graph->LoadScene(filePath.toStdString());
        SceneView::m_Instance->SetScene(new_Graph);
    }

    qDebug() << "Open file triggered";
}

void MainMenu::onExit() {
    qDebug() << "Exit triggered";
    //qApp->quit();
    exit(1);
}

void MainMenu::onSaveScene() {

    QString filter = "Scene Files (*.scene)";
    QString defaultFileName = "untitled.scene";

    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "Save Scene",
        Content::m_Instance->GetPath().c_str(),
        filter
    );

    if (!fileName.isEmpty()) {
        // Ensure the file ends with .scene
        if (!fileName.endsWith(".scene", Qt::CaseInsensitive)) {
            fileName += ".scene";
        }

        SceneView::m_Instance->GetScene()->SaveScene(fileName.toStdString());

        qDebug() << "Selected file to save:" << fileName;
        // Save logic goes here...
    }

}