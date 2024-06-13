#include <string>

#include "config/Config.h"
#include "module/ModuleManager.h"

static void loadModules(const ModuleList::Data& list) {
	for (auto& i : list) {
		ModuleManager::getInstance()->load(i.first, i.second);
	}
}

int main(int argc, char* argv[]){
	/** Load Configs */
	{
		std::string configPath = "config.lua";
		if (argc == 2) {
			configPath = argv[1];
		}
		Config::getInstance()->loadFrom(configPath);
	}

	/** Load Modules */
	{
		auto moduleList = Config::getInstance()->getModuleList();
		loadModules(moduleList);
	}

	/** Shutdown */
	Config::releaseInstance();
	ModuleManager::releaseInstance();

	/** Exit Code */
	return 0;
}
