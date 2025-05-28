#include "PropertiesEditor.h"

PropertiesEditor::PropertiesEditor(QWidget *parent)
	: QWidget(parent)
{
	//ui.setupUi(this);
	setAutoFillBackground(true);
	setWindowTitle("Properties");
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor("#502020"));
	setAutoFillBackground(true);
	setPalette(pal);
}

PropertiesEditor::~PropertiesEditor()
{}


QSize PropertiesEditor::sizeHint() const
{
	return QSize(300, 400);  // Suggested initial width: 300px, height: 400px
}

