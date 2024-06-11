#pragma once

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace lua {
	int add_module(lua_State* L);
	int remove_module(lua_State* L);
}

void loadLuaCommands(lua_State* L);
