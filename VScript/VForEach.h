#pragma once
#include "VAction.h"
#include "VName.h"

class VVar;
class VCodeBody;

class VForEach : public VAction
{
public:

	void SetTarget(VName name)
	{
		m_Name = name;
	}
	VName GetTarget() {
		return m_Name;
	}
	void SetList(VName list) {
		m_List = list;
	}
	VName GetList() {
		return m_List;
	}
	void SetCode(VCodeBody* code) {
		m_Code = code;
	}
	VVar* Exec();

private:

	VName m_Name;
	VName m_List;
	VCodeBody* m_Code;

};

