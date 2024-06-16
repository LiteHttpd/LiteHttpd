#include <string>
#include <memory>

#include "config/Config.h"
#include "module/ModuleManager.h"
#include "ssl/CtxManager.h"
#include "http/EventBase.h"
#include "http/HttpServer.h"
#include "Utils.h"

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

	/** Load Default Page */
	auto defaultPage = utils::loadFile(Config::getInstance()->getDefaultPage());

	/** Create Server */
	auto server = std::make_unique<HttpServer>(
		Config::getInstance()->getHttps()
		? HttpServer::ProtocolType::HTTPS : HttpServer::ProtocolType::HTTP,
		std::string{ "0.0.0.0" }, Config::getInstance()->getPort(),
		[](const std::string& host) { return CtxManager::getInstance()->get(host); },
		[] { return CtxManager::getInstance()->getDefault(); },
		[](const std::string& host) { return ModuleManager::getInstance()->get(host); }
	);
	server->setDefaultPage(defaultPage);

	/** Run Event Loop */
	int retCode = EventBase::getInstance()->runEventLoop();

	/** Shutdown */
	EventBase::releaseInstance();
	Config::releaseInstance();
	CtxManager::releaseInstance();
	ModuleManager::releaseInstance();

	/** Exit Code */
	return retCode;
}
