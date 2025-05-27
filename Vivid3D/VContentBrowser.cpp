#include "VContentBrowser.h"
#include "Editor.h"
#include "VContentView.h"
#include "qscrollarea.h"
//#include <qwidget.h>
#include <QVBoxLayout>

VContentBrowser::VContentBrowser(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	Editor::m_ContentBrowser = this;

	//VTreeView* tree = new VTreeView(this);
	VContentView* view = new VContentView(this);

	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(view);
	scrollArea->setWidgetResizable(true);

	auto lo = new QVBoxLayout(this);



	lo->addWidget(scrollArea);

	setLayout(lo);
	m_View = view;

	view->Browse("c:\\content\\");

	//UpdateGraph();


}

VContentBrowser::~VContentBrowser()
{}
