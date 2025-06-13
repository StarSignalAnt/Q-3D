#include "MonoHost.h"

#include <iostream>

MonoHost* MonoHost::m_Instance = nullptr;

MonoHost::MonoHost() {
    m_Instance = this;
    const char* domain_name = "VividEngineMono";
    mono_set_dirs("./mono/lib", "./mono/etc");
    mono_config_parse(NULL);
    m_Domain = mono_jit_init(domain_name);
    if (!m_Domain)

    {
        std::cerr << "Failed to initialize Mono JIT." << std::endl;
        return;
    }

    /*
   // MonoAssembly* assembly = mono_domain_assembly_open(domain, "scripts/TestLib.dll");
    //if (!assembly) {
        std::cout << "Failed to load TestAssembly.dll" << std::endl;
        std::cout << "Make sure TestAssembly.dll is in your working directory" << std::endl;
        mono_jit_cleanup(domain);
        return;
    }

    MonoImage* image = mono_assembly_get_image(assembly);

    // Get the class
    MonoClass* testClass = mono_class_from_name(image, "TestLib", "Test");
    if (!testClass) {
        std::cout << "Failed to find TestClass" << std::endl;
        mono_jit_cleanup(domain);
        return;
    }

    MonoMethod* sayHelloMethod = mono_class_get_method_from_name(testClass, "Init", 0);
    if (!sayHelloMethod) {
        std::cout << "Failed to find SayHello method" << std::endl;
        mono_jit_cleanup(domain);
        return;
    }


    mono_runtime_invoke(sayHelloMethod, NULL, NULL, NULL);
    */
}

MonoDomain* MonoHost::GetDomain() {
    return m_Domain;
}