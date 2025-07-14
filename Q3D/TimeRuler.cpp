#include "TimeRuler.h"
#include "Cinematic.h"
#include "GraphNode.h"
#include "SceneGraph.h"
#include <QStyle>
#include <QFileDialog>
#include <QMessageBox>
#include <QPaintEvent>
#include <QLinearGradient>
#include <QMimeData>
#include <QResizeEvent>

constexpr int PIXELS_PER_SECOND = 100;
constexpr int RULER_HEIGHT = 30;
// ===================================================================================
// TimeRuler Implementation
// ... (no changes)
// ===================================================================================
TimeRuler::TimeRuler(QWidget* parent) : QWidget(parent)
{
    setFixedHeight(RULER_HEIGHT);
    setStyleSheet("background-color: #353535; color: white;");
}

void TimeRuler::setScrollOffset(int offset)
{
    if (m_scrollOffset != offset) {
        m_scrollOffset = offset;
        update();
    }
}

void TimeRuler::setCurrentTime(float time)
{
    if (m_currentTime != time) {
        m_currentTime = time;
        update();
    }
}

float TimeRuler::timeFromPos(const QPoint& pos)
{
    return static_cast<float>(pos.x() + m_scrollOffset) / PIXELS_PER_SECOND;
}

void TimeRuler::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit timeChanged(timeFromPos(event->pos()));
    }
}

void TimeRuler::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        emit timeChanged(timeFromPos(event->pos()));
    }
}

void TimeRuler::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::white);
    const int majorTickHeight = 10;
    const int minorTickHeight = 5;
    const int majorTickInterval = PIXELS_PER_SECOND;
    const int minorTickInterval = PIXELS_PER_SECOND / 10;
    const int textYOffset = 15;
    int startX = -m_scrollOffset;

    for (int x = startX; x < width() - startX; ++x) {
        int currentPixel = x + m_scrollOffset;
        if (currentPixel < 0) continue;

        if (currentPixel % majorTickInterval == 0) {
            painter.drawLine(x, height(), x, height() - majorTickHeight);
            int seconds = currentPixel / PIXELS_PER_SECOND;
            painter.drawText(x + 2, textYOffset, QString::number(seconds) + "s");
        }
        else if (currentPixel % minorTickInterval == 0) {
            painter.drawLine(x, height(), x, height() - minorTickHeight);
        }
    }

    painter.setPen(QPen(Qt::red, 2));
    int playheadX = (m_currentTime * PIXELS_PER_SECOND) - m_scrollOffset;
    painter.drawLine(playheadX, 0, playheadX, height());
}
