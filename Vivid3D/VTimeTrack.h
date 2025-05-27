#pragma once

#include <QWidget>
#include "ui_VTimeTrack.h"

class VTimeTrack : public QWidget
{
	Q_OBJECT

public:
	VTimeTrack(QWidget *parent = nullptr);
	~VTimeTrack();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

private:
	Ui::VTimeTrackClass ui;
	float m_Position = 0.0f;
	bool m_MouseTrack = false;
};
