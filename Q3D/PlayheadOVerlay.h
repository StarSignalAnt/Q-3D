#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollBar>
#include <QToolBar>
#include <QAction>
#include <QSplitter>
#include <QPushButton>
#include <QGridLayout>
#include <memory>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QTimer> 
class PlayheadOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit PlayheadOverlay(QWidget* parent = nullptr);

public slots:
    void setCurrentTime(float time);
    void setScrollOffset(int offset);
    // --- ADDED --- Slot to receive the global dragging state.
    void setKeyframeDragInProgress(bool isDragging);
signals:
    void timeChanged(float newTime);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    float timeFromPos(const QPoint& pos);
    void forwardEvent(QMouseEvent* event);
    void forwardContextMenuEvent(QContextMenuEvent* event);


    float m_currentTime = 0.0f;
    int m_scrollOffset = 0;
    bool m_keyframeDragInProgress = false;
};

