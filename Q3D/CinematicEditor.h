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
#include <QTimer> // Added for robust resizing
#include "Cinematic.h"
// Forward-declare engine classes

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
    CinematicEditor(QWidget* parent = nullptr);
    ~CinematicEditor();

protected:
    // --- MODIFIED ---
    // The main widget no longer handles these events directly.
    // They are now handled by the event filter for the specific drop area.
    bool eventFilter(QObject* watched, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onNewCinematic();
    void onSaveCinematic();
    void onLoadCinematic();
    void onGlobalSnapshot();
    void updateCurrentTime(float newTime);
    void updateOverlayGeometry();
    void onKeyframeDragStarted();
    void onKeyframeDragFinished();
private:
    void addTrack(ITrack* track);
    void clearAllTracks();
    void rebuildUIFromCinematic();

    // Main layout for this widget
    QVBoxLayout* m_mainLayout;

    // UI Components
    QToolBar* m_toolBar;
    QWidget* m_headerSpacer;
    TimeRuler* m_timeRuler;
    QSplitter* m_splitter;

    // Left side of splitter (Headers)
    QScrollArea* m_headerScrollArea;
    QWidget* m_headerContainer;
    QVBoxLayout* m_headerLayout;

    // Right side of splitter (Lanes)
    QScrollArea* m_laneScrollArea;
    QVBoxLayout* m_laneLayout;
    PlayheadOverlay* m_playheadOverlay;

    // Toolbar Actions
    QAction* m_newAction;
    QAction* m_saveAction;
    QAction* m_loadAction;
    QAction* m_snapshotAction;

    // Data Model
    std::unique_ptr<Cinematic> m_activeCinematic;
    float m_currentTime;
};