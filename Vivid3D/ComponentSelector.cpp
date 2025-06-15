// ComponentSelector.cpp - Fixed version
#include "ComponentSelector.h"
#include <QApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QDebug>
#include <QTimer>

// Initialize static instance
ComponentSelector* ComponentSelector::s_instance = nullptr;

ComponentSelector::ComponentSelector(QWidget* parent)
    : QWidget(parent), m_showingCategory(false)
{
    setupUI();

    // CRITICAL FIX: Use Qt::Popup instead of Qt::Tool
    // Qt::Popup automatically handles focus and closes when clicked outside
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);

    // Remove these problematic attributes
    // setAttribute(Qt::WA_DeleteOnClose);  // This can cause premature deletion
    // setAttribute(Qt::WA_ShowWithoutActivating, false);

    // Set focus policy to receive focus and key events
    setFocusPolicy(Qt::StrongFocus);

    // Install event filter to catch focus loss
    installEventFilter(this);
}

ComponentSelector::~ComponentSelector()
{
    // Clear static instance when destroyed
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

ComponentSelector* ComponentSelector::getInstance(QWidget* parent)
{
    // FIXED: Always create a new instance instead of reusing
    // This prevents issues with deleted widgets
    if (s_instance) {
        s_instance->deleteLater();
        s_instance = nullptr;
    }

    s_instance = new ComponentSelector(parent);
    return s_instance;
}

void ComponentSelector::setupUI()
{
    // Set fixed size
    setFixedSize(250, 300);

    // Create main frame for border
    m_frame = new QFrame(this);
    m_frame->setFrameStyle(QFrame::Box | QFrame::Raised);
    m_frame->setLineWidth(1);
    m_frame->setStyleSheet("QFrame { background-color: #404040; border: 1px solid #606060; }");

    // Main layout
    m_layout = new QVBoxLayout(m_frame);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(5);

    // Title label
    m_titleLabel = new QLabel("Select Component");
    m_titleLabel->setStyleSheet("QLabel { font-weight: bold; color: white; background: transparent; }");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_titleLabel);

    // Back button (initially hidden)
    m_backButton = new QPushButton("← Back");
    m_backButton->setStyleSheet("QPushButton { background-color: #505050; color: white; border: 1px solid #606060; padding: 4px; }");
    m_backButton->setVisible(false);
    m_layout->addWidget(m_backButton);

    // List widget
    m_listWidget = new QListWidget();
    m_listWidget->setStyleSheet(
        "QListWidget { "
        "   background-color: #353535; "
        "   color: white; "
        "   border: 1px solid #606060; "
        "   selection-background-color: #505050; "
        "} "
        "QListWidget::item { "
        "   padding: 5px; "
        "   border-bottom: 1px solid #505050; "
        "} "
        "QListWidget::item:hover { "
        "   background-color: #454545; "
        "} "
        "QListWidget::item:selected { "
        "   background-color: #505050; "
        "}"
    );
    m_layout->addWidget(m_listWidget);

    // CRITICAL FIX: Make sure the frame fills the entire widget
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_frame);

    // Connect signals
    connect(m_listWidget, &QListWidget::itemClicked, this, &ComponentSelector::onListItemClicked);
    connect(m_backButton, &QPushButton::clicked, this, &ComponentSelector::onBackButtonClicked);
}

void ComponentSelector::addCategory(const QString& categoryName, const QStringList& componentTypes)
{
    m_categories[categoryName] = componentTypes;
    qDebug() << "ComponentSelector: Added category" << categoryName << "with" << componentTypes.size() << "components";
}

void ComponentSelector::clearCategories()
{
    m_categories.clear();
}

void ComponentSelector::showNearMouse()
{
    QPoint mousePos = QCursor::pos();
    showAt(mousePos);
}

void ComponentSelector::showAt(const QPoint& position)
{
    // CRITICAL FIX: Ensure we have categories before showing
    if (m_categories.isEmpty()) {
        qDebug() << "ComponentSelector: No categories to show!";
        return;
    }

    // Reset to show categories
    showCategories();

    // Position the widget
    positionWidget(position);

    // Debug output
    qDebug() << "ComponentSelector: Attempting to show at" << position;
    qDebug() << "ComponentSelector: Final position" << pos();
    qDebug() << "ComponentSelector: Size" << size();
    qDebug() << "ComponentSelector: Categories count:" << m_categories.size();

    // FIXED: Simpler show approach
    show();
    raise();
    activateWindow();

    // Give focus to the list widget for keyboard navigation
    m_listWidget->setFocus();

    qDebug() << "ComponentSelector: isVisible():" << isVisible();
    qDebug() << "ComponentSelector: windowFlags():" << windowFlags();
}

