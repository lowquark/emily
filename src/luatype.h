#ifndef LUATYPE_H
#define LUATYPE_H

#include <stddef.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct luamember
{
	const char * name;
	lua_CFunction function;
} luamember_t;
typedef struct luaobjtype
{
	const char * name;
	size_t size;
	void (*init)(void * lua_ud);
	void (*deinit)(void * lua_ud);

	luamember_t * members;
} luaobjtype_t;

void   luaobj_init(lua_State * L, const luaobjtype_t * lt);
void * luaobj_pushnew(lua_State * L, const luaobjtype_t * lt);

void * luaobj_check(lua_State * L, int idx, const luaobjtype_t * lt);

// a weak pointer in lua
typedef struct luaptrtype
{
	const char * name;

	luamember_t * members;
} luaptrtype_t;

void   luaptr_init(lua_State * L, const luaptrtype_t * lt);
void   luaptr_push(lua_State * L, void * ptr, const luaptrtype_t * lt);

void * luaptr_check(lua_State * L, int idx, const luaptrtype_t * lt);

#endif
