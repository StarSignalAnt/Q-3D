#include "MClass.h"
#include <iostream>
MClass::MClass(MonoClass* cls) {

	m_Class = cls;

}

MClass::MClass(MonoClass* cls, MonoObject* obj) {

    m_Class = cls;
    m_Instance = obj;

}

MClass* MClass::CreateInstance() {

    MonoObject* instance = mono_object_new(mono_domain_get(),m_Class);
    mono_runtime_object_init(instance);
	MClass* mInstance = new MClass(m_Class,instance);

    return mInstance;



}

/*
void MClass::Call(std::string func, Args&&... args) {

	MonoMethod* func1 = mono_class_get_method_from_name(m_Class,func.c_str(),pars);
	
	int a = 3;
	int b = 4;
	void* args[2] = { &a, &b };
	mono_runtime_invoke(func1, NULL,args, NULL);

}
*/




