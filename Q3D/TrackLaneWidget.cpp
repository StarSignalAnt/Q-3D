#include "TrackLaneWidget.h"
#include <QMouseEvent> // Make sure this is included
#include <optional>
#include <QMenu>
constexpr int PIXELS_PER_SECOND = 100;
constexpr int KEYFRAME_CLICK_TOLERANCE = 5;
TrackLaneWidget::TrackLaneWidget(ITrack* track, QWidget* parent)
    : QWidget(parent)
    , m_track(track)
    , m_scrollOffset(0)
{
    setFixedHeight(60);
    setMinimumWidth(3000);
}

void TrackLaneWidget::setScrollOffset(int offset)
{
    if (m_scrollOffset != offset) {
        m_scrollOffset = offset;
        update();
    }
}

void TrackLaneWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor("#2c5364"));
    gradient.setColorAt(1, QColor("#203a43"));
    painter.fillRect(rect(), gradient);

    if (m_track) {
        QPen keyframePen(QColor("#00ffc8"));
        keyframePen.setWidth(2);
        QPen draggedKeyframePen(QColor("#ffff00"));
        draggedKeyframePen.setWidth(2);

        // --- MODIFIED --- Get both times and interpolation types.
        std::vector<float> keyframeTimes = m_track->GetKeyframeTimes();
        std::vector<InterpolationType> keyframeTypes = m_track->GetKeyframeInterpolationTypes();

        for (size_t i = 0; i < keyframeTimes.size(); ++i) {
            if (m_draggedKeyframeIndex.has_value() && m_draggedKeyframeIndex.value() == i) {
                painter.setPen(draggedKeyframePen);
            }
            else {
                painter.setPen(keyframePen);
            }

            int xPos = (keyframeTimes[i] * PIXELS_PER_SECOND) - m_scrollOffset;

            // --- MODIFIED --- Draw differently based on interpolation type.
            if (i < keyframeTypes.size() && keyframeTypes[i] == InterpolationType::Stepped) {
                // Draw stepped keyframes as solid squares
                painter.setBrush(painter.pen().color());
                painter.drawRect(xPos - 2, 2, 5, 5);
            }
            else {
                // Draw linear keyframes as lines
                painter.drawLine(xPos, 0, xPos, height());
            }
        }
    }

    painter.setPen(QColor("#1a1a1a"));
    painter.drawLine(0, height() - 1, width(), height() - 1);
}

void TrackLaneWidget::mousePressEvent(QMouseEvent* event)
{
    // Handle Left-click for starting a keyframe drag
    if (event->button() == Qt::LeftButton && m_track) {
        int clickX = event->pos().x();
        std::vector<float> keyframeTimes = m_track->GetKeyframeTimes();

        for (size_t i = 0; i < keyframeTimes.size(); ++i) {
            int keyframeX = (keyframeTimes[i] * PIXELS_PER_SECOND) - m_scrollOffset;
            if (abs(clickX - keyframeX) <= KEYFRAME_CLICK_TOLERANCE) {
                m_draggedKeyframeIndex = i;
                emit keyframeDragStarted();
                event->accept();
                return;
            }
        }
    }
    // --- ADDED --- Handle Right-click to enable the context menu
    else if (event->button() == Qt::RightButton) {
        // By accepting the right-click press, we tell Qt that this widget
        // should receive the resulting context menu event.
        event->accept();
        return;
    }

    // Ignore all other mouse press events
    event->ignore();
}
void TrackLaneWidget::mouseMoveEvent(QMouseEvent* event)
{
    // --- Drag Logic ---
    // If we are holding the left mouse button and have a keyframe selected
    if (m_draggedKeyframeIndex.has_value() && (event->buttons() & Qt::LeftButton)) {
        float newTime = static_cast<float>(event->pos().x() + m_scrollOffset) / PIXELS_PER_SECOND;
        newTime = std::max(0.0f, newTime); // Prevent negative time

        // This call is now robust thanks to the changes in Cinematic.h
        m_draggedKeyframeIndex = m_track->ModifyKeyframeTime(m_draggedKeyframeIndex.value(), newTime);

        update(); // Repaint to show the keyframe at its new position
        event->accept();
        return;
    }

    // --- Hover Logic ---
    // If we are just moving the mouse, check if we are over a keyframe to update the cursor
    if (m_track) {
        bool onKeyframe = false;
        std::vector<float> keyframeTimes = m_track->GetKeyframeTimes();
        for (size_t i = 0; i < keyframeTimes.size(); ++i) {
            int keyframeX = (keyframeTimes[i] * PIXELS_PER_SECOND) - m_scrollOffset;
            if (abs(event->pos().x() - keyframeX) <= KEYFRAME_CLICK_TOLERANCE) {
                onKeyframe = true;
                break;
            }
        }
        if (onKeyframe) {
            setCursor(Qt::SizeHorCursor);
        }
        else {
            unsetCursor();
        }
    }
    event->ignore();
}
void TrackLaneWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_draggedKeyframeIndex.has_value()) {
        // --- ADDED --- Announce that the drag is finished.
        emit keyframeDragFinished();
        m_draggedKeyframeIndex.reset();
        unsetCursor();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void TrackLaneWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (!m_track) {
        return;
    }

    int clickX = event->pos().x();
    std::vector<float> keyframeTimes = m_track->GetKeyframeTimes();
    std::optional<size_t> clickedKeyframeIndex;

    // Find which keyframe (if any) was clicked on.
    for (size_t i = 0; i < keyframeTimes.size(); ++i) {
        int keyframeX = (keyframeTimes[i] * PIXELS_PER_SECOND) - m_scrollOffset;
        if (abs(clickX - keyframeX) <= KEYFRAME_CLICK_TOLERANCE) {
            clickedKeyframeIndex = i;
            break;
        }
    }

    // If we right-clicked on a keyframe, show the menu.
    if (clickedKeyframeIndex.has_value()) {
        QMenu contextMenu(this);
        QAction* linearAction = contextMenu.addAction("Set to Linear");
        QAction* steppedAction = contextMenu.addAction("Set to Stepped");

        // Connect the actions to lambdas that call our new function.
        connect(linearAction, &QAction::triggered, this, [this, clickedKeyframeIndex]() {
            m_track->SetKeyframeInterpolation(clickedKeyframeIndex.value(), InterpolationType::Linear);
            this->update(); // Repaint to show the change immediately.
            });

        connect(steppedAction, &QAction::triggered, this, [this, clickedKeyframeIndex]() {
            m_track->SetKeyframeInterpolation(clickedKeyframeIndex.value(), InterpolationType::Stepped);
            this->update(); // Repaint to show the change immediately.
            });

        // Show the menu at the global cursor position.
        contextMenu.exec(event->globalPos());
    }
    else {
        // If not on a keyframe, do nothing and let the event propagate.
        event->ignore();
    }
}