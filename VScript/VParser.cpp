#include "pch.h"
#include "VParser.h"
#include "VClass.h"
#include "VVarGroup.h"
#include "VFunction.h"
#include "VDefineParams.h"
#include "VCodeBody.h"
#include "VStatementCall.h"
#include "VCallParameters.h"
#include "VExpression.h"
#include "VVarAssign.h"
#include "VClassAssign.h"
#include "VReturn.h"
#include "VClassCall.h"
#include "VDeclareArray.h"
#include "VAssignArray.h"
#include "VIf.h"
#include "VWhile.h"
#include "VFor.h"
#include "VLambda.h"
#include "VInvoke.h"
#include "VForEach.h"
#include "VEnum.h"
#include "VBreak.h"
#include "VSwitch.h"


VParser::VParser() {

}

VModule* VParser::ParseModule(VTokenStream stream) {

	m_Stream = stream;

	VModule* module = new VModule;

	auto token = m_Stream.GetNext();

	Assert(token.GetType() == T_Module, "Expected 'module' keyword.");

	if (token.GetType() != T_Module) {
		return nullptr;
	}

	auto name = ParseName();

	module->SetName(name);


	while (!m_Stream.End()) {

		auto token = m_Stream.GetNext();
		if (m_Stream.End()) {
			return module;
		}

		switch (token.GetType()) {
		case T_Enum:
		{
			auto p_enum = ParseEnum();
			module->AddEnum(p_enum);
		}
			break;
		case T_Class:
		{
			auto p_class = ParseClass();
			int aa = 1;
			module->AddClass(p_class);
			if (p_class == nullptr) {
				Err("Failed to parse class.", "Class parser returned null.");
				return nullptr;
			}

		}
			break;
		default:
			Err("Error parsing module", "Unexpected token in module:" + token.GetLex());
			return nullptr;
			break;
		}

		int b = 5;

	}

	return module;

}

VName VParser::ParseName() {

	std::vector<std::string> names;

	while (true) {

		auto tok = m_Stream.GetNext();
		if (tok.GetType() == T_Ident) {
			names.push_back(tok.GetLex());
			if (m_Stream.Peek(0).GetType() == T_Access)
			{
				m_Stream.GetNext();
				continue;
			}
			else {
//				m_Stream.Back();
				break;
			}
		}
		else {
			m_Stream.Back();
			break;
		}

	}

	if (names.size() == 0) {

		Err("Parsing Name error", "Name produces empty result.");

	}

	return VName(names);

}

