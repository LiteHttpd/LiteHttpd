#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace lua {
	int add_module(lua_State* L);
	int remove_module(lua_State* L);

	int set_port(lua_State* L);
	int set_https(lua_State* L);

	int add_cer(lua_State* L);
	int remove_cer(lua_State* L);
}

void loadLuaCommands(lua_State* L);
