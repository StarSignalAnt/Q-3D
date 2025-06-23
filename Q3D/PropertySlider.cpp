#include "PropertySlider.h"
#include <QHBoxLayout>

PropertySlider::PropertySlider(const QString& label, int minValue, int maxValue, QWidget* parent)
	: QWidget(parent), m_minValue(minValue), m_maxValue(maxValue)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // Create and configure label
    m_label = new QLabel(label + ":");
    m_label->setMinimumWidth(80);
    m_label->setMaximumWidth(80);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create slider and spinbox
    m_slider = new QSlider(Qt::Horizontal);
    m_spinBox = new QSpinBox;

    // Configure slider and spinbox
    setupSlider();
    setupSpinBox();

    // Set initial value to minimum
    setValue(m_minValue);

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_slider, 1); // Slider takes most of the space
    layout->addWidget(m_spinBox);   // SpinBox stays compact

    // Set fixed height for consistency
    setFixedHeight(32);
}

PropertySlider::~PropertySlider()
{}


void PropertySlider::setupSlider()
{
    m_slider->setRange(m_minValue, m_maxValue);
    m_slider->setMinimumWidth(80);
    // Don't set maximum width to allow expansion

    // Set size policy to allow horizontal expansion
    m_slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(m_slider, &QSlider::valueChanged,
        this, &PropertySlider::onSliderChanged);
}

void PropertySlider::setupSpinBox()
{
    m_spinBox->setRange(m_minValue, m_maxValue);
    m_spinBox->setMinimumWidth(60);
    m_spinBox->setMaximumWidth(80);

    // Set size policy to stay fixed width
    m_spinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &PropertySlider::onSpinBoxChanged);
}

void PropertySlider::setValue(int value)
{
    // Clamp value to range
    value = qBound(m_minValue, value, m_maxValue);

    // Block signals to prevent unnecessary emissions
    m_slider->blockSignals(true);
    m_spinBox->blockSignals(true);

    m_slider->setValue(value);
    m_spinBox->setValue(value);

    m_slider->blockSignals(false);
    m_spinBox->blockSignals(false);
}

int PropertySlider::getValue() const
{
    return m_slider->value();
}

void PropertySlider::onSliderChanged()
{
    // Update spinbox to match slider (without triggering its signal)
    m_spinBox->blockSignals(true);
    m_spinBox->setValue(m_slider->value());
    m_spinBox->blockSignals(false);

    emit valueChanged(getValue());
}

void PropertySlider::onSpinBoxChanged()
{
    // Update slider to match spinbox (without triggering its signal)
    m_slider->blockSignals(true);
    m_slider->setValue(m_spinBox->value());
    m_slider->blockSignals(false);

    emit valueChanged(getValue());
}
