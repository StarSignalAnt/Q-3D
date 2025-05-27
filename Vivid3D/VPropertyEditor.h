#pragma once

#include <QWidget>
#include "ui_VPropertyEditor.h"
#include <qscrollarea.h>
#include "QVBoxLayout"

class MaterialBase;
class VPropEditor;
class Node;
class NodeTerrain;


class VPropertyEditor : public QWidget
{
	Q_OBJECT

public:
	VPropertyEditor(QWidget *parent = nullptr);
	~VPropertyEditor();

	void SetMaterial(MaterialBase* material);
	void SetNode(Node* node);
	void SetTerrain(NodeTerrain* node);
	void UpdateNode();

private:
	Ui::VPropertyEditorClass ui;
	QScrollArea* m_Scroll;
	QVBoxLayout* m_Prop;
	VPropEditor* m_PropEdit;
};
