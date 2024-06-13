#include "DynamicLibrary.h"

DynamicLibrary::DynamicLibrary(const char* libName) {
	this->loadLibrary(libName);
}

DynamicLibrary::~DynamicLibrary() {
	this->unloadLibrary();
}

void* DynamicLibrary::getFunction(const char* funcName) const {
#ifdef _WIN32
	FARPROC func = GetProcAddress(this->hModule, funcName);
	return reinterpret_cast<void*>(func);
#else
	void* func = dlsym(this->handle, funcName);
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		return nullptr;
	}
	return reinterpret_cast<void*>(func);
#endif
}

void DynamicLibrary::loadLibrary(const char* libName) {
#ifdef _WIN32
	this->hModule = LoadLibrary(libName);
#else
	this->handle = dlopen(libName, RTLD_LAZY);
#endif
}

void DynamicLibrary::unloadLibrary() {
#ifdef _WIN32
	if (this->hModule) {
		FreeLibrary(this->hModule);
	}
#else
	if (this->handle) {
		dlclose(this->handle);
	}
#endif
}
