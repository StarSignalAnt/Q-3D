#include "pch.h"
#include "VInvoke.h"
#include "VVar.h"
#include "VContext.h"
#include "VLambda.h"
#include "VCodeBody.h"
#include "VScope.h"

VVar* VInvoke::Exec() {

	auto con = GetContext();




	auto fv = con->FindVar(m_Target.GetNames()[0]);

	auto lam = fv->GetLambda();
	auto body = lam->GetBody();
	auto local = lam->GetScope();

	GetContext()->PushScope(local);


	body->SetContext(GetContext());
	body->Exec();
	GetContext()->PopScope();


	return nullptr;

}