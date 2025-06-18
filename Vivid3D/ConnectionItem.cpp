#include "ConnectionItem.h"
#include "SocketWidget.h"
#include "LGNode.h"
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene> // <-- Required include
#include <QGraphicsView>  // <-- Required include
#include <cmath>          // <-- Required for std::abs

// Private helper function for common setup
void ConnectionItem::initialize() {
    setFlag(QGraphicsItem::ItemIsSelectable);
    m_startSocket = nullptr;
    m_endSocket = nullptr;
    m_color = Qt::black;
    setZValue(-1);
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

// Constructor for temporary line while user is dragging
ConnectionItem::ConnectionItem() {
    initialize();
    m_color = QColor("#F39C12");
    setPen(QPen(m_color, 2));
}

// Constructor for a final, permanent connection
ConnectionItem::ConnectionItem(SocketWidget* startSocket, SocketWidget* endSocket) {
    initialize();
    setStartSocket(startSocket);
    setEndSocket(endSocket);

    // Set color based on socket type
    if (m_startSocket && m_startSocket->getType() == SocketWidget::SocketType::Exec) {
        m_color = Qt::white;
    }
    else {
        m_color = QColor("#95A5A6");
    }
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void ConnectionItem::setStartSocket(SocketWidget* socket) {
    m_startSocket = socket;
}

void ConnectionItem::setEndSocket(SocketWidget* socket) {
    m_endSocket = socket;
}

void ConnectionItem::updateEndPos(const QPointF& pos) {
    m_floatingEndPos = pos;
    updatePath();
}

// Overridden paint function to handle selection highlighting
void ConnectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    if (option->state & QStyle::State_Selected) {
        painter->setPen(QPen(QColor("#00A1FF"), 3.5));
    }
    else {
        painter->setPen(pen());
    }
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}


// --- BEGIN NEW, RECODED updatePath FUNCTION ---
void ConnectionItem::updatePath()
{
    // Safety check: The connection item must be in a scene and have a start socket.
    if (!m_startSocket || !scene()) {
        return;
    }
    QGraphicsView* view = scene()->views().first();
    if (!view) {
        return;
    }

    // 1. Reliably get the start and end positions in scene coordinates.
    QPoint globalStartPos = m_startSocket->mapToGlobal(m_startSocket->rect().center());
    QPointF startPos = view->mapToScene(view->mapFromGlobal(globalStartPos));

    QPointF endPos;
    if (m_endSocket) {
        QPoint globalEndPos = m_endSocket->mapToGlobal(m_endSocket->rect().center());
        endPos = view->mapToScene(view->mapFromGlobal(globalEndPos));
    }
    else {
        endPos = m_floatingEndPos;
    }

    // 2. Ensure the curve direction is always consistent (Out -> In).
    if (m_startSocket->getDirection() == SocketWidget::SocketDirection::In) {
        std::swap(startPos, endPos);
    }

    // 3. Calculate the dynamic horizontal offset for the curve's control points.
    qreal dx = endPos.x() - startPos.x();
    qreal dy = endPos.y() - startPos.y();

    // The "pull" on the curve is half the horizontal distance, but capped at 150 pixels.
    // This makes the curve look nice for both near and far nodes.
    qreal horizontalOffset = std::min(std::abs(dx) * 0.5, 150.0);

    // 4. Define the control points for a smooth S-shaped curve.
    QPointF ctr1(startPos.x() + horizontalOffset, startPos.y());
    QPointF ctr2(endPos.x() - horizontalOffset, endPos.y());

    // 5. Create the final path and set it.
    QPainterPath newPath;
    newPath.moveTo(startPos);
    newPath.cubicTo(ctr1, ctr2, endPos);
    setPath(newPath);
}
// --- END NEW, RECODED updatePath FUNCTION ---