#pragma once

#include <QWidget>
#include "ui_VColorPreview.h"
#include "qlabel.h"
#include "BasicMath.hpp"

using namespace Diligent;

class VColorPreview : public QLabel
{
	Q_OBJECT

public:
	VColorPreview(QWidget *parent = nullptr);
	~VColorPreview();
	void SetColor(float4 color);

private:
	Ui::VColorPreviewClass ui;
};
