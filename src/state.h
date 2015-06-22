#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <emily/types.h>
#include "node.h"
#include "vector.h"

struct emily_UI
{
	SDL_Renderer * rr;

	emily_Node * root;

	theme_t * theme;

	emily_Node * mouse_down_node;
	emily_Node * mouse_over_node;
	emily_Node * dragged_node;
};

#endif
