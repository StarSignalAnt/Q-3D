#include "VSettings.h"
#include "EngineSettings.h"

VSettings::VSettings(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// Splitter
	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
	mainLayout->addWidget(splitter);

	setWindowTitle("Vivid3D Settings");

	QTreeWidget* treeWidget = new QTreeWidget(this);
	treeWidget->setHeaderHidden(true);

	treeWidget->setMaximumWidth(200);

	QTreeWidgetItem* renderOptions = new QTreeWidgetItem(treeWidget);
	renderOptions->setText(0, "Engine options");

	QTreeWidgetItem* projectOptions = new QTreeWidgetItem(treeWidget);
	projectOptions->setText(0, "Scene options");

	splitter->addWidget(treeWidget);

	QStackedWidget* stackedWidget = new QStackedWidget(this);

	// Render Options Page
	QWidget* engineOptionsPage = new QWidget(this);
	QFormLayout* engineFormLayout = new QFormLayout(engineOptionsPage);

	QComboBox* textureQualityComboBox = new QComboBox(engineOptionsPage);
	textureQualityComboBox->addItem("Low");
	textureQualityComboBox->addItem("Mid");
	textureQualityComboBox->addItem("High");
	textureQualityComboBox->addItem("Ultra");

	switch (EngineSettings::TextureQuality) {
	case Q_Ultra:
		textureQualityComboBox->setCurrentIndex(3);
		break;
	case Q_High:
		textureQualityComboBox->setCurrentIndex(2);
		break;
	case Q_Mid:
		textureQualityComboBox->setCurrentIndex(1);
		break;
	case Q_Low:
		textureQualityComboBox->setCurrentIndex(0);
		break;
	}

	engineFormLayout->addRow("Texture Quality:",textureQualityComboBox);

	//
	QComboBox* shaderQualityComboBox = new QComboBox(engineOptionsPage);
	shaderQualityComboBox->addItem("Low");
	shaderQualityComboBox->addItem("Mid");
	shaderQualityComboBox->addItem("High");
	shaderQualityComboBox->addItem("Ultra");

	switch (EngineSettings::ShaderQuality) {
	case Q_Ultra:
		shaderQualityComboBox->setCurrentIndex(3);
		break;
	case Q_High:
		shaderQualityComboBox->setCurrentIndex(2);
		break;
	case Q_Mid:
		shaderQualityComboBox->setCurrentIndex(1);
		break;
	case Q_Low:
		shaderQualityComboBox->setCurrentIndex(0);
		break;
	}



	engineFormLayout->addRow("Shader Quality:", shaderQualityComboBox);

	//
	QComboBox* shadowQualityComboBox = new QComboBox(engineOptionsPage);
	shadowQualityComboBox->addItem("Low");
	shadowQualityComboBox->addItem("Mid");
	shadowQualityComboBox->addItem("High");
	shadowQualityComboBox->addItem("Ultra");

	switch (EngineSettings::ShadowQuality) {
	case Q_Ultra:
		shadowQualityComboBox->setCurrentIndex(3);
		break;
	case Q_High:
		shadowQualityComboBox->setCurrentIndex(2);
		break;
	case Q_Mid:
		shadowQualityComboBox->setCurrentIndex(1);
		break;
	case Q_Low:
		shadowQualityComboBox->setCurrentIndex(0);
		break;
	}



	engineFormLayout->addRow("Shadow Resolution:", shadowQualityComboBox);




	stackedWidget->addWidget(engineOptionsPage);


	QWidget *sceneOptionsPage = new QWidget(this);
	QFormLayout* sceneFormLayout = new QFormLayout(sceneOptionsPage);

	QComboBox* rendererComboBox = new QComboBox(sceneOptionsPage);
	rendererComboBox->addItem("Nitro(Rasterizer)");
	rendererComboBox->addItem("Solaris(RTX)");

	sceneFormLayout->addRow("Renderer:", rendererComboBox);

	switch (EngineSettings::UsedRenderer) {
	case R_Nitro:
		rendererComboBox->setCurrentIndex(0);
		break;
	case R_Solaris:
		rendererComboBox->setCurrentIndex(1);
		break;


	}

	m_URenderer = rendererComboBox;

//	shaderQualityComboBox->addItem("High");
//	shaderQualityComboBox->addItem("Ultra");



	stackedWidget->addWidget(sceneOptionsPage);


	splitter->addWidget(stackedWidget);

	connect(treeWidget, &QTreeWidget::currentItemChanged, this, [stackedWidget](QTreeWidgetItem* current, QTreeWidgetItem* previous) {
		if (!current) return;
		if (current->text(0) == "Engine options") {
			stackedWidget->setCurrentIndex(0);
		}
		else if (current->text(0) == "Scene options") {
			stackedWidget->setCurrentIndex(1);
		}
		});

	QPushButton* applyButton = new QPushButton("Apply", this);
	applyButton->setMaximumSize(120, 30);
	mainLayout->addWidget(applyButton);


	setLayout(mainLayout);

	m_QTexture = textureQualityComboBox;
	m_QShader = shaderQualityComboBox;
	m_QShadow = shadowQualityComboBox;

	connect(applyButton, &QPushButton::clicked, this, [this]() {

		switch (m_QTexture->currentIndex()) {
		case 3:
			EngineSettings::TextureQuality = Q_Ultra;
			break;
		case 2:
			EngineSettings::TextureQuality = Q_High;
			break;
		case 1:
			EngineSettings::TextureQuality = Q_Mid;
			break;
		case 0:
			EngineSettings::TextureQuality = Q_Low;
			break;
		}

		switch (m_QShader->currentIndex()) {
		case 3:
			EngineSettings::ShaderQuality = Q_Ultra;
			break;
		case 2:
			EngineSettings::ShaderQuality = Q_High;
			break;
		case 1:
			EngineSettings::ShaderQuality = Q_Mid;
			break;
		case 0:
			EngineSettings::ShaderQuality = Q_Low;
			break;
		}

		switch (m_QShadow->currentIndex()) {
		case 3:
			EngineSettings::ShadowQuality = Q_Ultra;
			break;
		case 2:
			EngineSettings::ShadowQuality = Q_High;
			break;
		case 1:
			EngineSettings::ShadowQuality = Q_Mid;
			break;
		case 0:
			EngineSettings::ShadowQuality = Q_Low;
			break;
		}

		switch (m_URenderer->currentIndex()) {
		case 0:
			EngineSettings::UsedRenderer = R_Nitro;
			break;
		case 1:
			EngineSettings::UsedRenderer = R_Solaris;
			break;
		}

		EngineSettings::SaveSettings();


		});

}

VSettings::~VSettings()
{}
