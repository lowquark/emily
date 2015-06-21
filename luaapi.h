#ifndef LUAAPI_H
#define LUAAPI_H

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "emily.h"

#include <stdbool.h>

extern void emily_luainit(lua_State * L);
extern void emily_pushui(lua_State * L, emily_UI * ui);
//extern void emily_luaevent(lua_State * L, const emily_Event * event);

#endif
