
#include "Vivid.h"
#include "RenderTargetCube.h"
#include "RenderTarget2D.h"
#include <pybind11/embed.h>
#include "GameAudio.h"
namespace py = pybind11;
RefCntAutoPtr<IRenderDevice>  Vivid::m_pDevice;
RefCntAutoPtr<IDeviceContext> Vivid::m_pImmediateContext;
RefCntAutoPtr<ISwapChain>     Vivid::m_pSwapChain;
RefCntAutoPtr<IShaderSourceInputStreamFactory> Vivid::m_pShaderFactory;

RenderTargetCube* Vivid::m_BoundRTC = nullptr;
RenderTarget2D* Vivid::m_BoundRT2D = nullptr;
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

void Vivid::InitEngine() {

	GameAudio* audio = new GameAudio;

}