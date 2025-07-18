#pragma once

#include <QWidget>
#include "ui_SceneView.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <qtimer.h>
#include "GameInput.h"
#include "SceneGrid.h"
#include "SceneSelectionOverlay.h"
#include "Draw2D.h"
#include "TerrainEditor.h"
#if D3D11_SUPPORTED
#    include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#endif
#if D3D12_SUPPORTED
#    include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#endif
#if GL_SUPPORTED
#    include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
#    include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#if METAL_SUPPORTED
#    include "Graphics/GraphicsEngineMetal/interface/EngineFactoryMtl.h"
#endif

#ifdef GetObject
#    undef GetObject
#endif
#ifdef CreateWindow
#    undef CreateWindow
#endif

#include <optional>
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "BasicMath.hpp"

using namespace Diligent;

class CameraController;
class SceneController;
class SceneGraph;
class GraphNode;
class TerrainEditor;
class MaterialVideo;
class GameFont;
class GameVideo;

#include <QResizeEvent>

enum SceneMode {
	Mode_Translate,Mode_Rotate,Mode_Scale,Mode_Sculpt,Mode_Paint
};

enum SceneSpace {
	Space_Local,Space_World,Space_Smart
};

enum RunMode {
	RM_Running,RM_Stopped
};

enum EditBrushMode {
	BM_Add, BM_Subtract, BM_Smooth,BM_Direct
};

class SceneView : public QWidget
{
	Q_OBJECT

public:
	SceneView(QWidget *parent = nullptr);
	~SceneView();
    GraphNode* GetTest() {
        return m_Test1;
    }
	void SetMode(SceneMode mode);
	void SetSpace(SceneSpace space);
	void AlignGizmo();
	void Run();
	SceneGraph* GetScene() {
		return m_SceneGraph;
	}
	Q3D::Engine::Texture::Texture2D* GetWhite() {
		return m_White;
	}
	void Update();
	void SetScene(SceneGraph* graph);
	void Stop();
	static SceneView* m_Instance;
	SceneMode m_Mode = SceneMode::Mode_Translate;
	SceneSpace m_Space = SceneSpace::Space_Local;
	GraphNode* m_SelectedNode = nullptr;
	void SelectNode(GraphNode* node);
	void SetEditLayer(int layer);
	float GetTerrainBrushSize() {
		if (m_Terrain == nullptr) return 0;
		return m_TerrainEditor->GetBrushSize();
	}
	float GetTerrainStrength() {
		
		if (m_Terrain) {
			return m_TerrainEditor->GetTerrainStrength();
		}
		return 0;
	}

	void SetTerrainStrength(float str) {
		m_TerrainStrength = str;
		if (m_Terrain) {
			if (m_TerrainEditor)
				m_TerrainEditor->SetBrushStrength(str);
		}
		
	}
	void SetTerrainBrushSize(float s)
	{
		if (m_Terrain) {
			m_TerrainBrushSize = s;
			m_TerrainEditor->SetBrushSize(s);
		}
	}
	void SetTerrainBrushMode(EditBrushMode mode) {
		if (m_Terrain == nullptr) return;
		m_BrushMode = mode;
		if (m_TerrainEditor)
			switch (mode) {
			case BM_Add:
				m_TerrainEditor->SetBrushMode(TerrainBrushMode::BrushMode_Add);
				break;
			case BM_Subtract:
				m_TerrainEditor->SetBrushMode(TerrainBrushMode::BrushMode_Subtract);
				break;
			case BM_Smooth:
				m_TerrainEditor->SetBrushMode(TerrainBrushMode::BrushMode_Smooth);
				break;
			case BM_Direct:
				m_TerrainEditor->SetBrushMode(TerrainBrushMode::BrushMode_Direct);
				break;
			}
		m_BrushMode = mode;
		//	m_TerrainEditor->SetBrushMode(mode);
	}

	void CreateTerrain();
	bool HasTerrain() {
		return m_Terrain != nullptr;
	}

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void CreateGraphics();
	void paintEvent(QPaintEvent* event) override;
	IEngineFactory* GetEngineFactory() { return m_pDevice->GetEngineFactory(); }
	IRenderDevice* GetDevice() { return m_pDevice; }
	IDeviceContext* GetContext() { return m_pImmediateContext; }
	ISwapChain* GetSwapChain() { return m_pSwapChain; }
	void resizeEvent(QResizeEvent* event) override;
    bool rightMousePressed = false;
    QPoint lastMousePos;
    QTimer movementTimer;
    // Movement keys
    QSet<int> keysHeld;
    float yaw = 0.0f, pitch = 0.0f;
	
	void focusOutEvent(QFocusEvent* event) override {
		keysHeld.clear(); // clear all keys on focus loss
		rightMousePressed = false;
		QWidget::focusOutEvent(event); // call base implementation
	
	}
	void leaveEvent(QEvent* event) override {
		// Optional: handle any visual/UI logic when mouse leaves
		keysHeld.clear(); // clear all keys on focus loss

		rightMousePressed = false;
		QWidget::leaveEvent(event);

	}
	void mousePressEvent(QMouseEvent* event) override;
	
	void mouseReleaseEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

    void handleMovement();
   
	void TerrainPlot();

	

private:
	RefCntAutoPtr<IRenderDevice>  m_pDevice;
	RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	RefCntAutoPtr<ISwapChain>     m_pSwapChain;
	RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderFactory;
	Ui::SceneViewClass ui;
	bool m_Pick = false;
	QTimer* m_timer;
	//TEST members
	GraphNode* m_Test1;
	SceneGraph* m_SceneGraph;
	GraphNode* m_L1;
    int LastTick = 0;
    float m_DeltaTime = 0.0f;
	CameraController* m_CameraController;
	SceneController* m_SceneController; 
	QPoint m_PickPos;
	RunMode m_RunMode = RunMode::RM_Stopped;
	SceneGrid* m_Grid;
	SceneSelectionOverlay* m_SelectionOverlay;
	Draw2D* m_Draw;
	Q3D::Engine::Texture::Texture2D* tex1;
	GraphNode* m_Terrain = nullptr;
	float m_TerrainX = 0;
	float m_TerrainZ = 0;
	float m_TerrainBrushSize = 0.75;
	GraphNode* m_BrushNode = nullptr;
	Q3D::Engine::Texture::Texture2D* m_White;
	TerrainEditor* m_TerrainEditor = nullptr;
	float m_TerrainStrength = 0.5f;
	bool m_TerrainEditing = false;
	int m_TerrainLayer = 1;
	bool m_Picking = false;
	EditBrushMode m_BrushMode = EditBrushMode::BM_Add;
	GameVideo* m_Vid1;
	GameFont* m_Font;
	GraphNode* m_ViewCam;
	int m_LastUpdate = 0;
	MaterialVideo* m_VidMat;

};


