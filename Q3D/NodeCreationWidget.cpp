#include "NodeCreationWidget.h"
#include "NodeRegistry.h"
#include "LGraph.h"
#include "LGraphVariable.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QKeyEvent>
#include <set>

NodeCreationWidget::NodeCreationWidget(LGraph* graph, NodeRegistry& registry, QWidget* parent)
    : QFrame(parent), m_graph(graph), m_registry(registry)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setFrameStyle(Panel | Sunken);
    setStyleSheet("background-color: #383838; color: white; border: 1px solid #555;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search Nodes...");
    m_searchBar->setStyleSheet("border: 1px solid #555;");

    QHBoxLayout* listsLayout = new QHBoxLayout();
    m_categoryList = new QListWidget();
    m_nodeList = new QListWidget();
    listsLayout->addWidget(m_categoryList, 1);
    listsLayout->addWidget(m_nodeList, 2);

    layout->addWidget(m_searchBar);
    layout->addLayout(listsLayout);

    populateCategories();
    populateNodes("All", "");

    connect(m_searchBar, &QLineEdit::textChanged, this, &NodeCreationWidget::onSearchTextChanged);
    connect(m_categoryList, &QListWidget::itemClicked, this, &NodeCreationWidget::onCategorySelected);
    connect(m_nodeList, &QListWidget::itemActivated, this, &NodeCreationWidget::onNodeSelected);

    m_searchBar->setFocus();
    resize(450, 300);
}

void NodeCreationWidget::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        close();
    }
    else {
        QFrame::keyPressEvent(event);
    }
}

void NodeCreationWidget::populateCategories() {
    m_categoryList->clear();
    m_categoryList->addItem("All");

    std::set<std::string> uniqueCategories;
    for (const auto& info : m_registry.GetAllNodes()) {
        uniqueCategories.insert(info.category);
    }
    for (const auto& category : uniqueCategories) {
        m_categoryList->addItem(QString::fromStdString(category));
    }

    // Add the "Variables" category if the graph exists and has variables
    if (m_graph && !m_graph->GetVariables().empty()) {
        m_categoryList->addItem("Variables");
    }
    m_categoryList->setCurrentRow(0);
}

void NodeCreationWidget::populateNodes(const QString& categoryFilter, const QString& searchFilter) {
    m_nodeList->clear();

    // --- Handle the special "Variables" category ---
    if (m_graph && categoryFilter == "Variables") {
        for (LGraphVariable* var : m_graph->GetVariables()) {
            std::string varName = var->GetName();

            // Create "Get" and "Set" entries for the variable
            QString getText = QString("Get %1").arg(QString::fromStdString(varName));
            QString setText = QString("Set %1").arg(QString::fromStdString(varName));

            if (searchFilter.isEmpty() || getText.contains(searchFilter, Qt::CaseInsensitive)) {
                auto* getItem = new QListWidgetItem(getText);
                // Store the special command string that LGDesigner will parse
                getItem->setData(Qt::UserRole, QVariant(QString::fromStdString("Get " + varName)));
                m_nodeList->addItem(getItem);
            }
            if (searchFilter.isEmpty() || setText.contains(searchFilter, Qt::CaseInsensitive)) {
                auto* setItem = new QListWidgetItem(setText);
                setItem->setData(Qt::UserRole, QVariant(QString::fromStdString("Set " + varName)));
                m_nodeList->addItem(setItem);
            }
        }
        return; // Stop here for the variables category
    }

    // --- Handle standard registered nodes ---
    for (const auto& info : m_registry.GetAllNodes()) {
        bool categoryMatch = (categoryFilter == "All" || categoryFilter.isEmpty() || QString::fromStdString(info.category) == categoryFilter);
        bool searchMatch = (searchFilter.isEmpty() || QString::fromStdString(info.name).contains(searchFilter, Qt::CaseInsensitive));

        if (categoryMatch && searchMatch) {
            auto* nodeItem = new QListWidgetItem(QString::fromStdString(info.name));
            // Store the internal, stable typeName to be emitted
            nodeItem->setData(Qt::UserRole, QVariant(QString::fromStdString(info.typeName)));
            m_nodeList->addItem(nodeItem);
        }
    }
}

void NodeCreationWidget::onCategorySelected(QListWidgetItem* item) {
    m_searchBar->clear();
    populateNodes(item->text(), "");
}

void NodeCreationWidget::onNodeSelected(QListWidgetItem* item) {
    if (!item) return;

    // Always emit the internal name/command stored in the UserRole data
    QVariant data = item->data(Qt::UserRole);
    if (data.isValid()) {
        emit nodeSelected(data.toString().toStdString());
    }
    close();
}

void NodeCreationWidget::onSearchTextChanged(const QString& text) {
    QString currentCategory = m_categoryList->currentItem() ? m_categoryList->currentItem()->text() : "All";
    populateNodes(currentCategory, text);
}