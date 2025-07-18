#include "QEngine.h"
#include "RenderTargetCube.h"
#include "RenderTarget2D.h"
#include <pybind11/embed.h>
#include "GameAudio.h"
#include "Physics.h"
#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"
#include "NodeRegistry.h"
#include "GameVideo.h"
#include "Draw2D.h"
#include "SharpComponent.h"
#include "Texture2D.h"
// --- Event & Core Nodes ---
#include "NodeTurnNode.h"
#include "TickEventNode.h"
#include "NodeTestLogic.h"
#include "NodeVec3Constant.h"
#include "GetSceneNode.h"
#include "NodeBoolConstant.h"
#include "NodeIf.h"
#include "NodeKeyDown.h"
#include "NodeAddFloat.h"
#include "NodeExposeGraphNode.h"
#include "NodeSetPositionNode.h"
#include "NodeCreateRotationMatrix.h"
#include "NodeMultMat4.h"
#include "NodeSetRotationNode.h"
#include "NodeSceneRayCast.h"
#include "GetVariableNode.h"
#include "NodeDebug.h"
#include "PlayEventNode.h"
#include "NodeStartVideo.h"
#include "RenderEventNode.h"
#include "NodeRenderVideo.h"
#include "LinesRendererComponent.h"
#include "MaterialVideo.h"

// --- Consolidated Logic Nodes Header ---
#include "LogicNodes1.h" // *** CHANGED: Include the new single header ***
#include "LogicNodes2.h"
#include "DllImporter.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "StaticDepthRendererComponent.h"
#include "SkyComponent.h"



namespace Q3D::Engine {
	namespace py = pybind11;
	RefCntAutoPtr<IRenderDevice>  QEngine::m_pDevice;
	RefCntAutoPtr<IDeviceContext> QEngine::m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     QEngine::m_pSwapChain;
	RefCntAutoPtr<IShaderSourceInputStreamFactory> QEngine::m_pShaderFactory;

	RenderTargetCube* QEngine::m_BoundRTC = nullptr;
	RenderTarget2D* QEngine::m_BoundRT2D = nullptr;
	std::vector<MaterialPBR*> QEngine::m_ActiveMaterials;
	MonoLib* QEngine::m_MonoLib = nullptr;
	std::vector<SharpClassInfo> QEngine::m_ComponentClasses;
	VividCallbackFunc QEngine::DebugLogCB = nullptr;

	Draw2D* QEngine::m_Draw = nullptr;

	GameVideo* QEngine::m_CurrentVideo = nullptr;

	std::string QEngine::m_ContentPath = "";

	int QEngine::ScX = 0;
	int QEngine::ScY = 0;
	int QEngine::ScW = 0;
	int QEngine::ScH = 0;

	void QEngine::RegisterCComponent(Component* component)
	{
		m_CComponents.push_back(component);
	}



	void QEngine::SetContentPath(std::string path) {

		m_ContentPath = path;

	}

	std::string QEngine::GetContentPath() {


		return m_ContentPath;

	}

	void QEngine::StartVideo(std::string path) {

		m_CurrentVideo = new GameVideo(path);
		m_CurrentVideo->Play();

	}

	void QEngine::SetVideo(GameVideo* video) {
		if (m_CurrentVideo) {
			delete m_CurrentVideo;
		}
		m_CurrentVideo = video;
	}

	void QEngine::RenderVideo() {

		if (m_CurrentVideo == nullptr) return;
	//	for (int i = 0; i < 5; i++) {
	//		m_CurrentVideo->Update();
		//}


		auto tex = m_CurrentVideo->GetYUVFrame();
		if (tex) {
			m_VidMat->m_Y = tex->Y;
			m_VidMat->m_U = tex->U;
			m_VidMat->m_V = tex->V;
			m_Draw->BeginFrame();
			m_Draw->Rect(nullptr, glm::vec2(0, 0), glm::vec2((float)GetFrameWidth(), (float)GetFrameHeight()), glm::vec4(1, 1, 1, 1));
			m_Draw->Flush();
		}


	}

