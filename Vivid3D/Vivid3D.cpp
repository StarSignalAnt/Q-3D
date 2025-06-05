#include "Vivid3D.h"
#include <QDockWidget>
#include "MainToolBar.h"
#include "MainMenu.h"
#include "ContentBrowser.h"
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

    QScrollArea* scrollArea = new QScrollArea(this);

    // 2. Configure the scroll area.
    scrollArea->setWidgetResizable(true); // VERY IMPORTANT! Allows the inner widget to be resized.
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // We don't want a horizontal scrollbar.
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // This is the key!

    // 3. Create our content browser and place it *inside* the scroll area.
    ContentBrowser* browser = new ContentBrowser();
    scrollArea->setWidget(browser);

    QDockWidget* contentDock = new QDockWidget("Content", this);
    contentDock->setWidget(scrollArea);
    contentDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    //SceneView* sceneView = new SceneView(this);
    //setCentralWidget(sceneView)
        ;
        contentDock->setMinimumHeight(250);

    setStyleSheet("background-color: #202020;");
    // Add it to the right
    addDockWidget(Qt::RightDockWidgetArea, dock);
    addDockWidget(Qt::LeftDockWidgetArea, treeDock);
    addDockWidget(Qt::BottomDockWidgetArea, contentDock);
    QPalette pal = palette();

    pal.setColor(QPalette::Window, QColor("#502020"));
    setAutoFillBackground(true);
    setPalette(pal);

    browser->Browse("c:\\content\\");

}

Vivid3D::~Vivid3D()
{}

