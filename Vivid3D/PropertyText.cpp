#include "PropertyText.h"
#include <QHBoxLayout>
PropertyText::PropertyText(const QString& label, const QString& defaultText, QWidget* parent)
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

    // Create line edit
    m_lineEdit = new QLineEdit;
    setupLineEdit();

    // Set default text
    m_lineEdit->setText(defaultText);

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_lineEdit, 1); // Expand to fill available space

    // Set fixed height for consistency
    setFixedHeight(32);


}

PropertyText::~PropertyText()
{}



void PropertyText::setupLineEdit()
{
    m_lineEdit->setMinimumWidth(120);
    // Don't set maximum width to allow expansion

    // Set size policy to allow horizontal expansion
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect text changed signal
    connect(m_lineEdit, &QLineEdit::textChanged,
        this, &PropertyText::onTextChanged);
}

void PropertyText::setText(const QString& text)
{
    // Block signals to prevent unnecessary emissions
    m_lineEdit->blockSignals(true);
    m_lineEdit->setText(text);
    m_lineEdit->blockSignals(false);
}

QString PropertyText::getText() const
{
    return m_lineEdit->text();
}

void PropertyText::onTextChanged()
{
    emit textChanged(getText());
}

