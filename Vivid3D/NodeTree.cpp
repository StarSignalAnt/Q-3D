#include "NodeTree.h"
#include <QPaintEvent>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include "SCeneView.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"

NodeTree* NodeTree::m_Instance = nullptr;

NodeTree::NodeTree(QWidget* parent)
    : QWidget(parent)
    , m_RootNode(nullptr)
    , m_SelectedNode(nullptr)
    , m_ScrollY(0)
    , m_ContentHeight(0)
    , m_IsDragging(false)
    , m_DragNode(nullptr)
    , m_PotentialDragNode(nullptr)
    , m_DropTargetNode(nullptr)
    , m_DropIndicatorY(-1)
    , m_ShowDropIndicator(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setAcceptDrops(true); // Enable drop events
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
    connect(m_VerticalScrollBar, &QScrollBar::valueChanged, this,&NodeTree::ScrollTo);

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

    // Fill background
    painter.fillRect(rect(), m_BackgroundColor);

    // Set up clipping
    painter.setClipRect(event->rect());

    // Translate for scrolling
    painter.translate(0, -m_ScrollY);

    // Draw connections first
    DrawConnections(painter);

    // Draw tree items
    for (const auto& item : m_TreeItems) {
        if (item.y_position + ITEM_HEIGHT >= m_ScrollY &&
            item.y_position <= m_ScrollY + height()) {

            // Don't draw the item being dragged with normal appearance
            bool isBeingDragged = (m_IsDragging && item.node == m_DragNode);
            bool isSelected = (item.node == m_SelectedNode && !isBeingDragged);

            if (isBeingDragged) {
                // Draw dragged item with reduced opacity
                painter.setOpacity(0.5);
                DrawTreeItem(painter, item, false);
                painter.setOpacity(1.0);
            }
            else {
                DrawTreeItem(painter, item, isSelected);
            }
        }
    }

    // Draw drop indicator
    if (m_ShowDropIndicator) {
        DrawDropIndicator(painter);
    }
}

