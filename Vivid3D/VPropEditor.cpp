#include "VPropEditor.h"
#include "MaterialBase.h"
#include "qpushbutton.h"
#include "qdrag.h"
#include <QDragEnterEvent>
#include "Texture2D.h"
#include "qmimedata.h"
#include <QColor>
#include <qcolordialog.h>
#include <qtimer.h>
#include "Editor.h"
#include "NodeLight.h"
#include "VSceneGraph.h"
#include "MaterialMeshPBR.h"
#include "VClass.h"
#include "VVar.h"
#include "VScope.h"
#include "VName.h"
#include "TextureCube.h"
#include "NodeTerrain.h"
#include "NodeActor.h"
#include "Animator.h"
#include "Animation.h"
#include "TerrainLayer.h"
#include <QFileDialog>
#include "Importer.h"
#include "VScriptClass.h"

//#include <qcheckbox.h>


VPropEditor::VPropEditor(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_LO = new QVBoxLayout(this);
	m_LO->setAlignment(Qt::AlignTop);
	m_LO->setSpacing(15);
	//setLayout(m_LO);

	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, [&]() {
			
		if (m_Node != nullptr) {
			
			auto check = dynamic_cast<NodeTerrain*>(m_Node);
			if (check != nullptr) return;


			if (m_BlockTimer) return;
			m_BlockRot = true;
			//update pos
			auto pos = m_Node->GetPosition();
			m_NodePosX->setValue(pos.x);
			m_NodePosY->setValue(pos.y);
			m_NodePosZ->setValue(pos.z);
			//update rot
			//return;
			auto rot = m_Node->GetRotationEU();
		//	m_NodeRotX->setValue(rot.y);
	//		m_NodeRotY->setValue(rot.x);
			//m_NodeRotZ->setValue(rot.z);
			m_BlockRot = false;


			
		}


		});

	// Start the timer with an interval of 1000 milliseconds (1 second)
	timer->start(1000);
}

void VPropEditor::UpdateNode() {

	if (m_Node == nullptr) return;
	if (m_NodeRotX == nullptr) return;
	auto check = dynamic_cast<NodeTerrain*>(m_Node);// != nullptr
	if (check != nullptr) return;

	m_BlockRot = true;
	auto rot = m_Node->GetRotationEU();
	m_NodeRotX->setValue(360-rot.x);
	m_NodeRotY->setValue(360-rot.y);
	m_NodeRotZ->setValue(360-rot.z);
	auto pos = m_Node->GetPosition();
	m_NodePosX->setValue(pos.x);
	m_NodePosY->setValue(pos.y);
	m_NodePosZ->setValue(pos.z);
	auto scale = m_Node->GetScale();
	m_NodeScaleX->setValue(scale.x);
	m_NodeScaleY->setValue(scale.y);
	m_NodeScaleZ->setValue(scale.z);

	if (dynamic_cast<NodeLight*>(m_Node) != nullptr) {

		auto light = (NodeLight*)m_Node;

		auto diff = light->GetDiffuse();
		auto spec = light->GetSpecular();

		m_LightDiffR->setValue(diff.x);
		m_LightDiffG->setValue(diff.y);
		m_LightDiffB->setValue(diff.z);
		
		m_LightSpecR->setValue(spec.x);
		m_LightSpecG->setValue(spec.y);
		m_LightSpecB->setValue(spec.z);

		if (m_DiffCol != nullptr) {
			m_DiffCol->SetColor(float4(diff, 1.0));
			m_SpecCol->SetColor(float4(spec, 1.0));
		}
		m_LightRange->setValue(light->GetRange());
	}


	m_BlockRot = false;
}

void ne_clearLayout(QLayout* layout)
{
	if (layout) {
		while (layout->count() > 0) {
			QLayoutItem* item = layout->takeAt(0);
			QWidget* widget = item->widget();
			QLayout* childLayout = item->layout(); // Check if the item is a layout

			// If the item is a widget, delete it
			if (widget) {
				widget->setParent(nullptr);
				delete widget;
			}
			// If the item is a layout, recursively clear it
			else if (childLayout) {
				ne_clearLayout(childLayout);
			}

			delete item;
		}
	}
}

