#pragma once

#include <QToolBar>
#include <QAction>

class MainToolBar : public QToolBar
{
	Q_OBJECT

public:
	MainToolBar(QWidget *parent = nullptr);
	~MainToolBar();

private:
    void setupActions();

    QAction* translateAction;
    QAction* rotateAction;
    QAction* scaleAction;

private slots:
    void onTranslateClicked();
    void onRotateClicked();
    void onScaleClicked();
    void onDropdownIndexChanged(int index);
};