VClass* VParser::ParseClass() {


	auto start = m_Stream.Peek(0).GetPlace();


	auto name = ParseName();

	auto n_cls = new VClass;

	n_cls->SetName(name);

	n_cls->SetStart(start);
	

	if (m_Stream.GetNext().GetType() == T_End) {

		//m_Stream.Back();
		return n_cls;

	}
	else {

		m_Stream.Back();

	}

	auto sub = m_Stream.Peek(0);

	std::string sub_class = "";

	if (sub.GetLex() == ":")
	{
		m_Stream.GetNext();
		sub_class = m_Stream.GetNext().GetLex();
	}

	int bbb = 5;
	n_cls->SetSubClass(sub_class);


	while (!m_Stream.End()) {

 		auto token = m_Stream.GetNext();

		bool is_static = false;
		if (token.GetLex() == "static")
		{
			is_static = true;
			token = m_Stream.GetNext();
		}

		switch (token.GetType()) {
		case T_Func:
		{ 
			int bb = 5;
			VFunction* func = ParseFunction();
			if (is_static) {
				n_cls->AddStaticFunction(func);
			}
			else {
				n_cls->AddFunction(func);
			}
		}
			break;
		case T_End:
		{
			int end = m_Stream.Peek(0).GetPlace();
			if (end == 0) {
				m_Stream.Back();
				end = m_Stream.Peek(0).GetPlace();
				m_Stream.GetNext();
			}
			n_cls->SetEnd(end);
			return n_cls;
		}
			break;
		case T_Ident:
		{

			auto checkt = token.GetType();
			switch (token.GetType()) {
			case T_Int:
			case T_Number:
			case T_Float:
			case T_FloatNumber:
			case T_String:
			case T_Ident:
				break;
			default:
				Err("Parse class field error.", "Unknown type.");
				return nullptr;
			}
			//auto t = m_Stream.GetNext();
			VVarGroup* group = new VVarGroup(TokenType::T_Ident);
			group->SetClassType(token.GetLex());

			n_cls->AddVarGroup(group);

			int pindex = m_Stream.GetIndex();
			int same_c = 0;

			while (!m_Stream.End()) {

				auto peek = m_Stream.Peek(0);
				if (m_Stream.GetIndex() == pindex) {
					same_c++;
					if (same_c >= 2)
					{
						Err("Parse class error", "Endless compile loop.");
						return nullptr;
					}
				}
				pindex = m_Stream.GetIndex();
				
				if (peek.GetLex() == ",")
				{
					m_Stream.GetNext();
					continue;
				}
				if (peek.GetType() == T_EOL) {

					//n_cls->AddVars(
					//)
					m_Stream.GetNext();
					break;

				}
				auto v_name = ParseName();

				if (v_name.GetNames().size() == 0) {

					Err("Parsing class failed dur to mal-formed name.", "Name error");
					return nullptr;

				}

				auto next = m_Stream.Peek(0);

				VExpression* expr = nullptr;

				if (next.GetLex() == "=")
				{
					expr = ParseExpression();

					auto tok = m_Stream.Peek(0);
				}

				group->AddName(v_name, expr);

			}

			int b = 5;
		}
			break;
		case T_Int:
		case T_Float:
		case T_String:
		case T_Bool:
		case T_CObject:
		{
			auto next = m_Stream.Peek(0);

			bool is_array = false;
			if (next.GetLex() == "[") {

				is_array = true;


				//auto siz_ex = ParseExpression();

				int b = 5;

			}

			VVarGroup* group = new VVarGroup(token.GetType());

			if (is_static) {
				n_cls->AddStaticVarGroup(group);
			}
			else {
				n_cls->AddVarGroup(group);
			}
			if (next.GetLex() == "[")
			{
				group->SetSizeExpression(ParseExpression());
			}

			while (!m_Stream.End()) {

				auto peek = m_Stream.Peek(0);

				if (is_array) {

					auto v_name = ParseName();
					group->AddName(v_name, nullptr);
					group->SetArray(true);

					break;


				}

				if (peek.GetLex() == ",")
				{
					m_Stream.GetNext();
					continue;
				}
				if (peek.GetType() == T_EOL) {

					//n_cls->AddVars(
					//)
					m_Stream.GetNext();
					break;

				}
				auto v_name = ParseName();

				if (v_name.GetNames().size() == 0) {

					return nullptr;

				}

				auto next = m_Stream.Peek(0);

				VExpression* expr = nullptr;

				if (next.GetLex() == "=")
				{
					expr = ParseExpression();

					auto tok = m_Stream.Peek(0);
				}

				group->AddName(v_name, expr);

			}
		}
			break;
		default:
			Err("Unknown class compile state.", "Failed to parse class.");
			return nullptr;
		}

		int a = 5;

	}

	m_Stream.Back();

	return n_cls;

	
}

VFunction* VParser::ParseFunction() {

	//auto token = m_Stream.GetNext();

	auto name = ParseName();

	VFunction* func = new VFunction;
	func->SetName(name);

	auto next = m_Stream.GetNext();

	if (next.GetLex() == "(")
	{
		auto next = m_Stream.GetNext();

		if (next.GetType() == T_End)
		{
			return func;
		}

		if (next.GetLex() == ")") {
			
			if (m_Stream.Peek(0).GetLex() == ":")
			{
				m_Stream.GetNext();
				auto guard = ParseExpression();
				func->SetGuard(guard);
				int b = 5;
				//auto tok = m_Stream.Peek(0);
				m_Stream.GetNext();
				int v = 5;
			}
			
			m_Stream.Back();


			auto body = ParseCodeBody();
			func->SetBody(body);
			auto nx = m_Stream.Peek(0);
			if (nx.GetType() == T_End)
			{
				m_Stream.GetNext();
			}
			return func;
		}
		m_Stream.Back();
		auto pars = ParseParameters();

		func->SetParams(pars);
		m_Stream.Back();
		auto body = ParseCodeBody();
		func->SetBody(body);
		auto nx = m_Stream.Peek(0);
		if (nx.GetLex() == ";")
		{
			nx = m_Stream.GetNext();
			nx = m_Stream.GetNext();

		}
		else {
			if (nx.GetType() == T_End)
			{
				m_Stream.GetNext();
			}
		}

		return func;
	}
	else {

		auto pars = ParseParameters();

		func->SetParams(pars);
		int aa = 5;

		auto next = m_Stream.GetNext();
		if (next.GetType() == T_End)
		{
			return func;
		}

	}

	int a = 5;

	return nullptr;
}

