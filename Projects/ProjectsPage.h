#pragma once

#include <QWidget>
#include "ui_ProjectsPage.h"
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPixmap>
#include <qlineedit.h>
#include <QInputDialog>
#include <qmessagebox.h>
#include <QFrame>
#include <QMap>
#include "QProject.h"
#include "PixelMap.h"
#include <QProcess>


class QListWidget;
class QPushButton;
class QTextEdit;
class QLabel;
class QListWidgetItem;

class ProjectsPage : public QWidget
{
	Q_OBJECT

public:
	ProjectsPage(QWidget *parent = nullptr);
	~ProjectsPage();
private slots:
    void onNewProject();
    void onLoadProject();
    void onDeleteProject();
    void onSetImage();

    void onDescriptionChanged();
private:
    // UI Elements
    void loadProjectsList();
    void saveProjectsList();
    void onDeleteAllProjects();
    void updateUIForSelectedProject(QListWidgetItem* item);
    QPixmap convertPixelMapToPixmap(PixelMap* map);
    bool copyDirectoryRecursively(const QString& sourcePath, const QString& destPath); // <-- ADD THIS

    // --- Data Model ---
    QMap<QString, QProject*> m_projects; // Maps project name to VProject object
    QString m_projectsListPath; // Path to "projects.list"
    QString m_currentIndexPath;
    QString m_newProjectTemplatePath;
    QString m_ideExecutablePath;
    // --- UI Elements ---
    QLabel* m_headerLabel;
    QListWidget* projectList;
    QPushButton* newButton;
    QPushButton* loadButton;
    QPushButton* deleteButton;
    QPushButton* setImageButton;
    QPushButton* deleteAllButton;
    QTextEdit* descriptionEdit;
    QLabel* imageLabel;
//	Ui::ProjectsPageClass ui;
};

