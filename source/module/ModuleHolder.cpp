#include "ModuleHolder.h"
#include "log/Logger.h"

ModuleHolder::ModuleHolder(const std::string& path) {
	Logger::info("Create module holder for: " + path);

	/** Load Library */
	this->library = std::make_unique<DynamicLibrary>(path.c_str());
	if (!this->library->isValid()) {
		Logger::error("Can't load module for: " + path);
	}

	/** Get Create And Destory Function */
	this->createFunc = reinterpret_cast<ModuleCreateFunc>(
		this->library->getFunction(LITEHTTPD_MODULE_CREATE_FUNCTION_NAME));
	this->destoryFunc = reinterpret_cast<ModuleDestoryFunc>(
		this->library->getFunction(LITEHTTPD_MODULE_DESTORY_FUNCTION_NAME));

	/** Create Module */
	if (this->createFunc && this->destoryFunc) {
		this->module = this->createFunc();
	}
	else {
		Logger::error("Can't create module object for: " + path);
	}
}

ModuleHolder::~ModuleHolder() {
	/** Destory Module */
	if (this->createFunc && this->destoryFunc) {
		if (this->module) {
			this->destoryFunc(this->module);
		}
	}
}

ModuleBase* ModuleHolder::getModule() const {
	return this->module;
}
