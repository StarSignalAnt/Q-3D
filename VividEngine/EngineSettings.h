#pragma once

enum QualitySetting {

	Q_Low,Q_Mid,Q_High,Q_Ultra

};

enum RendererSetting {

	R_Nitro, R_Solaris

};

class EngineSettings
{
public:

	static QualitySetting TextureQuality;
	static QualitySetting ShaderQuality;
	static QualitySetting ShadowQuality;
	static RendererSetting UsedRenderer;
	static void SaveSettings();
	static void LoadSettings();
	

};

