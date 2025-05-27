#include "pch.h"
#include "VExpression.h"
#include "VContext.h"
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include "VFunction.h"
#include <stdexcept>
#include "VStatementCall.h"
#include "VClassCall.h"
#include "VVar.h"
// INT EVAL
bool isOperator(const std::string& token) {
	return token == "+" || token == "-" || token == "*" || token == "/" ||
		token == "^" || token == "%" || token == "=" || token == "<" || token == ">";
}

// Helper function to get the precedence of an operator
// Helper function to get the precedence of an operator
int getPrecedence(const std::string& op) {
	if (op == "=") return 0; // Equality check has the lowest precedence
	if (op == "<" || op == ">") return 1; // Comparison operators have precedence between equality and arithmetic
	if (op == "+" || op == "-") return 2;
	if (op == "*" || op == "/" || op == "%") return 3;
	if (op == "^") return 4;
	return -1;
}

float performOperationFLOAT(float a, float b, const std::string& op) {
	if (op == "+") return a + b;
	if (op == "-") return a - b;
	if (op == "*") return a * b;
	if (op == "/") {
		if (b == 0) throw std::runtime_error("Division by zero");
		return a / b;
	}
	if (op == "%") {
		if (b == 0) throw std::runtime_error("Modulo by zero");
		return std::fmod(a, b);
	}
	if (op == "^") return std::pow(a, b);
	if (op == "=") return a == b ? 1 : 0;
	if (op == "<") return a < b ? 1 : 0;
	if (op == ">") return a > b ? 1 : 0;
	throw std::invalid_argument("Invalid operator");
}


VVar* performOperationOp(VVar* a, VVar* b, const std::string& op) {

	std::string _op = "";
	if (op == "+")
	{
		_op = "Plus";

	}
	else if (op == "*")
	{
		_op = "Times";
	}
	else if (op == "/")
	{
		_op = "Divide";
	}
	else if (op == "-")
	{
		_op = "Minus";
	}
		//auto op_type = Elements[1].OpType;

		
		auto cls = a->GetClassValue();

		VFunction* func = nullptr;

		VCallParameters* params = new VCallParameters;

		ExpElement e1;

		e1.EleType = T_Class;
		e1.VarName.Add(b->GetName());

		VExpression* op_ex = new VExpression;

		op_ex->Elements.push_back(e1);

		params->AddParam(op_ex);

		
		std::vector<std::string> sig;

		sig.push_back(b->GetClassType());

		func = cls->FindFunctionBySig2(_op,sig);
		if (func == nullptr) {
			printf("Function operator overload not found:");
			printf(op.c_str());
			printf("\n");
			exit(1);
		}
		return func->Call(params);
		//return new VVar;
	
	/*
	if (op == "+") return a + b;
	if (op == "-") return a - b;
	if (op == "*") return a * b;

	if (op == "/") {
		if (b == 0) throw std::runtime_error("Division by zero");
		return a / b;
	}
	if (op == "%") {
		if (b == 0) throw std::runtime_error("Modulo by zero");
		return a % b;
	}
	if (op == "^") return std::pow(a, b);
	if (op == "=") return a == b ? 1 : 0;
	if (op == "<") return a < b ? 1 : 0;
	if (op == ">") return a > b ? 1 : 0;
	throw std::invalid_argument("Invalid operator");
	*/
}


// Helper function to perform an operation
int performOperationINT(int a, int b, const std::string& op) {
	if (op == "+") return a + b;
	if (op == "-") return a - b;
	if (op == "*") return a * b;
	if (op == "/") {
		if (b == 0) throw std::runtime_error("Division by zero");
		return a / b;
	}
	if (op == "%") {
		if (b == 0) throw std::runtime_error("Modulo by zero");
		return a % b;
	}
	if (op == "^") return std::pow(a, b);
	if (op == "=") return a == b ? 1 : 0;
	if (op == "<") return a < b ? 1 : 0;
	if (op == ">") return a > b ? 1 : 0;
	throw std::invalid_argument("Invalid operator");
}

