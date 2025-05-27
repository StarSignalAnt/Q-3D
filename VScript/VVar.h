#pragma once
#include <string>
#include <vector>
#include "VClass.h"

class VVar
{
public:
	
	VVar();
	void Transfer1(VVar* other);
	float ToFloat() {
		if (m_Type == T_FloatNumber || m_Type == T_Float) {
			return m_FloatValue;
		}
		else {
			return (float)m_IntValue;
		}
	}
	int ToInt() {
		if (m_Type == T_Number || m_Type==T_Int) {
			return m_IntValue;
		}
		return (int)m_FloatValue;

	}
	std::string ToString() {
		return m_StrValue;
	}
	void SetName(std::string name) {
		if (name == "r")
		{
			int bb = 5;
		}
		m_Name = name;
	}
	void SetInt(int value) {
		m_IntValue = value;
	}
	void SetFloat(float value) {
		m_FloatValue = value;
	}
	void SetType(TokenType type) {
		m_Type = type;
	}
	void SetClassType(std::string type) {
		m_ClassType = type;
	}
	void SetArray(int index, VVar* value);
	VVar* GetArray(int index) {
		return m_ArrValues[index];
	}
	void SetClassValue(VClass* cls)
	{
		m_ClsValue = cls;
	}
	VClass* GetClassValue()
	{
		return m_ClsValue;
	}
	bool IsName(std::string name) {
		return name == m_Name;
	}
	bool IsType(TokenType type) {
		return m_Type == type;
	}
	std::string GetClassType() {
		return m_ClassType;
	}
	void SetString(std::string str)
	{
		m_StrValue = str;
	}
	TokenType GetType()
	{
		return m_Type;
	}
	void SetLambda(VLambda* lambda) {
		m_Lambda = lambda;
	}
	VLambda* GetLambda() {
		return m_Lambda;
	}
	std::string GetName() {
		return m_Name;
	}
	void CreateArray(int size) {

		for (int i = 0; i < size; i++) {
			m_ArrValues.push_back(new VVar);
		}

	}
	std::vector<VVar*> GetArrayData() {
		return m_ArrValues;
	}

	void SetArrayData(std::vector<VVar*> data) {
		m_ArrValues = data;
	}
	void* ToC() {
		return m_CValue;
	}

	void SetC(void* obj)
	{
		m_CValue = obj;
	}
	void ListAdd(VVar* val)
	{
		m_List.push_back(val);
	}
	std::vector<VVar*> GetList() {
		return m_List;
	}
	void SetDataName(std::string name) {
		m_DataName = name;
	}
	std::string GetDataName() {
		return m_DataName;
	}

private:

	std::string m_Name = "";
	int m_IntValue = 0;
	float m_FloatValue = 0.0f;
	std::string m_StrValue = "";
	bool m_BoolValue = false;
	VClass* m_ClsValue = nullptr;
	std::vector<VVar*> m_ArrValues;
	TokenType m_Type;
	std::string m_ClassType = "";
	VLambda* m_Lambda;
	void* m_CValue = nullptr;
	std::vector<VVar*> m_List;
	std::string m_DataName;

};

