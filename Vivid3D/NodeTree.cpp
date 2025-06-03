#include "NodeTree.h"
#include <QPaintEvent>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <algorithm>
#include "SCeneView.h"

NodeTree* NodeTree::m_Instance = nullptr;

NodeTree::NodeTree(QWidget* parent)
    : QWidget(parent)
    , m_RootNode(nullptr)
    , m_SelectedNode(nullptr)
    , m_ScrollY(0)
    , m_ContentHeight(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_Instance = this;

    // Initialize colors
    m_BackgroundColor = palette().color(QPalette::Base);
    m_SelectionColor = palette().color(QPalette::Highlight);
    m_TextColor = palette().color(QPalette::Text);
    m_LineColor = palette().color(QPalette::Mid);
    m_ExpanderColor = palette().color(QPalette::ButtonText);

    // Create vertical scroll bar
    m_VerticalScrollBar = new QScrollBar(Qt::Vertical, this);
    m_VerticalScrollBar->hide();
    connect(m_VerticalScrollBar, &QScrollBar::valueChanged, this, &NodeTree::ScrollTo);

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
            DrawTreeItem(painter, item, item.node == m_SelectedNode);
        }
    }
}void NodeTree::DrawTreeItem(QPainter& painter, const TreeItem& item, bool isSelected)
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

    // Draw node icon (wireframe box instead of circle) - smaller and centered with text
    int iconSize = 9;  // 25% smaller than 12 (12 * 0.75 = 9)
    int iconY = item.y_position + (ITEM_HEIGHT - iconSize) / 2;  // Center vertically with text
    QRect iconRect(item.depth * INDENT_SIZE + ICON_SIZE + 8, iconY, iconSize, iconSize);

    painter.setPen(QPen(QColor(144, 238, 144), 1));  // Light green color
    painter.setBrush(Qt::NoBrush);  // No fill for wireframe

    // Draw main box outline
    painter.drawRect(iconRect);

    // Draw additional lines to give it a 3D wireframe appearance
    int offset = 2;  // Keep same offset for 3D effect
    QRect backRect = iconRect.adjusted(offset, -offset, offset, -offset);
    painter.drawRect(backRect);

    // Connect the corners to create 3D effect
    painter.drawLine(iconRect.topLeft(), backRect.topLeft());
    painter.drawLine(iconRect.topRight(), backRect.topRight());
    painter.drawLine(iconRect.bottomLeft(), backRect.bottomLeft());
    painter.drawLine(iconRect.bottomRight(), backRect.bottomRight());

    // Draw text
    QRect textRect(iconRect.right() + 8, item.y_position-2,
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
    }

    QWidget::mousePressEvent(event);
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

void NodeTree::NodeSelected(GraphNode* node)
{

    SceneView::m_Instance->SelectNode(node);

}