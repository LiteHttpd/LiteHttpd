#include <string>

#include "config/Config.h"
#include "module/ModuleManager.h"
#include "ssl/CtxManager.h"

static void loadModules(const ModuleList::Data& list) {
	for (auto& i : list) {
		ModuleManager::getInstance()->load(i.first, i.second);
	}
}

static void loadCertificates(const CerList::Data& list) {
	for (auto& i : list) {
		CtxManager::getInstance()->load(i.first, i.second.key, i.second.cer);
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

	/** Load SSL Certificates */
	{
		auto cerList = Config::getInstance()->getCerList();
		loadCertificates(cerList);
	}

	/** Load Modules */
	{
		auto moduleList = Config::getInstance()->getModuleList();
		loadModules(moduleList);
	}

	/** Shutdown */
	Config::releaseInstance();
	CtxManager::releaseInstance();
	ModuleManager::releaseInstance();

	/** Exit Code */
	return 0;
}
