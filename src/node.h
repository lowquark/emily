#ifndef NODE_H
#define NODE_H

#include <SDL2/SDL.h>

#include <emily/types.h>
#include "vector.h"

struct emily_Node
{
	struct emily_Node * next;
	struct emily_Node * child;
	struct emily_Node * builtin;
	struct emily_Node * parent;

	uint32_t status;
	uint32_t events;

	emily_NodeDef def;

	int x, y, w, h;
	char * text;

	SDL_Texture * tex;
};

#define NODE_INIT { 0 }

#define NODE_STATUS_MOUSEDOWN 0x01
#define NODE_STATUS_MOUSEOVER 0x02
#define NODE_STATUS_DRAGGING 0x04

#endif
