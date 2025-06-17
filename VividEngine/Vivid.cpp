
#include "Vivid.h"
#include "RenderTargetCube.h"
#include "RenderTarget2D.h"
#include <pybind11/embed.h>
#include "GameAudio.h"
#include "Physics.h"
#include "MonoHost.h"
#include "MAsm.h"
#include "MClass.h"
#include "NodeRegistry.h"
#include "NodeTurnNode.h"
#include "TickEventNode.h"
#include "NodeTestLogic.h"
#include "NodeVec3Constant.h"
#include "GetSceneNode.h"
#include "NodeBoolConstant.h"
#include "NodeIf.h"
#include "NodeKeyDown.h"

namespace py = pybind11;
RefCntAutoPtr<IRenderDevice>  Vivid::m_pDevice;
RefCntAutoPtr<IDeviceContext> Vivid::m_pImmediateContext;
RefCntAutoPtr<ISwapChain>     Vivid::m_pSwapChain;
RefCntAutoPtr<IShaderSourceInputStreamFactory> Vivid::m_pShaderFactory;

RenderTargetCube* Vivid::m_BoundRTC = nullptr;
RenderTarget2D* Vivid::m_BoundRT2D = nullptr;
std::vector<MaterialPBR*> Vivid::m_ActiveMaterials;
MonoLib* Vivid::m_MonoLib = nullptr;
std::vector<SharpClassInfo> Vivid::m_ComponentClasses;
VividCallbackFunc Vivid::DebugLogCB = nullptr;




void Vivid::SetBoundRTC(RenderTargetCube* target) {

	m_BoundRTC = target;

}
int Vivid::GetFrameWidth() {

	if (m_BoundRTC != nullptr) {
		return m_BoundRTC->GetWidth();
	}
	if (m_BoundRT2D != nullptr) {
		return m_BoundRT2D->GetWidth();
	}
	return m_FrameWidth;

}

int Vivid::GetFrameHeight() {

	if (m_BoundRTC != nullptr) {
		return m_BoundRTC->GetHeight();
	}

	if (m_BoundRT2D != nullptr) {
		return m_BoundRT2D->GetHeight();
	}
	return m_FrameHeight;


}
void Vivid::SetFrameWidth(int w) {

	m_FrameWidth = w;

}

void Vivid::SetFrameHeight(int h) {

	m_FrameHeight = h;

}


void Vivid::SetBoundRT2D(RenderTarget2D* target) {

	m_BoundRT2D = target;

}

int Vivid::m_FrameWidth = 0;
int Vivid::m_FrameHeight = 0;


void Vivid::ClearZ() {

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

void Vivid::InitMono() {


	MonoHost* host = new MonoHost;

	m_MonoLib = new MonoLib("VividGame.dll");

	auto classes = m_MonoLib->GetClasses();
	for (auto c : classes) {

		if (c.baseClassName == "Vivid.Component.SharpComponent")
		{
			m_ComponentClasses.push_back(c);
		}

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

void Vivid::InitPython() {

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

Physics* Vivid::m_Physics = nullptr;

void Vivid::InitEngine() {

	
	GameAudio* audio = new GameAudio;
	m_Physics = new Physics;
	RegisterNodeTypes();
}

// --- MOVED: The registration logic now lives here, on the engine side ---
void Vivid::RegisterNodeTypes() {
	NodeRegistry& registry = Vivid::GetNodeRegistry();

	registry.RegisterNode<TickEventNode>("On Tick");
	registry.RegisterNode<NodeTestLogic>("Test Logic Node");
	registry.RegisterNode<NodeTurnNode>("Turn GraphNode");
	registry.RegisterNode<NodeVec3Constant>("Vec3 Constant");
	registry.RegisterNode<GetSceneNode>("Get Scene Node");
	registry.RegisterNode<NodeBoolConstant>("Bool Constant");
	registry.RegisterNode<NodeIf>("If");
	registry.RegisterNode<NodeKeyDown>("Input.KeyDown");
}