	void QEngine::SetBoundRTC(RenderTargetCube* target) {

		m_BoundRTC = target;

	}
	int QEngine::GetFrameWidth() {

		if (m_BoundRTC != nullptr) {
			return m_BoundRTC->GetWidth();
		}
		if (m_BoundRT2D != nullptr) {
			return m_BoundRT2D->GetWidth();
		}
		return m_FrameWidth;

	}

	int QEngine::GetFrameHeight() {

		if (m_BoundRTC != nullptr) {
			return m_BoundRTC->GetHeight();
		}

		if (m_BoundRT2D != nullptr) {
			return m_BoundRT2D->GetHeight();
		}
		return m_FrameHeight;


	}
	void QEngine::SetFrameWidth(int w) {

		m_FrameWidth = w;

	}

	void QEngine::SetFrameHeight(int h) {

		m_FrameHeight = h;

	}


	void QEngine::SetBoundRT2D(RenderTarget2D* target) {

		m_BoundRT2D = target;

	}

	int QEngine::m_FrameWidth = 0;
	int QEngine::m_FrameHeight = 0;


	void QEngine::ClearZ() {

		m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


	}
	int add(int a, int b) { return a + b; }
	int multiply(int a, int b) {
		return a * b;
	}

	struct check {
		int a = 5;
		int b = 10;
	};
	std::filesystem::file_time_type QEngine::s_last_dll_write_time;

