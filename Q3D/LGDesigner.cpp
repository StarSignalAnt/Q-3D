#include "LGDesigner.h"
#include <qpainter.h>
#include <QFileDialog>
#include <QKeyEvent>
#include <QCursor>
#include <QGraphicsProxyWidget>
#include <map>

#include "QEngine.h"
#include "NodeRegistry.h"
#include "LGraph.h"
#include "LGNode.h"
#include "NodeCreationWidget.h"
#include "ConnectionItem.h"
#include "SocketWidget.h"
#include "GraphNode.h"
#include "LGNode.h"
#include "GetVariableNode.h"
#include "SetVariableNode.h"
LGDesigner::LGDesigner(QWidget* parent)
    : QGraphicsView(parent),
    m_graph(nullptr),
    m_tempConnection(nullptr),
    m_startSocket(nullptr)
{
    setBackgroundBrush(QColor("#202020"));
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::RubberBandDrag);
    setCacheMode(QGraphicsView::CacheNone);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setSceneRect(-500, -500, 1000, 1000);

    scene = new QGraphicsScene(this);
    setScene(scene);
    connect(scene, &QGraphicsScene::selectionChanged, this, &LGDesigner::onSceneSelectionChanged);
    NewGraph(); // Start with a clean, empty graph
}

LGDesigner::~LGDesigner()
{
    delete m_graph;
}

void LGDesigner::NewGraph() {
    scene->clear();
    delete m_graph;
    m_graph = new LGraph("New Graph");
    auto nv = new LGraphVariable("Node", DataType::GraphNodeRef);
    m_graph->AddVariable(nv);
}

void LGDesigner::SaveGraph() {
    if (!m_graph) return;

    QString filePath = QFileDialog::getSaveFileName(this, "Save Graph", "", "Graph Files (*.graph)");
    if (filePath.isEmpty()) {
        return;
    }

    // Iterate through all items in the scene
    for (QGraphicsItem* item : scene->items()) {
        // First, check if the item is valid and if its type matches LGNode::Type
        if (item && item->type() == LGNode::Type) {
            // Because we've confirmed the type, we can safely cast it.
            // A static_cast is faster than the qgraphicsitem_cast here.
            LGNode* visualNode = static_cast<LGNode*>(item);

            if (LNode* logicNode = visualNode->getLogicNode()) { //
                QPointF pos = visualNode->pos();
                logicNode->SetEditorPosition({ (float)pos.x(), (float)pos.y() }); //
            }
        }
        // All other item types (like ConnectionItem) are safely ignored.
    }

    m_graph->SaveToFile(filePath.toStdString()); //
}

