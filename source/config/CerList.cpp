#include "CerList.h"

void CerList::insert(const std::string& host,
	const std::string& keyPath, const std::string& cerPath) {
	std::lock_guard locker(this->dataMutex);
	this->cerMap.insert({ host, { keyPath, cerPath } });
}

void CerList::remove(const std::string& host) {
	std::lock_guard locker(this->dataMutex);
	this->cerMap.erase(host);
}

const CerList::Data CerList::getCopy() const {
	std::lock_guard locker(this->dataMutex);
	return this->cerMap;
}
