#pragma once
#include "RenderMaterial.h"

class Texture3D;

class MaterialCloudGen : public RenderMaterial
{
public:

	MaterialCloudGen();
	void Dispatch(Texture3D* target, float time, float coverage);
	void Bind(bool add) override;
	void Render() override;
};

