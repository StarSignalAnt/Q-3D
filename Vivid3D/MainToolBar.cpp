#include "MainToolBar.h"
#include <QIcon>
#include <QSize>
#include <QStyle>
#include <QDebug>
#include "SceneView.h"
#include <QComboBox>

MainToolBar::MainToolBar(QWidget *parent)
	: QToolBar(parent)
{
    setIconSize(QSize(24, 24)); // Size of icons on buttons
    setMinimumHeight(32);
    setupActions();
}

MainToolBar::~MainToolBar()
{}


void MainToolBar::setupActions()
{
    QAction* translateAction = addAction(QIcon("edit/icons/translate.png"), "");
    QAction* rotateAction = addAction(QIcon("edit/icons/rotate.png"), "");
    QAction* scaleAction = addAction(QIcon("edit/icons/scale.png"), "");
    connect(translateAction, &QAction::triggered, this, &MainToolBar::onTranslateClicked);
    connect(rotateAction, &QAction::triggered, this, &MainToolBar::onRotateClicked);
    connect(scaleAction, &QAction::triggered, this, &MainToolBar::onScaleClicked);

    auto dropdown = new QComboBox(this);

    // Add the three options
    dropdown->addItem("Local");
    dropdown->addItem("World");
    dropdown->addItem("Smart");

    // Set default selection (optional)
    dropdown->setCurrentText("Local");

    addSeparator();

    // Add the dropdown to the toolbar
    addWidget(dropdown);

    addSeparator();

    QAction* runAction = addAction(QIcon("edit/icons/runicon.png"), "");
    QAction* stopAction = addAction(QIcon("edit/icons/stopicon.png"), "");


    connect(runAction, &QAction::triggered, this, &MainToolBar::onRunClicked);
    connect(stopAction, &QAction::triggered, this, &MainToolBar::onStopClicked);
    // Connect to handle selection changes
    //connect(dropdown, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
     //   this, &MainWindow::onDropdownChanged);

    // You can also connect using the index-based signal
    connect(dropdown, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &MainToolBar::onDropdownIndexChanged);


}

void MainToolBar::onDropdownIndexChanged(int index)
{
    switch (index) {
    case 0:
        SceneView::m_Instance->SetSpace(SceneSpace::Space_Local);
        break;
    case 1:
        SceneView::m_Instance->SetSpace(SceneSpace::Space_World);
        break;
    case 2:
        SceneView::m_Instance->SetSpace(SceneSpace::Space_Smart);
        break;
    }

    qDebug() << "Selected index:" << index;
    // You can use index-based handling if preferred
    // 0 = Local, 1 = World, 2 = Smart
}

void MainToolBar::onTranslateClicked() {
    qDebug() << "Select tool activated";
    SceneView::m_Instance->SetMode(SceneMode::Mode_Translate);
    // handle select mode activation here
}

void MainToolBar::onRotateClicked() {
    qDebug() << "Move tool activated";
    SceneView::m_Instance->SetMode(SceneMode::Mode_Rotate);
    // handle move mode activation here
}

void MainToolBar::onScaleClicked() {
    qDebug() << "Rotate tool activated";
    SceneView::m_Instance->SetMode(SceneMode::Mode_Scale);
    // handle rotate mode activation here
}

void MainToolBar::onRunClicked() {

    SceneView::m_Instance->Run();

}

void MainToolBar::onStopClicked() {

    SceneView::m_Instance->Stop();

}