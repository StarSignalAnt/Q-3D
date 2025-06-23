#include "NodeTree.h"
#include <QPaintEvent>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include "SceneView.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "Importer.h"
#include <QMimeData>
#include <QDrag>
#include <QPainter>
#include <functional> // Required for std::function
#include <QMenu> // Added for context menu
#include <QInputDialog> // Added for rename/create dialog

NodeTree* NodeTree::m_Instance = nullptr;

NodeTree::NodeTree(QWidget* parent)
    : QWidget(parent)
    , m_RootNode(nullptr)
    , m_SelectedNode(nullptr)
    , m_ScrollY(0)
    , m_ContentHeight(0)
    , m_PotentialDragNode(nullptr)
    , m_DropIndicatorY(-1)
    , m_ShowDropIndicator(false)
    , m_ContextMenuNode(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAcceptDrops(true); // Enable drop events
    setContextMenuPolicy(Qt::DefaultContextMenu); // Process contextMenuEvent
    m_Instance = this;

    // Initialize colors
    m_BackgroundColor = palette().color(QPalette::Base);
    m_SelectionColor = palette().color(QPalette::Highlight);
    m_TextColor = palette().color(QPalette::Text);
    m_LineColor = palette().color(QPalette::Mid);
    m_ExpanderColor = palette().color(QPalette::ButtonText);
    m_DropIndicatorColor = QColor(135, 206, 250); // Light blue drop indicator

    // Create vertical scroll bar
    m_VerticalScrollBar = new QScrollBar(Qt::Vertical, this);
    m_VerticalScrollBar->hide();
    connect(m_VerticalScrollBar, &QScrollBar::valueChanged, this, &NodeTree::ScrollTo);

    EntityIcon = QIcon("Edit/Icons/TreeEntity.png");
    LightIcon = QIcon("edit/icons/treelight.png");


    setMinimumSize(200, 100);
}

NodeTree::~NodeTree()
{
}

void NodeTree::SetRoot(GraphNode* node)
{
    m_RootNode = node;
    m_SelectedNode = nullptr;
    m_NodeOpenStates.clear();

    // Root node starts open by default
    if (m_RootNode) {
        m_NodeOpenStates[m_RootNode] = true;
    }

    BuildTreeItems();
    UpdateLayout();
    UpdateScrollBars();
    update();
}

void NodeTree::ClearSelection()
{
    m_SelectedNode = nullptr;
    update();
}

void NodeTree::BuildTreeItems()
{
    m_TreeItems.clear();
    if (m_RootNode) {
        BuildTreeItemsRecursive(m_RootNode, 0, m_TreeItems);
    }
}

void NodeTree::BuildTreeItemsRecursive(GraphNode* node, int depth, std::vector<TreeItem>& items)
{
    if (!node) return;

    if (node->GetHideFromEditor()) {
    }
    else {
        TreeItem item(node, depth);
        // Check if node is open (default to true for root, false for others)
        auto it = m_NodeOpenStates.find(node);
        if (it != m_NodeOpenStates.end()) {
            item.expanded = it->second;
        }
        else {
            item.expanded = (depth == 0); // Root starts open, others start closed
            m_NodeOpenStates[node] = item.expanded;
        }

        items.push_back(item);

        // Add children if expanded
        if (item.expanded) {
            auto children = node->GetNodes();
            for (GraphNode* child : children) {
                BuildTreeItemsRecursive(child, depth + 1, items);
            }
        }
    }
}

void NodeTree::UpdateLayout()
{
    int currentY = 0;
    for (auto& item : m_TreeItems) {
        item.y_position = currentY;
        item.bounds = QRect(item.depth * INDENT_SIZE, currentY, width() - item.depth * INDENT_SIZE, ITEM_HEIGHT);
        currentY += ITEM_HEIGHT;
    }
    m_ContentHeight = currentY;
}

void NodeTree::UpdateScrollBars()
{
    int visibleHeight = height();

    if (m_ContentHeight > visibleHeight) {
        m_VerticalScrollBar->setRange(0, m_ContentHeight - visibleHeight);
        m_VerticalScrollBar->setPageStep(visibleHeight);
        m_VerticalScrollBar->setSingleStep(ITEM_HEIGHT);
        m_VerticalScrollBar->setGeometry(width() - m_VerticalScrollBar->sizeHint().width(), 0,
            m_VerticalScrollBar->sizeHint().width(), height());
        m_VerticalScrollBar->show();
    }
    else {
        m_VerticalScrollBar->hide();
        m_ScrollY = 0;
    }
}

