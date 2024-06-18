#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class DynamicLibrary final {
public:
    DynamicLibrary(const char* libName);
    ~DynamicLibrary();

    void* getFunction(const char* funcName) const;
    bool isValid() const;

private:
#ifdef _WIN32
    HMODULE hModule;
#else
    void* handle;
#endif

    void loadLibrary(const char* libName);
    void unloadLibrary();
};
