#pragma once

#include <QWidget>
#include "ui_LGDesigner.h"

class LGDesigner : public QWidget
{
	Q_OBJECT

public:
	LGDesigner(QWidget *parent = nullptr);
	~LGDesigner();
protected:


	void paintEvent(QPaintEvent* ev) override;

private:
	Ui::LGDesignerClass ui;
};

