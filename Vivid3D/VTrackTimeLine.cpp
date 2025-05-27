#include "VTrackTimeLine.h"
#include "qpainter.h"
#include "qevent.h"
#include "CineTrack.h"
#include "Cinematic.h"
#include "TrackKeyFrame.h"

VTrackTimeLine::VTrackTimeLine(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

void VTrackTimeLine::paintEvent(QPaintEvent* event)
{
	//QPainter painter(this);



	QPainter painter(this);

    QColor c1(0, 60, 60);
    QColor c2(0, 90, 90);

    QLinearGradient gradient(0, 0, 0, height() - 1);
    gradient.setColorAt(0, c2);    // Start color (top)
    //gradient.setColorAt(0.5, Qt::darkGray); // Middle color
    gradient.setColorAt(1, c1);   // End color (bottom)


    // Set the brush to use the gradient
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    // Draw a rectangle filled with the gradient
    painter.drawRect(rect());

    //


    QColor c3(0, 20, 20);
    QColor c4(0, 40, 40);

    QLinearGradient gradient2(0, 0, 0, height() - 1);
    gradient2.setColorAt(0, c4);    // Start color (top)
    //gradient.setColorAt(0.5, Qt::darkGray); // Middle color
    gradient2.setColorAt(1, c3);   // End color (bottom)


    // Set the brush to use the gradient
    painter.setBrush(gradient2);
    painter.setPen(Qt::NoPen);
    // Draw a rectangle filled with the gradient
    painter.drawRect(QRect(2,2,size().width()-4,size().height()-4));

    auto track = m_Track;
    auto cine = track->GetOwner();

    int pix = cine->GetLength() * 80;

    for (auto frame : track->GetFrames()) {

        float time = frame->GetTime();
        float px = time * 80.0f;
        painter.setBrush(Qt::NoBrush);
        painter.setPen(Qt::white);
        painter.drawRect(QRect(px - 1, 2, 3, size().height() - 4));

    }


    setMinimumWidth(pix);
    setMaximumWidth(pix);

    //80


    update();


}


VTrackTimeLine::~VTrackTimeLine()
{}