PredictType VParser::PredictNext(VTokenStream stream)
{

	bool p_statement = false;
	int idents = 0;
	if (stream.Peek(0).GetLex() == ";")
	{
		stream.GetNext();
	}

	auto st = stream.Peek(0);

	int bb = 5;

	//if(stream.Match({TokenType::}))


	if (stream.Match({ TokenType::T_Return }))
	{

		return P_Return;

	}

	if (stream.Match({ TokenType::T_Ident,TokenType::T_Ident }))
	{
		return P_DeclareObject;
	}
	if (stream.Match({ TokenType::T_Ident,TokenType::T_LeftPara }))
	{
		return P_Statement;
	}

	if (stream.Match({ TokenType::T_Ident,TokenType::T_Access }))
	{
		if (stream.LineHas("=","("))
		{
			return P_ClassAssign;
		}
		else {
			return P_ClassCall;
		}
	}
	if (stream.Match({ TokenType::T_Ident,TokenType::T_Operator }))
	{

		if (m_Stream.Peek(1).GetLex() == "=") {
			return P_Assign;
		}
	}

	if (stream.Match({ TokenType::T_Ident,TokenType::T_LeftBracket }))
	{

		int a = 5;
		return P_AssignArray;

	}

	//if(m_Stream.Match({TokenType::T_Access}))


	while (!stream.End()) {

		auto tok = stream.GetNext();

		switch (tok.GetType()) {
		case T_Break:
			return P_Break;
			
		case T_Switch:
			return P_Switch;
			break;
		case T_ForEach:
			return P_ForEach;
			break;
		case T_Invoke:
			return P_Invoke;
			break;
		case T_For:
			return P_For;
			break;
		case T_While:
			return P_While;
			break;
		case T_If:
			return P_If;
			break;
		case T_Int:
		case T_Float:
		case T_Bool:
		case T_String:
		case T_Lambda:
		case T_CObject:
		case T_List:
		
			tok = stream.Peek(0);

			if (tok.GetLex() == "[")
			{
				return P_DeclareArray;
			}

			return P_DeclareVar;

			break;
		case T_Ident:
		//	p_statement = true;
			idents++;
			if (idents == 1) {

				//if (m_Stream.Peek(0).GetType() == TokenType::T_Ident)
				{
				//	return P_DeclareObject;
				}

			}





			break;
		default:
			if (tok.GetLex() == "(" || tok.GetLex() == "()")
			{
				return P_Statement;
			}
			if (tok.GetLex() == "end" || tok.GetLex()=="else" || tok.GetLex()=="elseif")
			{
				return P_End;
			}
			Err("Could not predict code body element.", "Code body error.");
			return P_Unknown;
			break;
		}

	}

	return P_Unknown;

}

VVarGroup* VParser::ParseDeclareObject() {

	auto tok = m_Stream.GetNext();

	VVarGroup* vg = new VVarGroup(TokenType::T_Class);
	vg->SetClassType(tok.GetLex());

	while (!m_Stream.End()) {

		auto tok = m_Stream.GetNext();
		if (tok.GetLex() == ",")
		{
			continue;
		}
		if (tok.GetLex() == ";")
		{
			return vg;
		}
		auto name = VName();
		name.Add(tok.GetLex());
		
		VExpression* def = nullptr;

		//auto test = m_Stream.Peek(0);
		if (m_Stream.Peek(0).GetLex()=="=") {

			def = ParseExpression();
			

			int b = 5;


		}

		vg->AddName(name,def);


		int b = 5;

	}


	int b = 5;

	return vg;

}

