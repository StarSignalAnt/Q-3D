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

class TimeRuler : public QWidget
{
    Q_OBJECT

public:
    explicit TimeRuler(QWidget* parent = nullptr);
    void setScrollOffset(int offset);
    void setCurrentTime(float time);

signals:
    void timeChanged(float newTime);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    float timeFromPos(const QPoint& pos);
    int m_scrollOffset = 0;
    float m_currentTime = 0.0f;
};

