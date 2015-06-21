
#include "node.h"
#include "types.h"
#include "theme.h"
#include "state.h"

static void pack_verticaloff(emily_Node * first, int x0, int y0, int pad, int * w, int * h)
{
	// local "cursor" location
	int locx = x0;
	int locy = y0;
	int maxw = 0;

	emily_Node * sub = first;

	while(sub)
	{
		sub->x = locx;
		sub->y = locy;

		if(locx + sub->w > maxw)
			maxw = locx + sub->w;

		locy += sub->h;

		if(sub->next)
			locy += pad;

		sub = sub->next;
	}

	*w = maxw;
	*h = locy;
}
static void pack_horizontaloff(emily_Node * first, int x0, int y0, int pad, int * w, int * h)
{
	// local "cursor" location
	int locx = x0;
	int locy = y0;
	int maxh = 0;

	emily_Node * sub = first;

	while(sub)
	{
		sub->x = locx;
		sub->y = locy;

		if(locy + sub->h > maxh)
			maxh = locy + sub->h;

		locx += sub->w;

		if(sub->next)
			locx += pad;

		sub = sub->next;
	}

	*w = locx;
	*h = maxh;
}
static void pack_vertical(emily_Node * first, int pad, int * w, int * h)
{
	pack_verticaloff(first, 0, 0, pad, w, h);
}
static void pack_horizontal(emily_Node * first, int pad, int * w, int * h)
{
	pack_horizontaloff(first, 0, 0, pad, w, h);
}

void packnode(theme_t * theme, emily_Node * node)
{
	if( node->def.type == EMILY_WINDOW ||
		node->def.type == EMILY_VBOX   ||
		node->def.type == EMILY_HBOX )
	{
		emily_Node * sub = node->child;
		while(sub)
		{
			packnode(theme, sub);

			sub = sub->next;
		}
	}

	if(node->def.type == EMILY_VBOX)
	{
		int w, h;
		pack_vertical(node->child, node->def.spacing, &w, &h);

		node->w = w;
		node->h = h;
	}
	else if(node->def.type == EMILY_HBOX)
	{
		int w, h;
		pack_horizontal(node->child, node->def.spacing, &w, &h);

		node->w = w;
		node->h = h;
	}
	else if(node->def.type == EMILY_WINDOW)
	{
		int w, h;

		int bar_height = theme_getbarheight(theme);

		pack_verticaloff(node->child, node->def.pad_l, node->def.pad_t + bar_height, node->def.spacing, &w, &h);

		// size is given
		node->w = node->def.w;
		node->h = node->def.h;

		// minimum size for contents
		int minw = w + node->def.pad_r;
		int minh = h + node->def.pad_b;

		// make sure it's bigger
		if(node->w < minw)
			node->w = minw;
		if(node->h < minh)
			node->h = minh;
	}
	else
	{
		theme_getsize(theme, node, &node->w, &node->h);
	}
}
void pack(emily_UI * E)
{
	// all given positions are absolute in the list of roots
	emily_Node * root = E->root;
	while(root)
	{
		root->x = root->def.x;
		root->y = root->def.y;

		packnode(E->theme, root);

		root = root->next;
	}
}
