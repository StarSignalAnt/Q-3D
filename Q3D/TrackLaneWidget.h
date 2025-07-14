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
#include "Cinematic.h"

class TrackLaneWidget : public QWidget
{
    Q_OBJECT

public:
    // --- MODIFIED ---
    explicit TrackLaneWidget(ITrack* track, QWidget* parent = nullptr);

public slots:
    // --- ADDED ---
    void setScrollOffset(int offset);
signals:
    void keyframeDragStarted();
    void keyframeDragFinished();
protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    // --- ADDED --- Override for the right-click menu event.
    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    // --- ADDED ---
    ITrack* m_track;
    int m_scrollOffset = 0;
    std::optional<size_t> m_draggedKeyframeIndex;
};
