#include "MonoHost.h"
#include <iostream>

// Initialize static instance pointer
MonoHost* MonoHost::s_instance = nullptr;

MonoHost* MonoHost::GetInstance() {
    if (!s_instance) {
        s_instance = new MonoHost();
    }
    return s_instance;
}

MonoHost::MonoHost() : m_rootDomain(nullptr), m_gameDomain(nullptr) {
    // Set the path to the Mono lib and etc directories
    mono_set_dirs("C:\\Program Files\\Mono\\lib", "C:\\Program Files\\Mono\\etc");

    mono_config_parse(NULL);
    //mono_debug_init(MONO_DEBUG_FORMAT_MONO);

    // Initialize the Mono JIT runtime and create the root domain
    m_rootDomain = mono_jit_init("VividEngineRoot");
    if (!m_rootDomain) {
        std::cerr << "FATAL: Failed to initialize Mono JIT." << std::endl;
    }
    const char* options[] = {
       "--debug",
       "--soft-breakpoints" // Optional: helps with non-crashing exceptions
    };
    // The first argument is the number of options.
   // mono_jit_parse_options(2, (char**)options);
}

MonoHost::~MonoHost() {
    // Ensure the game domain is unloaded before we shut down
    UnloadGameDomain();

    if (m_rootDomain) {
        // Clean up the JIT runtime
        mono_jit_cleanup(m_rootDomain);
        m_rootDomain = nullptr;
    }
}

void MonoHost::CreateGameDomain() {
    if (m_gameDomain) {
        std::cerr << "Warning: CreateGameDomain called while a game domain already exists. Unloading old one first." << std::endl;
        UnloadGameDomain();
    }

    // Create a new, separate app domain for the game logic
    m_gameDomain = mono_domain_create_appdomain((char*)"VividGameDomain", nullptr);
    if (!m_gameDomain) {
        std::cerr << "FATAL: Failed to create game domain." << std::endl;
        return;
    }

    // IMPORTANT: Set the new domain as the current one. All subsequent
    // assembly loads will happen in this domain until it's changed.
    if (!mono_domain_set(m_gameDomain, false)) {
        std::cerr << "FATAL: Failed to set the current domain." << std::endl;
    }
}

void MonoHost::UnloadGameDomain() {
    if (m_gameDomain) {
        // Set the current domain back to the root domain before unloading the game one
        mono_domain_set(m_rootDomain, false);

        // This function safely unloads the domain and all assemblies within it
        mono_domain_unload(m_gameDomain);
        m_gameDomain = nullptr;
        std::cout << "Game Domain has been unloaded." << std::endl;
    }
}

MonoDomain* MonoHost::GetGameDomain() const {
    return m_gameDomain;
}

MonoDomain* MonoHost::GetRootDomain() const {
    return m_rootDomain;
}