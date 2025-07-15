#include "TrackHeaderWidget.h"
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

TrackHeaderWidget::TrackHeaderWidget(const QString& name, QWidget* parent) : QWidget(parent)
{
    setFixedHeight(60);

    QGridLayout* gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(5, 5, 5, 5);
    gridLayout->setSpacing(2); // Add a little space

    m_nameLabel = new QLabel(name, this);
    m_nameLabel->setStyleSheet("color: white; background: transparent; border: none;");

    m_recordButton = new QPushButton(this);
    m_recordButton->setFixedSize(24, 24);
    m_recordButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_recordButton->setToolTip("Record a keyframe at the current time");
    m_recordButton->setStyleSheet(
        "QPushButton { background-color: #c0392b; border-radius: 12px; border: none; }"
        "QPushButton:hover { background-color: #e74c3c; }"
        "QPushButton:pressed { background-color: #a93226; }"
    );

    // --- ADDED --- Create and style the checkbox.
    m_SnappedCheck = new QCheckBox("Snapped", this);
    m_SnappedCheck->setStyleSheet("QCheckBox { color: white; background: transparent; }");
    m_SnappedCheck->setToolTip("Create a 'Snapped' keyframe that holds its value until the next key");


    gridLayout->addWidget(m_nameLabel, 0, 0, Qt::AlignTop | Qt::AlignLeft);
    // --- MODIFIED --- Add the new controls to the grid layout.
    gridLayout->addWidget(m_SnappedCheck, 1, 0, Qt::AlignBottom | Qt::AlignLeft);
    gridLayout->addWidget(m_recordButton, 1, 1, Qt::AlignBottom | Qt::AlignRight);

    gridLayout->setRowStretch(0, 1);
    gridLayout->setColumnStretch(0, 1);

    setLayout(gridLayout);
}

void TrackHeaderWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor("#5a5a5a"));
    gradient.setColorAt(1, QColor("#4f4f4f"));
    painter.fillRect(rect(), gradient);
    painter.setPen(QColor("#666"));
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

bool TrackHeaderWidget::isSnappedChecked() const
{
    return m_SnappedCheck->isChecked();
}
