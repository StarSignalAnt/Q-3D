#pragma once
#include <QFrame>
#include <string>

// Forward declarations
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class NodeRegistry;
class LGraph; // Forward declare LGraph

class NodeCreationWidget : public QFrame {
    Q_OBJECT

public:
    // Constructor now accepts the graph to get the variable list
    NodeCreationWidget(LGraph* graph, NodeRegistry& registry, QWidget* parent = nullptr);

signals:
    void nodeSelected(const std::string& nodeName);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onCategorySelected(QListWidgetItem* item);
    void onNodeSelected(QListWidgetItem* item);
    void onSearchTextChanged(const QString& text);

private:
    void populateCategories();
    void populateNodes(const QString& categoryFilter, const QString& searchFilter);

    LGraph* m_graph; // Pointer to the current graph
    NodeRegistry& m_registry;
    QLineEdit* m_searchBar;
    QListWidget* m_categoryList;
    QListWidget* m_nodeList;
};