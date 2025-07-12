#include "MaterialProducer.h"
#include "MaterialPBR.h"
#include "MaterialDepth.h"

MaterialProducer* MaterialProducer::m_Instance = nullptr;


MaterialProducer::MaterialProducer() {

	m_Instance = this;
	m_PBR = new MaterialPBR;
	m_Depth = new MaterialDepth;


}

MaterialDepth* MaterialProducer::GetDepth() {

	auto depth = new MaterialDepth(true);
	depth->SetPSO(m_Depth->GetPSO());
	depth->SetSRB(m_Depth->GetSRB());
	depth->SetUniformBuffer(m_Depth->GetUniformBuf());
	return depth;
}

MaterialPBR* MaterialProducer::GetPBR() {

	auto pbr = new MaterialPBR(true);
	
	pbr->SetPSO(m_PBR->GetPSO());
	pbr->SetPSOAdd(m_PBR->GetPSOAdd());
	pbr->SetSRB(m_PBR->GetSRB());
	pbr->SetSRBAdd(m_PBR->GetSRBAdd());
	pbr->SetUniformBuffer(m_PBR->GetUniformBuf());
	pbr->SetEnvironmentMap(m_PBR->GetEnvironmentMap());
	pbr->SetColorTexture(m_PBR->GetColorTexture());
	pbr->SetNormalTexture(m_PBR->GetNormalTexture());
	pbr->SetRoughnessTexture(m_PBR->GetRoughTexture());
	pbr->SetMetallicTexture(m_PBR->GetMetallicTexture());
	return pbr;
}