void LGDesigner::LoadGraph() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Graph", "", "Graph Files (*.graph)");
    if (filePath.isEmpty()) return;

    LGraph* loadedGraph = LGraph::LoadFromFile(filePath.toStdString(), Q3D::Engine::QEngine::GetNodeRegistry());

    if (loadedGraph) {
        scene->clear();
        delete m_graph;
        m_graph = loadedGraph;
        RebuildVisualsFromGraph();
    }
}void LGDesigner::RebuildVisualsFromGraph() {
    if (!m_graph) {
        return;
    }
    scene->clear();

    std::map<LNode*, LGNode*> logicToVisualMap;

    // PASS 1: Create all visual nodes (This part is unchanged and correct)
    for (LNode* const logicNode : m_graph->GetNodes()) {
        if (!logicNode) continue;
        auto* visualNode = new LGNode(logicNode);
        visualNode->setPos(logicNode->GetEditorPosition().x, logicNode->GetEditorPosition().y);
        scene->addItem(visualNode);
        logicToVisualMap[logicNode] = visualNode;
    }

    // PASS 2: Create all visual connections
    for (LNode* const logicNode : m_graph->GetNodes()) {
        if (!logicNode) continue;

        // --- BEGIN REFACTORED EXECUTION LOGIC ---
        // Loop through all of the node's execution output pins.
        const auto& execOutputs = logicNode->GetExecOutputs();
        for (int i = 0; i < execOutputs.size(); ++i) {
            const auto& execPin = execOutputs[i];

            // Check if this specific execution pin is connected to another node.
            if (execPin->nextNode) {
                LGNode* fromVisualNode = logicToVisualMap.count(logicNode) ? logicToVisualMap.at(logicNode) : nullptr;
                LGNode* toVisualNode = logicToVisualMap.count(execPin->nextNode) ? logicToVisualMap.at(execPin->nextNode) : nullptr;

                if (fromVisualNode && toVisualNode) {
                    // Get the 'from' socket using its index 'i'.
                    SocketWidget* fromSocket = fromVisualNode->getSocket(SocketWidget::SocketDirection::Out, SocketWidget::SocketType::Exec, i);
                    // The 'to' socket is always the single 'Exec In' pin, which has index 0.
                    SocketWidget* toSocket = toVisualNode->getSocket(SocketWidget::SocketDirection::In, SocketWidget::SocketType::Exec, 0);

                    if (fromSocket && toSocket) {
                        auto* conn = new ConnectionItem(fromSocket, toSocket);
                        scene->addItem(conn);
                        fromSocket->addConnection(conn);
                        toSocket->addConnection(conn);
                        conn->updatePath();
                    }
                }
            }
        }
        // --- END REFACTORED EXECUTION LOGIC ---

        // Rebuild Data Connections (This part is unchanged and correct)
        for (const auto& inputPort : logicNode->GetInputs()) {
            if (inputPort && inputPort->isConnected()) {
                LGOutput* outputPort = inputPort->GetConnection();
                LNode* fromLogicNode = outputPort->getParentNode();
                LGNode* fromVisual = logicToVisualMap.count(fromLogicNode) ? logicToVisualMap.at(fromLogicNode) : nullptr;
                LGNode* toVisual = logicToVisualMap.count(logicNode) ? logicToVisualMap.at(logicNode) : nullptr;

                if (fromVisual && toVisual) {
                    SocketWidget* fromSocket = fromVisual->getSocket(SocketWidget::SocketDirection::Out, SocketWidget::SocketType::Data, outputPort->getPortIndex());
                    SocketWidget* toSocket = toVisual->getSocket(SocketWidget::SocketDirection::In, SocketWidget::SocketType::Data, inputPort->getPortIndex());
                    if (fromSocket && toSocket) {
                        auto conn = new ConnectionItem(fromSocket, toSocket);
                        scene->addItem(conn);
                        fromSocket->addConnection(conn);
                        toSocket->addConnection(conn);
                        conn->updatePath();
                    }
                }
            }
        }
    }
}

void LGDesigner::keyPressEvent(QKeyEvent* event) {

    // Check if the Delete or Backspace key was pressed
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {

        // Get all selected items from the scene, but make a copy of the list.
        // This is important because we will be modifying the scene, which can invalidate the original list.
      

        const QList<QGraphicsItem*> selectedItems = scene->selectedItems();
        if (selectedItems.isEmpty()) {
            return;
        }

        // 2. Sort the selected items into two separate, safe lists.
        QList<ConnectionItem*> connectionsToDelete;
        QList<LGNode*> nodesToDelete;

        for (QGraphicsItem* item : selectedItems) {
            if (auto* conn = qgraphicsitem_cast<ConnectionItem*>(item)) {
                connectionsToDelete.append(conn);
            }
            else if (auto* node = qgraphicsitem_cast<LGNode*>(item)) {
                nodesToDelete.append(node);
            }
        }

        // 3. Process the connection deletions first. This doesn't require a full UI rebuild.
        for (ConnectionItem* conn : connectionsToDelete) {
            DeleteConnection(conn);
        }

        // 4. Process the node deletions.
        if (!nodesToDelete.isEmpty()) {
            // First, update all logical node positions to ensure they don't jump.
            for (QGraphicsItem* item : scene->items()) {
                if (auto* visualNode = qgraphicsitem_cast<LGNode*>(item)) {
                    if (LNode* logicNode = visualNode->getLogicNode()) {
                        logicNode->SetEditorPosition(glm::vec2(visualNode->pos().x(), visualNode->pos().y()));
                    }
                }
            }

            // Now, delete the logical nodes.
            for (LGNode* visualNode : nodesToDelete) {
                m_graph->RemoveNode(visualNode->getLogicNode());
            }

            // Finally, perform a single, safe rebuild of the entire visual graph.
            RebuildVisualsFromGraph();
        }

        // The event has been handled.
        return;
    }

    // Handle the spacebar for node creation (your existing logic)
    QPoint viewPos = mapFromGlobal(QCursor::pos());
    if (event->key() == Qt::Key_Space && itemAt(mapFromGlobal(QCursor::pos())) == nullptr) {
        m_nodeCreationPos = mapToScene(mapFromGlobal(QCursor::pos()));

        // --- FIX: Pass the current graph pointer to the constructor ---
        NodeCreationWidget* widget = new NodeCreationWidget(m_graph, Q3D::Engine::QEngine::GetNodeRegistry(), this);

        connect(widget, &NodeCreationWidget::nodeSelected, this, &LGDesigner::onCreateNode);
        widget->move(QCursor::pos());
        widget->show();
        event->accept();
        return;
    }

    // If it was any other key, pass it to the base class.
    QGraphicsView::keyPressEvent(event);
}

