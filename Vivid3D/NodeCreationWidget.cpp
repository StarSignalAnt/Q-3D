#include "NodeCreationWidget.h"
#include "NodeRegistry.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QKeyEvent>
#include <set>

NodeCreationWidget::NodeCreationWidget(NodeRegistry& registry, QWidget* parent)
    : QFrame(parent), m_registry(registry)
{
    // Make the widget a floating window without a title bar
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
    listsLayout->addWidget(m_categoryList, 1); // 1/3 of the space
    listsLayout->addWidget(m_nodeList, 2);   // 2/3 of the space

    layout->addWidget(m_searchBar);
    layout->addLayout(listsLayout);

    populateCategories();
    populateNodes("", ""); // Initially, show all nodes

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
    m_categoryList->setCurrentRow(0);
}

void NodeCreationWidget::populateNodes(const QString& categoryFilter, const QString& searchFilter) {
    m_nodeList->clear();
    for (const auto& info : m_registry.GetAllNodes()) {
        bool categoryMatch = (categoryFilter == "All" || categoryFilter.isEmpty() || QString::fromStdString(info.category) == categoryFilter);
        bool searchMatch = (searchFilter.isEmpty() || QString::fromStdString(info.name).contains(searchFilter, Qt::CaseInsensitive));

        if (categoryMatch && searchMatch) {
            m_nodeList->addItem(QString::fromStdString(info.name));
        }
    }
}

void NodeCreationWidget::onCategorySelected(QListWidgetItem* item) {
    m_searchBar->clear(); // Clear search when category changes
    populateNodes(item->text(), "");
}

void NodeCreationWidget::onNodeSelected(QListWidgetItem* item) {
    emit nodeSelected(item->text().toStdString());
    close();
}

void NodeCreationWidget::onSearchTextChanged(const QString& text) {
    QString currentCategory = m_categoryList->currentItem() ? m_categoryList->currentItem()->text() : "All";
    populateNodes(currentCategory, text);
}