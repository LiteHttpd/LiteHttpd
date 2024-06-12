#pragma once

#include <string>

#include "ModuleList.h"
#include "CerList.h"

class Config final {
public:
	Config();

	void loadFrom(const std::string& path);

public:
	void setPort(uint16_t port);
	void setHttps(bool https);

	void addModule(const std::string& host, const std::string& modulePath);
	void removeModule(const std::string& host);

	void addCer(const std::string& host, const std::string& keyPath, const std::string& cerPath);
	void removeCer(const std::string& host);

private:
	uint16_t port = 80;
	bool https = false;

	ModuleList modules;
	CerList certificates;

public:
	static Config* getInstance();
	static void releaseInstance();

private:
	static Config* instance;
};
