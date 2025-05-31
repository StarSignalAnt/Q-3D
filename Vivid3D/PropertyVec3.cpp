#include "PropertyVec3.h"
#include <QHBoxLayout>
#include <qvector3d.h>
PropertyVec3::PropertyVec3(const QString& label, QWidget* parent)
	: QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // Create and configure label
    m_label = new QLabel(label + ":");
    m_label->setMinimumWidth(80);
    m_label->setMaximumWidth(80);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create spinboxes
    m_xSpinBox = new QDoubleSpinBox;
    m_ySpinBox = new QDoubleSpinBox;
    m_zSpinBox = new QDoubleSpinBox;

    // Configure all spinboxes
    setupSpinBox(m_xSpinBox);
    setupSpinBox(m_ySpinBox);
    setupSpinBox(m_zSpinBox);

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_xSpinBox, 1); // The '1' means it will expand with stretch factor 1
    layout->addWidget(m_ySpinBox, 1); // All three spinboxes get equal stretch
    layout->addWidget(m_zSpinBox, 1);

    // Set fixed height for consistency
    setFixedHeight(32);
}

void PropertyVec3::setupSpinBox(QDoubleSpinBox* spinBox)
{
    spinBox->setRange(-99999.999, 99999.999);
    spinBox->setDecimals(3);
    spinBox->setSingleStep(0.1);
    spinBox->setMinimumWidth(60);
    // REMOVE setMaximumWidth to allow expansion:
    // spinBox->setMaximumWidth(80);

    // Set size policy to allow horizontal expansion
    spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &PropertyVec3::onSpinBoxChanged);
}

void PropertyVec3::setValue(const QVector3D& value)
{
    // Block signals to prevent unnecessary emissions
    m_xSpinBox->blockSignals(true);
    m_ySpinBox->blockSignals(true);
    m_zSpinBox->blockSignals(true);

    m_xSpinBox->setValue(value.x());
    m_ySpinBox->setValue(value.y());
    m_zSpinBox->setValue(value.z());

    m_xSpinBox->blockSignals(false);
    m_ySpinBox->blockSignals(false);
    m_zSpinBox->blockSignals(false);
}

QVector3D PropertyVec3::getValue() const
{
    return QVector3D(
        static_cast<float>(m_xSpinBox->value()),
        static_cast<float>(m_ySpinBox->value()),
        static_cast<float>(m_zSpinBox->value())
    );
}

void PropertyVec3::onSpinBoxChanged()
{
    emit valueChanged(getValue());
}


PropertyVec3::~PropertyVec3()
{}

