#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QIcon>
#include <string>
#include <vector>
#include <map>

enum ComponentType {
    CT_Actor, CT_Component, CT_SharpComponent
};

// Struct to hold information about a single component
struct ComponentInfo {
    std::string name;
    std::string category;
    void* componentPtr = nullptr;
    ComponentType type;
};

// Enum to represent the type of item in the browser
enum class ItemType {
    Category,
    Component
};

// Enum to represent the current view state
enum class ViewMode {
    Categories,
    Components
};

// Internal struct for drawing and interaction
struct DisplayItem {
    QString displayName;
    ItemType type;
    QRect rect;
    ComponentType componentType; // Store the type for drawing
    // Only used for component items
    void* componentPtr = nullptr;
};

class ComponentContent : public QWidget
{
    Q_OBJECT

public:
    ComponentContent(QWidget* parent = nullptr);
    ~ComponentContent();

    void SetComponents(const std::vector<ComponentInfo>& components);
    void SetSearch(std::string term);
    void GoBack();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void calculateLayout();
    void itemClicked(const DisplayItem& item);
    bool containsString(const std::string& text, const std::string& search);
    ComponentType getCategoryType(const QString& categoryName);


    // Data storage
    std::vector<ComponentInfo> m_allComponents;
    std::vector<QString> m_categoryNames;

    // View state
    ViewMode m_currentView;
    QString m_currentCategory;
    std::vector<DisplayItem> m_displayItems;

    // Drawing and layout
    int m_itemSize;
    bool m_layoutDirty;
    int m_contentHeight;

    // Icons
    QIcon m_categoryIcon;
    QIcon m_componentIcon;

    // Interaction
    DisplayItem* m_OverItem = nullptr;
    std::string m_SearchTerm = "";
    QPoint m_dragStartPosition;
};
