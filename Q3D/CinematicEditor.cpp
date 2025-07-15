#include "CinematicEditor.h"
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
#include "PlayheadOVerlay.h"
#include "TrackLaneWidget.h"
#include "TrackHeaderWidget.h"
#include "TimeRuler.h"
#include <QCoreApplication> // Required for event filtering
#include <qmenu.h>
#include <qmenubar.h>
#include "GameAudio.h"
constexpr int RULER_HEIGHT = 30;
constexpr int PIXELS_PER_SECOND2 = 100;

// ===================================================================================
// CinematicEditor Implementation
// ===================================================================================
CinematicEditor::CinematicEditor(QWidget* parent)
    : QWidget(parent)
    , m_currentTime(0.0f)
{
    m_activeCinematic = std::make_unique<Cinematic>();

    // Main Layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // --- Menu Bar ---
    m_menuBar = new QMenuBar(this);
    m_mainLayout->addWidget(m_menuBar);

    m_cinematicMenu = m_menuBar->addMenu("Cinematic");
    m_createMenu = m_menuBar->addMenu("Create");
    m_createGraphNodeTrackAction = m_createMenu->addAction("GraphNode Track");
    m_createAudioTrackAction = m_createMenu->addAction("Audio Track");

    // --- Toolbar ---
    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);

    // File Actions
    m_newAction = new QAction(QIcon("edit/icons/newCine.png"), "New", this);
    m_saveAction = new QAction(QIcon("edit/icons/savecine.png"), "Save", this);
    m_loadAction = new QAction(QIcon("edit/icons/loadcine.png"), "Load", this);
    m_toolBar->addAction(m_newAction);
    m_toolBar->addAction(m_saveAction);
    m_toolBar->addAction(m_loadAction);
    m_toolBar->addSeparator();

    // Playback Actions
    m_goToStartAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), "Go to Start", this);
    m_playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), "Play", this);
    m_pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), "Pause", this);
    m_stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), "Stop", this);
    m_goToEndAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), "Go to End", this);
    m_toolBar->addAction(m_goToStartAction);
    m_toolBar->addAction(m_playAction);
    m_toolBar->addAction(m_pauseAction);
    m_toolBar->addAction(m_stopAction);
    m_toolBar->addAction(m_goToEndAction);
    m_toolBar->addSeparator();

    // Snapshot Action
    m_snapshotAction = new QAction(style()->standardIcon(QStyle::SP_ArrowUp), "Snapshot All", this);
    m_snapshotAction->setToolTip("Record a keyframe on all tracks at the current time");
    m_toolBar->addAction(m_snapshotAction);
    m_mainLayout->addWidget(m_toolBar);

    m_playbackTimer = new QTimer(this);

    // --- Timeline Area Setup ---
    QWidget* timelineArea = new QWidget(this);
    QHBoxLayout* timelineLayout = new QHBoxLayout(timelineArea);
    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);

    QWidget* leftPaneWidget = new QWidget();
    QVBoxLayout* leftPaneLayout = new QVBoxLayout(leftPaneWidget);
    leftPaneLayout->setContentsMargins(0, 0, 0, 0);
    leftPaneLayout->setSpacing(0);
    m_headerSpacer = new QWidget();
    m_headerSpacer->setFixedHeight(RULER_HEIGHT);
    m_headerSpacer->setStyleSheet("background-color: #353535; border-bottom: 1px solid #2a2a2a;");
    leftPaneLayout->addWidget(m_headerSpacer);
    m_headerScrollArea = new QScrollArea();
    m_headerScrollArea->setWidgetResizable(true);
    m_headerScrollArea->setFrameShape(QFrame::NoFrame);
    m_headerScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_headerScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_headerContainer = new QWidget();
    m_headerContainer->setAcceptDrops(true);
    m_headerContainer->installEventFilter(this);
    m_headerContainer->setStyleSheet("background-color: #404040;");
    m_headerLayout = new QVBoxLayout(m_headerContainer);
    m_headerLayout->setContentsMargins(0, 0, 0, 0);
    m_headerLayout->setSpacing(0);
    m_headerLayout->addStretch();
    m_headerContainer->setLayout(m_headerLayout);
    m_headerScrollArea->setWidget(m_headerContainer);
    leftPaneLayout->addWidget(m_headerScrollArea);
    leftPaneWidget->setLayout(leftPaneLayout);

    QWidget* rightPaneWidget = new QWidget();
    QVBoxLayout* rightPaneLayout = new QVBoxLayout(rightPaneWidget);
    rightPaneLayout->setContentsMargins(0, 0, 0, 0);
    rightPaneLayout->setSpacing(0);
    m_timeRuler = new TimeRuler();
    rightPaneLayout->addWidget(m_timeRuler);
    m_laneScrollArea = new QScrollArea();
    m_laneScrollArea->setWidgetResizable(true);
    m_laneScrollArea->setFrameShape(QFrame::NoFrame);
    QWidget* laneViewportWidget = new QWidget();
    laneViewportWidget->setAcceptDrops(true);
    laneViewportWidget->installEventFilter(this);
    laneViewportWidget->setStyleSheet("background-color: #404040;");
    m_laneLayout = new QVBoxLayout();
    m_laneLayout->setContentsMargins(0, 0, 0, 0);
    m_laneLayout->setSpacing(0);
    m_laneLayout->addStretch();
    laneViewportWidget->setLayout(m_laneLayout);
    m_laneScrollArea->setWidget(laneViewportWidget);
    m_playheadOverlay = new PlayheadOverlay(laneViewportWidget);
    m_playheadOverlay->raise();
    rightPaneLayout->addWidget(m_laneScrollArea);
    rightPaneWidget->setLayout(rightPaneLayout);

    m_splitter = new QSplitter(Qt::Horizontal, timelineArea);
    m_splitter->setHandleWidth(2);
    m_splitter->addWidget(leftPaneWidget);
    m_splitter->addWidget(rightPaneWidget);
    m_splitter->setSizes({ 175, 1000 });
    timelineLayout->addWidget(m_splitter);
    m_mainLayout->addWidget(timelineArea);
    m_mainLayout->setStretchFactor(timelineArea, 1);
    this->setLayout(m_mainLayout);

    // --- Connections ---
    connect(m_newAction, &QAction::triggered, this, &CinematicEditor::onNewCinematic);
    connect(m_saveAction, &QAction::triggered, this, &CinematicEditor::onSaveCinematic);
    connect(m_loadAction, &QAction::triggered, this, &CinematicEditor::onLoadCinematic);
    connect(m_snapshotAction, &QAction::triggered, this, &CinematicEditor::onGlobalSnapshot);
    connect(m_goToStartAction, &QAction::triggered, this, &CinematicEditor::onGoToStart);
    connect(m_goToEndAction, &QAction::triggered, this, &CinematicEditor::onGoToEnd);
    connect(m_playAction, &QAction::triggered, this, &CinematicEditor::onPlay);
    connect(m_pauseAction, &QAction::triggered, this, &CinematicEditor::onPause);
    connect(m_stopAction, &QAction::triggered, this, &CinematicEditor::onStop);
    connect(m_playbackTimer, &QTimer::timeout, this, &CinematicEditor::onPlaybackUpdate);
    connect(m_createGraphNodeTrackAction, &QAction::triggered, this, &CinematicEditor::onCreateGraphNodeTrack);
    connect(m_createAudioTrackAction, &QAction::triggered, this, &CinematicEditor::onCreateAudioTrack);
    connect(m_timeRuler, &TimeRuler::timeChanged, this, [this](float newTime) {
        updateCurrentTime(newTime, false);
        });
    connect(m_playheadOverlay, &PlayheadOverlay::timeChanged, this, [this](float newTime) {
        updateCurrentTime(newTime, false);
        });
    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, m_timeRuler, &TimeRuler::setScrollOffset);
    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, m_playheadOverlay, &PlayheadOverlay::setScrollOffset);
    connect(m_headerScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, m_laneScrollArea->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_laneScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, m_headerScrollArea->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_timeRuler, &TimeRuler::scrubbingFinished, this, &CinematicEditor::onScrubbingFinished);
    connect(m_playheadOverlay, &PlayheadOverlay::scrubbingFinished, this, &CinematicEditor::onScrubbingFinished);
    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::sliderReleased, this, &CinematicEditor::onScrubbingFinished);

    rebuildUIFromCinematic();
    updateButtonStates();
}

