#pragma once

#include <map>
#include <string>
#include <mutex>

class ModuleList final {
public:
	ModuleList() = default;

	using Data = std::map<std::string, std::string>;

	void insert(const std::string& host, const std::string& modulePath);
	void remove(const std::string& host);

	const Data getCopy() const;

private:
	Data moduleMap;
	mutable std::mutex dataMutex;
};

