#include "ModuleList.h"

void ModuleList::insert(const std::string& host, const std::string& modulePath) {
	std::lock_guard locker(this->dataMutex);
	this->moduleMap.insert({ host, modulePath });
}

void ModuleList::remove(const std::string& host) {
	std::lock_guard locker(this->dataMutex);
	this->moduleMap.erase(host);
}

const ModuleList::Data ModuleList::getCopy() const {
	std::lock_guard locker(this->dataMutex);
	return this->moduleMap;
}
