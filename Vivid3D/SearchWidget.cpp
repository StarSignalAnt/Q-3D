#include "SearchWidget.h"
#include "ContentBrowser.h"

SearchWidget::SearchWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
    setMaximumWidth(256);
}

void SearchWidget::setupUI()
{
    // Create the layout
    m_layout = new QHBoxLayout(this);

    // Create the text edit (using QLineEdit for single line input)
    m_textEdit = new QLineEdit(this);
    m_textEdit->setPlaceholderText("Enter search term...");

    // Create the search button
    //m_searchButton = new QPushButton("Search", this);

    // Add widgets to layout
    m_layout->addWidget(m_textEdit);

    //m_layout->addWidget(m_searchButton);

    // Set layout margins
    m_layout->setContentsMargins(0,0,0,0);

    m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //m_searchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_textEdit->setMinimumHeight(24);
    //m_searchButton->setMinimumHeight(22);
    // Connect signals
    connect(m_textEdit, &QLineEdit::returnPressed, this, &SearchWidget::onSearchTriggered);

    // Connect search button click
//    connect(m_searchButton, &QPushButton::clicked, this, &SearchWidget::onSearchTriggered);

    // Connect text changed signal for real-time search
    connect(m_textEdit, &QLineEdit::textChanged, this, &SearchWidget::onSearchTriggered);
}

void SearchWidget::onSearchTriggered()
{
    QString searchText = m_textEdit->text();
    std::string searchTerm = searchText.toStdString();
    BeginSearch(searchTerm);
}

void SearchWidget::BeginSearch(std::string term)
{
    // This is where you would implement your search logic
    // For now, this is just a placeholder method

    // Example: Print the search term (you can replace this with your actual search implementation)
    qDebug() << "Searching for:" << QString::fromStdString(term);
    ContentBrowser::m_Instance->SetSearch(term);

    // Add your search implementation here
}