CinematicEditor::~CinematicEditor() {}
float max2(float v1, float v2)
{
    return (v1 > v2) ? v1 : v2;
}
// --- ADDED --- This new function now handles drag-and-drop for the correct widget.
bool CinematicEditor::eventFilter(QObject* watched, QEvent* event)
{
    bool isLaneWidget = m_laneScrollArea && (watched == m_laneScrollArea->widget());
    bool isHeaderWidget = m_headerContainer && (watched == m_headerContainer);

    if (isLaneWidget || isHeaderWidget) {
        // --- Handle Drag Operations ---
        if (event->type() == QEvent::DragEnter) {
            auto* dragEvent = static_cast<QDragEnterEvent*>(event);
            // Accept the drag if it contains either a graph node or a sound asset
            if (dragEvent->mimeData()->hasFormat("application/x-graphnode") ||
                dragEvent->mimeData()->hasFormat("application/x-sound-asset")) {
                dragEvent->acceptProposedAction();
                return true; // Event handled
            }
        }
        // --- Handle Drop Operations ---
        else if (event->type() == QEvent::Drop) {
            auto* dropEvent = static_cast<QDropEvent*>(event);
            QWidget* laneViewportWidget = m_laneScrollArea->widget();

            // --- Path for Dropped Sound Assets ---
            if (dropEvent->mimeData()->hasFormat("application/x-sound-asset") && watched == laneViewportWidget) {
                // Find the specific track lane under the cursor robustly
                TrackLaneWidget* targetLane = nullptr;
                for (int i = 0; i < m_laneLayout->count(); ++i) {
                    if (auto item = m_laneLayout->itemAt(i)) {
                        if (auto widget = qobject_cast<TrackLaneWidget*>(item->widget())) {
                            if (widget->geometry().contains(dropEvent->position().toPoint())) {
                                targetLane = widget;
                                break;
                            }
                        }
                    }
                }

                if (targetLane) {
                    // Check if the target is actually an audio track
                    if (auto* audioTrack = dynamic_cast<TrackAudio*>(targetLane->getTrack())) {
                        float dropTime = static_cast<float>(dropEvent->position().x() + m_laneScrollArea->horizontalScrollBar()->value()) / PIXELS_PER_SECOND2;
                        dropTime = max2(0.0f, dropTime);

                        QString filePath = QString::fromUtf8(dropEvent->mimeData()->data("application/x-sound-asset"));
                        GSound* sound = GameAudio::m_Instance->LoadSound(filePath.toStdString());

                        if (sound) {
                            audioTrack->AddAudioKeyframe(dropTime, sound);
                            targetLane->update();
                            updateTimelineWidth(); // Resize timeline to fit the new clip
                            dropEvent->acceptProposedAction();
                            return true; // Event handled
                        }
                    }
                }
            }
            // --- Path for Dropped GraphNodes ---
            else if (dropEvent->mimeData()->hasFormat("application/x-graphnode")) {
                QByteArray data = dropEvent->mimeData()->data("application/x-graphnode");
                bool ok;
                qulonglong nodeAddress = data.toULongLong(&ok);
                if (ok) {
                    auto* droppedNode = reinterpret_cast<GraphNode*>(nodeAddress);
                    if (droppedNode && m_activeCinematic) {
                        auto newTrack = std::make_unique<TrackTransform>(droppedNode);
                        m_activeCinematic.get()->AddTrack(std::move(newTrack));
                        rebuildUIFromCinematic(); // Rebuild is okay here since it's a new track
                        dropEvent->acceptProposedAction();
                        return true; // Event handled
                    }
                }
            }
        }
    }
    // For all other events, use the default behavior
    return QWidget::eventFilter(watched, event);
}

