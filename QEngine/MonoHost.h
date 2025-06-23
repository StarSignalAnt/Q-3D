#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>


class MonoHost
{
public:

    static MonoHost* GetInstance();

    // Delete copy constructor and assignment operator
    MonoHost(const MonoHost&) = delete;
    void operator=(const MonoHost&) = delete;

    // Functions to manage the game domain
    void CreateGameDomain();
    void UnloadGameDomain();
    MonoDomain* GetGameDomain() const;
    MonoDomain* GetRootDomain() const;
private:
    MonoHost();
    ~MonoHost(); // Add a destructor to clean up

    static MonoHost* s_instance;
    MonoDomain* m_rootDomain;
    MonoDomain* m_gameDomain;
};

