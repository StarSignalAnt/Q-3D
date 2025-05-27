#include "VTrackFrame.h"
#include "VTrackTimeLine.h"
#include <qboxlayout.h>
#include "Cinematic.h"
#include "Editor.h"

VTrackFrame::VTrackFrame(QWidget *parent)
	: QFrame(parent)
{
	ui.setupUi(this);
}

VTrackFrame::~VTrackFrame()
{}

VTrackTimeLine* VTrackFrame::AddTimeLine(Node* node) {

    auto lay = layout();

    VTrackTimeLine* new_Track = new VTrackTimeLine;

    //new_Track->setMaximumWidth(size().width() - 20);
    //new_Track->setMinimumWidth(size().width() - 20);
    new_Track->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    new_Track->setMaximumHeight(60);
    new_Track->setMinimumHeight(60);
    new_Track->SetNode(node);


    float len = Editor::m_EditAnimation->GetLength();

    float siz = len * 80;
    siz = siz + 40;

    this->setMinimumWidth(siz);
    this->setMaximumWidth(siz);



    lay->addWidget(new_Track);
    return new_Track;

}