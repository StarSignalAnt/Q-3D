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
        if (m_logicNode) {
            const QPointF newPos = value.toPointF();
            m_logicNode->SetEditorPosition({ (float)newPos.x(), (float)newPos.y() });
        }
        QList<LGNode*> allNodesInScene;
        for (QGraphicsItem* item : scene()->items()) {
            if (auto* node = qgraphicsitem_cast<LGNode*>(item)) {
                allNodesInScene.append(node);
            }
        }

        // Now, build a set of just the nodes that are overlapping another node.
        QSet<LGNode*> nodesInCollision;
        for (LGNode* node : allNodesInScene) {
            // Get all items this node is colliding with.
            const QList<QGraphicsItem*> collisions = node->collidingItems();
            for (QGraphicsItem* collidingItem : collisions) {
                // If the collision is with another LGNode, add both to the set.
                if (LGNode* otherNode = qgraphicsitem_cast<LGNode*>(collidingItem)) {
                    nodesInCollision.insert(node);
                    nodesInCollision.insert(otherNode);
                }
            }
        }

        // Pass 2: Update the opacity of ALL nodes based on the collision set.
        for (LGNode* node : allNodesInScene) {
            // If a node is in our set of colliding nodes, make it transparent.
            if (nodesInCollision.contains(node)) {
                node->setOpacity(0.5);
            }
            else {
                // Otherwise, ensure it is fully opaque.
                node->setOpacity(1.0);
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

void LGNodeWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    // Enable anti-aliasing for smooth rounded corners.
    painter.setRenderHint(QPainter::Antialiasing);

    // --- BEGIN FIX: Clipping Path for Rounded Corners ---

    // 1. Define the shape for the entire node with rounded corners.
    //    The '8' should match the border-radius from your stylesheet.
    QPainterPath clipPath;
    clipPath.addRoundedRect(this->rect(), 8, 8);

    // 2. Set this path as the clipping region. All subsequent drawing
    //    will be constrained within this shape.
    painter.setClipPath(clipPath);

    // --- END FIX ---


    // 3. Draw the header and body gradients as before.
    //    These are simple, sharp rectangles. The clipping path will handle rounding the corners.

    // Header Gradient
    const int headerHeight = 30;
    QRectF headerRect(0, 0, width(), headerHeight);
    QLinearGradient headerGradient(headerRect.topLeft(), headerRect.bottomLeft());
    headerGradient.setColorAt(0, QColor("#5c6c80")); // Lighter slate-blue
    headerGradient.setColorAt(1, QColor("#445061")); // Darker slate-blue
    painter.fillRect(headerRect, headerGradient);

    // Body Gradient
    QRectF bodyRect(0, headerHeight, width(), height() - headerHeight);
    QLinearGradient bodyGradient(bodyRect.topLeft(), bodyRect.bottomLeft());
    bodyGradient.setColorAt(0, QColor("#383838"));
    bodyGradient.setColorAt(1, QColor("#2c3e50")); // Subtle dark blue
    painter.fillRect(bodyRect, bodyGradient);

}