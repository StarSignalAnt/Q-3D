#pragma once
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>



class MonoHost
{
public:

	MonoHost();
	static MonoHost* m_Instance;
	MonoDomain* GetDomain();

private:
	MonoDomain* m_Domain;

};

