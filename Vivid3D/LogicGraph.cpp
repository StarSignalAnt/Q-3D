#include "LogicGraph.h"
#include <qboxlayout.h>
#include <qscrollarea.h>
#include "LGProperties.h"
#include "LGDesigner.h"
LogicGraph::LogicGraph(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setWindowTitle("Vivid3D - LogicGraph");
	resize(800, 600);
	m_Splitter = new QSplitter(Qt::Horizontal,this);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0); // Optional: no margins
	layout->addWidget(m_Splitter);

	m_Properties = new LGProperties(this);
	m_Designer = new LGDesigner(this);
	m_Properties->setMinimumWidth(200);
	m_Designer->setMinimumWidth(400);
	QScrollArea* scrollArea = new QScrollArea(this);

	// 2. Configure the scroll area.
	scrollArea->setWidgetResizable(true); // VERY IMPORTANT! Allows the inner widget to be resized.
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // We don't want a horizontal scrollbar.
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setWidget(m_Designer);

	m_Splitter->addWidget(m_Properties);
	m_Splitter->addWidget(scrollArea);

	// Set initial sizes
	m_Splitter->setStretchFactor(0, 1);  // Properties
	m_Splitter->setStretchFactor(1, 3); 
	// Graph area (wider)
	setLayout(layout); // <<< THIS IS CRUCIAL

//	m_Properties->setStyleSheet("background-color: lightgray;");
//	m_Designer->setStyleSheet("background-color: lightblue;");

	QList<int> sizes;
	sizes << 250 << 9999;  // 250 for left, "rest" to right
	m_Splitter->setSizes(sizes);
}

LogicGraph::~LogicGraph()
{}