// Function to convert infix expression to postfix (RPN)
std::vector<std::string> infixToPostfix(const std::vector<std::string>& tokens) {
	std::stack<std::string> operatorStack;
	std::vector<std::string> outputQueue;

	bool expectUnary = true; // True if we expect a unary operator or a number

	for (const std::string& token : tokens) {
		if (isOperator(token)) {
			if (expectUnary && token == "-") {
				outputQueue.push_back("0"); // Treat unary minus as subtraction from zero
			}
			while (!operatorStack.empty() && getPrecedence(operatorStack.top()) >= getPrecedence(token)) {
				outputQueue.push_back(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(token);
			expectUnary = true;
		}
		else {
			outputQueue.push_back(token);
			expectUnary = false;
		}
	}

	while (!operatorStack.empty()) {
		outputQueue.push_back(operatorStack.top());
		operatorStack.pop();
	}

	return outputQueue;
}


// Function to evaluate an RPN expression
float evaluateFLOAT(const std::vector<std::string>& tokens) {
	std::stack<float> stack;

	for (const std::string& token : tokens) {
		if (isOperator(token)) {
			if (stack.size() < 2) throw std::runtime_error("Invalid expression");
			float b = stack.top();
			stack.pop();
			float a = stack.top();
			stack.pop();
			float result = performOperationFLOAT(a, b, token);
			stack.push(result);
		}
		else {
			try {
				float value = std::stof(token);
				stack.push(value);
			}
			catch (const std::invalid_argument&) {
				throw std::invalid_argument("Invalid token: " + token);
			}
		}
	}

	if (stack.size() != 1) throw std::runtime_error("Invalid expression");
	return stack.top();
}



// Function to evaluate an RPN expression
VVar* evaluateOp(VContext* con,const std::vector<std::string>& tokens) {
	std::stack<VVar*> stack;

	for (const std::string& token : tokens) {
		if (isOperator(token)) {
			if (stack.size() < 2) throw std::runtime_error("Invalid expression");
			VVar* b = stack.top();
			stack.pop();
			VVar* a = stack.top();
			stack.pop();
			VVar* result = performOperationOp(a, b, token);
			stack.push(result);
		}
		else {
			try {

				//int value = std::stoi(token);
				stack.push(con->FindVar(token));
			}
			catch (const std::invalid_argument&) {
				throw std::invalid_argument("Invalid token: " + token);
			}
		}
	}

	if (stack.size() != 1) throw std::runtime_error("Invalid expression");
	return stack.top();
}


// Function to evaluate an RPN expression
int evaluateINT(const std::vector<std::string>& tokens) {
	std::stack<int> stack;

	for (const std::string& token : tokens) {
		if (isOperator(token)) {
			if (stack.size() < 2) throw std::runtime_error("Invalid expression");
			int b = stack.top();
			stack.pop();
			int a = stack.top();
			stack.pop();
			int result = performOperationINT(a, b, token);
			stack.push(result);
		}
		else {
			try {
				int value = std::stoi(token);
				stack.push(value);
			}
			catch (const std::invalid_argument&) {
				throw std::invalid_argument("Invalid token: " + token);
			}
		}
	}

	if (stack.size() != 1) throw std::runtime_error("Invalid expression");
	return stack.top();
}

bool VExpression::Is_String() {

	bool is_string = false;

	for (auto ele : Elements) {

		if (ele.EleType == T_String) {

			is_string = true;

		}
		else {

			if (ele.EleType == T_Operator) {
				continue;
			}
			if (ele.EleType == T_Number || ele.EleType == T_FloatNumber) {

				return false;

			}
			VVar* fv = nullptr;

			if (ele.VarName.GetNames().size() > 0) {
				fv = m_Context->FindVar(ele.VarName.GetNames());

				if (fv->IsType(T_String)) {

					is_string = true;
					int aa = 5;
				}
				else {

					is_string = false;
				}
			}

		}

	}

	return is_string;
}

bool VExpression::Is_Int() {

	bool is_int = true;
	bool is_float = false;

	for (auto ele : Elements) {
		if (ele.IsSubExpr) {
			ele.SubExpr->m_Context = m_Context;
			auto s_res = ele.SubExpr->Express();

			if (s_res->IsType(T_Number)) {
				is_int = true;
			}
			else if (s_res->IsType(T_FloatNumber)) {
				is_float = true;
				is_int = false;
			}

			int a = 5;

		}


		switch (ele.EleType) {
		case T_Ident:
		{

			VVar* fv = nullptr;
		

			fv = m_Context->FindVar(ele.VarName.GetNames());

				 
			
			if (fv->IsType(T_FloatNumber))
			{
				is_float = true;
				is_int = false;
			}
			else if (fv->IsType(T_Number))
			{
				is_int = true;
			}
			else if (fv->IsType(T_Class))
			{
				is_int = false;
				is_float = false;
			}
			int bb = 5;

		}
			break;
		case T_Number:

			if (!is_float) {
				is_int = true;
				
			}

			break;
		case T_FloatNumber:
			is_int = false;
			is_float = true;
			break;
		}

	}

	if (is_float) {
		return false;
	}
	if (is_int) {
		return true;
	}

	return false;

}

bool VExpression::Is_Float() {

	bool is_int = false;
	bool is_float = false;

 	for (auto ele : Elements) {

		if (ele.IsSubExpr) {
/*
			ele.SubExpr->m_Context = m_Context;
			auto s_res = ele.SubExpr->Express();

			if (s_res->m_Type == T_Number) {
				is_int = true;
			}
			else if (s_res->m_Type == T_FloatNumber) {
				is_float = true;
				is_int = false;
			}

			int a = 5;
	*/
	//continue;
		}

		switch (ele.EleType) {
		case T_Ident:
		{
			VVar* fv = nullptr;


			fv = m_Context->FindVar(ele.VarName.GetNames());

			if (fv->IsType(T_FloatNumber) || fv->IsType(T_Float))
			{
				is_float = true;
				is_int = false;
			}
			else if (fv->IsType(T_Number) || fv->IsType(T_Int))
			{
				is_int = true;
			}
			else if (fv->IsType(T_Class))
			{
				is_int = false;
				is_float = false;
			}
			int bb = 5;

		}
		break;
		case T_Number:
		case T_Int:

			if (!is_float) {
				is_int = true;

			}

			break;
		case T_Float:
		case T_FloatNumber:
			is_int = false;
			is_float = true;
			break;
		
		}

	}

	if (is_float) {
		return true;
	}
	if (is_int) {
		return false;
	}

	return false;

}

bool VExpression::Is_Object() {

	bool is_obj = false;

	for (auto e : Elements) {

		if (e.EleType == T_Ident) {

			is_obj = true;
			auto nv = m_Context->FindVar(e.VarName.GetNames());
			if (nv->GetClassValue() == nullptr)
			{
				return false;
			}
			else {
				if (nv->GetType() == T_Int || nv->GetType() == T_Number || nv->GetType() == T_FloatNumber || nv->GetType() == T_Float)
				{
					return false;
				}
			}


		}

	}

	return is_obj;

}

VVar* VExpression::Express() {

	auto con1 = m_Context;
	if (m_Context == nullptr) {
		printf("Expression has no context.\n");
		exit(1);
	}

	auto v = new VVar;

	if (Elements.size() == 1)

	{

		if (Elements[0].ClassCall != nullptr || Elements[0].Statement!=nullptr) {

			ExpElement e = Elements[0];

			if (e.EleType == T_Func) {

				if (e.Statement != nullptr) {
					e.Statement->SetContext(m_Context);
					auto res = e.Statement->Exec();
					return res;
					//stack.push_back(std::to_string(res->ToFloat()));
				}
				else {
					e.ClassCall->SetContext(m_Context);
					auto res = e.ClassCall->Exec();
					int bb = 5;
					return res;
					//stack.push_back(std::to_string(res->ToFloat()));
				}
			}

			int aa = 5;
		}

		if (Elements[0].VarName.GetNames().size() > 0) {
			auto fv = m_Context->FindVar(Elements[0].VarName.GetNames());

			if (fv->GetType() == T_Lambda) {

				return fv;

				int b = 5;
			}
			if (Elements[0].EleType == T_Ident || Elements[0].EleType == T_Class) {
				return fv;
			}
		}

		if (Elements[0].IsNew) {

			VVar* cls_v = new VVar;
			cls_v->SetClassType(Elements[0].ClassType);
			cls_v->SetClassValue(con1->CreateInstance(cls_v->GetClassType()));
			

			std::vector<VVar*> cps;
			std::vector<TokenType> sig;

			bool is_int = true;
			bool is_float = false;

			for (auto cp : Elements[0].NewParams->GetParams())
			{
				auto cv = cp->Express();
				cps.push_back(cv);
				if (cv->IsType(T_Number)) {
					is_int = true;
					sig.push_back(T_Number);
				}
				if (cv->IsType(T_FloatNumber)) {
					is_int = false;
					is_float = true;
					sig.push_back(T_FloatNumber);
				}
				int b = 5;
			}


			auto func = cls_v->GetClassValue()->FindFunctionBySig(cls_v->GetClassType(), sig);

			func->Call(Elements[0].NewParams);

			int b = 5;
			return cls_v;


		}

	}

	if (Is_Object()) {


		auto op_vector = ToOpVector();
		auto tvec = infixToPostfix(op_vector);

		auto res = evaluateOp(m_Context,tvec);
		return res;
		int b = 5;
		
		/*
		auto v1 = m_Context->FindVar(Elements[0].VarName.GetNames());
		auto v2 = m_Context->FindVar(Elements[2].VarName.GetNames());

		auto op_type = Elements[1].OpType;

		auto cls = v1->GetClassValue();

		VFunction* func = nullptr;

		VCallParameters* params = new VCallParameters;

		ExpElement e1;

		e1.EleType = T_Class;
		e1.VarName.Add(v2->GetName());

		VExpression* op_ex = new VExpression;

		op_ex->Elements.push_back(e1);

		params->AddParam(op_ex);

		switch (op_type) {
		case T_Plus:
		{
			func = cls->FindFunction("Plus");
			return func->Call(params);
		}
			break;
		case T_Minus:
		{
			func = cls->FindFunction("Minus");
			return func->Call(params);
		}
			break;
		case T_Divide:
		{
			func = cls->FindFunction("Divide");
			return func->Call(params);
		}
			break;
		case T_Times:
		{
			func = cls->FindFunction("Times");
			return func->Call(params);

		}
			break;
		}



		int b = 5;
		*/

	}else 

	if (Is_String()) {

		v->SetType(T_String);
		std::string res = "";

		for (auto e : Elements) {

			VVar* fv = nullptr;

			if (e.VarName.GetNames().size() > 0) {
				fv = m_Context->FindVar(e.VarName.GetNames());
				//if(fv.)
				if (e.IsArray) {
					res = res + fv->GetArray(e.IndexExpression->Express()->ToInt())->ToString();
				}

				if (fv->IsType(T_String)) {
					res = res + fv->ToString();
				}
			}
			else {
				res = res + e.StringValue;
			}


		}

		v->SetString(res);
		//v->m_StrValue = Elements[0].StringValue;


		return v;

	}else 
	if (Is_Float()) {

		int a = 5;
		auto vec = ToVector();

		auto tvec = infixToPostfix(vec);

		v->SetFloat(evaluateFLOAT(tvec));
		v->SetType(T_FloatNumber);

		return v;

	}
	else if (Is_Int()) {


		auto vec = ToVector();

		auto tvec = infixToPostfix(vec);

		v->SetInt(evaluateINT(tvec));

		v->SetType(T_Number);

		return v;

	}
	else {

		
		auto fv = m_Context->FindVar(Elements[0].VarName.GetNames());
		
		v->SetType(T_Class);

		v->SetClassType(fv->GetClassType());
		v->SetClassValue(fv->GetClassValue());


		return v;

		int aa = 5;

	}


	if (Elements[0].VarName.GetNames().size() > 0)
	{
		v->SetInt(0);
		auto fv = m_Context->FindVar(Elements[0].VarName.GetNames()[0]);
		v->SetInt(fv->ToInt());
		return v;


	}
	v->SetInt(Elements[0].IntValue);
	return v;
}

std::vector<std::string> VExpression::ToOpVector() {

	std::vector<std::string> stack;

	for (auto e : Elements) {

		int bb = 5;
		if (e.EleType == T_Ident) {
			stack.push_back(e.VarName.GetNames()[0]);
		}
		else if (e.EleType == T_Func) {

		}
		else if (e.EleType == T_Operator)
		{
			switch (e.OpType)
			{
			case T_Times:

				stack.push_back("*");
				break;
			case T_Divide:
				stack.push_back("/");
				break;
			case T_Minus:
				stack.push_back("-");
				break;
			case T_Plus:
				
				stack.push_back("+");



				break;
			}
		}
	}

	return stack;
}

std::vector<std::string> VExpression::ToVector() {

	std::vector<std::string> stack;

	for (auto e : Elements) {

		if (e.EleType == T_Func) {

			if (e.Statement != nullptr) {
				e.Statement->SetContext(m_Context);
				auto res = e.Statement->Exec();
				stack.push_back(std::to_string(res->ToFloat()));
			}
			else {
				e.ClassCall->SetContext(m_Context);
				auto res = e.ClassCall->Exec();
				stack.push_back(std::to_string(res->ToFloat()));
			}
		}

		if (e.IsSubExpr) {
			e.SubExpr->m_Context = m_Context;
			auto res = e.SubExpr->Express();


			if (res->IsType(T_FloatNumber)) {
				stack.push_back(std::to_string(res->ToFloat()));
			}
			else if (res->IsType(T_Number)) {
				stack.push_back(std::to_string(res->ToInt()));
			}
			continue;
		}

		switch (e.EleType) {
		case T_Ident:
		{
		//	auto fv = m_Context->FindVar(e.VarName.GetNames()[0]);
			VVar* fv = nullptr;

			if (e.IsArray) {

				fv = m_Context->FindVar(e.VarName.GetNames());
				e.IndexExpression->m_Context = m_Context;
				int idx = e.IndexExpression->Express()->ToInt();
				if (fv->IsType(T_Number) || fv->IsType(T_Int)) {
					stack.push_back(std::to_string(fv->GetArray(idx)->ToInt()));
				}
				else if(fv->IsType(T_FloatNumber) || fv->IsType(T_Float)) {

					stack.push_back(std::to_string(fv->GetArray(idx)->ToFloat()));

				}
				else {
					exit(100);
				}
				int bbb = 5;

				continue;
			}

			fv = m_Context->FindVar(e.VarName.GetNames());
			if (fv->IsType(T_FloatNumber) || fv->IsType(T_Float))
			{
				stack.push_back(std::to_string(fv->ToFloat()));
				//is_float = true;
				//is_int = false;

			}
			else if (fv->IsType(T_Number) || fv->IsType(T_Int))
			{
				stack.push_back(std::to_string(fv->ToInt()));
				//is_int = true;

			}

		}

			break;
		case T_Number:
		case T_Int:
		
			stack.push_back(std::to_string(e.IntValue));

			break;
		case T_FloatNumber:
		case T_Float:
			stack.push_back(std::to_string(e.FloatValue));
			break;
		case T_Operator:

			switch (e.OpType) {
			case T_Plus:
				stack.push_back("+");
				break;
			case T_Times:
				stack.push_back("*");
				break;
			case T_Divide:
				stack.push_back("/");
				break;
			case T_Minus:
				stack.push_back("-");
				break;
			case T_Equal:
				stack.push_back("=");
				break;
			case T_GreaterThan:
				stack.push_back(">");
				break;
			case T_LessThan:
				stack.push_back("<");
				break;
			}

			break;
		}
//		return stack;
	}
	return stack;
}