#include "MonoLib.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/metadata.h>
#include <iostream>
#include <vector>
#include <string>


MonoLib::MonoLib(std::string dll)
{

    std::string full = dll + "game.dll";
    std::string vivid = dll + "vividengine.dll";

    m_Assembly = new MAsm(full, dll);
    m_Vivid = new MAsm(vivid, dll);


	//	m_handle = LoadLibraryA(m_path.c_str());
//	if (!m_handle) {
//		throw std::runtime_error("Failed to load library: " + m_path);
//	}
}

std::vector<SharpClassInfo> MonoLib::GetClasses() {

    std::vector<SharpClassInfo> classList;

    MonoImage* image = mono_assembly_get_image(m_Assembly->GetAsm());
    int numTypes = mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF);

    for (int i = 1; i < numTypes; ++i)  // Row 0 is reserved
    {
        const MonoTableInfo* table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table, i, cols, MONO_TYPEDEF_SIZE);

        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);

        std::string className = name ? name : "";
        std::string namespaceName = name_space ? name_space : "";
        std::string baseClassName = "";

        // Get MonoClass*
        MonoClass* klass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
        if (klass)
        {
            MonoClass* parent = mono_class_get_parent(klass);
            if (parent)
            {
                const char* parentName = mono_class_get_name(parent);
                const char* parentNs = mono_class_get_namespace(parent);

                if (parentName)
                    baseClassName = (parentNs && *parentNs)
                    ? std::string(parentNs) + "." + parentName
                    : parentName;
            }
        }

        classList.push_back({ namespaceName, className, baseClassName });
    }

    return classList;

}

MClass* MonoLib::GetClass(std::string name) {

    //m_StaticClass = m_Assembly->GetClass("Vivid", name);

    return m_Assembly->GetClass("Vivid", name);

   

}