#include "VMainMenu.h"
#include "NodeTerrain.h"
#include "Editor.h"
#include "SceneGraph.h"
#include "Vivid3D.h"
#include <qfiledialog.h>
#include "Engine.h"
#include "VSceneGraph.h"
#include "RendererBase.h"
#include "VTools.h"
#include "VSettings.h"

VMainMenu::VMainMenu(QWidget *parent)
	: QMenuBar(parent)
{
	ui.setupUi(this);

	QMenu* project = addMenu("Scene");
	QMenu* edit = addMenu("Edit");
	QMenu* create = addMenu("Create");
	QMenu* editors = addMenu("Editors");

	//project
	project->addAction("New Scene");
	project->addSeparator();
	auto load_Scene = project->addAction("Load Scene");

	auto save_Scene = project->addAction("Save Scene");

	project->addSeparator();
	project->addAction("Quit V3");

	connect(save_Scene, &QAction::triggered, [this]() {

		QString filePath = QFileDialog::getSaveFileName(this,
			tr("Save Scene"),
			QString(),
			tr("Scene Files (*.scene);"),
			nullptr);

		if (!filePath.isEmpty())
		{
			// Ensure the file has the .scene extension
			if (!filePath.endsWith(".scene", Qt::CaseInsensitive))
			{
				filePath += ".scene";
				
			}
			Editor::m_Graph->SaveScene(filePath.toStdString());
		}
		//auto terrain = new NodeTerrain(128, 128, 3, 4);
		//Editor::m_Graph->AddNode(terrain);

		});

	//
	connect(load_Scene, &QAction::triggered, [this]() {

		QString filePath = QFileDialog::getOpenFileName(this,
			tr("Save Scene"),
			QString(),
			tr("Scene Files (*.scene);"),
			nullptr
			);

		if (!filePath.isEmpty())
		{
			// Ensure the file has the .scene extension
			if (!filePath.endsWith(".scene", Qt::CaseInsensitive))
			{
				filePath += ".scene";


			}
			auto cam = Engine::m_Camera;
			auto lines = Editor::m_Graph->GetLines();
			Editor::m_Graph = new SceneGraph;
			Editor::m_Graph->LoadScene(filePath.toStdString());
			Engine::m_Camera = cam;
			Editor::m_Graph->SetCamera(cam);
			Editor::m_Graph->SetLines(lines);
			Editor::m_SceneGraph->UpdateGraph();
			Editor::m_Graph->Updated();
			Engine::m_ActiveRenderer->SetSceneGraph(Editor::m_Graph);
			Editor::m_Cameras.clear();
			Engine::m_ActiveGraph = Editor::m_Graph;
			//Editor::m_Cameras.push_back(cam);
			for (auto c : Editor::m_Graph->GetCameras()) {
				Editor::m_Cameras.push_back(c);
			}

			VTools::m_This->Update();
			
			//Editor::m_Graph->SaveScene(filePath.toStdString());
		}
		//auto terrain = new NodeTerrain(128, 128, 3, 4);
		//Editor::m_Graph->AddNode(terrain);

		});


	//edit

	auto e_projs = edit->addAction("Project Settings");
	auto e_scenes = edit->addAction("Scene Settings");



	auto e_node = edit->addMenu("Node");

	auto align_to_cam = e_node->addAction("Align to Camera");
	auto cam_to_node = e_node->addAction("Align Camera to Node");


	connect(e_projs, &QAction::triggered, [this]() {

		auto settings = new VSettings();
		settings->show();

		});

	connect(align_to_cam, &QAction::triggered, [this]() {

		if (Editor::m_CurrentNode != nullptr) {
			
			Editor::m_CurrentNode->SetPosition(Editor::m_Graph->GetCamera()->GetPosition());
			Editor::m_CurrentNode->SetRotation(Editor::m_Graph->GetCamera()->GetRotation());

		}

		});

	connect(cam_to_node, &QAction::triggered, [this]() {

		if (Editor::m_CurrentNode != nullptr) {

			Editor::m_Graph->GetCamera()->SetPosition(Editor::m_CurrentNode->GetPosition());
			Editor::m_Graph->GetCamera()->SetRotation(Editor::m_CurrentNode->GetRotation());
			float p, y;
			p = Editor::m_Graph->GetCamera()->GetRotationEU().x;
			y = Editor::m_Graph->GetCamera()->GetRotationEU().y;
			VOutput::m_This->SetCam(360-p, 360-y);


			}

		});


	//Create
	auto create_terrain = create->addAction("Create Terrain");

	connect(create_terrain, &QAction::triggered, [this]() {

		auto terrain = new NodeTerrain(128, 128, 3, 4);
		Editor::m_Graph->AddNode(terrain);
		Editor::m_Graph->Updated();
		Editor::m_SceneGraph->UpdateGraph();
		});


	auto create_node = create->addMenu("Nodes");

	auto cr_camera = create_node->addAction("Create Camera");

	connect(cr_camera, &QAction::triggered, [this]() {

		auto cam = new NodeCamera;
		Editor::m_Graph->AddNode(cam);
		Editor::m_Graph->Updated();
		Editor::m_SceneGraph->UpdateGraph();
		Editor::m_Cameras.push_back(cam);
		VTools::m_This->Update();

		});

	//Editors

	auto edit_cine = editors->addAction("Cinema");

	connect(edit_cine, &QAction::triggered, [this]() {

		Editor::m_Main->CreateCinema();
		//auto terrain = new NodeTerrain(128, 128, 3, 4);
		//Editor::m_Graph->AddNode(terrain);

		});


}

VMainMenu::~VMainMenu()
{}