void NodeTree::ScrollTo(int y)
{
    m_ScrollY = y;
    update();
}

void NodeTree::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setClipRect(event->rect());
    painter.translate(0, -m_ScrollY);

    // Draw connections first
    DrawConnections(painter);

    // Draw tree items
    for (const auto& item : m_TreeItems) {
        if (item.y_position + ITEM_HEIGHT >= m_ScrollY &&
            item.y_position <= m_ScrollY + height()) {

            bool isBeingDragged = m_isInternalDragActive && item.node == m_DraggedNode;
            bool isSelected = (item.node == m_SelectedNode && !isBeingDragged);
            bool isExternalDropTarget = (m_ExternalDropTargetNode == item.node);

            if (isBeingDragged) {
                // Draw "ghost" of item being moved internally
                painter.setOpacity(0.5);
                DrawTreeItem(painter, item, false);
                painter.setOpacity(1.0);
            }
            else {
                // Draw backgrounds first (drop target, then selection)
                if (isExternalDropTarget) {
                    QColor highlightColor = m_SelectionColor;
                    highlightColor.setAlpha(100); // Semi-transparent highlight
                    painter.fillRect(item.bounds, highlightColor);
                }
                if (isSelected) {
                    painter.fillRect(item.bounds, m_SelectionColor);
                }

                // Draw the item content (expander, icon, text)
                DrawTreeItem(painter, item, isSelected);
            }
        }
    }

    // Draw the line indicator for internal re-ordering
    if (m_ShowDropIndicator) {
        DrawDropIndicator(painter);
    }
}

void NodeTree::DrawTreeItem(QPainter& painter, const TreeItem& item, bool isSelected)
{
    QRect itemRect = item.bounds;

    if (isSelected) {
        painter.fillRect(itemRect, m_SelectionColor);
    }

    // --- Draw Expander ---
    if (!item.node->GetNodes().empty()) {
        QRect expanderRect(item.depth * INDENT_SIZE + 4, item.y_position + 4, ICON_SIZE, ICON_SIZE);
        painter.setPen(QPen(m_ExpanderColor, 2));
        painter.setBrush(Qt::NoBrush);

        QPolygon triangle;
        if (item.expanded) { // Down-pointing triangle
            triangle << QPoint(expanderRect.left() + 2, expanderRect.top() + 4)
                << QPoint(expanderRect.right() - 2, expanderRect.top() + 4)
                << QPoint(expanderRect.center().x(), expanderRect.bottom() - 2);
        }
        else { // Right-pointing triangle
            triangle << QPoint(expanderRect.left() + 4, expanderRect.top() + 2)
                << QPoint(expanderRect.left() + 4, expanderRect.bottom() - 2)
                << QPoint(expanderRect.right() - 2, expanderRect.center().y());
        }
        painter.drawPolygon(triangle);
    }

    // --- Draw Icon ---
    int iconSize = 12;
    int iconY = item.y_position + (ITEM_HEIGHT - iconSize) / 2;
    QRect iconRect(item.depth * INDENT_SIZE + ICON_SIZE + 6, iconY, ICON_SIZE, ICON_SIZE);

    bool hasCamera = item.node->GetComponent<CameraComponent>() != nullptr;
    bool hasLight = item.node->GetComponent<LightComponent>() != nullptr;
    bool hasMesh = item.node->GetComponent<StaticMeshComponent>() != nullptr || item.node->GetComponent<SkeletalMeshComponent>() != nullptr;

    if (hasLight) {
        LightIcon.paint(&painter, iconRect);
    }
    else if (hasCamera) {
        // You can create a CameraIcon here if you have one
        // For now, drawing a primitive
        painter.setPen(QPen(QColor(200, 200, 200), 1));
        painter.drawRect(iconRect.adjusted(2, 3, -2, -3));
        painter.drawEllipse(iconRect.center().x() - 2, iconRect.center().y() - 2, 4, 4);
    }
    else {
        // Default icon for entities with/without meshes
        EntityIcon.paint(&painter, iconRect);
    }

    // --- Draw Text ---
    QRect textRect(iconRect.right() + TEXT_MARGIN, item.y_position,
        itemRect.width() - (iconRect.right() + TEXT_MARGIN), ITEM_HEIGHT);
    painter.setPen(isSelected ? palette().color(QPalette::HighlightedText) : m_TextColor);
    painter.setFont(font());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, QString::fromStdString(item.node->GetName()));
}

