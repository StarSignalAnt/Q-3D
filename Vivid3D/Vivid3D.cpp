#include "Vivid3D.h"
#include <QDockWidget>
#include "MainToolBar.h"
#include "MainMenu.h"
#include "NodeTree.h"
Vivid3D::Vivid3D(QWidget *parent)
    : QMainWindow(parent)
{
  //  ui.setupUi(this);
    resize(1024, 768);
    setWindowTitle("Vivid3D - (c)Star Signal Games");
    setWindowIcon(QIcon("edit/app/v4.png"));


    setMenuBar(new MainMenu(this)); // Same as setMenuBar(menuBarPtr)
    auto toolbar = new MainToolBar(this);
    addToolBar(Qt::TopToolBarArea, toolbar);

    NodeTree* nodeTree = new NodeTree(this);
    QDockWidget* treeDock = new QDockWidget("Scene Graph", this);
    treeDock->setWidget(nodeTree);
    treeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    SceneView* sceneView = new SceneView(this);
    setCentralWidget(sceneView);

    PropertiesEditor* props = new PropertiesEditor(this);
    QDockWidget* dock = new QDockWidget("Properties", this);
    dock->setWidget(props);
    dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

  

    setStyleSheet("background-color: #202020;");
    // Add it to the right
    addDockWidget(Qt::RightDockWidgetArea, dock);
    addDockWidget(Qt::LeftDockWidgetArea, treeDock);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#502020"));
    setAutoFillBackground(true);
    setPalette(pal);

}

Vivid3D::~Vivid3D()
{}

