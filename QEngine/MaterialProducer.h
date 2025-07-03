#pragma once

class MaterialPBR;
class MaterialDepth;

class MaterialProducer
{
public:

	MaterialProducer();
	MaterialPBR* GetPBR();
	MaterialDepth* GetDepth();
	static MaterialProducer* m_Instance;


private:

	MaterialPBR* m_PBR;
	MaterialDepth* m_Depth;

};