void LGDesigner::onCreateNode(const std::string& creationCommand) {
    LNode* logicNode = nullptr;

    // --- BEGIN FIX ---

    // First, try to create the node from the registry using its name.
    // This will correctly find and create "Get Scene Node", "Turn GraphNode", etc.
    logicNode = Q3D::Engine::QEngine::GetNodeRegistry().CreateNode(creationCommand);

    // If, and only if, no registered node was found, then we check for special
    // dynamic node types like variable getters.
    if (!logicNode && creationCommand.rfind("Get ", 0) == 0) {
        std::string varName = creationCommand.substr(4);
        auto* getNode = new GetVariableNode();
        getNode->Initialize(m_graph, varName);
        logicNode = getNode;
    }
    // (A similar `if (!logicNode && ...)` check for SetVariableNode will also be needed here
    // if you have one that follows a similar pattern)

    // --- END FIX ---


    if (logicNode) {
        // This part remains the same.
        m_graph->AddNode(logicNode);
        auto visualNode = new LGNode(logicNode);
        visualNode->setPos(m_nodeCreationPos);
        scene->addItem(visualNode);
    }
}

void LGDesigner::mousePressEvent(QMouseEvent* event) {
    QGraphicsItem* item = itemAt(event->pos());
    auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
    if (proxy) {
        QPoint localPos = proxy->widget()->mapFromGlobal(event->globalPosition().toPoint());
        QWidget* widget = proxy->widget()->childAt(localPos);
        m_startSocket = dynamic_cast<SocketWidget*>(widget);
    }
    else {
        m_startSocket = nullptr;
    }

    if (m_startSocket) {
        setDragMode(QGraphicsView::NoDrag);
        m_tempConnection = new ConnectionItem();
        scene->addItem(m_tempConnection);
        m_tempConnection->setStartSocket(m_startSocket);
        m_tempConnection->updateEndPos(mapToScene(event->pos()));
        return;
    }

    setDragMode(QGraphicsView::RubberBandDrag);
    QGraphicsView::mousePressEvent(event);
}

