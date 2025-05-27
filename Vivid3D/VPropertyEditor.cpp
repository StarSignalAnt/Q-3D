#include "VPropertyEditor.h"
#include "Editor.h"
#include "MaterialBase.h"
#include <qpushbutton.h>
#include "VPropEditor.h"

VPropertyEditor::VPropertyEditor(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	Editor::m_PropEditor = this;

	m_PropEdit = new VPropEditor(this);

	QScrollArea* scrollArea = new QScrollArea;
	scrollArea->setWidget(m_PropEdit);
	scrollArea->setWidgetResizable(true);

	auto lo = new QVBoxLayout(this);



	lo->addWidget(scrollArea);

	setLayout(lo);

}

void VPropertyEditor::SetMaterial(MaterialBase* material) {

	m_PropEdit->SetMaterial(material);
//	for (int i = 0; i < 50; i++) {

	//	QPushButton* b1 = new QPushButton("Testing!", this);
		//m_Prop->addWidget(b1);

	//}


}

void VPropertyEditor::SetNode(Node* node) {

	m_PropEdit->SetNode(node);

}

void VPropertyEditor::UpdateNode() {
	m_PropEdit->UpdateNode();
}

void VPropertyEditor::SetTerrain(NodeTerrain* node) {

	m_PropEdit->SetTerrain(node);

}

VPropertyEditor::~VPropertyEditor()
{}
