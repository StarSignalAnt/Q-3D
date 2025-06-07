#include "PropertyStringList.h"
#include <QHBoxLayout>

PropertyStringList::PropertyStringList(const QString& label, const QStringList& options, QWidget* parent)
    : QWidget(parent), m_options(options)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // Create and configure label
    m_label = new QLabel(label + ":");
    m_label->setMinimumWidth(80);
    m_label->setMaximumWidth(80);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create combo box
    m_comboBox = new QComboBox;
    setupComboBox();

    // Add widgets to layout
    layout->addWidget(m_label);
    layout->addWidget(m_comboBox, 1); // Expand to fill available space

    // Set fixed height for consistency
    setFixedHeight(32);
}

PropertyStringList::~PropertyStringList()
{
}

void PropertyStringList::setupComboBox()
{
    m_comboBox->addItems(m_options);
    m_comboBox->setMinimumWidth(80);

    // Set size policy to allow horizontal expansion
    m_comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Connect value changed signal
    connect(m_comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &PropertyStringList::onComboBoxChanged);
}

void PropertyStringList::setCurrentText(const QString& text)
{
    // Block signals to prevent unnecessary emissions
    m_comboBox->blockSignals(true);

    int index = m_comboBox->findText(text);
    if (index != -1) {
        m_comboBox->setCurrentIndex(index);
    }

    m_comboBox->blockSignals(false);
}

void PropertyStringList::setCurrentIndex(int index)
{
    if (index >= 0 && index < m_comboBox->count()) {
        // Block signals to prevent unnecessary emissions
        m_comboBox->blockSignals(true);
        m_comboBox->setCurrentIndex(index);
        m_comboBox->blockSignals(false);
    }
}

QString PropertyStringList::getCurrentText() const
{
    return m_comboBox->currentText();
}

int PropertyStringList::getCurrentIndex() const
{
    return m_comboBox->currentIndex();
}

void PropertyStringList::setOptions(const QStringList& options)
{
    m_options = options;

    // Store current selection
    QString currentText = m_comboBox->currentText();

    // Block signals and update items
    m_comboBox->blockSignals(true);
    m_comboBox->clear();
    m_comboBox->addItems(m_options);

    // Restore selection if possible
    int index = m_comboBox->findText(currentText);
    if (index != -1) {
        m_comboBox->setCurrentIndex(index);
    }

    m_comboBox->blockSignals(false);
}

QStringList PropertyStringList::getOptions() const
{
    return m_options;
}

void PropertyStringList::onComboBoxChanged()
{
    emit valueChanged(getCurrentText());
    emit indexChanged(getCurrentIndex());
}