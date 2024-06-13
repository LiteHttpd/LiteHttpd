#pragma once

#include "DynamicLibrary.h"

#include <LiteHttpdDev.h>
#include <string>
#include <memory>

class ModuleHolder final {
public:
	ModuleHolder() = delete;
	ModuleHolder(const std::string& path);
	~ModuleHolder();

	ModuleBase* getModule() const;

private:
	std::unique_ptr<DynamicLibrary> library = nullptr;
	using ModuleCreateFunc = ModuleBase*(*)(void);
	using ModuleDestoryFunc = void(*)(ModuleBase*);
	ModuleCreateFunc createFunc = nullptr;
	ModuleDestoryFunc destoryFunc = nullptr;

	ModuleBase* module = nullptr;
};
