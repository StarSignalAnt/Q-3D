#include "LGNode.h"
#include <QPainter>

QVariant LGNode::itemChange(GraphicsItemChange change, const QVariant& value) {
    // If the node's position has changed...
    if (change == ItemPositionHasChanged && scene()) {
        // ...find all SocketWidgets that are children of this node's widget.
        QList<SocketWidget*> sockets = m_proxy->widget()->findChildren<SocketWidget*>();
        for (SocketWidget* socket : sockets) {
            // For each socket, get its list of connections...
            for (ConnectionItem* conn : socket->getConnections()) {
                // ...and tell each connection to update its path.
                conn->updatePath();
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

LGNode::LGNode(LNode* logicNode)
    : m_logicNode(logicNode)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    m_proxy = new QGraphicsProxyWidget(this);
    m_proxy->setWidget(new LGNodeWidget(logicNode));
    m_bounds = m_proxy->boundingRect();
}

QRectF LGNode::boundingRect() const {
    return m_bounds;
}

void LGNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    if (isSelected()) {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QColor("#00A1FF"), 2));
        painter->drawRoundedRect(boundingRect().adjusted(-2, -2, 2, 2), 10, 10);
    }
}


SocketWidget* LGNode::getSocket(SocketWidget::SocketDirection dir, SocketWidget::SocketType type, int portIndex)
{
    QList<SocketWidget*> sockets = m_proxy->widget()->findChildren<SocketWidget*>();
    for (SocketWidget* socket : sockets) {
        if (socket->getDirection() == dir && socket->getType() == type && socket->getPortIndex() == portIndex) {
            return socket;
        }
    }
    return nullptr;
}