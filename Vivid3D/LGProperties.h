#pragma once

#include <QWidget>

// Forward declarations
class LNode;
class QVBoxLayout;
class QScrollArea;
class LGraph;

class LGProperties : public QWidget
{
    Q_OBJECT

public:
    LGProperties(QWidget* parent = nullptr);
    ~LGProperties();
    void setGraph(LGraph* graph);

public slots:
    void inspectNode(LNode* node);

private:
    LNode* m_currentNode;
    LGraph* m_graph;

    // The scroll area is the main widget in this panel's layout.
    QScrollArea* m_scrollArea;
    // We will delete and recreate this container every time the selection changes.
    QWidget* m_propertyContainer;
};