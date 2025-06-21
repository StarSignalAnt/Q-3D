#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Projects.h"

// Forward declare the widget classes to keep the header clean
class QSplitter;
class QListWidget;
class QStackedWidget;
class ProjectsPage;

class Projects : public QMainWindow
{
    Q_OBJECT

public:
    Projects(QWidget *parent = nullptr);
    ~Projects();

private:
    QSplitter* m_mainSplitter;
    QListWidget* m_projectList;
    QStackedWidget* m_rightPanelStack;
    ProjectsPage* m_ProjectsPage;
    //Ui::ProjectsClass ui;
};