void VPropEditor::SetMaterial(MaterialBase* material) {

	m_Material = material;
	if (layout() != nullptr) {

		ne_clearLayout(layout());
		//	return;
		m_LO = (QVBoxLayout*)layout();

	}
	else {
		m_LO = new QVBoxLayout(this);
		m_LO->setAlignment(Qt::AlignTop);
		m_LO->setSpacing(15);
		//	setLayout(m_LO);

	}



		auto pbr = (MaterialMeshPBR*)material;

		QLabel* mat_edit_lab = new QLabel(std::string("Material PBR:" + material->GetPath()).c_str());

		m_LO->addWidget(mat_edit_lab);
		QHBoxLayout* dif_box = new QHBoxLayout(this);

		auto dif_lab = new QLabel("Albedo");

		m_DiffuseImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_DiffuseImg->SetSize(64, 64);
		m_DiffuseImg->SetImage(material->GetDiffuse()->GetPath().c_str());
		QObject::connect(m_DiffuseImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetDiffuse(new Texture2D(filePath.toStdString()));
			});
		//m_DiffuseImg->setPixmap(scaledPixmap);
	//	m_DiffuseImg->setFixedSize(64, 64);
		//m_DiffuseImg->setAcceptDrops(true);


		dif_box->addWidget(dif_lab);
		dif_box->addWidget(m_DiffuseImg);

		dif_box->setAlignment(Qt::AlignLeft);
		dif_box->setSpacing(15);

		auto dif_browse = new QPushButton("Browse");

		dif_box->addWidget(dif_browse);

		m_LO->addLayout(dif_box);

		auto norm_box = new QHBoxLayout(this);

		auto norm_lab = new QLabel("Normals");

		m_NormImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_NormImg->SetSize(64, 64);
		m_NormImg->SetImage(material->GetNormal()->GetPath().c_str());
		QObject::connect(m_NormImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetNormals(new Texture2D(filePath.toStdString()));
			});



		norm_box->addWidget(norm_lab);
		norm_box->addWidget(m_NormImg);


		norm_box->setAlignment(Qt::AlignLeft);
		norm_box->setSpacing(15);


		auto norm_browse = new QPushButton("Browse");

		norm_box->addWidget(norm_browse);

		m_LO->addLayout(norm_box);


		//---

		auto rough_box = new QHBoxLayout(this);

		auto rough_lab = new QLabel("Roughness");

		m_RoughImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_RoughImg->SetSize(64, 64);
		m_RoughImg->SetImage(material->GetRough()->GetPath().c_str());
		QObject::connect(m_RoughImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetRough(new Texture2D(filePath.toStdString()));
			});


		rough_box->addWidget(rough_lab);
		rough_box->addWidget(m_RoughImg);

		rough_box->setAlignment(Qt::AlignLeft);
		rough_box->setSpacing(15);

		auto rough_browse = new QPushButton("Browse");

		rough_box->addWidget(rough_browse);

		
	//	spec_lab->setAlignment(Qt::AlignTop);
	//	norm_lab->setAlignment(Qt::AlignTop);


		m_LO->addLayout(rough_box);
		//
		auto metal_box = new QHBoxLayout(this);

		auto metal_lab = new QLabel("Metalic");

		m_MetalImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_MetalImg->SetSize(64, 64);
		m_MetalImg->SetImage(material->GetMetal()->GetPath().c_str());
		QObject::connect(m_MetalImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetMetal(new Texture2D(filePath.toStdString()));
			});


		metal_box->addWidget(metal_lab);
		metal_box->addWidget(m_MetalImg);

		metal_box->setAlignment(Qt::AlignLeft);
		metal_box->setSpacing(15);

		auto metal_browse = new QPushButton("Browse");

		metal_box->addWidget(metal_browse);

		m_LO->addLayout(metal_box);


		dif_lab->setMinimumWidth(55);
		norm_lab->setMinimumWidth(55);
		rough_lab->setMinimumWidth(55);
		metal_lab->setMinimumWidth(55);
		dif_lab->setAlignment(Qt::AlignTop);
		norm_lab->setAlignment(Qt::AlignTop);
		rough_lab->setAlignment(Qt::AlignTop);
		metal_lab->setAlignment(Qt::AlignTop);

		auto rod_box = new QHBoxLayout(this);

		auto rod_lab = new QLabel("Rough Overdrive");

		rod_lab->setMinimumWidth(55);



		/*
		auto diff_lab = new QLabel();
		diff_lab->setAutoFillBackground(true);


		QColor dif_col;


		float4 col = m_Material->GetDiffuseColor();
		dif_col.setRgb(col.x * 255.0f, col.y * 255.0f, col.z * 255.05, 255);
		QPalette pal = diff_lab->palette();
		pal.setColor(QPalette::Window, dif_col);
		diff_lab->setPalette(pal);

		col_diff_box->addWidget(col_dif_lab);
		col_diff_box->addWidget(diff_lab);
		col_dif_lab->setAlignment(Qt::AlignTop);

		diff_lab->setMinimumWidth(16);
		diff_lab->setMinimumHeight(12);
		*/
		//m_DiffCol = new VColorPreview;
		m_RoughOD = new QDoubleSpinBox();
		m_RoughOD->setSingleStep(0.01f);
		m_RoughOD->setRange(-2.0f, 16.0f);
		m_RoughOD->setValue(pbr->GetRoughOverdrive());

		QObject::connect(m_RoughOD, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double newValue) {
				//qDebug() << "New value:" << newValue;
				auto pbr = (MaterialMeshPBR*)m_Material;
				pbr->SetRoughOverdrive(newValue);
				// Do something with newValue...
			});


		m_RoughOD->setMinimumWidth(120);

		rod_box->addWidget(rod_lab);
		rod_box->addWidget(m_RoughOD);
		//m_DiffCol->SetColor(m_Material->GetDiffuseColor());
		//diff_prev->setAcceptDrops()
		rod_box->setAlignment(Qt::AlignLeft);


		




		//------

		auto mod_box = new QHBoxLayout(this);

		auto mod_lab = new QLabel("Metal Overdrive");

		mod_lab->setMinimumWidth(55);


		//m_SpecCol = new VColorPreview;
		m_MetalOD = new QDoubleSpinBox();

		m_MetalOD->setMinimumWidth(120);

		m_MetalOD->setValue(pbr->GetMetalOverdrive());
		m_MetalOD->setSingleStep(0.01f);
		m_MetalOD->setRange(-2.0f, 16.0f);

		QObject::connect(m_MetalOD, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double newValue) {
				//qDebug() << "New value:" << newValue;
				auto pbr = (MaterialMeshPBR*)m_Material;
				pbr->SetMetalOverdrive(newValue);
				// Do something with newValue...
			});
		mod_box->addWidget(mod_lab);
		mod_box->addWidget(m_MetalOD);

		//m_SpecCol->SetColor(m_Material->GetSpecularColor());
		//diff_prev->setAcceptDrops()
		mod_box->setAlignment(Qt::AlignLeft);


		auto env_box = new QHBoxLayout();

		auto env_lab = new QLabel("Cube Map");

		env_box->addWidget(env_lab);
		m_EnvImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);


		m_EnvImg->SetSize(64, 64);
		if (m_Material->GetEnvironmentTex() != nullptr) {
			m_EnvImg->SetImage(m_Material->GetEnvironmentTex()->GetPath());
		}
		QObject::connect(m_EnvImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetEnvironmentTex(new TextureCube(filePath.toStdString()));
			});


		
		env_box->addWidget(m_EnvImg);

		env_box->setAlignment(Qt::AlignLeft);
		env_box->setSpacing(15);

		auto env_browse = new QPushButton("Browse");

		env_box->addWidget(env_browse);

		m_LO->addLayout(env_box);
		


		QPushButton* save = new QPushButton("Apply");

		save->setMaximumWidth(80);

		QObject::connect(save, &QPushButton::clicked, [&]() {
			//	qDebug() << "Button clicked!";
			m_Material->SaveMaterial(m_Material->GetPath());
			});

		m_LO->addLayout(rod_box);
		m_LO->addLayout(mod_box);
		m_LO->addWidget(save);

	/*
		QLabel* mat_edit_lab = new QLabel(std::string("Material:" + material->GetPath()).c_str());

		m_LO->addWidget(mat_edit_lab);
		QHBoxLayout* dif_box = new QHBoxLayout(this);

		auto dif_lab = new QLabel("Diffuse");

		m_DiffuseImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_DiffuseImg->SetSize(64, 64);
		m_DiffuseImg->SetImage(material->GetDiffuse()->GetPath().c_str());
		QObject::connect(m_DiffuseImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetDiffuse(new Texture2D(filePath.toStdString()));
			});
		//m_DiffuseImg->setPixmap(scaledPixmap);
	//	m_DiffuseImg->setFixedSize(64, 64);
		//m_DiffuseImg->setAcceptDrops(true);


		dif_box->addWidget(dif_lab);
		dif_box->addWidget(m_DiffuseImg);

		dif_box->setAlignment(Qt::AlignLeft);
		dif_box->setSpacing(15);

		auto dif_browse = new QPushButton("Browse");

		dif_box->addWidget(dif_browse);

		m_LO->addLayout(dif_box);

		auto spec_box = new QHBoxLayout(this);

		auto spec_lab = new QLabel("Spcular");

		m_SpecImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_SpecImg->SetSize(64, 64);
		m_SpecImg->SetImage(material->GetSpecular()->GetPath().c_str());
		QObject::connect(m_SpecImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetSpecular(new Texture2D(filePath.toStdString()));
			});



		spec_box->addWidget(spec_lab);
		spec_box->addWidget(m_SpecImg);

		spec_box->setAlignment(Qt::AlignLeft);
		spec_box->setSpacing(15);

		auto spec_browse = new QPushButton("Browse");

		spec_box->addWidget(spec_browse);

		m_LO->addLayout(spec_box);


		//---

		auto norm_box = new QHBoxLayout(this);

		auto norm_lab = new QLabel("Normals");

		m_NormImg = new VImagePreview(this);
		//	QPixmap pix = QPixmap(material->GetDiffuse()->GetPath().c_str());
			//QPixmap scaledPixmap = pix.scaled(64,64, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		m_NormImg->SetSize(64, 64);
		m_NormImg->SetImage(material->GetNormal()->GetPath().c_str());
		QObject::connect(m_NormImg, &VImagePreview::dropped, [&](const QString& filePath) {
			//qDebug() << "File dropped:" << filePath;
			m_Material->SetNormals(new Texture2D(filePath.toStdString()));
			});


		norm_box->addWidget(norm_lab);
		norm_box->addWidget(m_NormImg);

		norm_box->setAlignment(Qt::AlignLeft);
		norm_box->setSpacing(15);

		auto norm_browse = new QPushButton("Browse");

		norm_box->addWidget(norm_browse);

		dif_lab->setMinimumWidth(55);
		spec_lab->setMinimumWidth(55);
		norm_lab->setMinimumWidth(55);
		dif_lab->setAlignment(Qt::AlignTop);
		spec_lab->setAlignment(Qt::AlignTop);
		norm_lab->setAlignment(Qt::AlignTop);

		m_LO->addLayout(norm_box);

		auto col_diff_box = new QHBoxLayout(this);

		auto col_dif_lab = new QLabel("Diffuse");

		col_dif_lab->setMinimumWidth(55);


		m_DiffCol = new VColorPreview;

		col_diff_box->addWidget(col_dif_lab);
		col_diff_box->addWidget(m_DiffCol);
		m_DiffCol->SetColor(m_Material->GetDiffuseColor());
		//diff_prev->setAcceptDrops()
		col_diff_box->setAlignment(Qt::AlignLeft);

		QPushButton* pick_diff = new QPushButton("Pick Color");
		col_diff_box->addWidget(pick_diff);
		pick_diff->setMinimumWidth(50);

		QObject::connect(pick_diff, &QPushButton::clicked, [&]() {
			//	qDebug() << "Button clicked!";
		//	m_Material->SaveMaterial(m_Material->GetPath());
			QColor color = QColorDialog::getColor(Qt::white, this, "Select Diffuse Color");
			if (color.isValid()) {
				//updateColor(color);
				m_DiffCol->SetColor(float4(color.redF(), color.greenF(), color.blueF(), 1.0));
				m_Material->SetDiffuseColor(float4(color.redF(), color.greenF(), color.blueF(), 1.0f));
			}
			});



		//------

		auto col_spec_box = new QHBoxLayout(this);

		auto col_spec_lab = new QLabel("Specular");

		col_spec_lab->setMinimumWidth(55);

		m_SpecCol = new VColorPreview;

		col_spec_box->addWidget(col_spec_lab);
		col_spec_box->addWidget(m_SpecCol);
		m_SpecCol->SetColor(m_Material->GetSpecularColor());
		//diff_prev->setAcceptDrops()
		col_spec_box->setAlignment(Qt::AlignLeft);

		QPushButton* pick_spec = new QPushButton("Pick Color");
		col_spec_box->addWidget(pick_spec);
		pick_spec->setMinimumWidth(50);
		//---------

		QObject::connect(pick_spec, &QPushButton::clicked, [&]() {
			//	qDebug() << "Button clicked!";
		//	m_Material->SaveMaterial(m_Material->GetPath());
			QColor color = QColorDialog::getColor(Qt::white, this, "Select Specular Color");
			if (color.isValid()) {
				//updateColor(color);
				m_SpecCol->SetColor(float4(color.redF(), color.greenF(), color.blueF(), 1.0));
				m_Material->SetSpecularColor(float4(color.redF(), color.greenF(), color.blueF(), 1.0f));
			}
			});




		QPushButton* save = new QPushButton("Apply");

		save->setMaximumWidth(80);

		QObject::connect(save, &QPushButton::clicked, [&]() {
			//	qDebug() << "Button clicked!";
			m_Material->SaveMaterial(m_Material->GetPath());
			});

		m_LO->addLayout(col_diff_box);
		m_LO->addLayout(col_spec_box);
		m_LO->addWidget(save);

	
	*/

}

