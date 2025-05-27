#include "VColorPreview.h"


VColorPreview::VColorPreview(QWidget *parent)
	: QLabel(parent)
{
	ui.setupUi(this);
	setAutoFillBackground(true);
	//setMinimumWidth(8);
	//setMinimumHeight(8);
	setFixedSize(16, 16);

}

void VColorPreview::SetColor(float4 color) {

	QColor dif_col;


	dif_col.setRgb(color.x * 255.0f, color.y * 255.0f, color.z * 255.05, 255);
	QPalette pal = palette();
	pal.setColor(QPalette::Window, dif_col);
	setPalette(pal);
	

}

VColorPreview::~VColorPreview()
{}