VClassAssign* VParser::ParseClassAssign() {

	//auto tok = m_Stream.GetNext();
	if (m_Stream.Peek(0).GetType() == T_EOL) {
		m_Stream.GetNext();
	}

	auto name = ParseName();

	VClassAssign* ass = new VClassAssign;

	ass->SetName(name);

	auto tok = m_Stream.Peek(0);
	if (tok.GetLex() == "=")
	{
		m_Stream.GetNext();
	}

	auto exp = ParseExpression();

	ass->SetExpression(exp);

	return ass;

}

VCodeBody* VParser::ParseCodeBody() {

	//auto token = m_Stream.GetNext();
	auto tok = m_Stream.Peek(0);
	if (tok.GetLex() == ")")
	{
		m_Stream.GetNext();
	}
	int ba = 5;
	VCodeBody* body = new VCodeBody;
	int pindex = m_Stream.GetIndex() - 1;

	while (!m_Stream.End()) {

		if (m_Stream.Peek(0).GetType() == T_EOL)
		{
			m_Stream.GetNext();
		}

		PredictType pt = PredictNext(m_Stream);

		auto i2 = m_Stream.GetIndex();

		if (i2 == pindex) {

			Err("Parsing code body failed", "Endless parsing.");
			return nullptr;

		}
		else {
			pindex = i2;
		}

		switch (pt) {
		case P_Unknown:

			Err("Compile code body error.", "Unknown code.");
			return nullptr;

			break;

		case P_Break:
		{
			auto br = new VBreak;
			body->AddCode(br);
			m_Stream.GetNext();
			return body;
		}
		break;
		case P_Switch:
		{

			auto ret = ParseSwitch();
			body->AddCode(ret);

		}

			break;
		case P_ForEach:
		{

			auto ret = ParseForEach();
			body->AddCode(ret);
		}

			break;
		case P_Invoke:
		{
			auto ret = ParseInvoke();
			body->AddCode(ret);
		}
			break;
		case P_Lambda:
		{
			auto ret = ParseLambda();
			body->AddCode(ret);
		}
			break;
		case P_For:
		{
			auto ret = ParseFor();
			body->AddCode(ret);
		}
		break;
		case P_While:
		{
			auto ret = ParseWhile();
			body->AddCode(ret);

		}
		break;
		case P_If:
		{
			auto ret = ParseIf();
			body->AddCode(ret);
			if (m_Stream.Peek(0).GetLex() == "end")
			{
				m_Stream.GetNext();
			}
			int b = 5;

		}
			break;
		case P_AssignArray:
		{
			auto ret = ParseAssignArray();
			body->AddCode(ret);
		}
			break;
		case P_DeclareArray:
		{
			auto ret = ParseDeclareArray();
			body->AddCode(ret);
		}

			break;
		case P_Return:
		{

			auto ret = ParseReturn();
			body->AddCode(ret);
			return body;

		}
			break;
		case P_Assign:
		{
			auto ass = ParseClassAssign();
			body->AddCode(ass);
		}
		break;
		case P_ClassAssign:

		{
			auto cls_assign = ParseClassAssign();
			body->AddCode(cls_assign);
		}
			break;
		case P_DeclareObject:
		{
			auto declare = ParseDeclareObject();
			body->AddCode(declare);

		}
			break;
		//case P_Assign:
		{
		//	auto ass = ParseAssign();
		//	body->AddCode(ass);
		}
			break;
		case P_New:

		

			break;
		case P_DeclareVar:
		{
			//auto tok = m_Stream.GetNext();
			auto declare = ParseDeclare();
			body->AddCode(declare);
			int b = 5;
		}
			break;
		case P_Statement:

		{
			//	m_Stream.Back();
			auto statement = ParseStatement();
			body->AddCode(statement);
		}
			break;
		case P_End:

			return body;

			break;
		case P_ClassCall:
		{
			auto call = ParseClassCall();
			body->AddCode(call);
		}
			break;
		}

		if (!m_Stream.End()) {
			auto t = m_Stream.Peek(1);
			if (t.GetLex() == ":")
			{
				m_Stream.GetNext();
				m_Stream.GetNext();
				auto action = body->GetCode()[body->GetCode().size() - 1];
				action->SetGuard(ParseExpression());
			}
 			int a = 5;
		}

		int a = 5;

	}
	int b = 5;

	return nullptr;
}

