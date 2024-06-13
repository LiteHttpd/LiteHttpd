#include "ModuleHolder.h"

ModuleHolder::ModuleHolder(const std::string& path) {
	/** Load Library */
	this->library = std::make_unique<DynamicLibrary>(path.c_str());

	/** Get Create And Destory Function */
	this->createFunc = reinterpret_cast<ModuleCreateFunc>(
		this->library->getFunction(LITEHTTPD_MODULE_CREATE_FUNCTION_NAME));
	this->destoryFunc = reinterpret_cast<ModuleDestoryFunc>(
		this->library->getFunction(LITEHTTPD_MODULE_DESTORY_FUNCTION_NAME));

	/** Create Module */
	if (this->createFunc && this->destoryFunc) {
		this->module = this->createFunc();
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
