
#include <emily/luaapi.h>
#include <emily/emily.h>

#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "luatype.h"

static luaptrtype_t lua_node;

static void Ltonodedef(emily_NodeDef * def, lua_State * L, int idx, int type)
{
	lua_pushvalue(L, idx);

	if(lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_newtable(L);
	}
	else if(!lua_istable(L, -1))
	{
		lua_pop(L, 1);
		luaL_argerror(L, idx, "expected table or nil");
	}

	*def = (emily_NodeDef){};

	def->type = type;

	lua_getfield(L, -1, "x"); def->x = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "y"); def->y = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "w"); def->w = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "h"); def->h = lua_tonumber(L, -1); lua_pop(L, 1);

	lua_getfield(L, -1, "margin_t"); def->pad_t = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "margin_b"); def->pad_b = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "margin_l"); def->pad_l = lua_tonumber(L, -1); lua_pop(L, 1);
	lua_getfield(L, -1, "margin_r"); def->pad_r = lua_tonumber(L, -1); lua_pop(L, 1);

	lua_getfield(L, -1, "spacing"); def->spacing = lua_tonumber(L, -1); lua_pop(L, 1);

	lua_getfield(L, -1, "text");
	size_t strlen;
	const char * luastr = lua_tolstring(L, -1, &strlen);
	if(strlen)
	{
		char * mystr = malloc(strlen + 1);
		strncpy(mystr, luastr, strlen);
		mystr[strlen] = '\0';

		def->text = mystr;
	}

	lua_pop(L, 1);
}

static int Lnode_addgeneric(lua_State * L, int type)
{
	emily_Node * self = luaptr_check(L, 1, &lua_node);

	emily_NodeDef def;
	Ltonodedef(&def, L, 2, type);

	emily_Node * added = emily_add(self, &def);

	luaptr_push(L, added, &lua_node);

	if(def.text)
		free((char *)def.text);

	return 1;
}
static int Lnode_addvbox(lua_State * L)
{
	return Lnode_addgeneric(L, EMILY_VBOX);
}
static int Lnode_addhbox(lua_State * L)
{
	return Lnode_addgeneric(L, EMILY_HBOX);
}
static int Lnode_addlabel(lua_State * L)
{
	return Lnode_addgeneric(L, EMILY_LABEL);
}
static int Lnode_addbutton(lua_State * L)
{
	return Lnode_addgeneric(L, EMILY_BUTTON);
}

static luamember_t node_members[] = {
	{ "addlabel", Lnode_addlabel },
	{ "addvbox", Lnode_addvbox },
	{ "addhbox", Lnode_addhbox },
	{ "addlabel", Lnode_addlabel },
	{ "addbutton", Lnode_addbutton },
	{ 0, 0 }
};
static luaptrtype_t lua_node = {
	.name = "emily_emily_Node",
	.members = node_members
};

static luaptrtype_t lua_ui;

static int Lui_theme(lua_State * L)
{
	luaptr_check(L, 1, &lua_ui);
	luaL_checktype(L, 2, LUA_TTABLE);

	printf("Setting theme is unimplemented!\n");

	return 0;
}

static int Lui_addgeneric(lua_State * L, int type)
{
	emily_UI * ui = luaptr_check(L, 1, &lua_ui);

	emily_NodeDef def;
	Ltonodedef(&def, L, 2, type);

	emily_Node * added = emily_addroot(ui, &def);

	luaptr_push(L, added, &lua_node);

	if(def.text)
		free((char *)def.text);

	return 1;
}
static int Lui_addwindow(lua_State * L)
{
	return Lui_addgeneric(L, EMILY_WINDOW);
}
static int Lui_addvbox(lua_State * L)
{
	return Lui_addgeneric(L, EMILY_VBOX);
}
static int Lui_addhbox(lua_State * L)
{
	return Lui_addgeneric(L, EMILY_HBOX);
}
static int Lui_addlabel(lua_State * L)
{
	return Lui_addgeneric(L, EMILY_LABEL);
}
static int Lui_addbutton(lua_State * L)
{
	return Lui_addgeneric(L, EMILY_BUTTON);
}

static luamember_t ui_members[] = {
	{ "theme", Lui_theme },
	{ "addwindow", Lui_addwindow },
	{ "addvbox", Lui_addvbox },
	{ "addhbox", Lui_addhbox },
	{ "addlabel", Lui_addlabel },
	{ "addbutton", Lui_addbutton },
	{ 0, 0 }
};
static luaptrtype_t lua_ui = {
	.name = "emily_ui_t",
	//.size = sizeof(ui_t),
	//.init = ui_init,
	//.deinit = ui_deinit,
	.members = ui_members
};

static void nodedef_deinit(void * nd)
{
	emily_NodeDef * nodedef = nd;

	// when lua uses this, it's always dynamically allocated.
	if(nodedef->text)
	{
		// fuck the police
		printf("Freeing %s\n", nodedef->text);
		free((char *)nodedef->text);
		nodedef->text = 0;
	}
}
static luamember_t nodedef_members[] = {
	{ 0, 0 }
};
static luaobjtype_t lua_nodedef = {
	.name = "emily_nodedef_t",
	.size = sizeof(emily_NodeDef),
	.init = 0,
	.deinit = nodedef_deinit,
	.members = nodedef_members,
};


void emily_luainit(lua_State * L)
{
	lua_newtable(L);
	lua_setglobal(L, "emily");

	// unlikely to go wrong, seriously who would choose these names in the registry
	luaptr_init(L, &lua_node);
	luaptr_init(L, &lua_ui);
	luaobj_init(L, &lua_nodedef);

	lua_pushliteral(L, "emily_handlers");
	lua_newtable(L);
	lua_rawset(L, LUA_REGISTRYINDEX);
}

void emily_pushui(lua_State * L, emily_UI * ui)
{
	luaptr_push(L, ui, &lua_ui);
}

/*
bool emily_pusheventfunc(lua_State * L, const emily_Event * event, int erridx)
{
	int errabsidx = erridx < 0 ? lua_gettop(L) + erridx + 1 : erridx;

	lua_getfield(L, LUA_REGISTRYINDEX, "emily_handlers");
	if(lua_isnil(L, -1))
	{
		lua_pop(L, 2);
		return false;
	}

	lua_pushlightuserdata(L, event->node);
	lua_rawget(L, -2);

	if(lua_isnil(L, -1))
	{
		lua_pop(L, 3);
		return false;
	}

	switch(event->type)
	{
		case EMILY_CLICK:
			lua_getfield(L, -1, "click");
			break;
		default:
			lua_pushnil(L);
			break;
	}

	if(lua_isnil(L, -1))
	{
		lua_pop(L, 4);
		return false;
	}

	lua_pcall(L, 0, 0, errabsidx);
}
*/
