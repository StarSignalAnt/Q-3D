#include <iostream>
#include <string>
#include <vector>
#include <windows.h> // Required for DLL handling functions

// Define a function pointer type that matches the signature of "InitDLL".
// This makes the code cleaner and easier to read.
// The function takes no arguments and returns void.
typedef void(*InitDLLFunc)();

/**
 * @class DllImporter
 * @brief Manages the loading and unloading of a DLL and provides a way to call functions from it.
 *
 * This class encapsulates the Windows API calls for dynamic library loading.
 * It handles loading a DLL from a given path, retrieving a specific function,
 * and ensuring the library is freed when the object is destroyed.
 */
class DllImporter
{
public:
    /**
     * @brief Constructor that takes the path to the DLL to be loaded.
     * @param dllPath The full or relative path to the DLL file as a standard string.
     */
    DllImporter(const std::string& dllPath)
        : m_dllPath(dllPath), m_hModule(NULL) // Initialize members
    {
        std::cout << "DllImporter created for path: " << m_dllPath << std::endl;
    }

    /**
     * @brief Destructor that ensures the loaded DLL is freed.
     */
    ~DllImporter()
    {
        // Check if the module handle is valid before trying to free it.
        if (m_hModule)
        {
            FreeLibrary(m_hModule);
            m_hModule = NULL;
            std::cout << "DLL unloaded: " << m_dllPath << std::endl;
        }
    }

    /**
     * @brief Loads the DLL and calls the "InitDLL" function.
     * @return True if the DLL was loaded and the function was called successfully, false otherwise.
     */
    bool LoadAndInit()
    {
        // Convert the std::string path to a std::wstring, which is required by LoadLibraryW.
        // This ensures Unicode paths are handled correctly.
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, m_dllPath.c_str(), (int)m_dllPath.size(), NULL, 0);
        std::wstring wstrPath(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, m_dllPath.c_str(), (int)m_dllPath.size(), &wstrPath[0], size_needed);

        // Use LoadLibraryW for wide-character string paths.
        m_hModule = LoadLibraryW(wstrPath.c_str());

        // Check if the DLL was loaded successfully.
        if (!m_hModule)
        {
            std::cerr << "Error: Could not load the DLL from " << m_dllPath << std::endl;
            std::cerr << "Windows Error Code: " << GetLastError() << std::endl;
            return false;
        }

        std::cout << "Successfully loaded DLL: " << m_dllPath << std::endl;

        // Get the address of the exported "InitDLL" function from the loaded module.
        // The second argument must be the exact name of the C-style exported function.
        InitDLLFunc initFunc = (InitDLLFunc)GetProcAddress(m_hModule, "InitDLL");

        // Check if the function address was retrieved successfully.
        if (!initFunc)
        {
            std::cerr << "Error: Could not find the 'InitDLL' function in the DLL." << std::endl;
            return false;
        }

        std::cout << "Found 'InitDLL' function. Calling it now..." << std::endl;

        // Call the function from the DLL.
        initFunc();

        std::cout << "'InitDLL' function executed." << std::endl;
        return true;
    }

private:
    // The path to the dynamic link library.
    std::string m_dllPath;

    // A handle to the loaded module (DLL).
    HMODULE m_hModule;
};
