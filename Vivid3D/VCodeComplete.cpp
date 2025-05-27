#include "VCodeComplete.h"
#include <QTextEdit>
#include <QTextCursor>
#include <QListWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QPoint>
#include <QRect>
#include <QTextDocument>
VCodeComplete::VCodeComplete(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    QListWidget* listWidget = new QListWidget(this);
    //listWidget->addItems(suggestions);
    layout->addWidget(listWidget);
    setLayout(layout);
    setMaximumSize(168, 256);
    setMinimumSize(168, 256);
    m_List = listWidget;

}


bool isMatchOrPrefix(const std::string& searchTerm, const std::string& item) {
    // Convert both strings to lowercase for case-insensitive comparison
    std::string lowerSearchTerm = searchTerm;
    std::string lowerItem = item;
    std::transform(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerSearchTerm.begin(), ::tolower);
    std::transform(lowerItem.begin(), lowerItem.end(), lowerItem.begin(), ::tolower);

    // Check if searchTerm is equal to item
    if (lowerSearchTerm == lowerItem) {
        return true;
    }

    // Check if searchTerm is a prefix of item
    if (lowerSearchTerm.length() <= lowerItem.length()) {
        return std::equal(lowerSearchTerm.begin(), lowerSearchTerm.end(), lowerItem.begin());
    }

    return false;
}


void VCodeComplete::AddItem(std::string item) {

    if (isMatchOrPrefix(m_Search, item)) {
        m_List->addItem(item.c_str());
        if (m_List->count() == 1)
        {
            m_List->setCurrentRow(0);
        }
    }

}

VCodeComplete::~VCodeComplete()
{}
