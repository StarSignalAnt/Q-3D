#include "Projects.h"

#include <QSplitter>
#include "ProjectsPage.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QLabel> // Used for creating simple placeholder widgets

Projects::Projects(QWidget *parent)
    : QMainWindow(parent)
{
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // --- 2. Create the left-side list widget ---
    m_projectList = new QListWidget(m_mainSplitter);
    m_projectList->addItem("Projects");
    m_projectList->addItem("Vivid3D SDKs");
    m_projectList->addItem("Build & Run");
    m_projectList->addItem("Team Hub");

    // --- 3. Create the right-side stacked widget ---
    m_rightPanelStack = new QStackedWidget(m_mainSplitter);

    m_ProjectsPage = new ProjectsPage();

    // --- 4. Create placeholder widgets for the stacked widget ---
    // You can replace these QLabels with your own custom widgets
    //QLabel* projectSettingsPage = new Q


    //m_ProjectsPage->setAlignment(Qt::AlignCenter);


    //QLabel* sceneManagerPage = new QLabel("Scene Manager Area", this);
    //sceneManagerPage->setAlignment(Qt::AlignCenter);

   // QLabel* buildAndRunPage = new QLabel("Build & Run Area", this);
   // buildAndRunPage->setAlignment(Qt::AlignCenter);


    // Add the pages to the stacked widget in the same order as the list widget
    m_rightPanelStack->addWidget(m_ProjectsPage);
   // m_rightPanelStack->addWidget(sceneManagerPage);
   // m_rightPanelStack->addWidget(buildAndRunPage);


    // --- 5. Add the list and stack to the splitter ---
    m_mainSplitter->addWidget(m_projectList);
    m_mainSplitter->addWidget(m_rightPanelStack);

    // --- 6. Set the splitter as the central widget ---
    setCentralWidget(m_mainSplitter);

    // --- 7. Connect the list widget selection to the stacked widget page ---
    // This makes it so clicking an item on the left changes the widget on the right
    connect(m_projectList, &QListWidget::currentRowChanged, m_rightPanelStack, &QStackedWidget::setCurrentIndex);

    // --- 8. Final Window Setup ---
    setWindowTitle("Vivid3D Project Manager");
    resize(1024, 768);

    // Set initial splitter sizes (left pane 250px, right pane takes the rest)
    m_mainSplitter->setSizes({ 250, 774 });


    // Ensure the first item is selected by default
    m_projectList->setCurrentRow(0); //   resize(800, 600);
}

Projects::~Projects()
{}

