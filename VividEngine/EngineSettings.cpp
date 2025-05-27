#include "pch.h"
#include "EngineSettings.h"
#include "VFile.h"

QualitySetting EngineSettings::TextureQuality = QualitySetting::Q_High;
QualitySetting EngineSettings::ShaderQuality = QualitySetting::Q_High;
QualitySetting EngineSettings::ShadowQuality = QualitySetting::Q_High;
RendererSetting EngineSettings::UsedRenderer = R_Nitro;

void EngineSettings::SaveSettings() {

	VFile* file = new VFile("settings.engine",FileMode::Write);

	file->WriteInt((int)TextureQuality);
	file->WriteInt((int)ShaderQuality);
	file->WriteInt((int)ShadowQuality);
	file->WriteInt((int)UsedRenderer);

	file->Close();
}

void EngineSettings::LoadSettings() {

	if (!VFile::Exists("settings.engine"))
	{
		return;
	}

	VFile* file = new VFile("settings.engine", FileMode::Read);

	TextureQuality = (QualitySetting)file->ReadInt();
	ShaderQuality = (QualitySetting)file->ReadInt();
	ShadowQuality = (QualitySetting)file->ReadInt();
	UsedRenderer = (RendererSetting)file->ReadInt();

	file->Close();

}