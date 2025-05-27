#include "VTextEditor.h"

VTextEditor::VTextEditor(QWidget *parent)
	: QTextEdit(parent)
{
	ui.setupUi(this);
	QFont f= font();
	f.setPointSize(10);
	setFont(f);
}

VTextEditor::~VTextEditor()
{}
