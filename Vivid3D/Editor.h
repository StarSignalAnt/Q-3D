#pragma once
#include <string>
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

enum GizmoMode {
	GM_Translate,GM_Rotate,GM_Scale
};
enum SpaceMode {
	SM_Local,SM_Global
};

enum RunMode {
	RM_Playing,RM_Stopped,RM_Paused,
};

enum TerrainEditMode {
	EM_Paint,EM_Sculpt
};

class SceneGraph;
class NodeEntity;
class VSceneGraph;
class VContentBrowser;
class VPropertyEditor;
class Node;
class Vivid3D;
class QFrame;
class QVBoxLayout;
class VTrackFrame;
class Cinematic;
class NodeCamera;
class VScriptEdit;

class Editor
{
public:
	static RunMode m_RunMode;
	static SceneGraph* m_Graph;
	static GizmoMode m_GizmoMode;
	static Node* m_CurrentNode;
	static SpaceMode m_SpaceMode;
	static TerrainEditMode m_TerrainEditMode;
	static VSceneGraph* m_SceneGraph;
	static VContentBrowser* m_ContentBrowser;
	static VPropertyEditor* m_PropEditor;
	static Vivid3D* m_Main;
	static VTrackFrame* m_TrackPanel;
	static Cinematic* m_EditAnimation;
	static void BeginPlay();
	static void Stop();
	static float TerrainX;
	static float TerrainZ;
	static float TerrainBrushSize;
	static float TerrainBrushStrength;
	static int TerrainEditLayer;
	static void CreateAnimation(std::string anim, float length);
	static double m_AnimEditTime;
	static QSystemTrayIcon* m_Tray;
	static void Notify(std::string title, std::string info, int display);
	static std::vector<NodeCamera*> m_Cameras;
	static NodeCamera* m_GameCamera;
	static VScriptEdit* m_ScriptEdit;
};