void NodeTree::DrawTreeItem(QPainter& painter, const TreeItem& item, bool isSelected)
{
    QRect itemRect = item.bounds;

    // Draw selection background
    if (isSelected) {
        painter.fillRect(itemRect, m_SelectionColor);
    }

    // Draw expander for nodes with children
    auto children = item.node->GetNodes();
    if (!children.empty()) {
        QRect expanderRect(item.depth * INDENT_SIZE + 4, item.y_position + 4, ICON_SIZE, ICON_SIZE);

        painter.setPen(QPen(m_ExpanderColor, 2));
        painter.setBrush(Qt::NoBrush);

        // Draw triangle (expanded) or square (collapsed)
        if (item.expanded) {
            // Draw down-pointing triangle
            QPolygon triangle;
            triangle << QPoint(expanderRect.left() + 2, expanderRect.top() + 4);
            triangle << QPoint(expanderRect.right() - 2, expanderRect.top() + 4);
            triangle << QPoint(expanderRect.center().x(), expanderRect.bottom() - 2);
            painter.drawPolygon(triangle);
        }
        else {
            // Draw right-pointing triangle
            QPolygon triangle;
            triangle << QPoint(expanderRect.left() + 4, expanderRect.top() + 2);
            triangle << QPoint(expanderRect.left() + 4, expanderRect.bottom() - 2);
            triangle << QPoint(expanderRect.right() - 2, expanderRect.center().y());
            painter.drawPolygon(triangle);
        }
    }

    // Draw node icon based on components
    int iconSize = 12;  // Increased size for better visibility
    int iconY = item.y_position + (ITEM_HEIGHT - iconSize) / 2;
    QRect iconRect(item.depth * INDENT_SIZE + ICON_SIZE + 8, iconY, iconSize, iconSize);

    // Determine icon type and color based on components
    bool hasStaticMesh = (item.node->GetComponent<StaticMeshComponent>() != nullptr);
    bool hasCamera = (item.node->GetComponent<CameraComponent>() != nullptr);
    bool hasLight = (item.node->GetComponent<LightComponent>() != nullptr);

    painter.setBrush(Qt::NoBrush);  // No fill for wireframe

    if (hasCamera) {
        // Draw camera icon
        painter.setPen(QPen(QColor(200, 200, 200), 1));  // Light gray color

        // Camera body (main rectangle)
        QRect cameraBody = iconRect.adjusted(2, 3, -2, -3);
        painter.drawRect(cameraBody);

        // Camera lens (circle)
        QRect lens(iconRect.center().x() - 2, iconRect.center().y() - 2, 4, 4);
        painter.drawEllipse(lens);

        // Camera viewfinder (small rectangle on top)
        QRect viewfinder(iconRect.center().x() - 1, iconRect.top() + 1, 2, 2);
        painter.drawRect(viewfinder);
    }
    else if (hasLight) {
        // Draw light bulb icon
        painter.setPen(QPen(QColor(255, 255, 100), 2));  // Yellow color

        // Light bulb (circle for the bulb)
        QRect bulb(iconRect.center().x() - 3, iconRect.top() + 2, 6, 6);
        painter.drawEllipse(bulb);

        // Light base (small rectangle)
        QRect base(iconRect.center().x() - 2, iconRect.bottom() - 3, 4, 2);
        painter.drawRect(base);

        // Light rays (small lines extending from bulb)
        painter.drawLine(iconRect.center().x() - 6, iconRect.center().y(),
            iconRect.center().x() - 4, iconRect.center().y());
        painter.drawLine(iconRect.center().x() + 4, iconRect.center().y(),
            iconRect.center().x() + 6, iconRect.center().y());
        painter.drawLine(iconRect.center().x(), iconRect.center().y() - 6,
            iconRect.center().x(), iconRect.center().y() - 4);
    }
    else if (hasStaticMesh) {
        // Draw blue 3D box for static mesh
        painter.setPen(QPen(QColor(100, 149, 237), 1));  // Cornflower blue

        // Draw main box outline
        painter.drawRect(iconRect);

        // Draw additional lines to give it a 3D wireframe appearance
        int offset = 2;
        QRect backRect = iconRect.adjusted(offset, -offset, offset, -offset);
        painter.drawRect(backRect);

        // Connect the corners to create 3D effect
        painter.drawLine(iconRect.topLeft(), backRect.topLeft());
        painter.drawLine(iconRect.topRight(), backRect.topRight());
        painter.drawLine(iconRect.bottomLeft(), backRect.bottomLeft());
        painter.drawLine(iconRect.bottomRight(), backRect.bottomRight());
    }
    else {
        // Default green wireframe box for nodes without specific components
        painter.setPen(QPen(QColor(144, 238, 144), 1));  // Light green color

        // Smaller default icon
        iconSize = 9;
        iconRect = QRect(item.depth * INDENT_SIZE + ICON_SIZE + 8,
            item.y_position + (ITEM_HEIGHT - iconSize) / 2, iconSize, iconSize);

        // Draw main box outline
        painter.drawRect(iconRect);

        // Draw additional lines to give it a 3D wireframe appearance
        int offset = 2;
        QRect backRect = iconRect.adjusted(offset, -offset, offset, -offset);
        painter.drawRect(backRect);

        // Connect the corners to create 3D effect
        painter.drawLine(iconRect.topLeft(), backRect.topLeft());
        painter.drawLine(iconRect.topRight(), backRect.topRight());
        painter.drawLine(iconRect.bottomLeft(), backRect.bottomLeft());
        painter.drawLine(iconRect.bottomRight(), backRect.bottomRight());
    }

    // Draw text
    QRect textRect(iconRect.right() + 8, item.y_position - 2,
        itemRect.width() - (iconRect.right() + 4), ITEM_HEIGHT);

    painter.setPen(isSelected ? palette().color(QPalette::HighlightedText) : m_TextColor);
    painter.setFont(font());

    QString nodeText = QString::fromStdString(item.node->GetName());
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, nodeText);
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
    if (event->button() == Qt::LeftButton) {
        TreeItem* item = GetItemAtPosition(event->pos());
        if (item) {
            // Always set the selected node when clicking on any part of the item
            m_SelectedNode = item->node;
            emit NodeSelected(m_SelectedNode);

            // Store potential drag start info - ONLY if we clicked on an actual item
            m_DragStartPos = event->pos();
            m_LastMousePos = event->pos();
            m_PotentialDragNode = item->node;  // Store which node we might drag

            // Check if clicked to the left of the text (expander area)
            auto children = item->node->GetNodes();
            if (!children.empty()) {
                // Calculate the text start position
                int textStartX = item->depth * INDENT_SIZE + ICON_SIZE + 8 + ICON_SIZE + TEXT_MARGIN;

                // If clicked to the left of the text, toggle open/close state
                if (event->pos().x() < textStartX) {
                    // Toggle the open/close state
                    bool currentState = m_NodeOpenStates[item->node];
                    m_NodeOpenStates[item->node] = !currentState;

                    // Rebuild the tree with new states
                    BuildTreeItems();
                    UpdateLayout();
                    UpdateScrollBars();
                }
            }

            update();
        }
        else {
            // Clicked in blank space - clear selection but don't set up for dragging
            m_SelectedNode = nullptr;
            m_PotentialDragNode = nullptr;
            update();
        }
    }

    QWidget::mousePressEvent(event);
}

