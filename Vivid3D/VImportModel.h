#pragma once

#include <QWidget>
#include "ui_VImportModel.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qspinbox.h"
#include "qcheckbox.h"
#include <string>

class VImportModel : public QWidget
{
	Q_OBJECT

public:
	VImportModel(QWidget *parent = nullptr);
	~VImportModel();
	void Set(std::string path);

private:
	Ui::VImportModelClass ui;
	QLineEdit* m_ModelName;
	QDoubleSpinBox* m_Scale;
	QCheckBox* m_ImportActor;
	std::string m_Path;
};
