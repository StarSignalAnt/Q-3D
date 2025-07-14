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

constexpr int RULER_HEIGHT = 30;

// ===================================================================================
// CinematicEditor Implementation
// ===================================================================================
CinematicEditor::CinematicEditor(QWidget* parent)
    : QWidget(parent)
    , m_currentTime(0.0f)
{
    // Data Model
    m_activeCinematic = std::make_unique<Cinematic>();

    // Main Layout & Toolbar
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_toolBar = new QToolBar(this);
    m_toolBar->setMovable(false);
    m_newAction = new QAction(style()->standardIcon(QStyle::SP_FileIcon), "New", this);
    m_saveAction = new QAction(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
    m_loadAction = new QAction(style()->standardIcon(QStyle::SP_DialogOpenButton), "Load", this);
    m_snapshotAction = new QAction(style()->standardIcon(QStyle::SP_ArrowUp), "Snapshot All", this);
    m_snapshotAction->setToolTip("Record a keyframe on all tracks at the current time");
    m_toolBar->addAction(m_newAction);
    m_toolBar->addAction(m_saveAction);
    m_toolBar->addAction(m_loadAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_snapshotAction);
    m_mainLayout->addWidget(m_toolBar);

    // Timeline Area
    QWidget* timelineArea = new QWidget(this);
    QHBoxLayout* timelineLayout = new QHBoxLayout(timelineArea);
    timelineLayout->setContentsMargins(0, 0, 0, 0);
    timelineLayout->setSpacing(0);

    // Left Pane (Headers)
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

    // --- MODIFIED --- Assign to the member variable m_headerContainer.
    m_headerContainer = new QWidget();
    m_headerContainer->setAcceptDrops(true);
    // --- ADDED --- Install the event filter on the header area as well.
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

    // Right Pane (Timelines)
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

    // Splitter
    m_splitter = new QSplitter(Qt::Horizontal, timelineArea);
    m_splitter->setHandleWidth(2);
    m_splitter->addWidget(leftPaneWidget);
    m_splitter->addWidget(rightPaneWidget);
    m_splitter->setSizes({ 175, 1000 });
    timelineLayout->addWidget(m_splitter);

    m_mainLayout->addWidget(timelineArea);
    this->setLayout(m_mainLayout);

    // Connections
    connect(m_newAction, &QAction::triggered, this, &CinematicEditor::onNewCinematic);
    connect(m_saveAction, &QAction::triggered, this, &CinematicEditor::onSaveCinematic);
    connect(m_loadAction, &QAction::triggered, this, &CinematicEditor::onLoadCinematic);
    connect(m_snapshotAction, &QAction::triggered, this, &CinematicEditor::onGlobalSnapshot);

    connect(m_timeRuler, &TimeRuler::timeChanged, this, &CinematicEditor::updateCurrentTime);
    connect(m_playheadOverlay, &PlayheadOverlay::timeChanged, this, &CinematicEditor::updateCurrentTime);

    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, m_timeRuler, &TimeRuler::setScrollOffset);
    connect(m_laneScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, m_playheadOverlay, &PlayheadOverlay::setScrollOffset);
    connect(m_headerScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, m_laneScrollArea->verticalScrollBar(), &QScrollBar::setValue);
    connect(m_laneScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, m_headerScrollArea->verticalScrollBar(), &QScrollBar::setValue);

    rebuildUIFromCinematic();
}

CinematicEditor::~CinematicEditor() {}

// --- ADDED --- This new function now handles drag-and-drop for the correct widget.
bool CinematicEditor::eventFilter(QObject* watched, QEvent* event)
{
    bool isLaneWidget = m_laneScrollArea && (watched == m_laneScrollArea->widget());
    bool isHeaderWidget = m_headerContainer && (watched == m_headerContainer);

    if (isLaneWidget || isHeaderWidget) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent* dragEvent = static_cast<QDragEnterEvent*>(event);
            if (dragEvent->mimeData()->hasFormat("application/x-graphnode")) {
                dragEvent->acceptProposedAction();
                return true;
            }
        }
        else if (event->type() == QEvent::Drop) {
            QDropEvent* dropEvent = static_cast<QDropEvent*>(event);
            if (dropEvent->mimeData()->hasFormat("application/x-graphnode")) {
                QByteArray data = dropEvent->mimeData()->data("application/x-graphnode");
                bool ok;
                qulonglong nodeAddress = data.toULongLong(&ok);
                if (ok) {
                    GraphNode* droppedNode = reinterpret_cast<GraphNode*>(nodeAddress);
                    if (droppedNode && m_activeCinematic) {
                        auto newTrack = std::make_unique<TrackTransform>(droppedNode);
                        m_activeCinematic.get()->AddTrack(std::move(newTrack));
                        rebuildUIFromCinematic();
                        dropEvent->acceptProposedAction();
                        return true;
                    }
                }
            }
        }
    }

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
        InterpolationType type = header->isSteppedChecked() ? InterpolationType::Stepped : InterpolationType::Linear;
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
            InterpolationType type = headerWidget->isSteppedChecked()
                ? InterpolationType::Stepped
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

void CinematicEditor::updateCurrentTime(float newTime)
{
    m_currentTime = std::max(0.0f, newTime);
    if (m_activeCinematic) {
        m_activeCinematic->SetTime(m_currentTime);
    }
    m_timeRuler->setCurrentTime(m_currentTime);
    m_playheadOverlay->setCurrentTime(m_currentTime);
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