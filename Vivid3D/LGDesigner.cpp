#include "LGDesigner.h"
#include <qpainter.h>
#include <qboxlayout.h>
#include "LogicGraphMenu.h"
LGDesigner::LGDesigner(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	auto menu = new LogicGraphMenu(this);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	// Menu bar
	LogicGraphMenu* menuBar = new LogicGraphMenu(this);

	mainLayout->addWidget(menuBar);

	setLayout
	(mainLayout);

}

LGDesigner::~LGDesigner()
{}

void LGDesigner::paintEvent(QPaintEvent* event)
{
	//QPainter painter(this);
	//painter.fillRect(rect(), QColor(173, 216, 230)); // Light blue (RGB)
	
}