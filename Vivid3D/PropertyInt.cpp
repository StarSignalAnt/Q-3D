#include "PropertyInt.h"
#include <QHBoxLayout>
PropertyInt::PropertyInt(const QString& label, int minValue, int maxValue, int interval, QWidget* parent)
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
    m_spinBox = new QSpinBox;
    setupSpinBox();


    // Set initial value to minimum
    setValue(m_minValue);

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_spinBox, 1); // Expand to fill available space

    // Set fixed height for consistency
    setFixedHeight(32);
}
void PropertyInt::setupSpinBox()
{
    m_spinBox->setRange(m_minValue, m_maxValue);
    m_spinBox->setSingleStep(m_interval);



    m_spinBox->setMinimumWidth(80);
    // Don't set maximum width to allow expansion

    // Set size policy to allow horizontal expansion
    m_spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(m_spinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &PropertyInt::onSpinBoxChanged);
}

void PropertyInt::setValue(int value)
{
    // Clamp value to range
    value = qBound(m_minValue, value, m_maxValue);

    // Block signals to prevent unnecessary emissions
    m_spinBox->blockSignals(true);
    m_spinBox->setValue(value);
    m_spinBox->blockSignals(false);
}

int PropertyInt::getValue() const
{
    return m_spinBox->value();
}

void PropertyInt::onSpinBoxChanged()
{
    emit valueChanged(getValue());
}

PropertyInt::~PropertyInt()
{
}