void NodeTree::DrawConnections(QPainter& painter)
{
    painter.setPen(QPen(m_LineColor, 1, Qt::DotLine));

    for (size_t i = 0; i < m_TreeItems.size(); ++i) {
        const auto& item = m_TreeItems[i];

        if (item.depth > 0) {
            int x = item.depth * INDENT_SIZE;
            int y = item.y_position + ITEM_HEIGHT / 2;

            // Draw horizontal line to parent
            painter.drawLine(x - INDENT_SIZE / 2, y, x, y);

            // Draw vertical line connecting siblings
            bool hasNextSibling = false;
            for (size_t j = i + 1; j < m_TreeItems.size(); ++j) {
                if (m_TreeItems[j].depth < item.depth) break;
                if (m_TreeItems[j].depth == item.depth) {
                    hasNextSibling = true;
                    break;
                }
            }

            if (hasNextSibling || i > 0) {
                int startY = (i > 0 && m_TreeItems[i - 1].depth >= item.depth) ?
                    item.y_position : item.y_position + ITEM_HEIGHT / 2;
                int endY = hasNextSibling ? item.y_position + ITEM_HEIGHT : item.y_position + ITEM_HEIGHT / 2;

                painter.drawLine(x - INDENT_SIZE / 2, startY, x - INDENT_SIZE / 2, endY);
            }
        }
    }
}

void NodeTree::DrawDropIndicator(QPainter& painter)
{
    if (m_DropIndicatorY >= 0) {
        painter.setPen(QPen(m_DropIndicatorColor, 2));
        painter.drawLine(0, m_DropIndicatorY, width(), m_DropIndicatorY);

        // Draw arrow indicators
        int arrowSize = 4;
        QPolygon arrow;
        arrow << QPoint(0, m_DropIndicatorY);
        arrow << QPoint(arrowSize, m_DropIndicatorY - arrowSize);
        arrow << QPoint(arrowSize, m_DropIndicatorY + arrowSize);
        painter.setBrush(QBrush(m_DropIndicatorColor));
        painter.drawPolygon(arrow);

        arrow.clear();
        arrow << QPoint(width(), m_DropIndicatorY);
        arrow << QPoint(width() - arrowSize, m_DropIndicatorY - arrowSize);
        arrow << QPoint(width() - arrowSize, m_DropIndicatorY + arrowSize);
        painter.drawPolygon(arrow);
    }
}

NodeTree::TreeItem* NodeTree::GetItemAtPosition(const QPoint& pos)
{
    QPoint adjustedPos = pos;
    adjustedPos.setY(pos.y() + m_ScrollY);

    for (auto& item : m_TreeItems) {
        if (item.bounds.contains(adjustedPos)) {
            return &item;
        }
    }
    return nullptr;
}

void NodeTree::mousePressEvent(QMouseEvent* event)
{
    TreeItem* item = GetItemAtPosition(event->pos());

    if (event->button() == Qt::LeftButton) {
        // Set up for a potential drag or selection-on-release.
        m_PotentialDragNode = item ? item->node : nullptr;
        m_DragStartPos = event->pos();

        // Handle expand/collapse immediately as it doesn't affect selection logic.
        if (item && !item->node->GetNodes().empty()) {
            QRect expanderRect(item->depth * INDENT_SIZE, item->y_position, INDENT_SIZE + ICON_SIZE, ITEM_HEIGHT);
            expanderRect.translate(0, -m_ScrollY);
            if (expanderRect.contains(event->pos())) {
                m_NodeOpenStates[item->node] = !m_NodeOpenStates[item->node];
                BuildTreeItems();
                UpdateLayout();
                UpdateScrollBars();
                update(); // need to redraw after expand/collapse
            }
        }
    }
    else if (event->button() == Qt::RightButton) {
        // Select the node under the cursor on right-click for context
        GraphNode* node = item ? item->node : nullptr;
        if (node != m_SelectedNode) {
            m_SelectedNode = node;
            NodeSelected(m_SelectedNode);
            update();
        }
    }

    QWidget::mousePressEvent(event);
}

