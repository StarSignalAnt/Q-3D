#pragma once

#include <QWidget>
#include "ui_PropertyVec3.h"
#include <qspinbox.h>
#include <QLabel>
class PropertyVec3 : public QWidget
{
	Q_OBJECT

public:

	PropertyVec3(const QString& label, QWidget* parent);
	~PropertyVec3();

	void setupSpinBox(QDoubleSpinBox* spinBox);

	void setValue(const QVector3D& value);
	QVector3D getValue() const;

	void setInterval(double interval);

signals:
	void valueChanged(const QVector3D& value);

private slots:
	void onSpinBoxChanged();

private:
	Ui::PropertyVec3Class ui;
	QLabel* m_label;
	QDoubleSpinBox* m_xSpinBox;
	QDoubleSpinBox* m_ySpinBox;
	QDoubleSpinBox* m_zSpinBox;

};