VForEach* VParser::ParseForEach() {

	auto tok = m_Stream.GetNext();

	tok = m_Stream.GetNext();

	auto name = ParseName();

	if (m_Stream.GetNext().GetLex() != "in")
	{
		printf("Expected 'in'\n");
	}

	auto list_name = ParseName();

	VForEach* fe = new VForEach();

	fe->SetTarget(name);
	fe->SetList(list_name);
	fe->SetCode(ParseCodeBody());

	//tok = m_Stream.GetNext();


	return fe;

}

VInvoke* VParser::ParseInvoke()
{

	auto token = m_Stream.GetNext();

	VInvoke* invoke = new VInvoke;
	invoke->SetTarget(ParseName());

	return invoke;

}

VLambda* VParser::ParseLambda() {

	//auto token = m_Stream.GetNext();

	VLambda* lam = new VLambda;
	
	auto body = ParseCodeBody();
	lam->SetBody(body);

	return lam;

}

VFor* VParser::ParseFor() {

	auto toke = m_Stream.GetNext();
	toke = m_Stream.GetNext();

	VFor* vfor = new VFor;

	vfor->SetInitial(ParseAssign());
	vfor->SetExpression(ParseExpression());
	if (m_Stream.Peek(0).GetLex() == ";") {
		toke = m_Stream.GetNext();
	}
	vfor->SetInc(ParseAssign());
	vfor->SetCode(ParseCodeBody());

	return vfor;

}

VWhile* VParser::ParseWhile() {

	auto tok = m_Stream.GetNext();
	auto res = new VWhile;
	m_Stream.GetNext();
	res->SetExpression(ParseExpression());
	res->SetCode(ParseCodeBody());
	return res;

}

VIf* VParser::ParseIf() {

	auto toke = m_Stream.GetNext();

	auto res = new VIf;

	if (toke.GetLex() == "if" || toke.GetLex()=="elseif") {
	//	toke = m_Stream.GetNext();
	}
	else {
		m_Stream.ToPrev("if");
		if (m_Stream.Peek(0).GetLex() != "if")
		{
			m_Stream.ToNext("if");
		}
		//	m_Stream.Back();
	}

	m_Stream.GetNext();

	res->SetIfExp(ParseExpression());

	toke = m_Stream.GetNext();
	//toke = m_Stream.GetNext();

	res->SetBody(ParseCodeBody());

	toke = m_Stream.Peek(0);

	if (toke.GetLex() == "end")
	{
		m_Stream.GetNext();
	}

	if (toke.GetLex() == "elseif")
	{
		//m_Stream.GetNext();

		res->SetElseIf(ParseIf());
	}

	if (toke.GetLex() == "else")
	{
		m_Stream.GetNext();
		res->SetElseBody(ParseCodeBody());

	}
	else {

	
	}

	int b = 5;

	return res;

}

VAssignArray* VParser::ParseAssignArray() {

	auto name = ParseName();

	VAssignArray* ass = new VAssignArray();
	
	auto n = m_Stream.GetNext();

	if (n.GetLex() != "[")
	{
		Err("Parsing Assign Array","Expecting '['");
	}

	ass->SetTarget(name);

	ass->SetIndexExpression(ParseExpression());
	
	n = m_Stream.GetNext();
	
	ass->SetExpression(ParseExpression());


	return ass;

}

VDeclareArray* VParser::ParseDeclareArray() {

	auto token = m_Stream.GetNext();

	VDeclareArray* res = new VDeclareArray();

	res->SetType(token.GetType());

	m_Stream.GetNext();

	res->SetSizeExpression(ParseExpression());

	auto name = ParseName();

	res->SetName(name);

	return res;
}

