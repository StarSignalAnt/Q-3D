#pragma once

class VContext;
class VVar;
class VExpression;

class VAction
{
public:

	virtual VVar* Exec() { return nullptr; };
	virtual void SetContext(VContext* context);
	VContext* GetContext();
	void SetGuard(VExpression* guard) {
		m_Guard = guard;
	}
	VExpression* GetGuard() {
		return m_Guard;
	}

protected:

	VContext* m_Context;
	VExpression* m_Guard = nullptr;

};


