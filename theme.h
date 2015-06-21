#ifndef THEME_H
#define THEME_H

#include "node.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct
{
	emily_ThemeDef def;

	TTF_Font * text_font;
	TTF_Font * title_font;
	TTF_Font * button_font;
} theme_t;

theme_t * theme_load(const emily_ThemeDef * def);
     void theme_unload(theme_t * theme);

void theme_render(const theme_t * theme, SDL_Renderer * rr, emily_Node * node, int relx, int rely);

void theme_getsize(const theme_t * theme, const emily_Node * node, int * w, int * h);
int theme_getbarheight(const theme_t * theme);

#endif