VReturn* VParser::ParseReturn() {

	auto tok = m_Stream.GetNext();
	if (tok.GetType() != TokenType::T_Return)
	{
		Err("Error parsing Return","Expecting 'return'");
	}




	VReturn* res = new VReturn;


	if (m_Stream.Peek(0).GetLex() != ";") {
		res->SetExpression(ParseExpression());

		tok = m_Stream.GetNext();
		int v = 5;
		return res;
	}
	else {
		return res;
	}
}

VClassCall* VParser::ParseClassCall() {

	if (m_Stream.Peek(0).GetType() == T_EOL) {
		m_Stream.GetNext();
	}

	VClassCall* call = new VClassCall;
	//auto tok = m_Stream.GetNext();
	auto name = ParseName();
	call->SetName(name);
	m_Stream.GetNext();
	call->SetParams(ParseCallParameters());
	//auto tok = m_Stream.GetNext();
	//m_Stream.ToNext(";");
	if (m_Stream.Peek(0).GetLex() == ")")
	{
		//m_Stream.GetNext();
	}
	int a = 5;

	return call;

}

VVarAssign* VParser::ParseAssign() {

	auto name = ParseName();

	if (!m_Stream.Peek(0).GetType() == T_Equal)
	{

		Err("Error parsing Assign","Expecting '='\n");

	}

	VVarAssign* ass = new VVarAssign;

	ass->SetExpression(ParseExpression());
	ass->SetTarget(name);

//	ass
	auto tok = m_Stream.GetNext();

	int aa = 5;



	return ass;

}

VVarGroup* VParser::ParseDeclare() {

	VVarGroup* group = new VVarGroup(m_Stream.GetNext().GetType());

	auto type = group->GetType();

	while (!m_Stream.End()) {

		auto tok = m_Stream.GetNext();
		if (tok.GetLex() == ",")
		{
			continue;
		}
		if (tok.GetLex() == ";")
		{
			return group;
		}
		if (tok.GetLex() == "end")
		{
			return group;
		}

		auto name = VName();
		name.Add(tok.GetLex());
	
		tok = m_Stream.Peek(0);

		if (type == T_Lambda) {

			group->SetLambda(ParseLambda());
			group->AddName(name, nullptr);

		}
		else {

			VExpression* exp = nullptr;

			if (tok.GetLex() == "=")
			{
				m_Stream.GetNext();
				exp = ParseExpression();
			}

			group->AddName(name, exp);
			int b = 5;

		}
	}

	return group;

}

VStatementCall* VParser::ParseStatement() {

	auto tok = m_Stream.GetNext();
	if (tok.GetLex() == ";")
	{
		tok = m_Stream.GetNext();
	}

	if (tok.GetLex() == "(")
	{
		//m_Stream.Back();
		//m_Stream.Back();
		//tok = m_Stream.GetNext();
	}

	VStatementCall* call = new VStatementCall;
	call->SetName(tok.GetLex());

	tok = m_Stream.GetNext();
	auto tok2 = m_Stream.Peek(0);

	if (tok.GetLex() == "(" && tok2.GetLex() == ")") {

	//	m_Stream.GetNext();
		//call->SetName(m_Stream.GetNext().GetLex());

		auto el = m_Stream.Peek(0);
		if (el.GetLex() == ")")
		{
			el = m_Stream.Peek(1);
		}
		if (el.GetType() == T_RightBracket) {
			m_Stream.GetNext();
			m_Stream.GetNext();
			return call;

		}

		if (el.GetType() == T_EOL)
		{
			m_Stream.GetNext();
			m_Stream.GetNext();
			return call;
		}
		else {
			auto c2 = m_Stream.GetNext();

			return call;
			//	Err("Unknown follow up to statement call.\n");
		}

	}
	else {

	//	tok2 = m_Stream.GetNext();

		auto call_params = ParseCallParameters();

		call->SetCallParameters(call_params);
		return call;

		//int aa = 5;

	}

	int a = 5;

	return nullptr;

}

