#include "PropertyFloat.h"
#include <QHBoxLayout>
PropertyFloat::PropertyFloat(const QString& label, double minValue, double maxValue, double interval, QWidget* parent)
	: QWidget(parent), m_minValue(minValue), m_maxValue(maxValue), m_interval(interval)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // Create and configure label
    m_label = new QLabel(label + ":");
    m_label->setMinimumWidth(80);
    m_label->setMaximumWidth(80);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create spinbox
    m_spinBox = new QDoubleSpinBox;
    setupSpinBox();


    // Set initial value to minimum
    setValue(m_minValue);

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_spinBox, 1); // Expand to fill available space

    // Set fixed height for consistency
    setFixedHeight(32);
}
void PropertyFloat::setupSpinBox()
{
    m_spinBox->setRange(m_minValue, m_maxValue);
    m_spinBox->setSingleStep(m_interval);

    // Set appropriate decimal places based on interval
    int decimals = 3; // Default
    if (m_interval >= 1.0) {
        decimals = 0;
    }
    else if (m_interval >= 0.1) {
        decimals = 1;
    }
    else if (m_interval >= 0.01) {
        decimals = 2;
    }
    else if (m_interval >= 0.001) {
        decimals = 3;
    }
    else {
        decimals = 4;
    }
    m_spinBox->setDecimals(decimals);

    m_spinBox->setMinimumWidth(80);
    // Don't set maximum width to allow expansion

    // Set size policy to allow horizontal expansion
    m_spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(m_spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &PropertyFloat::onSpinBoxChanged);
}

void PropertyFloat::setValue(double value)
{
    // Clamp value to range
    value = qBound(m_minValue, value, m_maxValue);

    // Block signals to prevent unnecessary emissions
    m_spinBox->blockSignals(true);
    m_spinBox->setValue(value);
    m_spinBox->blockSignals(false);
}

double PropertyFloat::getValue() const
{
    return m_spinBox->value();
}

void PropertyFloat::onSpinBoxChanged()
{
    emit valueChanged(getValue());
}

PropertyFloat::~PropertyFloat()
{
}

