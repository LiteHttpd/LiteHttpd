#include "Config.h"
#include "LuaCommands.h"
#include "log/Logger.h"

Config::Config() {}

void Config::loadFrom(const std::string& path) {
	Logger::info("Load config file: " + path);

	/** Create Lua State */
	auto lState = luaL_newstate();
	luaL_openlibs(lState);

	/** Reg Func */
	loadLuaCommands(lState);

	/** Run Config File */
	if (luaL_dofile(lState, path.c_str()) != LUA_OK) {
		const char* error = luaL_checkstring(lState, -1);
		/** Error */
		Logger::fatal(std::string{ error });
	}

	/** Close Lua State */
	lua_close(lState);
}

uint16_t Config::getPort() const {
	return this->port;
}

bool Config::getHttps() const {
	return this->https;
}

std::string Config::getDefaultPage() const {
	return this->defaultPage;
}

const ModuleList::Data Config::getModuleList() const {
	return this->modules.getCopy();
}

const CerList::Data Config::getCerList() const {
	return this->certificates.getCopy();
}

void Config::addModule(const std::string& host, const std::string& modulePath) {
	this->modules.insert(host, modulePath);
}

void Config::removeModule(const std::string& host) {
	this->modules.remove(host);
}

void Config::addCer(const std::string& host, const std::string& keyPath, const std::string& cerPath) {
	this->certificates.insert(host, keyPath, cerPath);
}

void Config::removeCer(const std::string& host) {
	this->certificates.remove(host);
}

void Config::setPort(uint16_t port) {
	this->port = port;
}

void Config::setHttps(bool https) {
	this->https = https;
}

void Config::setDefaultPage(const std::string& path) {
	this->defaultPage = path;
}

Config* Config::getInstance() {
	return Config::instance ? Config::instance
		: (Config::instance = new Config{});
}

void Config::releaseInstance() {
	if (Config::instance) {
		delete Config::instance;
		Config::instance = nullptr;
	}
}

Config* Config::instance = nullptr;
