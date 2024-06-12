#include "Config.h"
#include "LuaCommands.h"

Config::Config() {}

void Config::loadFrom(const std::string& path) {
	/** Create Lua State */
	auto lState = luaL_newstate();
	luaL_openlibs(lState);

	/** Reg Func */
	loadLuaCommands(lState);

	/** Run Config File */
	if (luaL_dofile(lState, path.c_str()) != LUA_OK) {
		const char* error = luaL_checkstring(lState, -1);
		/** TODO Error */
	}

	/** Close Lua State */
	lua_close(lState);
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
