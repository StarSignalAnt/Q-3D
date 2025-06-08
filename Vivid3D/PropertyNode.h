#pragma once

#include <QWidget>
#include <functional>

// Forward declarations to keep the header clean
class QLabel;
class QLineEdit;
class QDragEnterEvent;
class QDropEvent;
class QDragLeaveEvent;
class GraphNode; // Forward declare GraphNode

/**
 * @class PropertyNode
 * @brief A property widget that displays a label and a text field, and acts as a drop target for GraphNode objects.
 *
 * This widget is designed to receive a GraphNode via a drag-and-drop operation from the NodeTree.
 * When a node is dropped onto it, the widget's text field is updated with the node's name,
 * and it emits a `nodeDropped` signal containing a pointer to the dropped node.
 */
class PropertyNode : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PropertyNode widget.
     * @param label The text to display on the label next to the text field.
     * @param defaultText The initial text to display in the text field.
     * @param parent The parent widget.
     */
    PropertyNode(const QString& label, const QString& defaultText = "", QWidget* parent = nullptr);
    ~PropertyNode();

    /**
     * @brief Sets the text of the line edit programmatically and updates its style.
     * @param text The new text to set.
     */
    void setText(const QString& text);

    /**
     * @brief Gets the current text from the line edit.
     * @return The current text.
     */
    QString getText() const;

signals:
    /**
     * @brief This signal is emitted when a valid GraphNode is dropped onto the widget.
     * @param node A pointer to the GraphNode that was dropped.
     */
    void nodeDropped(GraphNode* node);

protected:
    // Re-implemented from QWidget to handle drag-and-drop
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;

private:
    QLabel* m_label;
    QLineEdit* m_lineEdit;
};