void LGDesigner::mouseMoveEvent(QMouseEvent* event) {
    if (m_tempConnection) {
        m_tempConnection->updateEndPos(mapToScene(event->pos()));
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}
void LGDesigner::mouseReleaseEvent(QMouseEvent* event) {
    // Check if we are in the process of dragging a connection
    if (m_tempConnection && m_startSocket) {
        SocketWidget* endSocket = nullptr;
        QGraphicsItem* item = itemAt(event->pos());
        // Find the socket under the cursor, if any
        if (auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item)) {
            QPoint localPos = proxy->widget()->mapFromGlobal(event->globalPosition().toPoint());
            QWidget* widget = proxy->widget()->childAt(localPos);
            endSocket = dynamic_cast<SocketWidget*>(widget);
        }

        bool connectionIsValid = false;
        if (endSocket && endSocket != m_startSocket && m_startSocket->getDirection() != endSocket->getDirection()) {
            SocketWidget* inSocket = (m_startSocket->getDirection() == SocketWidget::SocketDirection::In) ? m_startSocket : endSocket;
            SocketWidget* outSocket = (m_startSocket->getDirection() == SocketWidget::SocketDirection::Out) ? m_startSocket : endSocket;

            // Check for valid Exec -> Exec connection
            if (inSocket->getType() == SocketWidget::SocketType::Exec && outSocket->getType() == SocketWidget::SocketType::Exec) {
                outSocket->getParentNode()->SetNextExec(outSocket->getPortIndex(), inSocket->getParentNode());
                connectionIsValid = true;
            }
            // Check for valid Data -> Data connection
            else if (inSocket->getType() == SocketWidget::SocketType::Data && outSocket->getType() == SocketWidget::SocketType::Data) {
                LGInput* inputPort = inSocket->getParentNode()->GetInputs()[inSocket->getPortIndex()];
                LGOutput* outputPort = outSocket->getParentNode()->GetOutputs()[outSocket->getPortIndex()];

                // --- BEGIN FIX ---
                // Directly compare the DataType of the input and output.
                // The old static_cast to InputType is no longer needed.
                if (!inputPort->isConnected() && inputPort->GetType() == outputPort->GetType()) {
                    inputPort->setConnection(outputPort);
                    outputPort->addConnection(inputPort);
                    connectionIsValid = true;
                }
                // --- END FIX ---
            }
        }

        // Always delete the temporary yellow line
        delete m_tempConnection;
        m_tempConnection = nullptr;

        if (connectionIsValid) {
            // If the connection was valid, create a NEW, permanent ConnectionItem.
            auto* permanentConnection = new ConnectionItem(m_startSocket, endSocket);
            scene->addItem(permanentConnection);
            permanentConnection->updatePath();
            m_startSocket->addConnection(permanentConnection);
            endSocket->addConnection(permanentConnection);
        }

        m_startSocket = nullptr; // Reset state
        return;
    }

    // If we weren't dragging a connection, pass the event to the base class
    QGraphicsView::mouseReleaseEvent(event);
}
// ... All mouse event handlers (mousePress, mouseMove, mouseRelease) remain unchanged ...

void LGDesigner::dragEnterEvent(QDragEnterEvent* event)
{
    // We only care about drags that contain our specific GraphNode data type.
    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        event->acceptProposedAction();
    }
    else {
        // For other types, pass it to the base class.
        QGraphicsView::dragEnterEvent(event);
    }
}

// This event is called continuously as the drag moves over the widget.
void LGDesigner::dragMoveEvent(QDragMoveEvent* event)
{
    // Again, we only handle our specific data type.
    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        event->acceptProposedAction();
    }
    else {
        QGraphicsView::dragMoveEvent(event);
    }
}void LGDesigner::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasFormat("application/x-graphnode")) {
        event->ignore();
        return;
    }

    // Find the LGNode that is being dropped on.
    LGNode* targetLGNode = nullptr;
    QGraphicsItem* item = itemAt(event->position().toPoint());
    if (auto* directHit = qgraphicsitem_cast<LGNode*>(item)) {
        targetLGNode = directHit;
    }
    else if (auto* proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item)) {
        targetLGNode = qgraphicsitem_cast<LGNode*>(proxy->parentItem());
    }

    if (!targetLGNode) { event->ignore(); return; }

    // Find the specific QWidget under the cursor (e.g., the QLineEdit).
    QWidget* nodeWidget = targetLGNode->getProxyWidget()->widget();
    QPoint globalPos = this->mapToGlobal(event->position().toPoint());
    QPoint localPos = nodeWidget->mapFromGlobal(globalPos);
    QWidget* targetWidget = nodeWidget->childAt(localPos);

    if (!targetWidget) { event->ignore(); return; }

    // --- BEGIN FINAL FIX ---
    // Cast the containing widget to our LGNodeWidget type.
    if (auto* lgNodeWidget = qobject_cast<LGNodeWidget*>(nodeWidget))
    {
        // Get the map that links UI widgets to logical inputs.
        const auto& widgetMap = lgNodeWidget->getWidgetToInputMap();

        // Check if the widget we dropped on exists as a key in our map.
        if (widgetMap.contains(targetWidget))
        {
            LGInput* logicalInput = widgetMap.value(targetWidget);

            // Now we have the exact logical input pin. Validate its type.
            if (logicalInput->GetType() == DataType::String || logicalInput->GetType() == DataType::GraphNodeRef)
            {
                bool ok;
                GraphNode* droppedNode = reinterpret_cast<GraphNode*>(event->mimeData()->data("application/x-graphnode").toULongLong(&ok));
                if (!ok || !droppedNode) { event->ignore(); return; }

                // Update the logical data by creating a variant.
                LGInput::ValueVariant val = droppedNode->GetLongName();
                logicalInput->SetDefaultValue(val);

                // Update the UI widget directly.
                if (auto* lineEdit = qobject_cast<QLineEdit*>(targetWidget)) {
                    lineEdit->setText(QString::fromStdString(droppedNode->GetLongName()));
                }

                event->acceptProposedAction();
                return;
            }
        }
    }
    // --- END FINAL FIX ---

    event->ignore();
}