void VPropEditor::SetNode(Node* node) {

	if (layout() != nullptr) {

		ne_clearLayout(layout());
		//	return;
		m_LO = (QVBoxLayout*)layout();

	}
	else {
		m_LO = new QVBoxLayout(this);
		m_LO->setAlignment(Qt::AlignTop);
		m_LO->setSpacing(15);
	//	setLayout(m_LO);

	}
	if (node == nullptr) {
		m_Node = nullptr;
		ne_clearLayout(layout());
		return;
	}

	int b = 5;

	m_Node = node;

	QHBoxLayout* enable_box = new QHBoxLayout(this);



	m_NodeEnabled = new QCheckBox("Enabled");

	if (m_Node->GetEnabled()) {
		m_NodeEnabled->setChecked(true);
	}
	else {
		m_NodeEnabled->setChecked(false);
	}

	QObject::connect(m_NodeEnabled, &QCheckBox::checkStateChanged, [&](int state) {
		//		qDebug() << "Checkbox state changed to:" << state;
		if (m_NodeEnabled->isChecked()) {
			m_Node->SetEnable(true);
		}
		else {
			m_Node->SetEnable(false);
		}

		});


	enable_box->setAlignment(Qt::AlignLeft);
	enable_box->setSpacing(5);


	m_LO->addWidget(m_NodeEnabled);

	QHBoxLayout* name_lo = new QHBoxLayout(this);

	QLabel* node_edit_lab = new QLabel("Node  ");

	m_NodeName = new QLineEdit(this);

	m_NodeName->setText(m_Node->GetName().c_str());

	m_NodeName->setMinimumWidth(120);
	m_NodeName->setMaximumWidth(280);
	node_edit_lab->setMaximumWidth(50);

	QObject::connect(m_NodeName, &QLineEdit::textChanged, [&](const QString& text) {
	//	qDebug() << "Text changed:" << text;
		m_Node->SetName(text.toStdString());
		Editor::m_SceneGraph->UpdateGraph();
		});


	name_lo->addWidget(node_edit_lab);
	name_lo->addWidget(m_NodeName);
	name_lo->setAlignment(Qt::AlignLeft);
	name_lo->setSpacing(5);
	

	m_LO->addLayout(name_lo);

	


	//Node::Position

	QHBoxLayout* pos_box = new QHBoxLayout(this);

	auto pos_lab = new QLabel("Position");

	auto xlab = new QLabel("X");
	auto ylab = new QLabel("Y");
	auto zlab = new QLabel("Z");

	m_NodePosX = new QDoubleSpinBox(this);
	m_NodePosY = new QDoubleSpinBox(this);
	m_NodePosZ = new QDoubleSpinBox(this);
	m_NodePosX->setSingleStep(0.1f);
	m_NodePosY->setSingleStep(0.1f);
	m_NodePosZ->setSingleStep(0.1f);
	m_NodePosX->setMinimumWidth(100);
	m_NodePosY->setMinimumWidth(100);
	m_NodePosZ->setMinimumWidth(100);


	float3 pos = node->GetPosition();
	m_NodePosX->setValue(pos.x);
	m_NodePosY->setValue(pos.y);
	m_NodePosZ->setValue(pos.z);

	m_NodePosX->setDecimals(2);
	m_NodePosX->setRange(-10000, 10000);

	m_NodePosY->setDecimals(2);
	m_NodePosY->setRange(-10000, 10000);

	m_NodePosZ->setDecimals(2);
	m_NodePosZ->setRange(-10000, 10000);

	// Position
	pos_box->addWidget(pos_lab);
	pos_box->addWidget(xlab);
	pos_box->addWidget(m_NodePosX);
	pos_box->addWidget(ylab);
	pos_box->addWidget(m_NodePosY);
	pos_box->addWidget(zlab);
	pos_box->addWidget(m_NodePosZ);

	//position events

	QObject::connect(m_NodePosX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			auto pos = m_Node->GetPosition();
			pos.x = value;
			m_Node->SetPosition(pos);

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodePosY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			auto pos = m_Node->GetPosition();
			pos.y = value;
			m_Node->SetPosition(pos);

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodePosZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			auto pos = m_Node->GetPosition();
			pos.z = value;
			m_Node->SetPosition(pos);

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});

	pos_box->setAlignment(Qt::AlignLeft);
	pos_box->setSpacing(5);

	m_LO->addLayout(pos_box);

	//Rotation


	QHBoxLayout* rot_box = new QHBoxLayout(this);

	auto rot_lab = new QLabel("Rotation");

	xlab = new QLabel("P");
	ylab = new QLabel("Y");
	zlab = new QLabel("R");


	m_NodeRotX = new QDoubleSpinBox(this);
	m_NodeRotY = new QDoubleSpinBox(this);
	m_NodeRotZ = new QDoubleSpinBox(this);
	m_NodeRotX->setSingleStep(5);
	m_NodeRotY->setSingleStep(5);
	m_NodeRotZ->setSingleStep(5);
	m_NodeRotX->setMinimumWidth(100);
	m_NodeRotY->setMinimumWidth(100);
	m_NodeRotZ->setMinimumWidth(100);


	float3 rot = float3(0, 0, 0);// node->GetRotationEU();
	m_NodeRotX->setValue(rot.x);
	m_NodeRotY->setValue(rot.y);
	m_NodeRotZ->setValue(rot.z);

	m_NodeRotX->setDecimals(3);
	m_NodeRotX->setRange(-1000,1000);

	m_NodeRotY->setDecimals(3);
	m_NodeRotY->setRange(-1000,1000);

	m_NodeRotZ->setDecimals(3);
	m_NodeRotZ->setRange(-1000,10000);

	// Position
	rot_box->addWidget(rot_lab);
	rot_box->addWidget(xlab);
	rot_box->addWidget(m_NodeRotX);
	rot_box->addWidget(ylab);
	rot_box->addWidget(m_NodeRotY);
	rot_box->addWidget(zlab);
	rot_box->addWidget(m_NodeRotZ);

	QObject::connect(m_NodeRotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			m_BlockTimer = true;
			if (m_BlockRot) return;
			auto rot = m_Node->GetRotationEU();
			rot.x = value;
			rot.y = m_NodeRotY->value();
			rot.z = m_NodeRotZ->value();
			
			m_Node->SetRotation(rot.x, rot.y, rot.z,true);
			m_BlockTimer = false;
			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodeRotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {

			m_BlockTimer = true;
			if (m_BlockRot) return;
			auto rot = m_Node->GetRotationEU();
			rot.x = m_NodeRotX->value();
			rot.y = value;
			rot.z = m_NodeRotZ->value();


			m_Node->SetRotation(rot.x, rot.y, rot.z,true);
			m_BlockTimer = false;

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodeRotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			if (m_BlockRot) return;
			auto rot = m_Node->GetRotationEU();
			rot.z = value;


			//auto rot = m_Node->GetRotationEU();
			rot.x = m_NodeRotX->value();
			rot.z = value;
			rot.y = m_NodeRotY->value();


			m_Node->SetRotation(rot.x, rot.y, rot.z, true);

			//m_Node->SetRotation(rot.x, rot.y, rot.z);


			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	rot_box->setAlignment(Qt::AlignLeft);
	rot_box->setSpacing(5);

	m_LO->addLayout(rot_box);

	//Scale

	QHBoxLayout* scale_box = new QHBoxLayout(this);

	auto scale_lab = new QLabel("Scale    ");

	xlab = new QLabel("X");
	ylab = new QLabel("Y");
	zlab = new QLabel("Z");


	m_NodeScaleX = new QDoubleSpinBox(this);
	m_NodeScaleY = new QDoubleSpinBox(this);
	m_NodeScaleZ = new QDoubleSpinBox(this);
	m_NodeScaleX->setSingleStep(0.1f);
	m_NodeScaleY->setSingleStep(0.1f);
	m_NodeScaleZ->setSingleStep(0.1f);
	m_NodeScaleX->setMinimumWidth(100);
	m_NodeScaleY->setMinimumWidth(100);
	m_NodeScaleZ->setMinimumWidth(100);


	float3 scale = node->GetScale();
	m_NodeScaleX->setValue(scale.x);
	m_NodeScaleY->setValue(scale.y);
	m_NodeScaleZ->setValue(scale.z);

	m_NodeScaleX->setDecimals(3);
	m_NodeScaleX->setRange(-1000, 1000);

	m_NodeScaleY->setDecimals(3);
	m_NodeScaleY->setRange(-1000, 1000);

	m_NodeScaleZ->setDecimals(3);
	m_NodeScaleZ->setRange(-1000, 10000);

	// Position
	scale_box->addWidget(scale_lab);
	scale_box->addWidget(xlab);
	scale_box->addWidget(m_NodeScaleX);
	scale_box->addWidget(ylab);
	scale_box->addWidget(m_NodeScaleY);
	scale_box->addWidget(zlab);
	scale_box->addWidget(m_NodeScaleZ);


	QObject::connect(m_NodeScaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			m_BlockTimer = true;
			if (m_BlockRot) return;
			auto scale = m_Node->GetScale();
			scale.x = value;
		//	scale.y = m_NodeRotY->value();
			//rot.z = m_NodeRotZ->value();


			m_Node->SetScale(scale);
			//m_BlockTimer = false;

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodeScaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {

			auto scale = m_Node->GetScale();
			scale.y = value;
			//	scale.y = m_NodeRotY->value();
				//rot.z = m_NodeRotZ->value();


			m_Node->SetScale(scale);
			m_BlockTimer = false;

			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	QObject::connect(m_NodeScaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](double value) {
			auto scale = m_Node->GetScale();
			scale.z = value;
			//	scale.y = m_NodeRotY->value();
				//rot.z = m_NodeRotZ->value();


			m_Node->SetScale(scale);

			//m_Node->SetRotation(rot.x, rot.y, rot.z);


			//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
		});


	scale_box->setAlignment(Qt::AlignLeft);
	scale_box->setSpacing(5);

	m_LO->addLayout(scale_box);


	if (dynamic_cast<NodeActor*>(m_Node))
	{

		NodeActor* act = (NodeActor*)m_Node;
//		int a = 5;
		auto anims = act->GetAnimator()->GetAnimations();
		int a = 5;

		m_Anims = anims;
		auto an_list = new VAnimationList;

		m_AnimList = an_list;

		QObject::connect(m_AnimList, &QListWidget::itemSelectionChanged, [&]() {
			
			m_BlockAnimNameChange = true;
			auto item = m_AnimList->selectedItems();
			if (item.size() > 0)
			{
				auto actual = item[0];
				m_AnimName->setText(actual->text());
				for (auto a : m_Anims) {

					if (a->GetName() == actual->text()) {

						m_CurrentAnim = a;
						break;

					}

				}

				//m_AnimName->setText()
			}
			m_BlockAnimNameChange = false;

			});


		auto r_node = (NodeActor*)m_Node;

		r_node->LoadAnimNames();

		an_list->SetAnimations(anims);

		auto anim_l = new QLabel("Animations");

		auto list_h = new QHBoxLayout(this);


		auto lab = new QLabel("Animations");

		//list_h->addWidget(lab);
		m_LO->addWidget(anim_l);
		list_h->addWidget(an_list);

		QPushButton* add_anim = new QPushButton("Add Animation");
		add_anim->setMaximumWidth(120);

		//
		QObject::connect(add_anim, &QPushButton::clicked, [&]() {
			// Update the label with a message when the button is clicked
			//label.setText("Button Clicked!");
			QString path = Engine::m_ContentPath.c_str();

			QString filePath = QFileDialog::getOpenFileName(nullptr, "Open FBX File",path, "FBX Files (*.fbx)");
			if (!filePath.isEmpty()) {
				// Update the label with the selected file path
				//label.setText("Selected file: " + filePath);
				auto imp = new Importer;
				auto anims = (NodeActor*)imp->ImportAnims((NodeActor*)m_Node,filePath.toStdString());


				//auto as = anims->GetAnimator()->GetAnimations();

			auto r_node = (NodeActor*)m_Node;

			r_node->SaveAnimList();
			//	for (auto an : as) {
			//		r_node->GetAnimator()->AddAnimation(an);
		//		}
				m_AnimList->SetAnimations(r_node->GetAnimator()->GetAnimations());
				m_Anims = r_node->GetAnimator()->GetAnimations();
			}

			});


		//

		list_h->addWidget(add_anim);

		anim_l->setMaximumSize(70, 30);
		anim_l->setMinimumSize(70, 30);

		an_list->setMinimumSize(200, 200);
		an_list->setMaximumSize(200, 200);

		list_h->setAlignment(Qt::AlignLeft);
		lab->setMaximumWidth(65);

		auto edit_v = new QVBoxLayout(this);
		//list_h->addLayout(edit_v);


		auto name_edit_h = new QHBoxLayout(this);

		auto n_lab = new QLabel("Name");
		auto n_edit = new QLineEdit();

		m_AnimName = n_edit;
		n_edit->setMaximumWidth(250);
		n_lab->setMaximumWidth(60);
		name_edit_h->addWidget(n_lab);
		name_edit_h->addWidget(n_edit);
		name_edit_h->setAlignment(Qt::AlignTop);
		QObject::connect(n_edit, &QLineEdit::textChanged, [&](const QString& newText) {
			// Update the label with the new text from the lne edit
			//editedTextLabel.setText("Edited: " + newText);
			if (m_BlockAnimNameChange) return;
			if (m_CurrentAnim != nullptr) {
				m_CurrentAnim->SetName(newText.toStdString());
				m_AnimList->SetAnimations(m_Anims);
				auto r_node = (NodeActor*)m_Node;

				r_node->SaveAnimNames();

			}

			});



		m_LO->addLayout(list_h);
		m_LO->addLayout(name_edit_h);

		QPushButton* preview_anim = new QPushButton("Preview");

		m_LO->addWidget(preview_anim);

		QObject::connect(preview_anim, &QPushButton::clicked, [&]() {
			// Update the label with a message when the button is clicked
			//label.setText("Button Clicked!");
			//QString path = Engine::m_ContentPath.c_str();
			if (m_CurrentAnim != nullptr) {

				auto actor = (NodeActor*)m_Node;
				actor->GetAnimator()->SetAnimation(m_CurrentAnim);
				actor->SetAnimTime(0);

			}
			});


			
	}else
	if (dynamic_cast<NodeLight*>(m_Node) != nullptr) {


		auto light = (NodeLight*)m_Node;

		auto l_diff_box = new QHBoxLayout();

		auto ldif_lab = new QLabel("Diffuse ");
		l_diff_box->addWidget(ldif_lab);
		xlab = new QLabel("R");
		ylab = new QLabel("G");
		zlab = new QLabel("B");


		m_LightDiffR = new QDoubleSpinBox(this);
		m_LightDiffG = new QDoubleSpinBox(this);
		m_LightDiffB = new QDoubleSpinBox(this);
		m_LightDiffR->setSingleStep(0.05f);
		m_LightDiffG->setSingleStep(0.05f);
		m_LightDiffB->setSingleStep(0.05f);
		m_LightDiffR->setMinimumWidth(90);
		m_LightDiffG->setMinimumWidth(90);
		m_LightDiffB->setMinimumWidth(90);

		l_diff_box->addWidget(xlab);
		l_diff_box->addWidget(m_LightDiffR);
		l_diff_box->addWidget(ylab);
		l_diff_box->addWidget(m_LightDiffG);
		l_diff_box->addWidget(zlab);
		l_diff_box->addWidget(m_LightDiffB);



		m_LightDiff = new VColorPreview;
		m_LightDiff->SetColor(float4(1, 1, 1, 1));



		//l_diff_box->addWidget(m_LightDiff);


		l_diff_box->setAlignment(Qt::AlignLeft);
		l_diff_box->setSpacing(5);

		l_diff_box->addWidget(m_LightDiff);

		m_LO->addLayout(l_diff_box);

		auto l_spec_box = new QHBoxLayout();

		auto lspec_lab = new QLabel("Specular");
		l_spec_box->addWidget(lspec_lab);
		xlab = new QLabel("R");
		ylab = new QLabel("G");
		zlab = new QLabel("B");


		m_LightSpecR = new QDoubleSpinBox(this);
		m_LightSpecG = new QDoubleSpinBox(this);
		m_LightSpecB = new QDoubleSpinBox(this);
		m_LightSpecR->setSingleStep(0.05f);
		m_LightSpecG->setSingleStep(0.05f);
		m_LightSpecB->setSingleStep(0.05f);
		m_LightSpecR->setMinimumWidth(90);
		m_LightSpecG->setMinimumWidth(90);
		m_LightSpecB->setMinimumWidth(90);

		l_spec_box->addWidget(xlab);
		l_spec_box->addWidget(m_LightSpecR);
		l_spec_box->addWidget(ylab);
		l_spec_box->addWidget(m_LightSpecG);
		l_spec_box->addWidget(zlab);
		l_spec_box->addWidget(m_LightSpecB);


		m_LightSpec = new VColorPreview;
		m_LightSpec->SetColor(float4(1, 1, 1, 1));

		//l_diff_box->addWidget(m_LightDiff);


		l_spec_box->setAlignment(Qt::AlignLeft);
		l_spec_box->setSpacing(5);

		l_spec_box->addWidget(m_LightSpec);

		m_LO->addLayout(l_spec_box);

		//connects

		QObject::connect(m_LightDiffR, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double value) {

				auto l = (NodeLight*)m_Node;
				auto dif = l->GetDiffuse();
				dif.x = value;
				l->SetDiffuse(dif);
				//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
			});

		QObject::connect(m_LightDiffG, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double value) {

				auto l = (NodeLight*)m_Node;
				auto dif = l->GetDiffuse();
				dif.y = value;
				l->SetDiffuse(dif);
				//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
			});


		QObject::connect(m_LightDiffB, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double value) {

				auto l = (NodeLight*)m_Node;
				auto dif = l->GetDiffuse();
				dif.z = value;
				l->SetDiffuse(dif);
				//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
			});

		auto range_box = new QHBoxLayout();

		auto range_lab = new QLabel("Range");

		m_LightRange = new QDoubleSpinBox();

		range_box->addWidget(range_lab);
		range_box->addWidget(m_LightRange);

		range_box->setAlignment(Qt::AlignLeft);
		range_box->setSpacing(5);

	//	l_spec_box->addWidget(m_LightSpec);

		m_LO->addLayout(range_box);

		m_LightRange->setMinimumWidth(120);
		m_LightRange->setMaximumWidth(180);

		QObject::connect(m_LightRange, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
			[&](double value) {

				auto l = (NodeLight*)m_Node;
				l->SetRange(value);
				//	auto dif = l->GetDiffuse();
			//	dif.z = value;
				//l->SetDiffuse(dif);

				//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
			});

	}

	auto scripts = m_Node->GetScripts();

	for (auto script : scripts) {

		//auto vars = script-

		auto info_box = new QHBoxLayout();

		auto name = script->GetName().GetNames()[0];

		auto script_lab = new QLabel(std::string("Script:" + name).c_str());

		info_box->addWidget(script_lab);

		m_LO->addLayout(info_box);

		int vv = 0;
		for (auto var : script->GetScope()->GetVars())
		{
			auto vbox = new QHBoxLayout();
			vbox->setAlignment(Qt::AlignLeft);
			auto name_lab = new QLabel(var->GetName().c_str());

			vbox->addWidget(name_lab);
			name_lab->setMinimumWidth(50);
			name_lab->setMaximumWidth(50);


			switch (var->GetType()) {
			case T_Class:
			{


				if (var->GetClassType() == "Vec3")
				{

					auto x_l = new QLabel("X");
					auto y_l = new QLabel("Y");
					auto z_l = new QLabel("Z");

					auto x_v =new  QDoubleSpinBox(this);
					auto y_v =new  QDoubleSpinBox(this);
					auto z_v = new QDoubleSpinBox(this);
					x_v->setRange(-10000, 10000);
					y_v->setRange(-10000, 10000);
					z_v->setRange(-10000, 10000);

					x_v->setMinimumWidth(105);
					y_v->setMinimumWidth(105);
					z_v->setMinimumWidth(105);

					auto cx = var->GetClassValue()->GetScope()->FindVar("X")->ToFloat();
					auto cy = var->GetClassValue()->GetScope()->FindVar("Y")->ToFloat();
					auto cz = var->GetClassValue()->GetScope()->FindVar("Z")->ToFloat();

					auto var_x = var->GetClassValue()->GetScope()->FindVar("X");
					auto var_y = var->GetClassValue()->GetScope()->FindVar("Y");
					auto var_z = var->GetClassValue()->GetScope()->FindVar("Z");

					x_v->setValue(cx);
					y_v->setValue(cy);
					z_v->setValue(cz);

					vbox->addWidget(x_l);
					vbox->addWidget(x_v);
					vbox->addWidget(y_l);
					vbox->addWidget(y_v);
					vbox->addWidget(z_l);
					vbox->addWidget(z_v);

					QObject::connect(x_v, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
						[x_v,var_x](double value) {

							var_x->SetFloat(value);

						});
					QObject::connect(y_v, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
						[y_v, var_y](double value) {

							var_y->SetFloat(value);

						});
					QObject::connect(z_v, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
						[z_v, var_z](double value) {

							var_z->SetFloat(value);

						});
				}
				else {
					int c = 5;
					//				auto c_lab = new QLabel(var->GetClassType().c_str());
									//vbox->addWidget(c_lab);
					auto c_node = new VScriptClass;
					c_node->SetType(var->GetClassType());
					vbox->addWidget(c_node);
					c_node->setMinimumWidth(200);
					c_node->setMaximumWidth(200);
					c_node->SetEntry(var);

					auto cls = var->GetClassValue();

					if (cls == nullptr) {
						c_node->setText("null");
					}
					else {
						c_node->setText(var->GetClassValue()->GetDataName().c_str());
					}
					//c_lab->setMaximumWidth(60);
				}

			}
				break;
			case T_Number:
			case T_Int:
			{
				QSpinBox* var_edit = new QSpinBox();
				vbox->addWidget(var_edit);
				var_edit->setValue(var->ToInt());
				var_edit->setMinimumWidth(120);
				var_edit->setRange(-100000, 100000);
				var_edit->setProperty("Script", QVariant::fromValue(reinterpret_cast<quintptr>(var)));
				QObject::connect(var_edit, QOverload<int>::of(&QSpinBox::valueChanged),
					[var_edit](int value) {

						//	QDoubleSpinBox* senderSpinBox = qobject_cast<QDoubleSpinBox*>(sender());
						QVariant customData = var_edit->property("Script");
						if (customData.isValid()) {

							void* retrievedPointer = reinterpret_cast<void*>(customData.toULongLong());
							auto vv = (VVar*)retrievedPointer;
							vv->SetInt(value);
							// Use retrievedPointer as needed
						}

						//int b = vv;
						//int aa = 5;

						//var->SetFloat(value);

						//auto l = (NodeLight*)m_Node;
						//l->SetRange(value);
						//	auto dif = l->GetDiffuse();
					//	dif.z = value;
						//l->SetDiffuse(dif);

						//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
					});
			}
			break;
			case T_Float:
			case T_FloatNumber:

			{
				QDoubleSpinBox* var_edit = new QDoubleSpinBox();
				vbox->addWidget(var_edit);
				var_edit->setValue(var->ToFloat());
				var_edit->setMinimumWidth(120);
				var_edit->setRange(-100000, 100000);
				m_ScriptWidgets.push_back(var_edit);
				var_edit->setProperty("Script", QVariant::fromValue(reinterpret_cast<quintptr>(var)));

				//
				QObject::connect(var_edit, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
					[var_edit](double value) {

					//	QDoubleSpinBox* senderSpinBox = qobject_cast<QDoubleSpinBox*>(sender());
						QVariant customData = var_edit->property("Script");
						if (customData.isValid()) {

							void* retrievedPointer = reinterpret_cast<void*>(customData.toULongLong());
							auto vv = (VVar*)retrievedPointer;
							vv->SetFloat(value);
							// Use retrievedPointer as needed
						}

						//int b = vv;
						//int aa = 5;

						//var->SetFloat(value);

						//auto l = (NodeLight*)m_Node;
						//l->SetRange(value);
						//	auto dif = l->GetDiffuse();
					//	dif.z = value;
						//l->SetDiffuse(dif);

						//label->setText(QString("Value: %1").arg(value, 0, 'f', 2));
					});

				
			}
			break;
			case T_String:
			{
				QLineEdit* var_edit = new QLineEdit();
				vbox->addWidget(var_edit);
				var_edit->setText(var->ToString().c_str());
				var_edit->setMinimumWidth(200);
				var_edit->setMaximumWidth(250);
			}
			break;
			}
			vv++;
			m_LO->addLayout(vbox);

		}


	}

	UpdateNode();

	m_LO->addSpacing(32);

	setLayout(m_LO);

	

}

