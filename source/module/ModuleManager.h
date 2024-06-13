#pragma once

#include <string>
#include <map>
#include <memory>
#include <mutex>

#include "ModuleHolder.h"

class ModuleManager final {
public:
	ModuleManager() = default;

	void load(const std::string& host, const std::string& path);
	ModuleBase* get(const std::string& host) const;

private:
	std::map<std::string, std::shared_ptr<ModuleHolder>> modules;
	mutable std::mutex lock;

public:
	static ModuleManager* getInstance();
	static void releaseInstance();

private:
	static ModuleManager* instance;
};
