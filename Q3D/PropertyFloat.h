#pragma once

#include <QWidget>
#include "ui_PropertyFloat.h"
#include <QLabel>
#include <QDoubleSpinBox>
class PropertyFloat : public QWidget
{
	Q_OBJECT

public:
	PropertyFloat(const QString& label, double minValue, double maxValue, double interval, QWidget* parent = nullptr);
	~PropertyFloat();
    void setValue(double value);
    double getValue() const;

signals:
    void valueChanged(double value);

private slots:
    void onSpinBoxChanged();

private:
    void setupSpinBox();

    Ui::PropertyFloatClass ui;
    QLabel* m_label;
    QDoubleSpinBox* m_spinBox;
    double m_minValue;
    double m_maxValue;
    double m_interval;
};

