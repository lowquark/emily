
#include "luatype.h"

int luaobj_gc(lua_State * L)
{
	luaobjtype_t * lt = lua_touserdata(L, lua_upvalueindex(1));

	void * ud = lua_touserdata(L, 1);
	lt->deinit(ud);

	printf("__gc for %s at %p\n", lt->name, ud);

	return 0;
}

void luaobj_init(lua_State * L, const luaobjtype_t * lt)
{
	luaL_newmetatable(L, lt->name);

		// __index / member functions
		lua_newtable(L);
			luamember_t * m = lt->members;
			// wait until sentinel entry
			while(m->name && m->function)
			{
				lua_pushcfunction(L, m->function); lua_setfield(L, -2, m->name);
				m ++;
			}
		lua_setfield(L, -2, "__index");

		// don't even give it a gc if it doesn't have a deinit method
		if(lt->deinit)
		{
			// __gc
			lua_pushlightuserdata(L, (luaobjtype_t *)lt); // lua type at upvalue 1
			lua_pushcclosure(L, luaobj_gc, 1); lua_setfield(L, -2, "__gc");
		}
	lua_pop(L, 1);
}
void * luaobj_pushnew(lua_State * L, const luaobjtype_t * lt)
{
	void * ud = lua_newuserdata(L, lt->size);

	if(lt->init)
	{
		lt->init(ud);
	}

	luaL_getmetatable(L, lt->name); lua_setmetatable(L, -2);

	printf("pushed %s %p\n", lt->name, ud);

	return ud;
}

void * luaobj_check(lua_State * L, int idx, const luaobjtype_t * lt)
{
	return luaL_checkudata(L, idx, lt->name);
}

void luaptr_init(lua_State * L, const luaptrtype_t * lt)
{
	luaL_newmetatable(L, lt->name);

		// __index / member functions
		lua_newtable(L);
			luamember_t * m = lt->members;
			// wait until sentinel entry
			while(m->name && m->function)
			{
				lua_pushcfunction(L, m->function); lua_setfield(L, -2, m->name);
				m ++;
			}
		lua_setfield(L, -2, "__index");

		// no __gc here
	lua_pop(L, 1);
}
void luaptr_push(lua_State * L, void * ptr, const luaptrtype_t * lt)
{
	void ** ud = lua_newuserdata(L, sizeof(void *));

	*ud = ptr;

	luaL_getmetatable(L, lt->name); lua_setmetatable(L, -2);
}

void * luaptr_check(lua_State * L, int idx, const luaptrtype_t * lt)
{
	void ** lua_ud = luaL_checkudata(L, idx, lt->name);

	return *lua_ud;
}

