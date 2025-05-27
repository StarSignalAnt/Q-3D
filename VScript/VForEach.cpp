#include "pch.h"
#include "VForEach.h"
#include "VContext.h"
#include "VCodeBody.h"

VVar* VForEach::Exec() {

	VVar* obj = m_Context->FindVar(m_Name.GetNames());
	VVar* list = m_Context->FindVar(m_List.GetNames());
	m_Code->SetContext(m_Context);

	auto c_list = list->GetList();

	for (auto i : c_list)
	{

		obj->Transfer1(i);
		m_Code->Exec();


	}

	return nullptr;
}