#pragma once

#include <QWidget>
#include "ui_VSettings.h"
#include <QVBoxLayout>
#include <QSplitter>
#include <QTreeWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QFormLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>

class VSettings : public QWidget
{
	Q_OBJECT

public:
	VSettings(QWidget *parent = nullptr);
	~VSettings();

private:
	Ui::VSettingsClass ui;
	QComboBox* m_QTexture, * m_QShader, * m_QShadow;
	QComboBox* m_URenderer;
};
