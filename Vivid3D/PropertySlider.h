#pragma once

#include <QWidget>
#include "ui_PropertySlider.h"
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
class PropertySlider : public QWidget
{
	Q_OBJECT

public:
	PropertySlider(const QString& label, int minValue, int maxValue, QWidget* parent = nullptr);
	~PropertySlider();
    void setValue(int value);
    int getValue() const;

signals:
    void valueChanged(int value);

private slots:
    void onSliderChanged();
    void onSpinBoxChanged();

private:
    void setupSlider();
    void setupSpinBox();

    Ui::PropertySliderClass ui;
    QLabel* m_label;
    QSlider* m_slider;
    QSpinBox* m_spinBox;
    int m_minValue;
    int m_maxValue;
};

