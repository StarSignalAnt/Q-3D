#pragma once

#include <QDialog>
#include "ui_VCreateScript.h"

class VCreateScript : public QDialog
{
	Q_OBJECT

public:
	VCreateScript(QWidget *parent = nullptr);
	~VCreateScript();
	std::string m_Path;

private:
	Ui::VCreateScriptClass ui;
};