void NodeTree::mouseMoveEvent(QMouseEvent* event)
{
    if ((event->buttons() & Qt::LeftButton) && m_PotentialDragNode) {
        // Check if the mouse has moved far enough to be considered a drag
        if ((event->pos() - m_DragStartPos).manhattanLength() >= DRAG_THRESHOLD) {
            // Do not allow the root node to be dragged
            if (m_PotentialDragNode == m_RootNode) return;

            // A drag is starting. The potential drag node is now the actively dragged node.
            m_DraggedNode = m_PotentialDragNode;
            // Clear potential node so mouseRelease doesn't trigger a selection.
            m_PotentialDragNode = nullptr;

            // --- Initiate a Standard Qt Drag Operation ---
            QDrag* drag = new QDrag(this);
            QMimeData* mimeData = new QMimeData();

            // We pass the memory address of the node being dragged.
            qulonglong nodeAddress = reinterpret_cast<qulonglong>(m_DraggedNode);
            mimeData->setData("application/x-graphnode", QByteArray::number(nodeAddress));
            drag->setMimeData(mimeData);

            // Create a pixmap to visualize the item being dragged
            QPixmap pixmap(width(), ITEM_HEIGHT);
            pixmap.fill(Qt::transparent);
            QPainter painter(&pixmap);
            // Find the item corresponding to the dragged node to draw it
            auto it = std::find_if(m_TreeItems.begin(), m_TreeItems.end(),
                [this](const TreeItem& item) { return item.node == m_DraggedNode; });
            if (it != m_TreeItems.end()) DrawTreeItem(painter, *it, true);
            drag->setPixmap(pixmap);
            drag->setHotSpot(QPoint(10, ITEM_HEIGHT / 2));

            // This is a blocking call. The drag operation starts here.
            m_isInternalDragActive = true;
            drag->exec(Qt::MoveAction);
            m_isInternalDragActive = false;

            // Clean up the dragged node pointer after the drag is complete
            m_DraggedNode = nullptr;
        }
    }
    QWidget::mouseMoveEvent(event);
}

void NodeTree::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        // If m_PotentialDragNode is not null, a drag didn't start. This is a click.
        if (m_PotentialDragNode) {
            // Select the node that was clicked.
            if (m_SelectedNode != m_PotentialDragNode) {
                m_SelectedNode = m_PotentialDragNode;
                NodeSelected(m_SelectedNode); // Direct call instead of emit
            }
            update();
        }
        else {
            // m_PotentialDragNode is null. This can happen if a drag occurred,
            // or if the user clicked on empty space.
            // If it was a click on empty space, we deselect.
            TreeItem* item = GetItemAtPosition(event->pos());
            if (item == nullptr && m_SelectedNode != nullptr) {
                m_SelectedNode = nullptr;
                NodeSelected(nullptr); // Direct call instead of emit
                update();
            }
        }
        // Always clear the potential drag node state on release.
        m_PotentialDragNode = nullptr;
    }
    QWidget::mouseReleaseEvent(event);
}

void NodeTree::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        TreeItem* item = GetItemAtPosition(event->pos());
        if (item) {
            emit NodeDoubleClicked(item->node);
        }
    }

    QWidget::mouseDoubleClickEvent(event);
}

void NodeTree::wheelEvent(QWheelEvent* event)
{
    if (m_VerticalScrollBar->isVisible()) {
        int delta = -event->angleDelta().y() / 8; // Convert to pixels
        int newValue = m_VerticalScrollBar->value() + delta;
        m_VerticalScrollBar->setValue(newValue);
    }

    QWidget::wheelEvent(event);
}

void NodeTree::resizeEvent(QResizeEvent* event)
{
    UpdateLayout();
    UpdateScrollBars();
    QWidget::resizeEvent(event);
}


