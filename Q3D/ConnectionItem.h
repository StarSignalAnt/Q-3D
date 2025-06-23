#pragma once
#include <QGraphicsPathItem>
#include <QColor>

class SocketWidget; // Forward declaration

class ConnectionItem : public QGraphicsPathItem {
public:
    // Constructor for temporary lines created while dragging
    ConnectionItem();

    // Constructor for final, established connections between two sockets
    ConnectionItem(SocketWidget* startSocket, SocketWidget* endSocket);

    // Public methods to set the start and end sockets
    void setStartSocket(SocketWidget* socket);
    void setEndSocket(SocketWidget* socket);
    SocketWidget* getStartSocket() {
        return m_startSocket;
    }
    SocketWidget* getEndSocket() {
        return m_endSocket;
    }
    // Public method to update the floating end point during a mouse drag
    void updateEndPos(const QPointF& pos);

    // Recalculates the curve based on the current state of the sockets/end position
    void updatePath();

protected:

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    void initialize(); // Common setup logic

    SocketWidget* m_startSocket;
    SocketWidget* m_endSocket;
    QPointF m_floatingEndPos; // Stores the floating end position for the temp line
    QColor m_color;
};