#include "VTools.h"
#include "Editor.h"
#include "qlabel.h"
#include "NodeCamera.h"
#include "SceneGraph.h"



VTools::VTools(QWidget* parent)
	: QToolBar(parent)
{
	ui.setupUi(this);
	QAction* translate = addAction(QIcon("edit/icons/Translate.png"), "ActTranslate");
	QAction* rotate = addAction(QIcon("edit/icons/Rotate.png"), "ActRotate");
	QAction* scale = addAction(QIcon("edit/icons/Scale.png"), "ActScale");
	addSeparator();


	connect(translate, &QAction::triggered, [this]() {

		Editor::m_GizmoMode = GM_Translate;

		});

	connect(rotate, &QAction::triggered, [this]() {

		Editor::m_GizmoMode = GM_Rotate;

		});



	QComboBox* space = new QComboBox(this);

	space->addItem("Local");
	space->addItem("Global");
	space->setMinimumWidth(80);

	addWidget(space);


	connect(space, QOverload<int>::of(&QComboBox::currentIndexChanged),
		[this](int index) {
			//label->setText(QString("Selected Option: %1").arg(index + 1));
			if (index == 0) {
				Editor::m_SpaceMode = SM_Local;
			}
			else {
				Editor::m_SpaceMode = SM_Global;
			}
		});

		//&MainWindow::onComboBoxIndexChanged);



	auto gamecam_lab = new QLabel("Game Camera");


	m_GameCamera = new QComboBox(this);


	m_GameCamera->addItem("None");

	addSeparator();
	addWidget(gamecam_lab);
	addWidget(m_GameCamera);

	m_GameCamera->setMinimumWidth(120);
	m_GameCamera->setMaximumWidth(120);
	m_This = this;

	connect(m_GameCamera, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
		// Handle the selection change
	//	qDebug() << "Selected index:" << index;
		// Perform any additional actions here
		auto name = m_GameCamera->itemText(index);
		auto cam = Editor::m_Graph->FindNode(std::string(name.toStdString()));
		Editor::m_GameCamera = (NodeCamera*)cam;
		int b = 5;

		});


	for (int i = 0; i < 15; i++) {
		addSeparator();
	}

	QAction* run = addAction(QIcon("edit/icons/runicon.png"), "ActRun");
	QAction* stop = addAction(QIcon("edit/icons/stopicon.png"), "ActStop");

	connect(run, &QAction::triggered, [this]() {

		Editor::BeginPlay();

		});

	connect(stop, &QAction::triggered, [this]() {

		Editor::Stop();

		});


}

VTools::~VTools()
{}

void VTools::Update() {

	m_GameCamera->clear();
	m_GameCamera->addItem("None");
	for (auto cam : Editor::m_Cameras) {

		m_GameCamera->addItem(cam->GetFullName().c_str());

	}

}

VTools* VTools::m_This = nullptr;