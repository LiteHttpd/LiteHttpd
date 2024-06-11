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
}

#define LUA_PUSH_FUNC(L, f) \
	do { \
		lua_pushcfunction((L), (lua::f)); \
		lua_setglobal((L), (#f)); \
	} while (false)

void loadLuaCommands(lua_State* L) {
	LUA_PUSH_FUNC(L, add_module);
	LUA_PUSH_FUNC(L, remove_module);
}
