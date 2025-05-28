#pragma once

#include <QWidget>
#include "ui_PropertiesEditor.h"

class PropertiesEditor : public QWidget
{
	Q_OBJECT

public:
	PropertiesEditor(QWidget *parent = nullptr);
	~PropertiesEditor();
	QSize sizeHint() const override;

private:
	Ui::PropertiesEditorClass ui;
};