VCallParameters* VParser::ParseCallParameters() {

	VCallParameters* params = new VCallParameters;
	//auto tok = m_Stream.GetNext();
	while (!m_Stream.End()) {

		auto token = m_Stream.Peek(0);

		if (token.GetLex() == ")" || token.GetLex() == ";")
		{
			if (token.GetLex() == ";")
			{
				int aa = 5;
				//m_Stream.GetNext();
			}
			m_Stream.GetNext();
			return params;
		}
		

		VExpression* expression = ParseExpression();

		params->AddParam(expression);



	}

	int b = 5;

	return nullptr;
}

VExpression* VParser::ParseExpression() {

	//auto toke = m_Stream.GetNext();
	if (m_Stream.Peek(0).GetLex() == "=") {
		m_Stream.GetNext();
	}

	VExpression* expr = new VExpression;

	VName v_name;

	while (!m_Stream.End()) {

		auto toke = m_Stream.GetNext();
		if (toke.GetLex() == "]")
		{
			return expr;
		}
		if (toke.GetLex() == ",")
		{
			return expr;
		}
		if (toke.GetLex() == ")")
		{
			m_Stream.Back();
			return expr;
		}
		if (toke.GetLex() == ";")
		{
			m_Stream.Back();
			return expr;
		}
		if (toke.GetLex() == ":")
		{
			m_Stream.Back();
			return expr;
		}
		if (toke.GetLex() == "(")
		{
			ExpElement sub_ele;
			sub_ele.IsSubExpr = true;
			sub_ele.SubExpr = ParseExpression();
			expr->Elements.push_back(sub_ele);
			m_Stream.GetNext();

			continue;
		}

		if (toke.GetLex() == "new")
		{
			toke = m_Stream.GetNext();
			auto cls_type = toke.GetLex();
			int bb = 5;
			ExpElement new_ele;
			new_ele.ClassType = cls_type;
			new_ele.IsNew = true;
			m_Stream.GetNext();
			new_ele.NewParams = ParseCallParameters();
		//	m_Stream.Back();
			m_Stream.ToNext(";");
			expr->Elements.push_back(new_ele);
			return expr;


		}

		switch (toke.GetType()) {
		case T_Operator:
		{
			ExpElement op_ele;
			op_ele.EleType = T_Operator;
			if (toke.GetLex() == "+")
			{
				op_ele.OpType = T_Plus;
			}
			if (toke.GetLex() == "-")
			{
				op_ele.OpType = T_Minus;
			}
			if (toke.GetLex() == "*")
			{
				op_ele.OpType = T_Times;
			}
			if (toke.GetLex() == "/")
			{
				op_ele.OpType = T_Divide;
			}
			if (toke.GetLex() == "=") {
				op_ele.OpType = T_Equal;
			}
			if (toke.GetLex() == "<") {
				op_ele.OpType = T_LessThan;
			}
			if (toke.GetLex() == ">") {
				op_ele.OpType = T_GreaterThan;
			}
			expr->Elements.push_back(op_ele);
		}

			break;
		case T_Ident:
		{

			ExpElement var_ele;

			m_Stream.Back();

			VName qname = ParseName();

			auto next = m_Stream.GetNext();
			
			if (next.GetLex() == "(")
			{
				if (qname.GetNames().size() == 1) {
					m_Stream.Back();
					for (int i = 0; i < qname.GetNames().size(); i++)
					{
						m_Stream.Back();
					}
					//next = m_Stream.GetNext();
					var_ele.Statement = ParseStatement();
					var_ele.EleType = T_Func;
					int bb = 5;
					m_Stream.Back();
				}
				else {
					m_Stream.Back();
					for (int i = 0; i < qname.GetNames().size(); i++)
					{
						m_Stream.Back();
					}
					m_Stream.Back();
					if (m_Stream.Peek(0).GetLex() == ".")
					{
						m_Stream.Back();
					}
					var_ele.ClassCall = ParseClassCall();
					var_ele.EleType = T_Func;
					int bb = 5;
					//m_Stream.Back();
				}
			}
			else {

				//qname.Add(toke.GetLex());
				var_ele.VarName = qname;
				if (next.GetLex() == "[")
				{
					int b = 5;
					var_ele.IndexExpression = ParseExpression();
					var_ele.IsArray = true;
					//next = m_Stream.GetNext();
				}

				var_ele.EleType = T_Ident;
				m_Stream.Back();
			}

			expr->Elements.push_back(var_ele);


		}
			break;
		case T_Number:
		{
			ExpElement num_ele;

			num_ele.EleType = T_Number;
			num_ele.IntValue = std::stoi(toke.GetLex());

			expr->Elements.push_back(num_ele);
			 
		}

			break;
		case T_FloatNumber:
		{
			ExpElement num_ele;

			num_ele.FloatValue = std::stof(toke.GetLex());
			num_ele.EleType = T_FloatNumber;

			expr->Elements.push_back(num_ele);
		}
			break;
		case T_String:
		{
			ExpElement str_ele;
			str_ele.StringValue = toke.GetLex();
			str_ele.EleType = T_String;
			expr->Elements.push_back(str_ele);
			int bb = 5;
		}
			break;
		default:

			if (toke.GetLex() == ")")
			{
			
			
					return expr;
			
			}
			if (toke.GetLex() == ";")
			{
				m_Stream.Back();
				return expr;
			}

			break;
		}

		int b = 5;

	}

	return nullptr;
}

