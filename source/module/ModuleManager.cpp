#include "ModuleManager.h"

void ModuleManager::load(const std::string& host, const std::string& path) {
	std::lock_guard locker(this->lock);

	auto moduleHolder = std::make_unique<ModuleHolder>(path);
	if (moduleHolder->getModule()) {
		this->modules.insert({ host,
			std::shared_ptr<ModuleHolder>{ moduleHolder.release() } });
	}
}

ModuleBase* ModuleManager::get(const std::string& host) const {
	std::lock_guard locker(this->lock);

	auto it = this->modules.find(host);
	if (it != this->modules.end()) {
		return it->second->getModule();
	}

	return nullptr;
}

ModuleManager* ModuleManager::getInstance() {
	return ModuleManager::instance ? ModuleManager::instance
		: (ModuleManager::instance = new ModuleManager{});
}

void ModuleManager::releaseInstance() {
	if (ModuleManager::instance) {
		delete ModuleManager::instance;
		ModuleManager::instance = nullptr;
	}
}

ModuleManager* ModuleManager::instance = nullptr;
