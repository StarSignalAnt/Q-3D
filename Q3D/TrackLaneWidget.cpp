#include "TrackLaneWidget.h"
#include <QMouseEvent> // Make sure this is included
#include <optional>
#include <QMenu>
#include "GameAudio.h"
#include "TimelineMetrics.h"
#include "GameVideo.h"
#include <QImage>
constexpr int KEYFRAME_CLICK_TOLERANCE = 5;
TrackLaneWidget::TrackLaneWidget(ITrack* track, QWidget* parent)
    : QWidget(parent)
    , m_track(track)
    , m_scrollOffset(0)
{
    setFixedHeight(60);
//    setMinimumWidth(3000);
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

    // 1. Draw the base gradient background based on the track's type
    QLinearGradient gradient(0, 0, 0, height());
    if (m_track) {
        if (dynamic_cast<TrackAudio*>(m_track)) {
            gradient.setColorAt(0, QColor("#993333")); // Red for Audio
            gradient.setColorAt(1, QColor("#662222"));
        }
        else if (dynamic_cast<TrackVideo*>(m_track)) {
            gradient.setColorAt(0, QColor("#336699")); // Blue for Video
            gradient.setColorAt(1, QColor("#224466"));
        }
        else if (dynamic_cast<TrackTransform*>(m_track)) {
            gradient.setColorAt(0, QColor("#5a5a5a")); // Grey for Transform
            gradient.setColorAt(1, QColor("#4f4f4f"));
        }
        else {
            gradient.setColorAt(0, QColor("#2c5364")); // Default
        }
    }
    else {
        gradient.setColorAt(0, QColor("#333"));
    }
    painter.fillRect(rect(), gradient);

    if (!m_track) return;

    // 2. Draw Keyframes (logic varies by track type)

    // --- Special drawing for Audio Tracks ---
    if (auto* audioTrack = dynamic_cast<TrackAudio*>(m_track))
    {
        const auto& keyframes = audioTrack->GetKeyframes();
        for (const auto& keyframe : keyframes) {
            GSound* sound = keyframe.GetValue();
            if (!sound) continue;

            int startX = keyframe.GetTime() * PIXELS_PER_SECOND;
            int widthPx = sound->Length() * PIXELS_PER_SECOND;
            QRectF clipRect(startX, 2, widthPx, height() - 4);

            // Step A: Draw the semi-transparent background rectangle
            QColor rectColor("#E57373");
            rectColor.setAlphaF(0.6);
            painter.setPen(Qt::NoPen);
            painter.setBrush(rectColor);
            painter.drawRoundedRect(clipRect, 3, 3);

            // Step B: Draw the waveform over the background
            if (!sound->waveformData.empty()) {
                const std::vector<float>& waveform = sound->waveformData;
                int laneCenterY = height() / 2;
                float lineSpacing = static_cast<float>(widthPx) / waveform.size();
                QPen wavePen(QColor("#FFEBEE"));
                wavePen.setWidth(1);
                painter.setPen(wavePen);
                for (size_t i = 0; i < waveform.size(); ++i) {
                    float peak = waveform[i];
                    int lineHeight = static_cast<int>(peak * (height() - 8));
                    int lineY = laneCenterY - (lineHeight / 2);
                    int lineX = startX + static_cast<int>(i * lineSpacing);
                    painter.drawLine(lineX, lineY, lineX, lineY + lineHeight);
                }
            }

            // Step C: Draw the keyframe tick on top
            painter.setPen(QColor("#00ffc8"));
            painter.setBrush(QColor("#00ffc8"));
            painter.drawRect(startX - 1, 0, 3, height());
        }
    }
    // --- Special drawing for Video Tracks ---
    else if (auto* videoTrack = dynamic_cast<TrackVideo*>(m_track))
    {
        for (const auto& keyframe : videoTrack->GetKeyframes()) {
            GameVideo* video = keyframe.GetValue();
            if (!video) continue;

            // --- Step A: Check if thumbnails have been converted to QPixmaps yet ---
            if (m_pixmapCache.find(video) == m_pixmapCache.end()) {
                // If not, perform the one-time conversion now.
                const auto& pixelMapCache = videoTrack->GetThumbnailCache();
                auto it = pixelMapCache.find(video);
                if (it != pixelMapCache.end()) {
                    std::vector<QPixmap> pixmaps;
                    for (PixelMap* pm : it->second) {
                        if (pm) {
                            // Convert the raw PixelMap data to a QPixmap.
                            QImage img(static_cast<uchar*>(pm->GetData()), pm->GetWidth(), pm->GetHeight(), QImage::Format_RGBA8888);
                            pixmaps.push_back(QPixmap::fromImage(img.copy()));
                        }
                    }
                    // Store the converted pixmaps in our UI-side cache.
                    m_pixmapCache[video] = pixmaps;
                }
            }

            // --- Step B: Draw the cached QPixmaps ---
            auto cacheIt = m_pixmapCache.find(video);
            if (cacheIt != m_pixmapCache.end()) {
                const std::vector<QPixmap>& thumbnails = cacheIt->second;
                float startTime = keyframe.GetTime();
                float duration = video->GetDuration();

                for (size_t i = 0; i < thumbnails.size(); ++i) {
                    float thumbStartTime = startTime + i;
                    int thumbStartX = thumbStartTime * PIXELS_PER_SECOND;
                    int thumbWidthPx = PIXELS_PER_SECOND;

                    if (thumbStartTime + 1.0f > startTime + duration) {
                        thumbWidthPx = (startTime + duration - thumbStartTime) * PIXELS_PER_SECOND;
                    }

                    QRect thumbRect(thumbStartX, 0, thumbWidthPx, height());
                    painter.drawPixmap(thumbRect, thumbnails[i]);
                }
            }

            // --- Step C: Draw the main keyframe tick on top ---
            int startX = keyframe.GetTime() * PIXELS_PER_SECOND;
            painter.setPen(QColor("#00ffc8"));
            painter.setBrush(QColor("#00ffc8"));
            painter.drawRect(startX - 1, 0, 3, height());
        }
    }
    // --- Fallback for all other track types (e.g., Transform) ---
    else
    {
        QPen keyframePen(QColor("#00ffc8"));
        keyframePen.setWidth(2);
        QPen draggedKeyframePen(QColor("#ffff00"));
        draggedKeyframePen.setWidth(2);

        std::vector<float> keyframeTimes = m_track->GetKeyframeTimes();
        std::vector<InterpolationType> keyframeTypes = m_track->GetKeyframeInterpolationTypes();

        for (size_t i = 0; i < keyframeTimes.size(); ++i) {
            if (m_draggedKeyframeIndex.has_value() && m_draggedKeyframeIndex.value() == i) {
                painter.setPen(draggedKeyframePen);
            }
            else {
                painter.setPen(keyframePen);
            }
            int xPos = keyframeTimes[i] * PIXELS_PER_SECOND;

            if (i < keyframeTypes.size() && keyframeTypes[i] == InterpolationType::Snapped) {
                painter.setBrush(painter.pen().color());
                painter.drawRect(xPos - 2, 2, 5, 5);
            }
            else {
                painter.drawLine(xPos, 0, xPos, height());
            }
        }
    }

    // 4. Draw the bottom border line for the track lane
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

float max3(float a, float b) {
    return (a > b) ? a : b;
}

void TrackLaneWidget::mouseMoveEvent(QMouseEvent* event)
{
    // --- Drag Logic ---
    // If we are holding the left mouse button and have a keyframe selected
    if (m_draggedKeyframeIndex.has_value() && (event->buttons() & Qt::LeftButton)) {
        float newTime = static_cast<float>(event->pos().x() + m_scrollOffset) / PIXELS_PER_SECOND;
        newTime = max3(0.0f, newTime); // Prevent negative time

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
        QAction* SnappedAction = contextMenu.addAction("Set to Snapped");

        // Connect the actions to lambdas that call our new function.
        connect(linearAction, &QAction::triggered, this, [this, clickedKeyframeIndex]() {
            m_track->SetKeyframeInterpolation(clickedKeyframeIndex.value(), InterpolationType::Linear);
            this->update(); // Repaint to show the change immediately.
            });

        connect(SnappedAction, &QAction::triggered, this, [this, clickedKeyframeIndex]() {
            m_track->SetKeyframeInterpolation(clickedKeyframeIndex.value(), InterpolationType::Snapped);
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