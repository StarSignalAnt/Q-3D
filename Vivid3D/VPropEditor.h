#pragma once

#include <QWidget>
#include "ui_VPropEditor.h"
#include <QVBoxLayout>
#include <qlabel.h>
#include "VImagePreview.h"
#include "VColorPreview.h"
#include "qspinbox.h"
#include "qcheckbox.h"
#include <qlineedit.h>
#include <qcombobox.h>
#include "VAnimationList.h"
class MaterialBase;
class Node;
class NodeTerrain;

class VPropEditor : public QWidget
{
	Q_OBJECT

public:
	VPropEditor(QWidget *parent = nullptr);
	~VPropEditor();
	void SetMaterial(MaterialBase* material);
	void SetNode(Node* node);
	
	void UpdateNode();
	void SetTerrain(NodeTerrain* node);

private:
	Ui::VPropEditorClass ui;
	QVBoxLayout* m_LO;
	MaterialBase* m_Material;
	Node* m_Node;

	//Material Editor
	VImagePreview* m_DiffuseImg;
	VImagePreview* m_SpecImg;
	VImagePreview* m_NormImg;
	VImagePreview* m_RoughImg;
	VImagePreview* m_MetalImg;
	VImagePreview* m_EnvImg;
	VColorPreview* m_DiffCol;
	VColorPreview* m_SpecCol;
	QDoubleSpinBox* m_RoughOD;
	QDoubleSpinBox* m_MetalOD;
	

	//Node
	QCheckBox* m_NodeEnabled;
	QLineEdit* m_NodeName;
	QDoubleSpinBox* m_NodePosX;
	QDoubleSpinBox* m_NodePosY;
	QDoubleSpinBox* m_NodePosZ;
	QDoubleSpinBox* m_NodeRotX;
	QDoubleSpinBox* m_NodeRotY;
	QDoubleSpinBox* m_NodeRotZ;
	QDoubleSpinBox* m_NodeScaleX;
	QDoubleSpinBox* m_NodeScaleY;
	QDoubleSpinBox* m_NodeScaleZ;
	bool m_BlockRot = false;
	bool m_BlockTimer = false;
	//Light
	VColorPreview* m_LightDiff;
	VColorPreview* m_LightSpec;
	QDoubleSpinBox* m_LightDiffR;
	QDoubleSpinBox* m_LightDiffG;
	QDoubleSpinBox* m_LightDiffB;
	QDoubleSpinBox* m_LightSpecR;
	QDoubleSpinBox* m_LightSpecG;
	QDoubleSpinBox* m_LightSpecB;
	QDoubleSpinBox* m_LightRange;

	std::vector<QWidget*> m_ScriptWidgets;

	//Terrain
	QSpinBox* m_EditLayer;
	NodeTerrain* m_Terrain = nullptr;
	QComboBox* m_EditTool;


	//Actor
	VAnimationList* m_AnimList;
	QLineEdit* m_AnimName;
	std::vector<Animation*> m_Anims;
	Animation* m_CurrentAnim = nullptr;
	bool m_BlockAnimNameChange = false;

};