void LGDesigner::DeleteConnection(ConnectionItem* conn)
{
    if (!conn) return;

    SocketWidget* startSocket = conn->getStartSocket();
    SocketWidget* endSocket = conn->getEndSocket();

    // 1. Logically disconnect the data pins.
    if (startSocket && endSocket && startSocket->getType() == SocketWidget::SocketType::Data) {
        SocketWidget* inSocket = (startSocket->getDirection() == SocketWidget::SocketDirection::In) ? startSocket : endSocket;
        SocketWidget* outSocket = (startSocket->getDirection() == SocketWidget::SocketDirection::In) ? endSocket : startSocket;

        if (inSocket->getParentNode() && outSocket->getParentNode()) {
            LGInput* logicalInput = inSocket->getParentNode()->GetInputs()[inSocket->getPortIndex()];
            LGOutput* logicalOutput = outSocket->getParentNode()->GetOutputs()[outSocket->getPortIndex()];

            if (logicalInput && logicalOutput) {
                logicalInput->setConnection(nullptr);
                logicalOutput->removeConnection(logicalInput);
            }
        }
    }

    // 2. Visually disconnect from sockets.
    if (startSocket) startSocket->removeConnection(conn);
    if (endSocket) endSocket->removeConnection(conn);

    // 3. Delete the connection item itself.
    scene->removeItem(conn);
    delete conn;
}

void LGDesigner::wheelEvent(QWheelEvent* event)
{
    // Check if the Control key is being held down.
    if (event->modifiers() & Qt::ControlModifier) {
        // Set the anchor to be where the mouse is.
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        const int delta = event->angleDelta().y();
        const qreal zoomFactor = 1.15;

        if (delta > 0) {
            scale(zoomFactor, zoomFactor);
        }
        else {
            scale(1.0 / zoomFactor, 1.0 / zoomFactor);
        }

        // Optional: Reset the anchor to the default behavior.
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);

        event->accept();
    }
    else {
        QGraphicsView::wheelEvent(event);
    }
}

void LGDesigner::onSceneSelectionChanged()
{
    // Get all items currently selected in the scene.
    const auto selectedItems = scene->selectedItems();

    LNode* firstSelectedNode = nullptr;

    // Check if the selection is not empty.
    if (!selectedItems.isEmpty()) {
        // We only care about the first selected item for the properties panel.
        QGraphicsItem* topItem = selectedItems.first();
        
        // Check if the selected item is a node.
        if (auto* visualNode = qgraphicsitem_cast<LGNode*>(topItem)) {
            firstSelectedNode = visualNode->getLogicNode();
        }
    }

    // Emit our signal to notify the properties panel.
    // This will send either the first selected node or nullptr if the selection is empty.
    emit nodeSelected(firstSelectedNode);
}