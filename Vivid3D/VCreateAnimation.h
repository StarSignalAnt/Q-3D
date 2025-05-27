#pragma once

#include <QDialog>
#include "ui_VCreateAnimation.h"
#include "qspinbox.h"

class VCreateAnimation : public QDialog
{
	Q_OBJECT

public:
	VCreateAnimation(QWidget *parent = nullptr);
	~VCreateAnimation();

private:
	Ui::VCreateAnimationClass ui;
	QLineEdit* m_Name;
	QSpinBox* m_Len;
};
