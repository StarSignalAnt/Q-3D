#include "Vivid3D.h"
#include <QtWidgets>
#include "VSceneGraph.h"
#include "VContentBrowser.h"
#include "VTools.h"
#include "Editor.h"
#include "VPropertyEditor.h"
#include "VMainMenu.h"
#include "VCinemaEditor.h"
Vivid3D::Vivid3D(QWidget *parent)
    : QMainWindow(parent)
{
   // ui.setupUi(this);
    resize(1624, 950);
    setWindowTitle("Vivid3D - (c)Star Signal Games");
  //  dockScene = new QDockWidget("Scene", this);
  //  SetDockWidget(dockScene);
    QIcon icon("engine/v6.png");

    // Set the window icon
    setWindowIcon(icon);


    mSceneView = new VOutput;

    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("test/emtest1.png"));
    trayIcon->show();

    m_SysTray = trayIcon;
    Editor::m_Tray = trayIcon;


    Editor::Notify("Vivid3D", "Welcome to Vivid3D\nYou may now begin editing your project", 3000);

   

    m_SceneGraph = new QDockWidget("Scene Graph", this);
    m_SceneGraph->setWidget(new VSceneGraph);
    SetDockWidget(m_SceneGraph);
    addDockWidget(Qt::LeftDockWidgetArea, m_SceneGraph);
    m_SceneGraph->setMinimumWidth(300);

    setCentralWidget(mSceneView);

    addToolBar(Qt::TopToolBarArea, new VTools(this));

    m_ContentBrowser = new QDockWidget("Content Browser", this);
    m_ContentBrowser->setWidget(new VContentBrowser);
    SetDockWidget(m_ContentBrowser);
    addDockWidget(Qt::BottomDockWidgetArea, m_ContentBrowser);
    m_ContentBrowser->setMinimumWidth(300);
    m_ContentBrowser->setMinimumHeight(250);

    m_PropEditor = new QDockWidget("Properties", this);
    m_PropEditor->setWidget(new VPropertyEditor);
    SetDockWidget(m_PropEditor);
    addDockWidget(Qt::RightDockWidgetArea, m_PropEditor);
    m_PropEditor->setMinimumWidth(450);
    m_PropEditor->setMinimumHeight(250);


    auto menuBar = new VMainMenu(this);
    // Set the menu bar for the main window
    setMenuBar(menuBar);

    Editor::m_Main = this;


}

void Vivid3D::CreateCinema() {

    auto editor = new QDockWidget("Cinema Editor", this);
//    m_SceneGraph->setWidget(new VSceneGraph);
    SetDockWidget(m_SceneGraph);
    //addDockWidget(Qt::LeftDockWidgetArea, m_SceneGraph);
    tabifyDockWidget(m_ContentBrowser, editor);
    editor->setWidget(new VCinemaEditor);
    m_CinemaEditor = editor;
}

void Vivid3D::SetDockWidget(QDockWidget* dock) {

    dock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

}
Vivid3D::~Vivid3D()
{}
