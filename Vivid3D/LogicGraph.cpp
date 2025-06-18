#include "LogicGraph.h"
#include <qboxlayout.h>
#include <qscrollarea.h>
#include <qpushbutton.h> // Include for QPushButton
#include "LGProperties.h"
#include "LGDesigner.h"

LogicGraph::LogicGraph(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setWindowTitle("Vivid3D - LogicGraph");
    resize(1280, 720);

    // --- Create Toolbar ---
    QWidget* toolbar = new QWidget(this);
    toolbar->setFixedHeight(32);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(5, 0, 5, 0);
    toolbarLayout->setSpacing(5);

    QPushButton* newButton = new QPushButton("New");
    QPushButton* saveButton = new QPushButton("Save");
    QPushButton* loadButton = new QPushButton("Load");
    toolbarLayout->addWidget(newButton);
    toolbarLayout->addWidget(saveButton);
    toolbarLayout->addWidget(loadButton);
    toolbarLayout->addStretch();

    // --- Create Main Content ---
    m_Splitter = new QSplitter(Qt::Horizontal, this);
    m_Properties = new LGProperties(this);
    m_Designer = new LGDesigner(this);
    // ... splitter setup is the same ...
    m_Splitter->addWidget(m_Properties);
    m_Splitter->addWidget(m_Designer);

    // --- Main Layout ---
    // The main layout is now a QVBoxLayout to hold the toolbar and splitter
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(m_Splitter);
    setLayout(mainLayout);

    // --- Connect Signals ---
    connect(newButton, &QPushButton::clicked, this, &LogicGraph::onNewGraph);
    connect(saveButton, &QPushButton::clicked, this, &LogicGraph::onSaveGraph);
    connect(loadButton, &QPushButton::clicked, this, &LogicGraph::onLoadGraph);

    connect(m_Designer, &LGDesigner::nodeSelected, m_Properties, &LGProperties::inspectNode);
    m_Properties->setGraph(m_Designer->GetGraph());
}

LogicGraph::~LogicGraph()
{
}

// --- SLOTS ---
void LogicGraph::onSaveGraph() {
    if (m_Designer) m_Designer->SaveGraph();
}

void LogicGraph::onLoadGraph() {
    if (m_Designer) {
        m_Designer->LoadGraph();
        m_Properties->setGraph(m_Designer->GetGraph());
    }
}

void LogicGraph::onNewGraph() {
    if (m_Designer) {
        m_Designer->NewGraph();
        m_Properties->setGraph(m_Designer->GetGraph());
    }
}