#include "PlayheadOVerlay.h"
#include <QPainter>       // --- ADDED FIX: Essential for drawing
#include <QMouseEvent>    // --- ADDED FIX: Essential for mouse events
#include <QPen>   
#include <qapplication.h>
constexpr int PIXELS_PER_SECOND = 100;
constexpr int PLAYHEAD_CLICK_TOLERANCE = 5;

PlayheadOverlay::PlayheadOverlay(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TranslucentBackground);
}

void PlayheadOverlay::setCurrentTime(float time)
{
    if (m_currentTime != time) {
        m_currentTime = time;
        update();
    }
}

void PlayheadOverlay::setScrollOffset(int offset)
{
    if (m_scrollOffset != offset) {
        m_scrollOffset = offset;
        update();
    }
}

void PlayheadOverlay::forwardEvent(QMouseEvent* event)
{
    // To find the widget underneath, we temporarily hide this overlay,
    // ask Qt for the widget at the global cursor position, then show it again.
    // This is instantaneous and not visible to the user.
    QObjectList children = parentWidget()->children();
    QWidget* widgetUnder = nullptr;

    for (int i = children.size() - 1; i >= 0; --i) {
        QWidget* child = qobject_cast<QWidget*>(children.at(i));

        // Find a visible sibling that isn't the overlay itself and is under the mouse
        if (child && child != this && child->isVisible() && child->geometry().contains(event->pos())) {
            widgetUnder = child;
            break;
        }
    }

    if (widgetUnder) {
        // Map coordinates and post the event, just like before.
        QPoint localPos = widgetUnder->mapFrom(parentWidget(), event->pos());
        QMouseEvent* newEvent = new QMouseEvent(event->type(), localPos, event->scenePosition(), event->globalPosition(), event->button(), event->buttons(), event->modifiers());
        QCoreApplication::postEvent(widgetUnder, newEvent);
    }
}
void PlayheadOverlay::setKeyframeDragInProgress(bool isDragging)
{
    m_keyframeDragInProgress = isDragging;
}

float PlayheadOverlay::timeFromPos(const QPoint& pos)
{
    return static_cast<float>(pos.x() + m_scrollOffset) / PIXELS_PER_SECOND;
}

void PlayheadOverlay::mousePressEvent(QMouseEvent* event)
{
    if (!m_keyframeDragInProgress) {
        int playheadX = (m_currentTime * PIXELS_PER_SECOND) - m_scrollOffset;
        if (event->button() == Qt::LeftButton && abs(event->pos().x() - playheadX) <= PLAYHEAD_CLICK_TOLERANCE) {
            emit timeChanged(timeFromPos(event->pos()));
            event->accept();
            return; // Return early after handling the event.
        }
    }
    // If a drag is in progress, or we didn't click the line, forward the event.
    forwardEvent(event);
}

void PlayheadOverlay::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_keyframeDragInProgress) {
        int playheadX = (m_currentTime * PIXELS_PER_SECOND) - m_scrollOffset;
        bool onPlayhead = abs(event->pos().x() - playheadX) <= PLAYHEAD_CLICK_TOLERANCE;

        if (event->buttons() & Qt::LeftButton) {
            if (onPlayhead) {
                emit timeChanged(timeFromPos(event->pos()));
                event->accept();
                return; // Return early.
            }
        }
        else {
            if (onPlayhead) {
                setCursor(Qt::SizeHorCursor);
            }
            else {
                unsetCursor();
            }
        }
    }
    // Always forward move events.
    forwardEvent(event);
}
void PlayheadOverlay::mouseReleaseEvent(QMouseEvent* event)
{
    forwardEvent(event);
    event->accept();
}

void PlayheadOverlay::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QPen(Qt::red, 1));
    int playheadX = (m_currentTime * PIXELS_PER_SECOND) - m_scrollOffset;
    painter.drawLine(playheadX, 0, playheadX, height());
}

void PlayheadOverlay::forwardContextMenuEvent(QContextMenuEvent* event)
{
    if (!parentWidget()) return;
    QObjectList children = parentWidget()->children();
    QWidget* widgetUnder = nullptr;
    for (int i = children.size() - 1; i >= 0; --i) {
        QWidget* child = qobject_cast<QWidget*>(children.at(i));
        if (child && child != this && child->isVisible() && child->geometry().contains(event->pos())) {
            widgetUnder = child;
            break;
        }
    }
    if (widgetUnder) {
        QPoint localPos = widgetUnder->mapFrom(parentWidget(), event->pos());
        QContextMenuEvent* newEvent = new QContextMenuEvent(event->reason(), localPos, event->globalPos(), event->modifiers());
        QCoreApplication::postEvent(widgetUnder, newEvent);
    }
}
void PlayheadOverlay::contextMenuEvent(QContextMenuEvent* event)
{
    forwardContextMenuEvent(event);
    event->accept();
}