VDefineParams* VParser::ParseParameters() {

	VDefineParams* params = new VDefineParams;

	while (!m_Stream.End()) {
	
		auto token = m_Stream.GetNext();
		if (token.GetLex() == ",")
		{
			continue;
		}
		if (token.GetLex() == ")")
		{
			return params;
		}
		bool cls = false;
		VParameter* new_param = new VParameter;
		if (token.GetType() == T_Ident) {

			cls = true;
			int b = 5;
		}
		auto name = ParseName();
		
		if (m_Stream.Peek(0).GetLex() == "=")
		{

		}

	
		if (cls) {
			new_param->SetType(T_Class);
			new_param->SetClassType(token.GetLex());

		}
		else {
			new_param->SetType(token.GetType());
		}
		new_param->SetName(name);



		params->AddParam(new_param);

	}
	int b = 5;

	return nullptr;

}

VEnum* VParser::ParseEnum() {

	auto tok = m_Stream.GetNext();


	int b = 5;

	VEnum* e = new VEnum;

	e->SetName(tok.GetLex());

	int value = 0;
	while (!m_Stream.End()) {

		auto v = m_Stream.GetNext();

		if (v.GetLex() == ",")
		{
			continue;
		}
		if (v.GetLex() == ";")
		{
			continue;
		}
		if (v.GetLex() == "end")
		{
			return e;
		}

		if(m_Stream.Peek(0).GetLex()=="=")
		{
			m_Stream.GetNext();
			auto val = m_Stream.GetNext();
			value = std::stoi(val.GetLex());
		}


		e->AddValue(v.GetLex(), value);



		int a = 5;
		value++;

	}

	return e;

}


VSwitch* VParser::ParseSwitch() {
	

	auto tok = m_Stream.GetNext();

	if (tok.GetLex() != "switch") {
		m_Stream.ToNext("switch");
		tok = m_Stream.GetNext();
	}



	//tok = m_Stream.GetNext();
	tok = m_Stream.GetNext();

	auto check = ParseExpression();

	m_Stream.GetNext();

	VSwitch* sw = new VSwitch;

	sw->SetCheck(check);

	while (!m_Stream.End()) {

		tok = m_Stream.GetNext();

		if (tok.GetLex() == "case")
		{

			auto expr = ParseExpression();

			tok = m_Stream.GetNext();

			auto code = ParseCodeBody();

			sw->AddValue(code, expr);

		

		}

		if (tok.GetLex() == "end")
		{
			return sw;
		}

		int a = 5;

	}

	int b = 5;

	return sw;


}

void VParser::Err(std::string type,std::string msg) {

	printf("Error parsing.\n");
	printf("Type:");
	printf(type.c_str());
	printf(" Error:");
	printf(msg.c_str());
	printf("\n");
	Print(type, msg);



}

void VParser::Assert(bool value, std::string msg)
{
	if (!value) {
		Err("Error in assert",msg);
		return;
	}
}