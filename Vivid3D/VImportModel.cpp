#include "VImportModel.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "FileHelp.h"
#include "Importer.h"
#include "Node.h"
#include "ImportSettings.h"

VImportModel::VImportModel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//setBaseSize(size().width(), 80);
	setMaximumSize(size().width(), 250);

}

VImportModel::~VImportModel()
{}

void VImportModel::Set(std::string name) {

	auto v_l = new QVBoxLayout(this);

	setWindowTitle("Import Model");

	auto name_l = new QLabel("Entity Name");

	m_ModelName = new QLineEdit;

	auto h_1 = new QHBoxLayout(this);

	v_l->setAlignment(Qt::AlignTop);

	h_1->setAlignment(Qt::AlignLeft);

	auto imp = new Importer;



	auto imp_node = imp->ImportNode(name);

	m_Path = name;

	m_ModelName->setText(imp_node->GetName().c_str());

	h_1->addWidget(name_l);
	h_1->addWidget(m_ModelName);

	auto h_2 = new QHBoxLayout(this);

	auto scal_l = new QLabel("Scale");
	m_Scale = new QDoubleSpinBox(this);

	h_2->addWidget(scal_l);
	h_2->addWidget(m_Scale);

	scal_l->setMaximumWidth(90);
	h_2->setAlignment(Qt::AlignLeft);

	m_Scale->setMinimumWidth(120);
	m_Scale->setMaximumWidth(120);


	m_Scale->setValue(1.0);
	m_Scale->setMinimum(0.01);
	m_Scale->setMaximum(1000);
	m_Scale->setSingleStep(0.05);

	auto h_3 = new QHBoxLayout(this);
	h_3->setAlignment(Qt::AlignLeft);

	auto imp_actor = new QCheckBox("Import as Actor");

	imp_actor->setChecked(false);

	h_3->addWidget(imp_actor);

	m_ImportActor = imp_actor;

	auto h_4 = new QHBoxLayout(this);

	auto accept = new QPushButton("Import");
	auto cancel = new QPushButton("Cancel");

	//

	QObject::connect(accept, &QPushButton::clicked, [&]() {
		
		ImportSettings* imp = new ImportSettings;
		imp->SetScale(m_Scale->value());
		imp->SetPath(m_Path);
		imp->SetActor(m_ImportActor->checkState());
		imp->SetName(m_ModelName->text().toStdString());
		imp->Save();
		close();
		//QMessageBox::information(&window, "Lambda", "Button Clicked!");
		});

	QObject::connect(cancel, &QPushButton::clicked, [&]() {
	//	QMessageBox::information(&window, "Lambda", "Button Clicked!");
		close();
		});


	//h_4->setAlignment(Qt::AlignLeft);

	h_4->addWidget(accept);
	h_4->addWidget(cancel);


	v_l->addLayout(h_1);
	v_l->addLayout(h_2);
	v_l->addLayout(h_3);
	v_l->addSpacing(100);
	v_l->addLayout(h_4);

	setLayout(v_l);
	
//	v_l->setMaximumSize(250);


}