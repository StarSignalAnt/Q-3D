#include "PropertyHeader.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>

PropertyHeader::PropertyHeader(const QString& text, QWidget* parent)
	: QWidget(parent), m_text(text)
{
    setFixedHeight(26);
}

PropertyHeader::~PropertyHeader()
{}

void PropertyHeader::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Create multi-stop gradient for smoother transition
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, QColor(50, 50, 50));    // Darkest
    gradient.setColorAt(0.3, QColor(80, 80, 80));    // Dark gray
    gradient.setColorAt(0.7, QColor(110, 110, 110)); // Light gray
    gradient.setColorAt(1.0, QColor(140, 140, 140)); // Lightest

    // Draw gradient background
    painter.fillRect(rect(), gradient);

    // Set up font for text
    QFont font = painter.font();
    font.setPointSize(font.pointSize() + 2);
    font.setBold(true);
    painter.setFont(font);

    // Set text color to white for contrast
    painter.setPen(QColor(255, 255, 255));

    // Draw text with 28px left margin (20px more than the original 8px)
    QRect textRect = rect().adjusted(5, 0, 0, 0);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, m_text);

    // Call base class paint event
    QWidget::paintEvent(event);
}