void NodeTree::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && m_PotentialDragNode && !m_IsDragging) {
        // Check if we've moved far enough to start dragging
        int distance = (event->pos() - m_DragStartPos).manhattanLength();
        if (distance >= DRAG_THRESHOLD && m_PotentialDragNode != m_RootNode) {
            // Only start drag if the current mouse position is still over a tree item
            TreeItem* currentItem = GetItemAtPosition(event->pos());
            if (currentItem) {
                StartDrag(m_PotentialDragNode, m_DragStartPos);
            }
        }
    }

    if (m_IsDragging) {
        m_LastMousePos = event->pos();

        // Find drop target
        TreeItem* item = GetItemAtPosition(event->pos());
        if (item && CanDropOn(m_DragNode, item->node)) {
            m_DropTargetNode = item->node;
            m_DropIndicatorY = item->y_position + ITEM_HEIGHT;
            m_ShowDropIndicator = true;
        }
        else {
            m_DropTargetNode = nullptr;
            m_ShowDropIndicator = false;
        }

        update();
    }

    QWidget::mouseMoveEvent(event);
}

void NodeTree::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_IsDragging) {
            // Perform drop if we have a valid target
            if (m_DropTargetNode && CanDropOn(m_DragNode, m_DropTargetNode)) {
                PerformDrop(m_DragNode, m_DropTargetNode);
            }

            // Reset drag state
            m_IsDragging = false;
            m_DragNode = nullptr;
            m_DropTargetNode = nullptr;
            m_ShowDropIndicator = false;
            unsetCursor(); // or setCursor(Qt::ArrowCursor);

            update();
        }

        // Clear potential drag node
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

void NodeTree::StartDrag(GraphNode* node, const QPoint& startPos)
{
    if (!node || node == m_RootNode) return;

    m_IsDragging = true;
    m_DragNode = node;
    setCursor(Qt::ClosedHandCursor);
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

    // Use the new RemoveNode method (preferred solution)
    parent->RemoveNode(node);
}

// Drag and drop event handlers (for external drops, if needed in future)
void NodeTree::dragEnterEvent(QDragEnterEvent* event)
{
    // For now, only accept internal drags
    event->acceptProposedAction();
}

void NodeTree::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void NodeTree::dragLeaveEvent(QDragLeaveEvent* event)
{
    m_ShowDropIndicator = false;
    update();
    event->accept();
}

void NodeTree::dropEvent(QDropEvent* event)
{
    event->acceptProposedAction();
}

void NodeTree::NodeSelected(GraphNode* node)
{
    SceneView::m_Instance->SelectNode(node);
}