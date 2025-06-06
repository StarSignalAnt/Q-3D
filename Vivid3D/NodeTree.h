#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDrag>
#include <vector>
#include <unordered_map>
#include "GraphNode.h"

class NodeTree : public QWidget
{
    Q_OBJECT

public:
    explicit NodeTree(QWidget* parent = nullptr);
    ~NodeTree();

    void SetRoot(GraphNode* node);
    void ClearSelection();
    GraphNode* GetSelectedNode() const { return m_SelectedNode; }

    static NodeTree* m_Instance;

signals:
    void NodeDoubleClicked(GraphNode* node);
    void NodeStructureChanged(); // New signal for when structure changes

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // Drag and drop events
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    struct TreeItem {
        GraphNode* node;
        int depth;
        int y_position;
        QRect bounds;
        bool expanded;

        TreeItem(GraphNode* n, int d) : node(n), depth(d), y_position(0), expanded(true) {}
    };

    void BuildTreeItems();
    void BuildTreeItemsRecursive(GraphNode* node, int depth, std::vector<TreeItem>& items);
    void UpdateLayout();
    void DrawTreeItem(QPainter& painter, const TreeItem& item, bool isSelected);
    void DrawConnections(QPainter& painter);
    TreeItem* GetItemAtPosition(const QPoint& pos);
    void UpdateScrollBars();
    void ScrollTo(int y);

    // Drag and drop helper functions
    void StartDrag(GraphNode* node, const QPoint& startPos);
    bool CanDropOn(GraphNode* dragNode, GraphNode* dropTarget);
    void PerformDrop(GraphNode* dragNode, GraphNode* dropTarget);
    void RemoveNodeFromParent(GraphNode* node);
    void DrawDropIndicator(QPainter& painter);

    // Override this method to handle node selection
    virtual void NodeSelected(GraphNode* node);

    GraphNode* m_RootNode;
    GraphNode* m_SelectedNode;
    std::vector<TreeItem> m_TreeItems;
    std::unordered_map<GraphNode*, bool> m_NodeOpenStates;

    // Drag and drop state
    bool m_IsDragging;
    GraphNode* m_DragNode;
    GraphNode* m_PotentialDragNode;  // Node that might be dragged if threshold is met
    QPoint m_DragStartPos;
    QPoint m_LastMousePos;
    GraphNode* m_DropTargetNode;
    int m_DropIndicatorY;
    bool m_ShowDropIndicator;

    // Drawing constants
    static const int ITEM_HEIGHT = 24;
    static const int INDENT_SIZE = 20;
    static const int ICON_SIZE = 16;
    static const int TEXT_MARGIN = 4;
    static const int DRAG_THRESHOLD = 5; // Minimum pixels to start drag

    // Scrolling
    int m_ScrollY;
    int m_ContentHeight;
    QScrollBar* m_VerticalScrollBar;

    // Colors
    QColor m_BackgroundColor;
    QColor m_SelectionColor;
    QColor m_TextColor;
    QColor m_LineColor;
    QColor m_ExpanderColor;
    QColor m_DropIndicatorColor;

    QIcon EntityIcon;
    QIcon LightIcon;
    QIcon NodeIcon;
};