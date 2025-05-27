#include "pch.h"
#include "VLambda.h"
#include "VCodeBody.h"
#include "VVar.h"

void VLambda::SetBody(VCodeBody* body) {

	m_Code = body;

}

VCodeBody* VLambda::GetBody() {

	return m_Code;

}

VVar* VLambda::Exec() {


	return nullptr;
}