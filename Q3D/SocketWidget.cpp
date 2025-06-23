#include "SocketWidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QStyle>

SocketWidget::SocketWidget(LNode* parentNode, SocketDirection dir, SocketType type, int portIndex, QWidget* parent)
    : QWidget(parent),
    m_parentNode(parentNode),
    m_direction(dir),
    m_type(type),
    m_portIndex(portIndex)
{
}

void SocketWidget::paintEvent(QPaintEvent* event) {
    // This paint implementation ensures that the widget's stylesheet is correctly rendered.
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}