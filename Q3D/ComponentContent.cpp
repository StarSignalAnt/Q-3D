#include "ComponentContent.h"
#include <QPainter>
#include <QApplication>
#include <QStyle>
#include <QFontMetrics>
#include <QDrag>
#include <QMimeData>
#include <QDebug>
#include <algorithm>
#include <set>

ComponentContent::ComponentContent(QWidget* parent)
    : QWidget(parent)
    , m_currentView(ViewMode::Categories)
    , m_itemSize(64)
    , m_layoutDirty(true)
    , m_contentHeight(250)
    , m_OverItem(nullptr)
{
    setMinimumHeight(250);
    setMouseTracking(true);

    // Load icons using paths from the original Content Browser
    m_categoryIcon = QIcon("edit/icons/folderIcon.png");
    m_componentIcon = QIcon("edit/icons/scripticon.png");

    // Provide fallbacks if icons are not found
    if (m_categoryIcon.isNull()) {
        m_categoryIcon = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    }
    if (m_componentIcon.isNull()) {
        m_componentIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
}

ComponentContent::~ComponentContent()
{
}

void ComponentContent::SetComponents(const std::vector<ComponentInfo>& components)
{
    m_allComponents = components;

    // Extract unique category names
    std::set<QString> uniqueCategories;
    for (const auto& info : m_allComponents) {
        uniqueCategories.insert(QString::fromStdString(info.category));
    }
    m_categoryNames.assign(uniqueCategories.begin(), uniqueCategories.end());
    std::sort(m_categoryNames.begin(), m_categoryNames.end());

    GoBack(); // Start at the category view
}

void ComponentContent::SetSearch(std::string term)
{
    m_SearchTerm = term;
    std::transform(m_SearchTerm.begin(), m_SearchTerm.end(), m_SearchTerm.begin(),
        [](unsigned char c) { return std::tolower(c); });
    m_layoutDirty = true;
    update();
}

void ComponentContent::GoBack()
{
    m_currentView = ViewMode::Categories;
    m_currentCategory.clear();
    m_layoutDirty = true;
    update();
}

bool ComponentContent::containsString(const std::string& text, const std::string& search)
{
    if (search.empty()) {
        return true;
    }
    std::string lowerText = text;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return lowerText.find(search) != std::string::npos;
}

ComponentType ComponentContent::getCategoryType(const QString& categoryName)
{
    for (const auto& component : m_allComponents)
    {
        if (QString::fromStdString(component.category) == categoryName)
        {
            return component.type;
        }
    }
    return CT_Component; // Default
}


void ComponentContent::calculateLayout()
{
    if (!m_layoutDirty) return;

    m_displayItems.clear();

    // Populate display items based on the current view
    if (m_currentView == ViewMode::Categories)
    {
        for (const auto& categoryName : m_categoryNames)
        {
            if (m_SearchTerm.empty() || containsString(categoryName.toStdString(), m_SearchTerm))
            {
                DisplayItem item;
                item.displayName = categoryName;
                item.type = ItemType::Category;
                item.componentType = getCategoryType(categoryName);
                m_displayItems.push_back(item);
            }
        }
    }
    else // ViewMode::Components
    {
        for (const auto& componentInfo : m_allComponents)
        {
            if (QString::fromStdString(componentInfo.category) == m_currentCategory)
            {
                if (m_SearchTerm.empty() || containsString(componentInfo.name, m_SearchTerm))
                {
                    DisplayItem item;
                    item.displayName = QString::fromStdString(componentInfo.name);
                    item.type = ItemType::Component;
                    item.componentType = componentInfo.type;
                    item.componentPtr = componentInfo.componentPtr;
                    m_displayItems.push_back(item);
                }
            }
        }
    }


    // Calculate rectangles for the visible items
    int x = 10;
    int y = 10;
    const int itemWidth = m_itemSize + 40;
    const int itemHeight = m_itemSize + 25 + 5;
    const int viewWidth = width();

    for (size_t i = 0; i < m_displayItems.size(); ++i) {
        QRect itemRect(x, y, m_itemSize, itemHeight);
        m_displayItems[i].rect = itemRect;

        x += itemWidth;
        if (x > (viewWidth - m_itemSize)) {
            x = 10;
            y += 92;
        }
    }

    m_contentHeight = y + 92 + 10;
    setFixedHeight(m_contentHeight);
    updateGeometry();
    m_layoutDirty = false;
}

void ComponentContent::paintEvent(QPaintEvent* event)
{
    if (m_layoutDirty) {
        calculateLayout();
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // For smooth rounded corners

    if (m_displayItems.empty()) {
        painter.setPen(palette().text().color());
        painter.drawText(rect(), Qt::AlignCenter, "No items to display");
        return;
    }

    // --- PASS 1: Draw icons, highlights, and type labels ---
    for (const DisplayItem& item : m_displayItems) {
        if (item.rect.isNull()) continue;

        QRect iconRect(item.rect.x(), item.rect.y(), m_itemSize, m_itemSize);
        bool isHovered = (&item == m_OverItem);

        if (isHovered) {
            painter.setBrush(QColor(255, 255, 255, 30));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(iconRect.adjusted(-2, -2, 2, 2), 5, 5);
        }

        if (item.type == ItemType::Category) {
            m_categoryIcon.paint(&painter, iconRect);
        }
        else {
            m_componentIcon.paint(&painter, iconRect);
        }

        // --- Draw C++/C# Label ---
        QString typeLabel;
        if (item.componentType == CT_SharpComponent) {
            typeLabel = "C#";
        }
        else if (item.componentType == CT_Component || item.componentType == CT_Actor) {
            typeLabel = "CPP";
        }

        if (!typeLabel.isEmpty()) {
            QFont typeFont = font();
            typeFont.setPointSize(7);
            painter.setFont(typeFont);
            painter.setPen(Qt::white);

            QRectF textRect = iconRect;
            textRect.setHeight(textRect.height() - 4); // Add some padding
            painter.drawText(textRect, Qt::AlignBottom | Qt::AlignRight, typeLabel);
        }
    }

    // --- PASS 2: Draw non-hovered text ---
    for (const DisplayItem& item : m_displayItems) {
        if (item.rect.isNull() || &item == m_OverItem) continue; // Skip hovered item

        QRect textRect;
        if (item.type == ItemType::Category) {
            textRect = QRect(item.rect.x(), item.rect.y() + m_itemSize - 12, m_itemSize, 40);
        }
        else {
            const int itemCellWidth = m_itemSize + 40;
            textRect = QRect(item.rect.x(), item.rect.y() + m_itemSize - 12, itemCellWidth, 40);
        }

        painter.setFont(font()); // Reset font
        painter.setPen(palette().text().color());
        QFontMetrics fm(font());
        QString elidedText = fm.elidedText(item.displayName, Qt::ElideRight, textRect.width());
        painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, elidedText);
    }

    // --- PASS 3: Draw hovered text on top ---
    if (m_OverItem) {
        QFontMetrics fm(font());

        int textPadding = 5;
        QRect textBoundingRect = fm.boundingRect(m_OverItem->displayName);
        QRect fullTextRect(0, 0, textBoundingRect.width() + textPadding * 2, textBoundingRect.height() + textPadding * 2 + 2);

        int standardTextTopY = m_OverItem->rect.y() + m_itemSize - 12;
        int standardTextCenterY = standardTextTopY + 20;

        fullTextRect.moveCenter(QPoint(m_OverItem->rect.center().x(), standardTextCenterY));

        if (fullTextRect.right() > width() - 5) {
            fullTextRect.moveRight(width() - 5);
        }
        if (fullTextRect.left() < 5) {
            fullTextRect.moveLeft(5);
        }

        painter.setBrush(palette().color(QPalette::ToolTipBase));
        painter.setPen(palette().color(QPalette::ToolTipText));
        painter.drawRoundedRect(fullTextRect, 5, 5);

        painter.setPen(palette().text().color());
        painter.drawText(fullTextRect, Qt::AlignCenter, m_OverItem->displayName);
    }
}


void ComponentContent::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        for (const auto& item : m_displayItems) {
            if (item.rect.contains(event->pos())) {
                itemClicked(item);
                return;
            }
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void ComponentContent::itemClicked(const DisplayItem& item)
{
    if (item.type == ItemType::Category)
    {
        m_currentView = ViewMode::Components;
        m_currentCategory = item.displayName;
        m_layoutDirty = true;
        update();
    }
    else // ItemType::Component
    {
        qDebug() << "Component double-clicked:" << item.displayName;
        qDebug() << "Category:" << m_currentCategory;
        qDebug() << "Pointer:" << item.componentPtr;
        // Example: PropertiesEditor::m_Instance->SetComponent(item.componentPtr);
    }
}

void ComponentContent::mouseMoveEvent(QMouseEvent* event)
{
    DisplayItem* previousOverItem = m_OverItem;
    m_OverItem = nullptr;

    for (auto& item : m_displayItems) {
        if (item.rect.contains(event->pos())) {
            m_OverItem = &item;
            break;
        }
    }

    if (previousOverItem != m_OverItem) {
        update();
    }

    if (!(event->buttons() & Qt::LeftButton)) return;
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) return;

    if (m_OverItem && m_OverItem->type == ItemType::Component) {
        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;
        mimeData->setText(m_OverItem->displayName); // Simplistic drag data
        drag->setMimeData(mimeData);
        QPixmap dragPixmap = m_componentIcon.pixmap(QSize(m_itemSize, m_itemSize));
        drag->setPixmap(dragPixmap);
        drag->setHotSpot(QPoint(dragPixmap.width() / 2, dragPixmap.height() / 2));
        drag->exec(Qt::CopyAction);
    }
}

void ComponentContent::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::BackButton)
    {
        GoBack();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void ComponentContent::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (event->oldSize().width() != event->size().width()) {
        m_layoutDirty = true;
    }
}

void ComponentContent::leaveEvent(QEvent* event)
{
    if (m_OverItem) {
        m_OverItem = nullptr;
        update();
    }
    QWidget::leaveEvent(event);
}

QSize ComponentContent::sizeHint() const
{
    return QSize(400, m_contentHeight);
}

QSize ComponentContent::minimumSizeHint() const
{
    return QSize(200, 250);
}
