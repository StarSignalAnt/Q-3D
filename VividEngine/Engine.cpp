#include "pch.h"
#include "Engine.h"
#include "RenderTargetCube.h"
#include "MaterialBase.h"
#include "ScriptHost.h"
#include "RenderTarget2D.h"
#include <filesystem>


namespace fs = std::filesystem;
bool contains_search_string(const std::string& file_path, const std::string& search_string) {
	std::string lower_file_path = file_path;
	std::string lower_search_string = search_string;

	// Convert both the file path and the search string to lowercase
	std::transform(lower_file_path.begin(), lower_file_path.end(), lower_file_path.begin(), ::tolower);
	std::transform(lower_search_string.begin(), lower_search_string.end(), lower_search_string.begin(), ::tolower);

	// Check if the lowercased search string is found in the lowercased file path
	return lower_file_path.find(lower_search_string) != std::string::npos;
}

std::string find_file_with_search_string(const std::string& path, const std::string& search_string) {
	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (entry.is_regular_file()) {
			if (contains_search_string(entry.path().string(), search_string)) {
				return entry.path().string();
			}
		}
		else if (entry.is_directory()) {
			if (contains_search_string(entry.path().string(), search_string)) {
				return entry.path().string();
			}
		}
	}
	return ""; // Return an empty string if no file or directory is found
}



RefCntAutoPtr<IRenderDevice>  Engine::m_pDevice;
RefCntAutoPtr<IDeviceContext> Engine::m_pImmediateContext;
RefCntAutoPtr<ISwapChain>     Engine::m_pSwapChain;
RefCntAutoPtr<IShaderSourceInputStreamFactory> Engine::m_pShaderFactory;

ScriptHost* Engine::m_ScriptHost = nullptr;

NodeCamera* Engine::m_Camera = nullptr;
Node* Engine::m_Node = nullptr;
std::vector<NodeLight*> Engine::m_Lights;
RenderTargetCube* Engine::m_BoundRTC = nullptr;
RenderTarget2D* Engine::m_BoundRT2D = nullptr;

int Engine::GetFrameWidth() {

	if (m_BoundRTC != nullptr) {
		return m_BoundRTC->GetWidth();
	}
	if (m_BoundRT2D != nullptr) {
		return m_BoundRT2D->GetWidth();
	}
	return m_FrameWidth;

}

int Engine::GetFrameHeight() {

	if (m_BoundRTC != nullptr) {
		return m_BoundRTC->GetHeight();
	}
	if (m_BoundRT2D != nullptr) {
		return m_BoundRT2D->GetHeight();
	}
	return m_FrameHeight;


}

void Engine::SetFrameWidth(int w) {

	m_FrameWidth = w;

}

void Engine::SetFrameHeight(int h) {

	m_FrameHeight = h;

}

int Engine::m_FrameWidth = 0;
int Engine::m_FrameHeight = 0;

void Engine::ClearZ() {

	m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);


}

void Engine::SetBoundRT2D(RenderTarget2D* target) {

	m_BoundRT2D = target;

}

void Engine::SetBoundRTC(RenderTargetCube* target) {

	m_BoundRTC = target;

}

bool containsSubstring(const std::string& str, const std::string& substring) {
	return str.find(substring) != std::string::npos;
}

MaterialBase* Engine::FindActiveMaterial(std::string path) {

	for (auto mat : m_ActiveMaterials) {

		if (mat->GetPath() == path) {
			return mat;
		}
		if (containsSubstring(mat->GetPath(), path))
		{
			return mat;
		}
		if (mat->GetPath().length() > 0) {
			if (containsSubstring(path, mat->GetPath()))
			{
				return mat;
			}
		}

	}
	return nullptr;

}

std::vector<MaterialBase*> Engine::m_ActiveMaterials;

NodeLight* Engine::m_Light = nullptr;

SceneGraph* Engine::m_ActiveGraph = nullptr;

bool Engine::m_MeterialsUpdated = false;
std::string Engine::m_ContentPath = "c:\\content\\";


std::optional<std::string> Engine::searchStringInPath(const std::string& path, const std::string& searchString) {
	try {
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (entry.is_regular_file()) {
				std::string filePath = entry.path().string();
				if (filePath.find(searchString) != std::string::npos) {
					return filePath;
				}
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		// Handle or log the error as needed
	}
	return std::nullopt;
}

std::optional<std::string> Engine::FindResource(std::string name) {

	return find_file_with_search_string(Engine::m_ContentPath, name);

}

SolarisRenderer* Engine::m_Solaris = nullptr;
RendererBase* Engine::m_ActiveRenderer = nullptr;