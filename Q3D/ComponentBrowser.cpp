#include "ComponentBrowser.h"
#include "ComponentContent.h"
#include "SearchWidget.h" // Assuming you have a search widget
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QAction>
#include <QStyle>

ComponentBrowser* ComponentBrowser::m_Instance = nullptr;

ComponentBrowser::ComponentBrowser(QWidget* parent)
    : QWidget(parent)
{
    m_Instance = this;

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create and configure the scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Create toolbar
    QToolBar* toolbar = new QToolBar("Component Toolbar", this);
    toolbar->setFixedHeight(22);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Add a back button
    QAction* backAction = new QAction(this);
    backAction->setIcon(this->style()->standardIcon(QStyle::SP_ArrowLeft));
    backAction->setToolTip("Back to categories");
    connect(backAction, &QAction::triggered, this, &ComponentBrowser::GoBack);
    toolbar->addAction(backAction);


    // Add search widget to the toolbar
    SearchWidget* search = new SearchWidget; // Assuming SearchWidget exists and is compatible
    toolbar->addWidget(search);
    // You might need to connect the search widget's signal to the SetSearch slot
    // connect(search, &SearchWidget::searchChanged, this, &ComponentBrowser::SetSearch);

    // Create the component content widget
    m_ComponentContent = new ComponentContent(this);
    scrollArea->setWidget(m_ComponentContent);

    // Add toolbar and content to the layout
    layout->addWidget(toolbar);
    layout->addWidget(scrollArea);

    setLayout(layout);
}

void ComponentBrowser::SetComponents(const std::vector<ComponentInfo>& components)
{
    if (m_ComponentContent) {
        m_ComponentContent->SetComponents(components);
    }
}

void ComponentBrowser::SetSearch(std::string term)
{
    if (m_ComponentContent) {
        m_ComponentContent->SetSearch(term);
    }
}

void ComponentBrowser::GoBack()
{
    if (m_ComponentContent)
    {
        m_ComponentContent->GoBack();
    }
}
