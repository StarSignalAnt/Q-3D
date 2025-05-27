#pragma once

#include <QWidget>
#include "ui_VCinemaEditor.h"

class VCinemaEditor : public QWidget
{
	Q_OBJECT

public:
	VCinemaEditor(QWidget *parent = nullptr);
	~VCinemaEditor();

private:
	Ui::VCinemaEditorClass ui;
};
