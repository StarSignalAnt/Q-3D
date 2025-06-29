#include "MonoLib.h"
#include <mono/metadata/metadata.h> // For mono_image_get_table_rows and other metadata functions
#include <stdexcept>                 // For std::runtime_error
#include <iostream>                  // For std::cerr

MonoLib::MonoLib(const std::string& dllDirectory) {
    // Construct the full paths to the DLLs
    std::string game_dll_path = dllDirectory + "game.dll";
    std::string vivid_dll_path = dllDirectory + "qnet.dll";

    MonoHost* host = MonoHost::GetInstance();
    MonoDomain* gameDomain = host->GetGameDomain(); // Get the one and only game domain

    if (!gameDomain) {
        throw std::runtime_error("MonoLib Error: Game domain has not been created! Call MonoHost::CreateGameDomain first.");
    }

    // --- FIX: Load the ENGINE assembly (qnet.dll) into the GAME DOMAIN ---
    std::cout << "Loading " << vivid_dll_path << " into Game Domain." << std::endl;
    m_Vivid = new MAsm(gameDomain, vivid_dll_path);

    // --- FIX: Load the GAME assembly (game.dll) into the SAME GAME DOMAIN ---
    std::cout << "Loading " << game_dll_path << " into Game Domain." << std::endl;
    m_Assembly = new MAsm(gameDomain, game_dll_path);
}

MonoLib::~MonoLib() {
    delete m_Assembly;
    delete m_Vivid;
    m_Assembly = nullptr;
    m_Vivid = nullptr;
}

std::vector<SharpClassInfo> MonoLib::GetClasses() {
    std::vector<SharpClassInfo> classList;

    // This correctly gets the image from m_Assembly (game.dll in the game domain)
    MonoImage* image = m_Assembly->GetAsm() ? mono_assembly_get_image(m_Assembly->GetAsm()) : nullptr;
    if (!image) {
        std::cerr << "Cannot get classes: MonoImage from game assembly is null." << std::endl;
        return classList;
    }

    const MonoTableInfo* table = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int numTypes = mono_image_get_table_rows(image, MONO_TABLE_TYPEDEF);

    // Iterate through all types defined in the assembly's metadata
    for (int i = 1; i < numTypes; ++i) { // Start at 1, row 0 is the <Module> type
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(table, i, cols, MONO_TYPEDEF_SIZE);

        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);

        std::string className = name ? name : "";
        std::string namespaceName = name_space ? name_space : "";
        std::string baseClassName = "";

        // To get the base class, we need to get the MonoClass representation
        MonoClass* klass = mono_class_from_name(image, namespaceName.c_str(), className.c_str());
        if (klass) {
            MonoClass* parent = mono_class_get_parent(klass);
            if (parent && parent != mono_get_object_class()) { // Ensure it's not System.Object
                const char* parentName = mono_class_get_name(parent);
                const char* parentNs = mono_class_get_namespace(parent);

                if (parentName) {
                    // Concatenate namespace and name for the full base class name
                    baseClassName = (parentNs && *parentNs)
                        ? std::string(parentNs) + "." + parentName
                        : parentName;
                }
            }
        }

        classList.push_back({ namespaceName, className, baseClassName });
    }

    return classList;
}

MClass* MonoLib::GetClass(const std::string& name) {
    // This will correctly ask m_Assembly (which wraps game.dll) for the class
    if (!m_Assembly) {
        return nullptr;
    }
    // Assuming your scripts are in a "Vivid" namespace.
    // Modify as needed if the namespace is different.
    return m_Assembly->GetClass("QNet", name);
}