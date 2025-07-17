#pragma once

#include <QWidget>
#include "Cinematic.h"
#include <QElapsedTimer>

// Forward-declare Qt classes
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QToolBar;
class QAction;
class QSplitter;
class QTimer;
class QMenuBar; // --- ADDED
class QMenu;    // --- ADDED

// Forward-declare custom classes
class GraphNode;
class ITrack;
class TimeRuler;
class TrackHeaderWidget;
class TrackLaneWidget;
class PlayheadOverlay;

class CinematicEditor : public QWidget
{
    Q_OBJECT

public:
    CinematicEditor(SceneGraph* scene, QWidget* parent = nullptr);
    ~CinematicEditor();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onNewCinematic();
    void onSaveCinematic();
    void onLoadCinematic();
    void onGlobalSnapshot();
    void updateCurrentTime(float newTime, bool fromPlayback = false);
    void updateOverlayGeometry();
    void onKeyframeDragStarted();
    void onKeyframeDragFinished();
    void onGoToStart();
    void onGoToEnd();
    void onPlay();
    void onPause();
    void onStop();
    void onPlaybackUpdate();
    void onScrubbingFinished();
    // --- ADDED --- Slots for the new "Create" menu actions.
    void onCreateGraphNodeTrack();
    void onCreateAudioTrack();

private:
    void addTrack(ITrack* track);
    void clearAllTracks();
    void rebuildUIFromCinematic();
    void updateButtonStates();
    void updateTimelineWidth();
    enum PlaybackState { Stopped, Playing, Paused };
    // UI Components
    QVBoxLayout* m_mainLayout;
    QMenuBar* m_menuBar; // --- ADDED
    QToolBar* m_toolBar;
    QWidget* m_headerSpacer;
    TimeRuler* m_timeRuler;
    QSplitter* m_splitter;
    QScrollArea* m_headerScrollArea;
    QWidget* m_headerContainer;
    QVBoxLayout* m_headerLayout;
    QScrollArea* m_laneScrollArea;
    QVBoxLayout* m_laneLayout;
    PlayheadOverlay* m_playheadOverlay;
    SceneGraph* m_sceneGraph;
    // Menu Components
    QMenu* m_cinematicMenu; // --- ADDED
    QMenu* m_createMenu;    // --- ADDED

    // Toolbar & Menu Actions
    QAction* m_newAction;
    QAction* m_saveAction;
    QAction* m_loadAction;
    QAction* m_snapshotAction;
    QAction* m_goToStartAction;
    QAction* m_playAction;
    QAction* m_pauseAction;
    QAction* m_stopAction;
    QAction* m_goToEndAction;
    QAction* m_createGraphNodeTrackAction; // --- ADDED
    QAction* m_createAudioTrackAction;     // --- ADDED

    // Data Model
    std::unique_ptr<Cinematic> m_activeCinematic;
    float m_currentTime;

    // Playback state
    QTimer* m_playbackTimer;
    QElapsedTimer m_playbackTime;
    float m_timeAtPlaybackStart = 0.0f;
    PlaybackState m_playbackState = Stopped;
};