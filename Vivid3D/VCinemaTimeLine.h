#pragma once

#include <QWidget>
#include "ui_VCinemaTimeLine.h"
#include "qboxlayout.h"

class VCinemaTimeLine : public QWidget
{
	Q_OBJECT

public:
	VCinemaTimeLine(QWidget *parent = nullptr);
	~VCinemaTimeLine();

private:
	Ui::VCinemaTimeLineClass ui;
	QVBoxLayout* m_LO;
	QVBoxLayout* m_NodeLO;
	QVBoxLayout* m_TrackLO;
};
