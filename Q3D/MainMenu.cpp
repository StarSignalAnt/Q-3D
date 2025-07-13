#include "MainMenu.h"
#include <QFileDialog>
#include <QString>
#include "Content.h"
#include "SceneView.h"
#include "LogicGraph.h"
#include "Importer.h"
#include "LightComponent.h"
#include "NodeTree.h"
#include "SceneView.h"
#include <QDesktopServices>
#include <QUrl>
#include <QString>
MainMenu::MainMenu(QWidget *parent)
	: QMenuBar(parent)
{


	setupMenus();
	//ui.setupUi(this);
}

MainMenu::~MainMenu()
{}


void MainMenu::setupMenus()
{
    // File menu
    QMenu* fileMenu = addMenu("Project");
    QMenu* editMenu = addMenu("Edit");
	QMenu* crMenu = addMenu("Create");
    QMenu* csMenu = addMenu("Scripting");
    QMenu* toolsMenu = addMenu("Tools");

    QAction* openDLL = csMenu->addAction("Open Visual Studio Solution");

    QAction* newAction = fileMenu->addAction("New Scene");
    QAction* openAction = fileMenu->addAction("Open Scene");
    fileMenu->addSeparator();
    QAction* saveAction = fileMenu->addAction("Save Scene");
    QAction* saveAsAction = fileMenu->addAction("Save Scene As..");
    fileMenu->addSeparator();
    QAction* exitAction = fileMenu->addAction("Exit Vivid3D");

    connect(newAction, &QAction::triggered, this, &MainMenu::onNewFile);
    connect(openAction, &QAction::triggered, this, &MainMenu::onOpenFile);
    connect(exitAction, &QAction::triggered, this, &MainMenu::onExit);
    connect(saveAction, &QAction::triggered, this, &MainMenu::onSaveScene);

    connect(openDLL, &QAction::triggered, this, &MainMenu::onOpenSolution);

	QMenu* crPrim = crMenu->addMenu("Primitives");
	QMenu* crLight = crMenu->addMenu("Lights");
	QMenu* crCamera = crMenu->addMenu("Camera");
	QMenu* crTerr = crMenu->addMenu("Terrain");

	auto pCube = crPrim->addAction("Cube");
	auto pSphere = crPrim->addAction("Sphere");
	auto pPlane = crPrim->addAction("Plane");
	auto pCylinder = crPrim->addAction("Cylinder");

	//crPrim->addAction("Capsule");

	auto pCone = crPrim->addAction("Cone");
	auto pTorus = crPrim->addAction("Torus");
	//crPrim->addAction("Quad");



   

    connect(pCube, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/Cube.gltf");

		p->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p);
        });

        //SceneGraph::m_Instance->AddPrimitive(p);

    connect(pSphere, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/Sphere.fbx");

        p->GetNodes()[0]->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p->GetNodes()[0]);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p->GetNodes()[0]);
        });

    connect(pPlane, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/Plane.fbx");

        p->GetNodes()[0]->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p->GetNodes()[0]);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p->GetNodes()[0]);
        });
   
    connect(pCylinder, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/cylinder.fbx");

        p->GetNodes()[0]->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p->GetNodes()[0]);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p->GetNodes()[0]);
        });



    connect(pCone, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/cone.fbx");

        p->GetNodes()[0]->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p->GetNodes()[0]);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p->GetNodes()[0]);
        });

    connect(pTorus, &QAction::triggered, []() {

        Importer* imp = new Importer;

        auto p = imp->ImportEntity("Edit/Primitives/torus.fbx");

        p->GetNodes()[0]->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

        SceneGraph::m_Instance->AddNode(p->GetNodes()[0]);
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(p->GetNodes()[0]);
        });





	auto lPoint = crLight->addAction("Point Light");
	auto lDir = crLight->addAction("Directional Light");
	auto lSpot = crLight->addAction("Spot Light");
	auto lAmb = crLight->addAction("Ambient Light");
    auto tFlat = crTerr->addAction("Flat Terrain");

   
    connect(lPoint, &QAction::triggered, []() {
        
        auto l = new GraphNode;
        l->SetName("Point Light");
		l->AddComponent(new LightComponent);
        SceneGraph::m_Instance->AddLight(l);
		l->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(l);

		});


    connect(lDir, &QAction::triggered, []() {

        auto l = new GraphNode;
        l->SetName("Point Light");
        auto lc = new LightComponent;
        l->AddComponent(lc);
		lc->SetLightType(LightType::Directional);
        SceneGraph::m_Instance->AddLight(l);
        l->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(l);

        });

    connect(lSpot, &QAction::triggered, []() {

        auto l = new GraphNode;
        l->SetName("Point Light");
        auto lc = new LightComponent;
        l->AddComponent(lc);
        lc->SetLightType(LightType::Spot);
        SceneGraph::m_Instance->AddLight(l);
        l->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
        NodeTree::m_Instance->SetRoot(SceneGraph::m_Instance->GetRootNode());
        SceneView::m_Instance->SelectNode(l);

        });

    connect(tFlat, &QAction::triggered, []() {

        if (SceneView::m_Instance->HasTerrain() == false) {
            SceneView::m_Instance->CreateTerrain();
        }
        });

    auto tHeightMap = crTerr->addAction("Heightmap Terrain");


    QAction* openLG = toolsMenu->addAction("Logic Graph");

    connect(openLG, &QAction::triggered, this, &MainMenu::onLG);
    // You can add more menus here: Edit, View, Help, etc.
}

void MainMenu::onNewFile() {
    qDebug() << "New file triggered";
    SceneView::m_Instance->GetScene()->Reset();
    NodeTree::m_Instance->SetRoot(SceneView::m_Instance->GetScene()->GetRootNode());
}

void MainMenu::onOpenFile() {

    QString filePath = QFileDialog::getOpenFileName(
        this,                               // parent widget (e.g., QMainWindow*)
        "Open Scene",                       // dialog title
        Content::m_Instance->GetPath().c_str(),                   // default path
        "Scene Files (*.scene);;All Files (*)"  // filter
    );

    if (!filePath.isEmpty()) {
        // use filePath
        
        auto g = SceneView::m_Instance->GetScene();

        //SceneGraph* new_Graph = new SceneGraph;
        g->LoadScene(filePath.toStdString());
  

        SceneView::m_Instance->Update();
    }

    qDebug() << "Open file triggered";
}

void MainMenu::onExit() {
    qDebug() << "Exit triggered";
    //qApp->quit();
    exit(1);
}

void MainMenu::onSaveScene() {

    QString filter = "Scene Files (*.scene)";
    QString defaultFileName = "untitled.scene";

    QString fileName = QFileDialog::getSaveFileName(
        nullptr,
        "Save Scene",
        Content::m_Instance->GetPath().c_str(),
        filter
    );

    if (!fileName.isEmpty()) {
        // Ensure the file ends with .scene
        if (!fileName.endsWith(".scene", Qt::CaseInsensitive)) {
            fileName += ".scene";
        }

        SceneView::m_Instance->GetScene()->SaveScene(fileName.toStdString());

        qDebug() << "Selected file to save:" << fileName;
        // Save logic goes here...
    }

}

void MainMenu::onLG() {

    auto lg = new LogicGraph;
    lg->showMaximized();

}

void MainMenu::onOpenSolution() {

    auto path = Q3D::Engine::QEngine::GetContentPath() + "Game\\GameDLL.sln";
    QString qPath = QString::fromStdString(path);
    QDesktopServices::openUrl(QUrl::fromLocalFile(qPath));

}