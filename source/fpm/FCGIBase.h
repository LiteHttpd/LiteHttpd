#pragma once

#include <vector>
#include <string>
#include <map>

class FCGIBase final {
public:
	FCGIBase();
	~FCGIBase();

	using ParamList = std::map<std::string, std::string>;
	using Result = std::tuple<std::vector<char>, bool>;
	const Result sendData(
		const std::string& addr, uint16_t port,
		const ParamList& params, const std::vector<char>& postData);

private:
	int requestIDCounter = 0;

public:
	static FCGIBase* getInstance();
	static void releaseInstance();

private:
	static FCGIBase* instance;
};
