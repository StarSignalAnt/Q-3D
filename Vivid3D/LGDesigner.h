#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QDragEnterEvent> // For handling when a drag enters the widget
#include <QDragMoveEvent>  // For handling when a drag moves over the widget
#include <QDropEvent>      // For handling the final drop action
#include <QMimeData>       // For reading the data package being dragged
// Forward declarations
class ConnectionItem;
class SocketWidget;
class LGraph;
class NodeCreationWidget;

class LGDesigner : public QGraphicsView
{
    Q_OBJECT

public:
    LGDesigner(QWidget* parent = nullptr);
    ~LGDesigner();

    // Public API for the main window to call
    void NewGraph();
    void SaveGraph();
    void LoadGraph();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
private slots:
    void onCreateNode(const std::string& nodeName);

private:
    void RebuildVisualsFromGraph();
    void DeleteConnection(ConnectionItem* conn);
    QGraphicsScene* scene;
    LGraph* m_graph;

    // State for creating connections/nodes
    ConnectionItem* m_tempConnection;
    SocketWidget* m_startSocket;
    QPointF m_nodeCreationPos;
};