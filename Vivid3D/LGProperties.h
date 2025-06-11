#pragma once

#include <QWidget>
#include "ui_LGProperties.h"

class LGProperties : public QWidget
{
	Q_OBJECT

public:
	LGProperties(QWidget *parent = nullptr);
	~LGProperties();

private:
	Ui::LGPropertiesClass ui;
};

