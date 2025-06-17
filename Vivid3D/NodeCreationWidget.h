#pragma once
#include <QFrame>
#include <string>

// Forward declarations
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class NodeRegistry;

class NodeCreationWidget : public QFrame {
    Q_OBJECT

public:
    NodeCreationWidget(NodeRegistry& registry, QWidget* parent = nullptr);

signals:
    // Emitted when the user selects a node to be created
    void nodeSelected(const std::string& nodeName);

protected:
    // Close the widget if the Escape key is pressed
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onCategorySelected(QListWidgetItem* item);
    void onNodeSelected(QListWidgetItem* item);
    void onSearchTextChanged(const QString& text);

private:
    void populateCategories();
    void populateNodes(const QString& categoryFilter, const QString& searchFilter);

    NodeRegistry& m_registry;
    QLineEdit* m_searchBar;
    QListWidget* m_categoryList;
    QListWidget* m_nodeList;
};