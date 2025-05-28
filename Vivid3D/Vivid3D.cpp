#include "Vivid3D.h"
#include <QDockWidget>
Vivid3D::Vivid3D(QWidget *parent)
    : QMainWindow(parent)
{
  //  ui.setupUi(this);
    resize(1024, 768);
    setWindowTitle("Vivid3D - (c)Star Signal Games");
    SceneView* sceneView = new SceneView(this);
    setCentralWidget(sceneView);

    PropertiesEditor* props = new PropertiesEditor(this);
    QDockWidget* dock = new QDockWidget("Properties", this);
    dock->setWidget(props);
    dock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

    setStyleSheet("background-color: #202020;");
    // Add it to the right
    addDockWidget(Qt::RightDockWidgetArea, dock);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor("#502020"));
    setAutoFillBackground(true);
    setPalette(pal);
}

Vivid3D::~Vivid3D()
{}