void CinematicEditor::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateOverlayGeometry();
}

void CinematicEditor::updateOverlayGeometry()
{
    if (m_playheadOverlay->parentWidget()) {
        m_playheadOverlay->setGeometry(m_playheadOverlay->parentWidget()->rect());
    }
}

void CinematicEditor::addTrack(ITrack* track)
{
    auto* header = new TrackHeaderWidget(QString::fromStdString(track->GetName()));
    auto* lane = new TrackLaneWidget(track);

    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, lane, &TrackLaneWidget::setScrollOffset);
    connect(lane, &TrackLaneWidget::keyframeDragStarted, this, &CinematicEditor::onKeyframeDragStarted);
    connect(lane, &TrackLaneWidget::keyframeDragFinished, this, &CinematicEditor::onKeyframeDragFinished);

    // --- FIXED --- Connect the record button to a lambda that only updates the relevant lane.
    connect(header->getRecordButton(), &QPushButton::clicked, this, [this, track, header, lane]() {
        InterpolationType type = header->isSnappedChecked() ? InterpolationType::Snapped : InterpolationType::Linear;
        track->RecordKeyframe(m_currentTime, type);
        // This is much more efficient than rebuilding the entire UI.
        lane->update();
        });

    m_headerLayout->insertWidget(m_headerLayout->count() - 1, header);
    m_laneLayout->insertWidget(m_laneLayout->count() - 1, lane);

    m_playheadOverlay->raise();
}

