#include "VAnimationList.h"
#include "Animation.h"


VAnimationList::VAnimationList(QWidget *parent)
	: QListWidget(parent)
{
	
	ui.setupUi(this);
	setAcceptDrops(true);
}

VAnimationList::~VAnimationList()
{}

void VAnimationList::SetAnimations(std::vector<Animation*> anims)
{
	clear();
	for (auto anim : anims) {

		addItem(anim->GetName().c_str());

	}

}


void VAnimationList::dragEnterEvent(QDragEnterEvent* event)
{
//	 if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();
	
	// }
}

void VAnimationList::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	auto url = mimeData->urls()[0];
	int a = 5;


}