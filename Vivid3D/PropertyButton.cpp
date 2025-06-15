#include "PropertyButton.h"
#include <QHBoxLayout>

PropertyButton::PropertyButton(const QString& buttonText, QWidget* parent)
    : QWidget(parent), m_buttonText(buttonText)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(0);

    // Create button
    m_button = new QPushButton(buttonText);
    setupButton();

    // Add button to layout (full width)
    layout->addWidget(m_button);

    // Set fixed height for consistency
    setFixedHeight(32);
}

void PropertyButton::setupButton()
{
    // Set size policy to fill available space
    m_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect button clicked signal
    connect(m_button, &QPushButton::clicked, this, &PropertyButton::onButtonClicked);
}

void PropertyButton::setButtonText(const QString& text)
{
    m_buttonText = text;
    m_button->setText(text);
}

QString PropertyButton::getButtonText() const
{
    return m_buttonText;
}

void PropertyButton::onButtonClicked()
{
    emit buttonClicked();
}

PropertyButton::~PropertyButton()
{
}