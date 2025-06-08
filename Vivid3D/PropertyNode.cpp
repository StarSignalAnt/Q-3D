#include "PropertyNode.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMimeData>
#include <QDebug>
#include "GraphNode.h" // This must be included to access GraphNode properties

PropertyNode::PropertyNode(const QString& label, const QString& defaultText, QWidget* parent)
    : QWidget(parent)
{
    // Enable drop events for the entire widget.
    setAcceptDrops(true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // Create and configure the label
    m_label = new QLabel(label + ":");
    m_label->setMinimumWidth(100);
    m_label->setMaximumWidth(180);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Create the line edit
    m_lineEdit = new QLineEdit;
    m_lineEdit->setReadOnly(true); // The value is set by dropping, so it shouldn't be user-editable.
    m_lineEdit->setMinimumWidth(120);
    m_lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Set initial text and style
    setText(defaultText);

    // Add widgets to the layout
    layout->addWidget(m_label);
    layout->addWidget(m_lineEdit, 1); // The '1' makes the line edit expand.

    setFixedHeight(32);
}

PropertyNode::~PropertyNode()
{
}

void PropertyNode::setText(const QString& text)
{
    m_lineEdit->setText(text);

    // Update style to give a visual cue if a node has been assigned
    if (!text.isEmpty()) {
        m_lineEdit->setStyleSheet("color: white;");
    }
    else {
        m_lineEdit->setStyleSheet("color: #a0a0a0;"); // Greyed out if empty
    }
}

QString PropertyNode::getText() const
{
    return m_lineEdit->text();
}

void PropertyNode::dragEnterEvent(QDragEnterEvent* event)
{
    // Check if the drag contains the data format we expect from NodeTree.
    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        event->acceptProposedAction();
        // Provide visual feedback to the user that this is a valid drop target.
        m_lineEdit->setStyleSheet("background-color: lightblue; border: 1px solid blue;");
    }
    else {
        event->ignore();
    }
}

void PropertyNode::dropEvent(QDropEvent* event)
{
    // Reset the visual feedback style by reapplying the correct text style
    setText(m_lineEdit->text());

    if (event->mimeData()->hasFormat("application/x-graphnode")) {
        bool ok;
        // Extract the node's memory address from the MIME data.
        qulonglong addr = event->mimeData()->data("application/x-graphnode").toULongLong(&ok);
        if (ok) {
            GraphNode* droppedNode = reinterpret_cast<GraphNode*>(addr);
            if (droppedNode) {
                // Update the visual text
                setText(QString::fromStdString(droppedNode->GetName()));

                // Emit the signal with the actual node pointer
                emit nodeDropped(droppedNode);

                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void PropertyNode::dragLeaveEvent(QDragLeaveEvent* event)
{
    // If the user drags over but then leaves without dropping, reset the style.
    setText(m_lineEdit->text());
    event->accept();
}
