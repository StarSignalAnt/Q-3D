#include "VTimeTrack.h"
#include <qpainter.h>
#include <qstatictext.h>
#include "qevent.h"
#include "Editor.h"
#include "Cinematic.h"

VTimeTrack::VTimeTrack(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setMouseTracking(true);
}


void VTimeTrack::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	int cx = 9;
	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(1);
	painter.setPen(pen);
	int sec = 0;

	for (int x = 0; x < size().width(); x = x + 8) {

		if (cx == 9) {

			int bb = 5;
			painter.drawLine(QLine(x, 4, x, 8+15));
			painter.drawStaticText(QPoint(x+4, 0), QStaticText(std::to_string(sec).c_str()));
			sec++;
		}
		else if (cx == 4) {
			painter.drawLine(QLine(x, 10, x, (8 + 15)));
		}else{
			painter.drawLine(QLine(x,8+15, x, 8+12));
		}

		cx++;
		if (cx == 10) {
			cx = 0;
		}
	
	}

	pen.setColor(Qt::green);
	painter.setPen(pen);
	float px = 80 * m_Position;
	painter.drawLine(QLine(px-1, 0, px-1, 30));
	painter.drawLine(QLine(px, 0, px, 30));
	painter.drawLine(QLine(px+1, 0, px+1, 30));

}

void VTimeTrack::mouseMoveEvent(QMouseEvent* event) {

	if (m_MouseTrack) {
		float px = (float)event->pos().x();
		m_Position = px / 80.0f;
		Editor::m_AnimEditTime = m_Position;
		Editor::m_EditAnimation->SetTime(m_Position);
		update();
	}
}

void VTimeTrack::mousePressEvent(QMouseEvent* event) {
	float px = (float)event->pos().x();
	m_Position = px / 80.0f;
	Editor::m_AnimEditTime = m_Position;
	m_MouseTrack = true;
	Editor::m_EditAnimation->SetTime(m_Position);
	update();

}

void VTimeTrack::mouseReleaseEvent(QMouseEvent* event) {

	m_MouseTrack = false;

}

VTimeTrack::~VTimeTrack()
{}
