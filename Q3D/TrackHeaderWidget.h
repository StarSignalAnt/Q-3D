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
#include <QCheckBox>

class ITrack;

class TrackHeaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrackHeaderWidget(ITrack* track, QWidget* parent = nullptr);

    QPushButton* getRecordButton() const { return m_recordButton; }
    bool isSnappedChecked() const;
protected:
    void paintEvent(QPaintEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
private:
    QLabel* m_nameLabel;
    QPushButton* m_recordButton;
    QCheckBox* m_SnappedCheck;
    ITrack* m_track;
};
