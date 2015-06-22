
#include <emily/emily.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vector.h"
#include "theme.h"
#include "state.h"

static emily_Node * new_node(const emily_NodeDef * def)
{
	emily_Node * nn = malloc(sizeof(emily_Node));
	*nn = (emily_Node)NODE_INIT;
	//printf("Allocated node %p\n", nn);
	nn->def = *def;
	if(nn->def.text)
	{
		// copy the string, nullify the local def pointer (arg remains unchanged)
		size_t len = strlen(nn->def.text);
		nn->text = malloc(len + 1);
		strncpy(nn->text, nn->def.text, len);
		nn->text[len] = '\0';
		nn->def.text = 0;
	}

	return nn;
}
static void free_node(emily_Node * node)
{
	if(node->tex)
	{
		SDL_DestroyTexture(node->tex);
		node->tex = 0;
	}
	if(node->text)
	{
		free(node->text);
		node->text = 0;
	}

	free(node);
}

// free's the node and it's children, returns its successor
static emily_Node * destroy_node(emily_Node * node)
{
	emily_Node * successor = node->next;

	if(node->child)
		destroy_node(node->child);
	if(node->builtin)
		destroy_node(node->builtin);

	//printf("destroy: %p\n", node);
	free_node(node);

	return successor;
}

static void burn_node(emily_Node * node)
{
	//printf("BURNINATE: %p\n", node);
	//printf("BURNINATE: node->child: %p\n", node->child);
	//printf("BURNINATE: node->builtin: %p\n", node->builtin);
	//printf("BURNINATE: node->next: %p\n", node->next);

	if(node->child)
		burn_node(node->child);
	if(node->builtin)
		burn_node(node->builtin);
	if(node->next)
		burn_node(node->next);

	free_node(node);
}

emily_Node * emily_add(emily_Node * self, const emily_NodeDef * def)
{
	emily_Node * toadd = new_node(def);

	emily_Node ** dst = &self->child;
	while(*dst)
	{
		dst = &(*dst)->next;
	}

	*dst = toadd;
	toadd->parent = self;
	toadd->next = 0;

	return toadd;
}

emily_UI * emily_createui(SDL_Renderer * rr, const emily_ThemeDef * themedef)
{
	emily_UI * E = malloc(sizeof(emily_UI));

	E->rr = rr;

	E->root = 0;

	E->theme = theme_load(themedef);

	E->mouse_down_node = 0;
	E->mouse_over_node = 0;
	E->dragged_node = 0;

	//SDL_Cursor * handcursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

	//SDL_SetCursor(handcursor);

	return E;
}
void emily_destroyui(emily_UI * E)
{
	E->rr = 0;

	if(E->root)
		burn_node(E->root);
	E->root = 0;
	theme_unload(E->theme);
	E->theme = 0;

	E->mouse_down_node = 0;
	E->mouse_over_node = 0;
	E->dragged_node = 0;

	free(E);
}

emily_Node * emily_addroot(emily_UI * E, const emily_NodeDef * def)
{
	emily_Node * toadd = new_node(def);

	emily_Node ** dst = &E->root;
	while(*dst)
	{
		dst = &(*dst)->next;
	}

	*dst = toadd;
	toadd->parent = 0;
	toadd->next = 0;

	return toadd;
}

void pack(emily_UI * E, emily_Node * node);

void emily_pack(emily_UI * E)
{
	// time to pack it all
	if(E->root)
		pack(E, E->root);
}

static const char * node_type_names[] = {
	"window",
	"label",
	"button",
	"vbox",
	"hbox",
	"textinput",
	"scroller"
};
static void dumpnode(emily_Node * node, int indent)
{
	for(int i = 0 ; i < indent ; i ++) putc('\t', stdout);
	printf("%p: type = %s\n", node, node_type_names[node->def.type]);

	for(int i = 0 ; i < indent ; i ++) putc('\t', stdout);
	printf(" { x = %d, y = %d, w = %d, h = %d, text = \"%s\" }\n", node->x, node->y, node->w, node->h, node->text);

	if(node->child)
		dumpnode(node->child, indent + 1);
	if(node->next)
		dumpnode(node->next, indent);
}
void emily_dump(emily_UI * E)
{
	dumpnode(E->root, 0);
}

void emily_render(emily_UI * E)
{
	SDL_SetRenderDrawBlendMode(E->rr, SDL_BLENDMODE_BLEND);

	if(E->root)
		theme_render(E->theme, E->rr, E->root, 0, 0);

	SDL_RenderPresent(E->rr);
}

// search for containment, one level
static emily_Node * nodeat(emily_Node * list, int x, int y)
{
	emily_Node * node = list;

	while(node)
	{
		if(x >= node->x          && y >= node->y           &&
		   x <  node->x + node->w && y <  node->y + node->h )
		{
			return node;
		}

		node = node->next;
	}

	return 0;
}
// search recursively for containment.. without recursion!
static emily_Node * nodeat_rec(emily_Node * list, int x, int y)
{
	int relx = x;
	int rely = y;

	emily_Node * node = nodeat(list, x, y);
	emily_Node * n = 0;

	while(node)
	{
		n = node;
		relx -= node->x;
		rely -= node->y;
		node = nodeat(node->child, relx, rely);
	}

	return n;
}

bool emily_event(emily_UI * E, emily_Event * event, const SDL_Event * sdl_event)
{
	if(sdl_event->type == SDL_MOUSEBUTTONDOWN)
	{
		// things in the root may be dragged
		emily_Node * toplevel = nodeat(E->root, sdl_event->button.x, sdl_event->button.y);

		if(toplevel)
		{
			// check its children
			emily_Node * recv = nodeat_rec(toplevel->child, sdl_event->button.x - toplevel->x, sdl_event->button.y - toplevel->y);

			if(!recv)
			{
				// only the top level node caught this mouse click, start dragging
				E->dragged_node = toplevel;
				toplevel->status |= NODE_STATUS_DRAGGING;
			}
			else
			{
				// whatever node is underneath caugh this click
				E->mouse_down_node = recv;
				recv->status |= NODE_STATUS_MOUSEDOWN;

				*event = (emily_Event) { .node = recv, .type = EMILY_CLICK };

				return true;
			}
		}
	}
	else if(sdl_event->type == SDL_MOUSEBUTTONUP)
	{
		if(E->dragged_node)
		{
			E->dragged_node->status &= ~NODE_STATUS_DRAGGING;
			E->dragged_node = 0;
		}
		if(E->mouse_down_node)
		{
			E->mouse_down_node->status &= ~NODE_STATUS_MOUSEDOWN;
			E->mouse_down_node = 0;
		}
	}
	else if(sdl_event->type == SDL_MOUSEMOTION)
	{
		emily_Node * over = nodeat_rec(E->root, sdl_event->button.x, sdl_event->button.y);

		// we rolled over a different node!
		if(over != E->mouse_over_node)
		{
			if(E->mouse_over_node)
			{
				// TODO: generate the event here too
				E->mouse_over_node->status &= ~NODE_STATUS_MOUSEOVER;
			}

			E->mouse_over_node = over;

			if(E->mouse_over_node)
			{
				// TODO: generate the event here too
				E->mouse_over_node->status |= NODE_STATUS_MOUSEOVER;
			}
		}

		if(E->dragged_node)
		{
			E->dragged_node->x += sdl_event->motion.xrel;
			E->dragged_node->y += sdl_event->motion.yrel;
		}
	}

	return false;
}