void CinematicEditor::clearAllTracks()
{
    QLayoutItem* item;
    while ((item = m_headerLayout->takeAt(0)) != nullptr)
    {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    while ((item = m_laneLayout->takeAt(0)) != nullptr)
    {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    m_headerLayout->addStretch();
    m_laneLayout->addStretch();
}

void CinematicEditor::rebuildUIFromCinematic()
{
    clearAllTracks();
    if (!m_activeCinematic) return;

    for (const auto& track : m_activeCinematic->GetTracks())
    {
        addTrack(track.get());
    }

    QTimer::singleShot(0, m_laneScrollArea->horizontalScrollBar(), [this]() {
        emit m_laneScrollArea->horizontalScrollBar()->valueChanged(m_laneScrollArea->horizontalScrollBar()->value());
        });

    QTimer::singleShot(0, this, &CinematicEditor::updateOverlayGeometry);
}

void CinematicEditor::onNewCinematic()
{
    m_activeCinematic = std::make_unique<Cinematic>();
    rebuildUIFromCinematic();
    QMessageBox::information(this, "New Cinematic", "Timeline has been cleared.");
}

void CinematicEditor::onSaveCinematic()
{
    QString filePath = QFileDialog::getSaveFileName(this, "Save Cinematic", "", "Cinematic Files (*.cine)");
    if (!filePath.isEmpty()) {
        QMessageBox::information(this, "Save Cinematic", "Saving to:\n" + filePath);
    }
}

void CinematicEditor::onLoadCinematic()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Load Cinematic", "", "Cinematic Files (*.cine)");
    if (!filePath.isEmpty()) {
        m_activeCinematic = std::make_unique<Cinematic>();
        rebuildUIFromCinematic();
        QMessageBox::information(this, "Load Cinematic", "Loading from:\n" + filePath);
    }
}

void CinematicEditor::onGlobalSnapshot()
{
    if (!m_activeCinematic) return;

    const auto& tracks = m_activeCinematic->GetTracks();

    // The header layout widgets and the data model tracks have a 1-to-1 correspondence.
    // We iterate through the UI headers to get the state of each checkbox.
    for (int i = 0; i < m_headerLayout->count(); ++i)
    {
        QLayoutItem* layoutItem = m_headerLayout->itemAt(i);
        // Attempt to cast the item's widget to our header widget class.
        // This will correctly be null for the stretch item at the end of the layout.
        TrackHeaderWidget* headerWidget = qobject_cast<TrackHeaderWidget*>(layoutItem->widget());

        // If we have a valid header widget and a corresponding track in our data model...
        if (headerWidget && i < tracks.size())
        {
            // Get the corresponding track from the data model.
            ITrack* track = tracks[i].get();

            // Read the state of THIS specific track's checkbox.
            InterpolationType type = headerWidget->isSnappedChecked()
                ? InterpolationType::Snapped
                : InterpolationType::Linear;

            // Record the keyframe with the correct type.
            track->RecordKeyframe(m_currentTime, type);
        }
    }

    // After updating all the data, trigger a repaint on all the lane widgets
    // to make sure the new keyframes (lines or squares) are visible.
    for (int i = 0; i < m_laneLayout->count(); ++i) {
        if (QLayoutItem* item = m_laneLayout->itemAt(i)) {
            if (QWidget* widget = item->widget()) {
                widget->update();
            }
        }
    }

}
void CinematicEditor::updateCurrentTime(float newTime, bool fromPlayback)
{
    m_currentTime = max2(0.0f, newTime);

    if (!fromPlayback && m_playbackState == Playing) {
        m_timeAtPlaybackStart = m_currentTime;
        m_playbackTime.restart();
    }

    if (m_activeCinematic) {
        // --- MODIFIED --- Pass the scrubbing state down to the data model.
        bool isScrubbing = !fromPlayback;
        m_activeCinematic->SetTime(m_currentTime, isScrubbing);
    }

    if (m_timeRuler) m_timeRuler->setCurrentTime(m_currentTime);
    if (m_playheadOverlay) m_playheadOverlay->setCurrentTime(m_currentTime);

}

// --- REMOVED --- The main widget no longer needs these event handlers.
// void CinematicEditor::dragEnterEvent(QDragEnterEvent* event) { ... }
// void CinematicEditor::dropEvent(QDropEvent* event) { ... }

void CinematicEditor::onKeyframeDragStarted()
{
    m_playheadOverlay->setKeyframeDragInProgress(true);
}

void CinematicEditor::onKeyframeDragFinished()
{
    m_playheadOverlay->setKeyframeDragInProgress(false);
}
void CinematicEditor::onGoToStart()
{
    updateCurrentTime(0.0f);
}

void CinematicEditor::onGoToEnd()
{
    if (m_activeCinematic) {
        updateCurrentTime(m_activeCinematic->GetDuration());
    }
}

void CinematicEditor::onPlaybackUpdate()
{
    if (m_playbackState != Playing || !m_activeCinematic) {
        return;
    }

    float elapsedTimeSecs = m_playbackTime.elapsed() / 1000.0f;
    float newTime = m_timeAtPlaybackStart + elapsedTimeSecs;
    float duration = m_activeCinematic->GetDuration();

    if (newTime >= duration && duration > 0) {
        newTime = duration;
        onStop(); // Stop and reset to zero when playback finishes.
    }

    updateCurrentTime(newTime, true);
}

void CinematicEditor::onCreateGraphNodeTrack()
{
    // The current design requires a valid GraphNode* to create a transform track.
    // The drag-and-drop workflow is the best way to provide this context.
    QMessageBox::information(this, "Create GraphNode Track",
        "This action requires a target node.\n\nPlease drag and drop a node from the scene graph onto the timeline to create a GraphNode track.");
}

void CinematicEditor::onCreateAudioTrack()
{
    if (m_activeCinematic) {
        // Create a new audio track.
        auto newTrack = std::make_unique<TrackAudio>("Audio Track");
        // Add it to the data model.
        m_activeCinematic.get()->AddTrack(std::move(newTrack));
        // Rebuild the UI to show the new track.
        rebuildUIFromCinematic();
    }
}

void CinematicEditor::updateButtonStates()
{
    switch (m_playbackState)
    {
    case Stopped:
        m_playAction->setEnabled(true);
        m_pauseAction->setEnabled(false);
        m_stopAction->setEnabled(false);
        break;
    case Playing:
        m_playAction->setEnabled(false);
        m_pauseAction->setEnabled(true);
        m_stopAction->setEnabled(true);
        break;
    case Paused:
        m_playAction->setEnabled(true);
        m_pauseAction->setEnabled(false);
        m_stopAction->setEnabled(true);
        break;
    }
}
void CinematicEditor::onPlay()
{
    if (m_playbackState == Playing) return;

    if (m_playbackState == Paused) {
        // Resume all paused sounds
        if (GameAudio::m_Instance) GameAudio::m_Instance->gSoloud.setPauseAll(false);
    }

    // If starting from a stopped state, m_currentTime will be 0.
    // If resuming from pause, it will be the paused time.
    m_timeAtPlaybackStart = m_currentTime;
    m_playbackTime.start();
    m_playbackTimer->start(16);

    m_playbackState = Playing;
    updateButtonStates();
}

void CinematicEditor::onPause()
{
    if (m_playbackState != Playing) return;

    m_playbackTimer->stop();
    if (GameAudio::m_Instance) GameAudio::m_Instance->gSoloud.setPauseAll(true);

    m_playbackState = Paused;
    updateButtonStates();
}

void CinematicEditor::onStop()
{
    if (m_playbackState == Stopped) return;

    m_playbackTimer->stop();
    if (GameAudio::m_Instance) GameAudio::m_Instance->gSoloud.stopAll();

    m_playbackState = Stopped;
    updateCurrentTime(0.0f);
    updateButtonStates();
}
void CinematicEditor::updateTimelineWidth()
{
    if (!m_activeCinematic || !m_laneScrollArea || !m_laneScrollArea->widget() || !m_timeRuler) {
        return;
    }

    // Get the cinematic's total duration (this now correctly includes audio length).
    float duration = m_activeCinematic->GetDuration();

    // Add the 1-second safety buffer.
    float totalTimeWithBuffer = duration + 1.0f;

    // Calculate the required width in pixels.
    int requiredWidth = totalTimeWithBuffer * PIXELS_PER_SECOND2;

    // Use the viewport's current width as a sensible minimum.
    int minPracticalWidth = 1000;
    if (m_laneScrollArea->viewport()) {
        minPracticalWidth = m_laneScrollArea->viewport()->width();
    }

    // --- FIXED --- Only apply the minimum width to the widget INSIDE the scroll area.
    // The TimeRuler does not need to be resized.
    m_laneScrollArea->widget()->setMinimumWidth(max2(minPracticalWidth, requiredWidth));
}

void CinematicEditor::onScrubbingFinished()
{
    if (GameAudio::m_Instance) {
        // Use SoLoud's function to stop every sound currently playing.
        GameAudio::m_Instance->gSoloud.stopAll();
    }
    // Also notify all tracks to clear their internal state.
    if (m_activeCinematic) {
        for (const auto& track : m_activeCinematic->GetTracks()) {
            track->StopAllSounds();
        }
    }
}