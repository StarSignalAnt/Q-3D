#pragma once

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QMouseEvent>
#include <QApplication>
#include <QScreen>
#include <QTimer> // Add this include

struct ComponentCategory {
    QString categoryName;
    QStringList componentTypes;
};

class ComponentSelector : public QWidget
{
    Q_OBJECT

public:
    ComponentSelector(QWidget* parent = nullptr);
    ~ComponentSelector();

    // Static method to get or create the singleton instance
    static ComponentSelector* getInstance(QWidget* parent = nullptr);

    // Add a category with its component types
    void addCategory(const QString& categoryName, const QStringList& componentTypes);

    // Clear all categories
    void clearCategories();

    // Show the popup near the mouse cursor
    void showNearMouse();

    // Show the popup at a specific position
    void showAt(const QPoint& position);

signals:
    void itemSelected(const QString& categoryName, const QString& componentType);

private slots:
    void onListItemClicked(QListWidgetItem* item);
    void onBackButtonClicked();

protected:
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUI();
    void showCategories();
    void showComponentsInCategory(const QString& categoryName);
    void positionWidget(const QPoint& position);

    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QPushButton* m_backButton;
    QListWidget* m_listWidget;
    QFrame* m_frame;

    QMap<QString, QStringList> m_categories;
    QString m_currentCategory;
    bool m_showingCategory;

    // Static instance for singleton pattern
    static ComponentSelector* s_instance;
};