void NodeTree::contextMenuEvent(QContextMenuEvent* event)
{
    TreeItem* item = GetItemAtPosition(event->pos());
    m_ContextMenuNode = item ? item->node : nullptr;

    QMenu contextMenu(this);

    if (m_ContextMenuNode)
    {
        // Menu when right-clicking on a node
        QAction* createAction = contextMenu.addAction("Create New Node");
        contextMenu.addSeparator();
        QAction* renameAction = contextMenu.addAction("Rename Node");
        QAction* deleteAction = contextMenu.addAction("Delete Node");

        connect(createAction, &QAction::triggered, this, &NodeTree::OnCreateNode);
        connect(renameAction, &QAction::triggered, this, &NodeTree::OnRenameNode);
        connect(deleteAction, &QAction::triggered, this, &NodeTree::OnDeleteNode);

        // Don't allow deleting or renaming the root node
        if (m_ContextMenuNode == m_RootNode) {
            deleteAction->setEnabled(false);
            renameAction->setEnabled(false);
        }
    }
    else
    {
        // Menu when right-clicking on empty space
        QAction* createAction = contextMenu.addAction("Create New Node");
        connect(createAction, &QAction::triggered, this, &NodeTree::OnCreateNode);
    }

    contextMenu.exec(event->globalPos());
}

void NodeTree::OnCreateNode()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Create Node",
        "Node name:", QLineEdit::Normal,
        "New Node", &ok);
    if (ok && !text.isEmpty())
    {
        GraphNode* newNode = new GraphNode();
        newNode->SetName(text.toStdString());

        // If we right-clicked a node, add the new node as its child.
        // Otherwise, add it to the root.
        GraphNode* parentNode = m_ContextMenuNode ? m_ContextMenuNode : m_RootNode;

        if (parentNode)
        {
            parentNode->AddNode(newNode);
            m_NodeOpenStates[parentNode] = true; // Make sure parent is open to show the new node

            BuildTreeItems();
            UpdateLayout();
            UpdateScrollBars();
            emit NodeStructureChanged();
            update();
        }
        else {
            // Should not happen if a root exists, but as a fallback, clean up.
            delete newNode;
        }
    }
}

void NodeTree::OnDeleteNode()
{
    if (!m_ContextMenuNode || m_ContextMenuNode == m_RootNode) return;

    // Clear selection if the deleted node was selected
    if (m_SelectedNode == m_ContextMenuNode)
    {
        m_SelectedNode = nullptr;
        NodeSelected(nullptr);
    }

    RemoveNodeFromParent(m_ContextMenuNode);

    // Assuming the NodeTree is responsible for deleting nodes.
    // If GraphNode's destructor handles deleting its children, this is safe.
    // If not, this will leak children of the deleted node.
    delete m_ContextMenuNode;

    m_ContextMenuNode = nullptr; // Avoid dangling pointer

    BuildTreeItems();
    UpdateLayout();
    UpdateScrollBars();
    emit NodeStructureChanged();
    update();
}

void NodeTree::OnRenameNode()
{
    if (!m_ContextMenuNode || m_ContextMenuNode == m_RootNode) return;

    bool ok;
    QString currentName = QString::fromStdString(m_ContextMenuNode->GetName());
    QString text = QInputDialog::getText(this, "Rename Node",
        "New name:", QLineEdit::Normal,
        currentName, &ok);

    if (ok && !text.isEmpty())
    {
        m_ContextMenuNode->SetName(text.toStdString());
        update(); // Just need a redraw, not a full hierarchy rebuild
    }
}


bool NodeTree::CanDropOn(GraphNode* dragNode, GraphNode* dropTarget)
{
    if (!dragNode || !dropTarget) return false;
    if (dragNode == dropTarget) return false;
    if (dragNode == m_RootNode) return false; // Can't move root

    // Check if dropTarget is a descendant of dragNode (would create cycle)
    std::function<bool(GraphNode*, GraphNode*)> isDescendant = [&](GraphNode* ancestor, GraphNode* node) -> bool {
        auto children = ancestor->GetNodes();
        for (GraphNode* child : children) {
            if (child == node) return true;
            if (isDescendant(child, node)) return true;
        }
        return false;
        };

    return !isDescendant(dragNode, dropTarget);
}