void VPropEditor::SetTerrain(NodeTerrain* node) {

	if (layout() != nullptr) {

		ne_clearLayout(layout());
		//	return;
		m_LO = (QVBoxLayout*)layout();

	}
	else {
		m_LO = new QVBoxLayout(this);
		m_LO->setAlignment(Qt::AlignTop);
		m_LO->setSpacing(15);
		//	setLayout(m_LO);

	}
	if (node == nullptr) {
		m_Node = nullptr;
		ne_clearLayout(layout());
		return;
	}
	m_Node = (Node*)node;

	NodeTerrain* ter = dynamic_cast<NodeTerrain*>(node);

	QHBoxLayout* info_box = new QHBoxLayout(this);

	info_box->setAlignment(Qt::AlignLeft);

	QLabel* ter_lab = new QLabel("Terrain Editor");
	info_box->addWidget(ter_lab);


	QHBoxLayout* tool_box = new QHBoxLayout(this);

	m_EditTool = new QComboBox;

	m_EditTool->setMinimumWidth(120);

	m_EditTool->addItem("Paint");
	m_EditTool->addItem("Sculpt");

	QObject::connect(m_EditTool, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[&](int index) {
			switch (index) {
			case 0:
				Editor::m_TerrainEditMode = EM_Paint;
				break;
			case 1:
				Editor::m_TerrainEditMode = EM_Sculpt;
				break;
			}
			//QString selectedItem = comboBox->itemText(index);
			//label->setText("Selected: " + selectedItem);
		});

	tool_box->addWidget(m_EditTool);

	tool_box->setAlignment(Qt::AlignLeft);


	QHBoxLayout* edit_box = new QHBoxLayout(this);

	QLabel* layer_lab = new QLabel("Edit Layer");
	QSpinBox* m_EditLayer = new QSpinBox();


	m_EditLayer->setValue(Editor::TerrainEditLayer);
	m_EditLayer->setSingleStep(1);
	m_EditLayer->setMaximum(ter->GetLayers().size()-1);
	m_EditLayer->setMinimum(0);
	m_EditLayer->setMinimumWidth(90);

	QHBoxLayout* edit2_box = new QHBoxLayout(this);
	edit2_box->setAlignment(Qt::AlignLeft);

	QLabel* strength_lab = new QLabel("Strength");
	QDoubleSpinBox* m_Strength = new QDoubleSpinBox();

	m_Strength->setValue(Editor::TerrainBrushStrength);
	m_Strength->setSingleStep(0.05f);
	m_Strength->setMaximum(1.0f);
	m_Strength->setMinimum(0.01f);
	m_Strength->setMinimumWidth(90);

	QObject::connect(m_Strength, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](float value) {

			Editor::TerrainBrushStrength = value;

		});


	QLabel* size_lab = new QLabel("Size");
	QDoubleSpinBox* m_Size = new QDoubleSpinBox();

	
	m_Size->setSingleStep(0.05f);
	m_Size->setMaximum(100.0f);
	m_Size->setMinimum(0.1f);
	m_Size->setMinimumWidth(90);
	m_Size->setValue(Editor::TerrainBrushSize);

	QObject::connect(m_Size, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		[&](float value) {

			Editor::TerrainBrushSize = value;

		});

	edit_box->addWidget(layer_lab);
	edit_box->addWidget(m_EditLayer);

	edit2_box->addWidget(strength_lab);
	edit2_box->addWidget(m_Strength);
	edit2_box->addWidget(size_lab);
	edit2_box->addWidget(m_Size);

	edit_box->setAlignment(Qt::AlignLeft);

	QObject::connect(m_EditLayer, QOverload<int>::of(&QSpinBox::valueChanged),
		[&](int value) {

			Editor::TerrainEditLayer = value;

		});

	auto layers = ter->GetLayers();
	
	m_LO->addLayout(info_box);
	
	m_LO->addLayout(tool_box);
	m_LO->addLayout(edit_box);
	m_LO->addLayout(edit2_box);
	int i = 1;
	for (auto layer : layers) {

		QHBoxLayout* layer_box = new QHBoxLayout(this);

		std::string lab = "Layer" + std::to_string(i);

		QLabel* layer_lab = new QLabel(lab.c_str());
		layer_box->addWidget(layer_lab);

		QVBoxLayout* img_box = new QVBoxLayout(this);

		QHBoxLayout* imgs_box = new QHBoxLayout(this);

		VImagePreview* col_prev = new VImagePreview();
		col_prev->SetSize(96, 96);

		col_prev->SetImage(layer->GetColor()->GetPath());
		col_prev->setProperty("LayerNumber", i);


		QObject::connect(col_prev, &VImagePreview::dropped, [col_prev,this](const QString& filePath) {
			
			int id = col_prev->property("LayerNumber").toInt();
			auto layer = m_Terrain->GetLayer(id - 1);
			layer->SetColor(new Texture2D(filePath.toStdString(), true));
			
			//qDebug() << "File dropped:" << filePath;
			//m_Material->SetDiffuse(new Texture2D(filePath.toStdString()));
			int bb = 5;

			});
		
		imgs_box->addWidget(col_prev);

		//
		VImagePreview* norm_prev = new VImagePreview();
		norm_prev->SetSize(96,96);

		norm_prev->SetImage(layer->GetNormal()->GetPath());
		norm_prev->setProperty("LayerNumber", i);


		QObject::connect(norm_prev, &VImagePreview::dropped, [norm_prev, this](const QString& filePath) {

			int id = norm_prev->property("LayerNumber").toInt();
			auto layer = m_Terrain->GetLayer(id - 1);
			layer->SetNormal(new Texture2D(filePath.toStdString(), true));


			//qDebug() << "File dropped:" << filePath;
			//m_Material->SetDiffuse(new Texture2D(filePath.toStdString()));
			int bb = 5;

			});

		imgs_box->addWidget(norm_prev);

		//

		VImagePreview* spec_prev = new VImagePreview();
		spec_prev->SetSize(96, 96);

		spec_prev->SetImage(layer->GetSpec()->GetPath());
		spec_prev->setProperty("LayerNumber", i);


		QObject::connect(spec_prev, &VImagePreview::dropped, [norm_prev, this](const QString& filePath) {

			int id = norm_prev->property("LayerNumber").toInt();
			auto layer = m_Terrain->GetLayer(id - 1);
			layer->SetSpecular(new Texture2D(filePath.toStdString(), true));


			//qDebug() << "File dropped:" << filePath;
			//m_Material->SetDiffuse(new Texture2D(filePath.toStdString()));
			int bb = 5;

			});

		imgs_box->addWidget(spec_prev);



		//
		img_box->addLayout(imgs_box);
		auto fill = new QPushButton("Fill Layer");

		fill->setMaximumWidth(80);
		img_box->addWidget(fill);
		fill->setProperty("Layer", i);
		QObject::connect(fill, &QPushButton::clicked, [fill,this]() {
			//label->setText("Button was clicked!");
			auto ter = m_Terrain;
			int edit_lay = fill->property("Layer").toInt() - 1;

			auto layer = ter->GetLayer(edit_lay);
			layer->FillBrush(float4(1, 1, 1, 1));
			for (auto l : ter->GetLayers())
			{
				if (l == layer) continue;
				l->FillBrush(float4(0, 0, 0, 1));
			}
			});



		img_box->setAlignment(Qt::AlignLeft);
		//(layer_box);
		m_LO->addLayout(layer_box);
		m_LO->addLayout(img_box);
		i++;
	}

	m_Terrain = node;



}

VPropEditor::~VPropEditor()
{}
