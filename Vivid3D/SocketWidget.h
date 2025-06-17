#pragma once
#include <QWidget>
#include <QList>

class LNode;
class ConnectionItem;

class SocketWidget : public QWidget {
    Q_OBJECT // Good practice for QWidget subclasses

public:
    enum class SocketDirection { In, Out };
    enum class SocketType { Exec, Data };

    SocketWidget(LNode* parentNode, SocketDirection dir, SocketType type, int portIndex, QWidget* parent = nullptr);

    void addConnection(ConnectionItem* conn) { m_connections.append(conn); }
    const QList<ConnectionItem*>& getConnections() const { return m_connections; }

    LNode* getParentNode() const { return m_parentNode; }
    SocketDirection getDirection() const { return m_direction; }
    SocketType getType() const { return m_type; }
    int getPortIndex() const { return m_portIndex; }

protected:
    // --- NEW: Override paintEvent to force drawing ---
    void paintEvent(QPaintEvent* event) override;

private:
    LNode* m_parentNode;
    SocketDirection m_direction;
    SocketType m_type;
    int m_portIndex;
    QList<ConnectionItem*> m_connections;
};