void NodeTree::PerformDrop(GraphNode* dragNode, GraphNode* dropTarget)
{
    if (!CanDropOn(dragNode, dropTarget)) return;

    // Remove from old parent
    RemoveNodeFromParent(dragNode);

    // Add to new parent
    dropTarget->AddNode(dragNode);

    // Expand the drop target to show the new child
    m_NodeOpenStates[dropTarget] = true;

    // Rebuild tree structure
    BuildTreeItems();
    UpdateLayout();
    UpdateScrollBars();

    // Emit signal that structure changed
    emit NodeStructureChanged();

    update();
}

void NodeTree::RemoveNodeFromParent(GraphNode* node)
{
    if (!node) return;

    GraphNode* parent = node->GetRootNode();
    if (!parent) return;

    parent->RemoveNode(node);
}

void NodeTree::dragEnterEvent(QDragEnterEvent* event)
{
    // Check for internal drag type
    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        event->acceptProposedAction();
        return;
    }
    // Check for external file drag type
    if (event->mimeData()->hasText()) {
        QString path = event->mimeData()->text();
        if (path.endsWith(".fbx", Qt::CaseInsensitive) || path.endsWith(".gltf", Qt::CaseInsensitive)) {
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

void NodeTree::dragMoveEvent(QDragMoveEvent* event)
{
    m_ShowDropIndicator = false;
    m_ExternalDropTargetNode = nullptr;

    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        TreeItem* item = GetItemAtPosition(event->position().toPoint());
        GraphNode* dropTargetNode = item ? item->node : m_RootNode;

        bool ok;
        qulonglong addr = event->mimeData()->data("application/x-graphnode").toULongLong(&ok);
        GraphNode* dragNode = ok ? reinterpret_cast<GraphNode*>(addr) : nullptr;

        if (CanDropOn(dragNode, dropTargetNode)) {
            if (item) {
                m_DropIndicatorY = item->y_position + ITEM_HEIGHT;
                m_ShowDropIndicator = true;
            }
            event->acceptProposedAction();
        }
        else {
            event->ignore();
        }
    }
    else if (event->mimeData()->hasText()) {
        TreeItem* item = GetItemAtPosition(event->position().toPoint());
        if (item) {
            m_ExternalDropTargetNode = item->node;
        }
        event->acceptProposedAction();
    }
    else {
        event->ignore();
    }

    update();
}

void NodeTree::dragLeaveEvent(QDragLeaveEvent* event)
{
    m_ShowDropIndicator = false;
    m_ExternalDropTargetNode = nullptr;
    update();
    event->accept();
}

void NodeTree::dropEvent(QDropEvent* event)
{
    m_ShowDropIndicator = false;
    m_ExternalDropTargetNode = nullptr;
    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        bool ok;
        qulonglong addr = event->mimeData()->data("application/x-graphnode").toULongLong(&ok);
        if (!ok) { event->ignore(); update(); return; }

        GraphNode* dragNode = reinterpret_cast<GraphNode*>(addr);
        TreeItem* targetItem = GetItemAtPosition(event->position().toPoint());
        GraphNode* dropTargetNode = targetItem ? targetItem->node : m_RootNode;

        if (CanDropOn(dragNode, dropTargetNode)) {
            PerformDrop(dragNode, dropTargetNode);
            event->acceptProposedAction();
        }
        else {
            event->ignore();
        }
    }
    else if (event->mimeData()->hasText()) {
        QString fullPath = event->mimeData()->text();
        if (fullPath.endsWith(".fbx", Qt::CaseInsensitive) || fullPath.endsWith(".gltf", Qt::CaseInsensitive)) {
            GraphNode* importedNode = Importer::ImportEntity(fullPath.toStdString());
            if (importedNode) {
                TreeItem* targetItem = GetItemAtPosition(event->position().toPoint());
                GraphNode* parentNode = targetItem ? targetItem->node : m_RootNode;

                parentNode->AddNode(importedNode);
                m_NodeOpenStates[parentNode] = true;

                BuildTreeItems();
                UpdateLayout();
                UpdateScrollBars();
                emit NodeStructureChanged();
            }
            event->acceptProposedAction();
        }
        else {
            event->ignore();
        }
    }
    else {
        event->ignore();
    }
    update();
}

void NodeTree::NodeSelected(GraphNode* node)
{
    // This implementation now matches the original one.
    SceneView::m_Instance->SelectNode(node);
}