void ComponentSelector::positionWidget(const QPoint& position)
{
    QPoint newPos = position;

    // Get screen geometry
    QScreen* screen = QApplication::screenAt(position);
    if (!screen) {
        screen = QApplication::primaryScreen();
    }
    QRect screenGeometry = screen->geometry();

    // Adjust position to keep widget on screen
    if (newPos.x() + width() > screenGeometry.right()) {
        newPos.setX(screenGeometry.right() - width());
    }
    if (newPos.x() < screenGeometry.left()) {
        newPos.setX(screenGeometry.left());
    }

    if (newPos.y() + height() > screenGeometry.bottom()) {
        newPos.setY(position.y() - height());
    }
    if (newPos.y() < screenGeometry.top()) {
        newPos.setY(screenGeometry.top());
    }

    move(newPos);
}

void ComponentSelector::showCategories()
{
    m_showingCategory = false;
    m_currentCategory.clear();

    m_titleLabel->setText("Select Component");
    m_backButton->setVisible(false);

    m_listWidget->clear();

    // Add categories to list
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        QListWidgetItem* item = new QListWidgetItem(it.key());
        item->setData(Qt::UserRole, "category");
        m_listWidget->addItem(item);
    }

    // Debug: check if items were added
    qDebug() << "ComponentSelector: Added" << m_listWidget->count() << "categories";

    // DEBUGGING: Print all categories
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        qDebug() << "Category:" << it.key() << "Components:" << it.value();
    }
}

void ComponentSelector::showComponentsInCategory(const QString& categoryName)
{
    if (!m_categories.contains(categoryName)) {
        qDebug() << "ComponentSelector: Category not found:" << categoryName;
        return;
    }

    m_showingCategory = true;
    m_currentCategory = categoryName;

    m_titleLabel->setText(categoryName);
    m_backButton->setVisible(true);

    m_listWidget->clear();

    // Add components to list
    const QStringList& components = m_categories[categoryName];
    for (const QString& component : components) {
        QListWidgetItem* item = new QListWidgetItem(component);
        item->setData(Qt::UserRole, "component");
        m_listWidget->addItem(item);
    }

    qDebug() << "ComponentSelector: Added" << components.size() << "components for category" << categoryName;
}

void ComponentSelector::onListItemClicked(QListWidgetItem* item)
{
    if (!item) return;

    QString itemType = item->data(Qt::UserRole).toString();
    qDebug() << "ComponentSelector: Item clicked:" << item->text() << "Type:" << itemType;

    if (itemType == "category") {
        // Show components in this category
        showComponentsInCategory(item->text());
    }
    else if (itemType == "component") {
        // Emit signal with selected component
        qDebug() << "ComponentSelector: Emitting itemSelected for:" << m_currentCategory << item->text();
        emit itemSelected(m_currentCategory, item->text());

        // FIXED: Use deleteLater() instead of close() to ensure proper cleanup
        deleteLater();
        s_instance = nullptr;
    }
}

void ComponentSelector::onBackButtonClicked()
{
    showCategories();
}

// FIXED: Simplified focus handling
void ComponentSelector::focusOutEvent(QFocusEvent* event)
{
    // Only close if focus was lost to something outside this widget
    if (event->reason() != Qt::PopupFocusReason) {
        qDebug() << "ComponentSelector: Lost focus, closing. Reason:" << event->reason();
        deleteLater();
        s_instance = nullptr;
    }
    QWidget::focusOutEvent(event);
}

bool ComponentSelector::eventFilter(QObject* obj, QEvent* event)
{
    // Simplified event filtering - let Qt::Popup handle most cases
    return QWidget::eventFilter(obj, event);
}

void ComponentSelector::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        deleteLater();
        s_instance = nullptr;
    }
    else if (event->key() == Qt::Key_Backspace && m_showingCategory) {
        onBackButtonClicked();
    }

    QWidget::keyPressEvent(event);
}