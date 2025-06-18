#include "ConnectionItem.h"
#include "SocketWidget.h"
#include <QPen>
#include <QGraphicsScene>
#include <QGraphicsView>

// Private helper function for common setup
void ConnectionItem::initialize() {
    m_startSocket = nullptr;
    m_endSocket = nullptr;
    m_color = Qt::black;
    setZValue(-1); // Draw connections behind nodes
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

// Constructor for temporary line while user is dragging
ConnectionItem::ConnectionItem() {
    initialize();
    m_color = QColor("#F39C12"); // Bright orange for temporary connections
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
        m_color = QColor("#95A5A6"); // A neutral gray for data connections
    }
    setPen(QPen(m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void ConnectionItem::setStartSocket(SocketWidget* socket) {
    m_startSocket = socket;
}

void ConnectionItem::setEndSocket(SocketWidget* socket) {
    m_endSocket = socket;
}

// Used to update the line while it's being dragged
void ConnectionItem::updateEndPos(const QPointF& pos) {
    m_floatingEndPos = pos;
    updatePath();
}

// Recalculates and redraws the bezier curve
void ConnectionItem::updatePath() {
    // Safety Check: The connection item must be in a scene and have a start socket.
    if (!m_startSocket || !scene()) {
        return;
    }

    QGraphicsView* view = scene()->views().first();
    if (!view) {
        return;
    }

    // --- COORDINATE CALCULATION (Corrected) ---
    // This is the reliable method to get the socket's center in scene coordinates.
    QPoint globalStartPos = m_startSocket->mapToGlobal(m_startSocket->rect().center());
    QPointF startPos = view->mapToScene(view->mapFromGlobal(globalStartPos));

    QPointF endPos;
    if (m_endSocket) {
        // For a permanent connection, get the end socket's position.
        QPoint globalEndPos = m_endSocket->mapToGlobal(m_endSocket->rect().center());
        endPos = view->mapToScene(view->mapFromGlobal(globalEndPos));
    }
    else {
        // For a temporary connection, use the floating mouse position.
        endPos = m_floatingEndPos;
    }

    // Ensure the curve direction is always consistent (Out -> In) for a uniform look.
    if (m_startSocket->getDirection() == SocketWidget::SocketDirection::In) {
        std::swap(startPos, endPos);
    }

    // --- PATH DRAWING ---
    QPainterPath p;
    p.moveTo(startPos);
    qreal dx = endPos.x() - startPos.x();
    QPointF ctr1(startPos.x() + dx * 0.5, startPos.y());
    QPointF ctr2(endPos.x() - dx * 0.5, endPos.y());
    p.cubicTo(ctr1, ctr2, endPos);
    setPath(p);
}