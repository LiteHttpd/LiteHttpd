#pragma once

#include <string>

#include "ModuleList.h"

class Config final {
public:
	Config();

	void loadFrom(const std::string& path);

public:
	void addModule(const std::string& host, const std::string& modulePath);
	void removeModule(const std::string& host);

private:
	ModuleList modules;

public:
	static Config* getInstance();
	static void releaseInstance();

private:
	static Config* instance;
};
