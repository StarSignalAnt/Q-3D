#include "VTrackNode.h"
#include <qpainter.h>
#include "Node.h"
#include "qboxlayout.h"
#include "qpushbutton.h"
#include "VTrackTimeLine.h"
#include "TrackKeyFrame.h"
#include "Editor.h"
#include "CineTrack.h"

VTrackNode::VTrackNode(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

    auto v = new QVBoxLayout;
    auto h1 = new QHBoxLayout;

    auto rec = new QPushButton("O");
    h1->addWidget(rec);
    h1->setAlignment(Qt::AlignRight);
    rec->setMaximumWidth(32);

    QObject::connect(rec, &QPushButton::clicked, [&]() {
    //    QMessageBox::information(&window, "Lambda Example", "Button was clicked!");
        
        auto cine_Track = m_TimeLine->GetCineTrack();

        TrackKeyFrame* frame = new TrackKeyFrame;
        frame->SetPosition(m_Node->GetPosition());
        frame->SetRotation(m_Node->GetRotation());
        frame->SetScale(m_Node->GetScale());
        frame->SetTime(Editor::m_AnimEditTime);
        cine_Track->InsertKeyFrame(frame, Editor::m_AnimEditTime);

        });



    v->addLayout(h1);
    setLayout(v);
}


VTrackNode::~VTrackNode()
{}


void VTrackNode::paintEvent(QPaintEvent* event)
{
	//QPainter painter(this);



    QPainter painter(this);

    QColor c1(60,60,60);
    QColor c2(90, 90, 90);

    QLinearGradient gradient(0, 0, 0, height()-1);
    gradient.setColorAt(0, c2);    // Start color (top)
    //gradient.setColorAt(0.5, Qt::darkGray); // Middle color
    gradient.setColorAt(1,c1);   // End color (bottom)


    // Set the brush to use the gradient
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    // Draw a rectangle filled with the gradient
    painter.drawRect(rect());

    QColor c5(30, 30, 30);
    QColor c6(60, 60, 60);

    QLinearGradient gradient3(0, 0, 0, height() - 1);
    gradient3.setColorAt(0, c5);    // Start color (top)
    //gradient.setColorAt(0.5, Qt::darkGray); // Middle color
    gradient3.setColorAt(1, c6);   // End color (bottom)


    // Set the brush to use the gradient
    painter.setBrush(gradient3);
    painter.setPen(Qt::NoPen);
    // Draw a rectangle filled with the gradient
    painter.drawRect(QRect(2,2,size().width()-4,size().height()-4));


    //
    auto c3 = QColor(200, 200, 200);
    auto c4 = QColor(150, 150, 150);


    auto grad2 = QLinearGradient(0, height() - 4,0, height());
    grad2.setColorAt(0, c3);
    grad2.setColorAt(1, c4);
    painter.setBrush(grad2);

    QRect bottomRect(0, height() - 4, width(), 4);
    painter.drawRect(bottomRect);


    //QColor bg(55, 55, 55, 200);
    // Set the brush color and style
   // painter.setBrush(bg); // Fill color
   // painter.setPen(Qt::white); // Outline color (if needed), or Qt::NoPen for no outline

    // Draw a filled rectangle
    //painter.drawRect(0,0,size().width()-1,size().height()-1);

    painter.setPen(Qt::black);
    painter.drawText(QPoint(7, 17), m_Node->GetFullName().c_str());
    painter.setPen(Qt::white);
    painter.drawText(QPoint(5, 15), m_Node->GetFullName().c_str());

}

void VTrackNode::SetNode(Node* node) {

    m_Node = node;

}