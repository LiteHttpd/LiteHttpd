#include "LuaCommands.h"

#include "Config.h"

namespace lua {
	int add_module(lua_State* L) {
		const char* host = luaL_checkstring(L, 1);
		const char* modulePath = luaL_checkstring(L, 2);

		Config::getInstance()->addModule(host, modulePath);

		return 0;
	}

	int remove_module(lua_State* L) {
		const char* host = luaL_checkstring(L, 1);

		Config::getInstance()->removeModule(host);

		return 0;
	}

	int set_port(lua_State* L) {
		uint16_t port = luaL_checkinteger(L, 1);

		Config::getInstance()->setPort(port);

		return 0;
	}

	int set_https(lua_State* L) {
		bool https = lua_toboolean(L, 1);

		Config::getInstance()->setHttps(https);

		return 0;
	}

	int set_default_page(lua_State* L) {
		const char* path = luaL_checkstring(L, 1);

		Config::getInstance()->setDefaultPage(path);

		return 0;
	}

	int add_cer(lua_State* L) {
		const char* host = luaL_checkstring(L, 1);
		const char* keyPath = luaL_checkstring(L, 2);
		const char* cerPath = luaL_checkstring(L, 3);

		Config::getInstance()->addCer(host, keyPath, cerPath);

		return 0;
	}

	int remove_cer(lua_State* L) {
		const char* host = luaL_checkstring(L, 1);

		Config::getInstance()->removeCer(host);

		return 0;
	}
}

#define LUA_PUSH_FUNC(L, f) \
	do { \
		lua_pushcfunction((L), (lua::f)); \
		lua_setglobal((L), (#f)); \
	} while (false)

void loadLuaCommands(lua_State* L) {
	LUA_PUSH_FUNC(L, add_module);
	LUA_PUSH_FUNC(L, remove_module);
	LUA_PUSH_FUNC(L, set_port);
	LUA_PUSH_FUNC(L, set_https);
	LUA_PUSH_FUNC(L, set_default_page);
	LUA_PUSH_FUNC(L, add_cer);
	LUA_PUSH_FUNC(L, remove_cer);
}
