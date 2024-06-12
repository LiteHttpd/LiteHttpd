#pragma once

#include <map>
#include <string>
#include <mutex>

class CerList final {
public:
	CerList() = default;

	struct CerData final {
		std::string key;
		std::string cer;
	};
	using Data = std::map<std::string, CerData>;

	void insert(const std::string& host, const std::string& keyPath, const std::string& cerPath);
	void remove(const std::string& host);

	const Data getCopy() const;

private:
	Data cerMap;
	mutable std::mutex dataMutex;
};