	void QEngine::CheckDLL() {

		//DebugLog("Checking DLL");
		auto dll_path = m_ContentPath + "Game\\bin\\Debug\\netstandard2.0\\Game.dll";


		try {

			// Get the current last write time of the DLL
			auto current_dll_write_time = std::filesystem::last_write_time(dll_path);

			// If the DLL has been updated (and the stored time is valid)
			if (s_last_dll_write_time.time_since_epoch().count() != 0 &&
				current_dll_write_time != s_last_dll_write_time) {
				std::cout << "DLL has been updated. Re-initializing Mono." << std::endl;
				InitMono();
				//		DebugLog("Updated C# dll");
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			//	std::cerr << "Error checking DLL: " << e.what() << std::endl;
		}

	}



	void QEngine::InitMono() {

		auto dll_path = m_ContentPath + "Game\\bin\\Debug\\netstandard2.0\\Game.dll";

		std::cout << "--- DLL Hot-Reload Initiated ---" << std::endl;

		MonoHost* host = MonoHost::GetInstance();

		// --- Unload and Reload Domains ---
		host->UnloadGameDomain();
		host->CreateGameDomain();

		// --- Rebuild Core Mono Library Wrapper ---
		if (m_MonoLib) {
			delete m_MonoLib;
		}
		m_MonoLib = new MonoLib(m_ContentPath + "Game\\bin\\Debug\\netstandard2.0\\");

		// --- HERE IS THE SOLUTION: Rebuild the Component Class Cache ---

		// 1. Clear the old, invalid list of component class definitions.
		m_ComponentClasses.clear();
		std::cout << "--- Re-caching all available SharpComponent classes... ---" << std::endl;

		// 2. Get the fresh list of all classes from the newly loaded assembly.
		auto all_classes_in_new_dll = m_MonoLib->GetClasses();

		// 3. Filter this list to find only the classes that can be used as components.
		for (const auto& classInfo : all_classes_in_new_dll)
		{
			// Check if the class's base class is the one we use for scripts.
			if (classInfo.baseClassName == "QNet.Component.SharpComponent")
			{
				// Add the valid class info to our cache.
				m_ComponentClasses.push_back(classInfo);
				std::cout << "Found component class: " << classInfo.namespaceName << "." << classInfo.className << std::endl;
			}
		}

		// --- Re-initialize all active script instances ---
		std::cout << "--- Re-initializing all active SharpComponent instances... ---" << std::endl;
		auto all_sharp_components = SceneGraph::m_Instance->GetAllSharpComponents();
		for (auto* component : all_sharp_components)
		{
			component->ReInit();
		}

		std::cout << "--- DLL Hot-Reload Complete ---" << std::endl;

		// Set the time of the last edit to compare against later
		try {
			s_last_dll_write_time = std::filesystem::last_write_time(dll_path);
			std::cout << "Mono initialized. DLL last write time recorded." << std::endl;
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Error getting DLL last write time: " << e.what() << std::endl;
			// Handle the error, maybe set the time to a default invalid state
			s_last_dll_write_time = std::filesystem::file_time_type();
		}
		return;
		/*
			MAsm* test = new MAsm("scripts/testlib.dll");
			auto cls = test->GetClass("TestLib", "Test");
			//cls->CallFunction("Init", 10, 30);
		//	cls->SetStaticFieldValue<int>("TestInt", 88);
		//	cls->CallStaticFunction("Init", 10, 30);
			auto i1 = cls->CreateInstance();
			auto other = test->GetClass("TestLib", "Other")->CreateInstance();


		//	i1->SetFieldValue<int>("TestInt", 150);


			//cls->SetStaticClass("ThisTest", i1);


			//auto i1 = cls->CreateInstance();
			//int val1 = i1->GetFieldValue<int>("TestInt");

			//i1->SetFieldClass("Test1", other);

			check* ch1 = new check;
			ch1->a = 10;
			ch1->b = 25;

			//i1->SetNativePtr("pt1", (void*)ch1);

			i1->CallFunction("Init", 20, 30);

		//	check* ch2 = (check*)i1->GetNativePtr("pt1");

			check* p = (check*)i1->CallFunctionValue<void*>("TestPtr", (void*)&ch1, 25);




			//check* ch2 = (check*)i1->GetNativePtr("pt1");

			auto v = i1->GetInstanceFields();

			for (auto a : v) {

				switch (a.ctype) {
				case SharpType::SHARP_TYPE_INT:
					std::cout << "F:" << a.name << " Value:" << i1->GetFieldValue<int>(a.name) << std::endl;
					//	printf("Field %s is int with value %d\n", a.name.c_str(), a.value.i);
						break;
				}

			}

			int b = 5;

			//MClass* ncls = i1->GetStaticFieldValue<MClass*>("ThisTest");
			//ncls->CallFunction("Init", 20, 30);




			//int val2 = ncls->GetFieldValue<int>("OtherInt");




			//int res = i1->CallFunctionValue<int>("Add", 50, 100);
			//std::string val = cls->GetStaticFieldValue<std::string>("TestVal");
			*/

	}

	void QEngine::InitPython() {

		py::scoped_interpreter guard{}; // Start the interpreter



		//--Inject global functions
		/*
		py::object main = py::module::import("__main__");
		py::object global = main.attr("__dict__");

		// Expose multiply as a callable directly in the global namespace
		global["multiply"] = py::cpp_function(multiply);
		*/
		py::module_ mymodule = py::module_::create_extension_module(
			"mymodule",       // module name
			nullptr,          // Optional: can pass module docstring here
			new py::module_::module_def() // new empty module definition
		);

		mymodule.def("multiply", &multiply, "Multiply two integers");
		mymodule.def("add", &add, "Add two integers");

		py::module_ sys = py::module_::import("sys");
		sys.attr("modules")["mymodule"] = mymodule;



		py::exec(R"(
        import mymodule
        print("116 * 7 =", mymodule.multiply(6, 7))
    )");


		// Evaluate and call Python function
		py::object result = py::eval("mymodule.add(10, 20)");
		int value = result.cast<int>();

		printf("Result from Python: %d\n", value);


	}

	Physics* QEngine::m_Physics = nullptr;

	void QEngine::InitEngine() {

		m_Draw = new Draw2D(SceneGraph::m_Instance->GetCamera());
		m_VidMat = new MaterialVideo;
		m_Draw->SetMaterial(m_VidMat);
		GameAudio* audio = new GameAudio;
		m_Physics = new Physics;
		RegisterNodeTypes();
		RegisterCComponent(new StaticMeshComponent);
		RegisterCComponent(new StaticRendererComponent);
		RegisterCComponent(new StaticDepthRendererComponent);
		RegisterCComponent(new SkyComponent);
		RegisterCComponent(new LightComponent);
		RegisterCComponent(new CameraComponent);
		RegisterCComponent(new LinesRendererComponent);


	}

	// --- MOVED: The registration logic now lives here, on the engine side ---
	void QEngine::RegisterNodeTypes() {
		NodeRegistry& registry = QEngine::GetNodeRegistry();

		registry.RegisterNode<TickEventNode>("On Tick");
		registry.RegisterNode<NodeTestLogic>("Test Logic Node");
		registry.RegisterNode<NodeTurnNode>("Turn GraphNode");
		registry.RegisterNode<NodeVec3Constant>("Vec3 Constant");
		registry.RegisterNode<GetSceneNode>("Get Scene Node");
		registry.RegisterNode<NodeBoolConstant>("Bool Constant");
		registry.RegisterNode<NodeIf>("If");
		registry.RegisterNode<NodeKeyDown>("Input.KeyDown");
		registry.RegisterNode<NodeAddFloat>("Add(Float)");
		registry.RegisterNode<NodeExposeGraphNode>("Expose Node");
		registry.RegisterNode<NodeSetPositionNode>("Set Position GraphNode");
		registry.RegisterNode<NodeCreateRotationMatrix>("Create Rotation Matrix");
		registry.RegisterNode<NodeMultMat4>("Mult (mat4)");
		registry.RegisterNode<NodeSetRotationNode>("Set Rotation GraphNode");
		registry.RegisterNode<NodeSceneRayCast>("Scene Raycast");
		registry.RegisterNode<GetVariableNode>("GetVariableNode");
		registry.RegisterNode<NodeDebug>("Debug Message");
		registry.RegisterNode<PlayEventNode>("Play Node");
		registry.RegisterNode<NodeStartVideo>("Start Video");
		registry.RegisterNode<RenderEventNode>("Render Node");
		registry.RegisterNode<NodeRenderVideo>("Render Video");

		// --- Math & Vector Nodes ---
		registry.RegisterNode<NodeAddVec3>("Add(Vec3)");
		registry.RegisterNode<NodeSubtractFloat>("Subtract (Float)");
		registry.RegisterNode<NodeSubtractVec3>("Subtract (Vec3)");
		registry.RegisterNode<NodeDivideFloat>("Divide (Float)");
		registry.RegisterNode<NodeDivideVec3>("Divide (Vec3)");
		registry.RegisterNode<NodeMultFloat>("Multiply (Float)");
		registry.RegisterNode<NodeMultVec3>("Multiply (Vec3)");
		registry.RegisterNode<NodeDotProduct>("Dot Product (Vec3)");
		registry.RegisterNode<NodeCrossProduct>("Cross Product (Vec3)");
		registry.RegisterNode<NodeNormalizeVec3>("Normalize (Vec3)");
		registry.RegisterNode<NodeVectorLength>("Length (Vec3)");
		registry.RegisterNode<NodeComposeVec3>("Compose Vec3");
		registry.RegisterNode<NodeDecomposeVec3>("Decompose Vec3");
		registry.RegisterNode<NodeLerpFloat>("Lerp (Float)");
		registry.RegisterNode<NodeLerpVec3>("Lerp (Vec3)");
		registry.RegisterNode<NodeSine>("Sine");
		registry.RegisterNode<NodeRandomFloatInRange>("Random Float In Range");

		// --- GraphNode & Scene Interaction ---
		registry.RegisterNode<LNodeGetPosition>("Get Position");
		registry.RegisterNode<LNodeGetRotation>("Get Rotation");
		registry.RegisterNode<NodeRotateNode>("Rotate Node");
		registry.RegisterNode<NodeMoveNode>("Move Node");
		registry.RegisterNode<NodeFindNodeByName>("Find Node By Name");
		registry.RegisterNode<LNodeGetScale>("Get Scale");
		registry.RegisterNode<NodeGetEulerRotation>("Get Euler Rotation");
		registry.RegisterNode<NodeGetParent>("Get Parent");
		registry.RegisterNode<LNodeSetScale>("Set Scale");
		registry.RegisterNode<NodeSetEulerRotation>("Set Euler Rotation");
		registry.RegisterNode<NodeLookAt>("Look At");
		registry.RegisterNode<NodeAttachTo>("Attach To");
		registry.RegisterNode<NodeDetach>("Detach");
		registry.RegisterNode<NodeGetBounds>("Get Bounds");
		registry.RegisterNode<NodeGetMainCamera>("Get Main Camera");
		registry.RegisterNode<NodeMousePick>("Mouse Pick");
		//	registry.RegisterNode<NodeDecomposeHitResult>("Decompose Hit Result");

			// --- Component & Physics Interaction ---
		registry.RegisterNode<NodeGetLightProperties>("Get Light Properties");
		registry.RegisterNode<NodeSetLightProperties>("Set Light Properties");
		registry.RegisterNode<NodeSetPhysicsBody>("Set Physics Body");

		// --- Constants & Conversion ---
		registry.RegisterNode<NodeConstantFloat>("Constant (Float)");
		registry.RegisterNode<NodeConstantString>("Constant (String)");
		registry.RegisterNode<NodeVec3ToString>("Vec3 To String");
		registry.RegisterNode<NodeFloatToString>("Float To String");

		// --- Logic, Time & Flow Control ---
		registry.RegisterNode<NodeBranch>("Branch"); // Note: Using NodeBranch for "If" functionality
		registry.RegisterNode<NodePrintString>("Print String"); // Note: Using NodePrintString for "Debug"
		registry.RegisterNode<NodeCompareFloat>("Compare (Float)");
		registry.RegisterNode<NodeBooleanAND>("AND (Boolean)");
		registry.RegisterNode<NodeBooleanOR>("OR (Boolean)");
		registry.RegisterNode<NodeBooleanNOT>("NOT (Boolean)");
		registry.RegisterNode<NodeGetDeltaTime>("Get Delta Time");
		registry.RegisterNode<NodeAppendString>("Append String");
		registry.RegisterNode<NodeSequence>("Sequence");
		registry.RegisterNode<NodeForLoop>("For Loop");
		registry.RegisterNode<NodeDoOnce>("Do Once");
	}

	DllImporter* QEngine::m_CDLL = nullptr;
	std::vector<Component*> QEngine::m_CComponents;


	void QEngine::DebugLog(const std::string& message)
	{
		if (DebugLogCB) {
			//DebugLogCB(message);
			std::cout << "MSG:" << message << std::endl;
		}
	}

	void QEngine::InitCDLL() {

		auto path = m_ContentPath + "CGame\\x64\\Debug\\CGame.dll";

		m_CDLL = new DllImporter(path);
		m_CDLL->LoadAndInit();


		int b = 5;

	}

	bool QEngine::IsCComponent(Component* comp) {

		for (auto c : m_CComponents) {

			if (c->GetName() == comp->GetName())
			{
				return true;
			}

		}

		return false;

	}


	float QEngine::m_DeltaTime = 0.1;
	GameContent* QEngine::m_Content = nullptr;
	MaterialVideo* QEngine::m_VidMat = nullptr;

}