#include "ContentBrowser.h"
#include <qtoolbar.h>
#include "Content.h"
#include "SearchWidget.h"

ContentBrowser* ContentBrowser::m_Instance = nullptr;

ContentBrowser::ContentBrowser(QWidget* parent)
    : QWidget(parent)

{

    m_Instance = this;
  
    //}
    auto* layout = new QVBoxLayout(this);

    QScrollArea* scrollArea = new QScrollArea(this);

    // 2. Configure the scroll area.
    scrollArea->setWidgetResizable(true); // VERY IMPORTANT! Allows the inner widget to be resized.
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // We don't want a horizontal scrollbar.
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded); // This is the key!

    // Create toolbar
    QToolBar* toolbar = new QToolBar("My Toolbar", this);
   // QAction* action1 = toolbar->addAction("Action 1");
   // QAction* action2 = toolbar->addAction("Action 2");

    auto search = new SearchWidget;

    //toolbar->setMaximumHeight(20);

    toolbar->setFixedHeight(22);

    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // fixes height to preferred


    auto size = new QWidget;
    size->setMinimumWidth(32);
    toolbar->addWidget(size);
    toolbar->addWidget(search);
    // Example content
    Content* content = new Content(this);
    scrollArea->setWidget(content);

    m_Content = content;

    // Add toolbar and content to layout
    layout->addWidget(toolbar);
    layout->addWidget(scrollArea);

    layout->setContentsMargins(0, 0, 0, 0); // Optional: remove margins
    setLayout(layout);

    // Optional: connect actions
    //connect(action1, &QAction::triggered, this, []() {
    //    qDebug("Action 1 triggered");
   //     });



}

void ContentBrowser::Browse(std::string path) {

    m_Content->Browse(path);

}

void ContentBrowser::SetSearch(std::string term) {

    m_Content->SetSearch(term);

}