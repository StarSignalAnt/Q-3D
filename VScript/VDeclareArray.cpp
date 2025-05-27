#include "pch.h"
#include "VDeclareArray.h"
#include "VToken.h"
#include "VExpression.h"
#include "VContext.h"
#include "VVar.h"

void VDeclareArray::SetType(TokenType type) {

	m_Type = type;

}

void VDeclareArray::SetSizeExpression(VExpression* expression) {

	m_SizeExp = expression;

}

void VDeclareArray::SetName(VName name) {

	m_Name = name;

}

VVar* VDeclareArray::Exec() {

	auto con = GetContext();

	auto top_scope = con->GetTopScope();

	auto nv = new VVar;
//	nv->m_Type = m_Type;
//	nv->m_Name = m_Name.GetNames()[0];
	nv->SetType(m_Type);
	nv->SetName(m_Name.GetNames()[0]);


	int size = m_SizeExp->Express()->ToInt();

	nv->CreateArray(size);

	for (int i = 0; i < size; i++) {
		VVar* ave = new VVar;
	//	ave->m_Type = m_Type;
		ave->SetType(m_Type);
		nv->SetArray(i, ave);

		//nv->m_ArrValues.push_back(ave);

	}

	top_scope->RegisterVar(nv);

	int b = 5;


	return nullptr;
}