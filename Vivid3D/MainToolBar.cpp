#include "MainToolBar.h"
#include <QIcon>
#include <QSize>
#include <QStyle>
#include <QDebug>
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
}

void MainToolBar::onTranslateClicked() {
    qDebug() << "Select tool activated";
    // handle select mode activation here
}

void MainToolBar::onRotateClicked() {
    qDebug() << "Move tool activated";
    // handle move mode activation here
}

void MainToolBar::onScaleClicked() {
    qDebug() << "Rotate tool activated";
    // handle rotate mode activation here
}
