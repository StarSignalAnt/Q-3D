#include "VCinemaEditor.h"
#include "VCinemaTimeLine.h"
#include "qscrollarea.h"
#include <qboxlayout.h>

VCinemaEditor::VCinemaEditor(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	VCinemaTimeLine* time_line = new VCinemaTimeLine(this);



	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(time_line);
	scrollArea->setWidgetResizable(true);

	auto lo = new QVBoxLayout(this);


	lo->addWidget(scrollArea);

	setLayout(lo);
}

VCinemaEditor::~VCinemaEditor()
{}
