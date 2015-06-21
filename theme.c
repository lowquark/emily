
#include "theme.h"

#include <stdlib.h>
#include <stdio.h>

static TTF_Font * load_font(const char * filename, int size)
{
	if(!TTF_WasInit())
		TTF_Init();

	TTF_Font * muh_font = TTF_OpenFont(filename, size);
	if(!muh_font)
		printf("Error loading font \"%s\"\n", filename);
	else
		printf("Loaded font \"%s\"\n", filename);

	return muh_font;
}

theme_t * theme_load(const emily_ThemeDef * def)
{
	theme_t * newtheme = malloc(sizeof(theme_t));

	newtheme->def = *def;

	newtheme->text_font = load_font(def->text_font.path, def->text_font.pt);
	newtheme->title_font = load_font(def->title_font.path, def->title_font.pt);
	newtheme->button_font = load_font(def->button_font.path, def->button_font.pt);

	return newtheme;
}
void theme_unload(theme_t * theme)
{
	if(theme->text_font)
		TTF_CloseFont(theme->text_font);

	if(theme->title_font)
		TTF_CloseFont(theme->title_font);

	if(theme->button_font)
		TTF_CloseFont(theme->button_font);

	theme->text_font = 0;
	theme->title_font = 0;
	theme->button_font = 0;
}

static void gettexture(SDL_Renderer * rr, SDL_Texture ** tex, SDL_Surface * surface)
{
	if(!*tex)
		*tex = SDL_CreateTextureFromSurface(rr, surface);
	else
		SDL_UpdateTexture(*tex, 0, surface->pixels, surface->pitch);
}

static void drawtext(SDL_Renderer * rr, SDL_Texture ** tex, TTF_Font * font, const char * text, int relx, int rely)
{
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface * text_surface = TTF_RenderText_Blended(font, text, color);

	gettexture(rr, tex, text_surface);

	SDL_FreeSurface(text_surface);
	text_surface = 0;

	SDL_Rect dst_rect = { relx, rely, 0, 0 };
	SDL_QueryTexture(*tex, 0, 0, &dst_rect.w, &dst_rect.h);
	SDL_RenderCopy(rr, *tex, 0, &dst_rect);
}

static void drawbox(SDL_Renderer * rr, const SDL_Rect * rect, const emily_Color * fill, const emily_Color * border)
{
	SDL_SetRenderDrawColor(rr, fill->r, fill->g, fill->b, fill->a);
	SDL_Rect frect = { rect->x + 1, rect->y + 1, rect->w - 2, rect->h - 2 };
	SDL_RenderFillRect(rr, &frect);
	SDL_SetRenderDrawColor(rr, border->r, border->g, border->b, border->a);
	SDL_RenderDrawRect(rr, rect);
}

void theme_render(const theme_t * theme, SDL_Renderer * rr, emily_Node * node, int relx, int rely)
{
	int x = relx + node->x;
	int y = rely + node->y;

	if(node->def.type == EMILY_WINDOW)
	{
		int barheight = theme_getbarheight(theme);

		//printf("%d %d %d %d\n", x, y, node->w, barheight);

		SDL_Rect bar_rect = { x, y, node->w, barheight };
		drawbox(rr, &bar_rect, &theme->def.secondary_color, &theme->def.secondary_color);

		drawtext(rr, &node->tex, theme->title_font, node->text, x + 4, y + 2);

		// fill with primary, border with secondary
		SDL_Rect window_rect = { x, y + barheight, node->w, node->h - barheight };
		drawbox(rr, &window_rect, &theme->def.primary_color, &theme->def.secondary_color);
	}
	else if(node->def.type == EMILY_LABEL && theme->text_font)
	{
		drawtext(rr, &node->tex, theme->text_font, node->text, x, y);
	}
	else if(node->def.type == EMILY_BUTTON)
	{
		int texw, texh;
		TTF_SizeText(theme->button_font, node->text, &texw, &texh);

		SDL_Rect btn_rect = { 
			x, y, 
			node->def.pad_l + node->def.pad_r + texw, 
			node->def.pad_t + node->def.pad_b + texh };

		if((node->status & NODE_STATUS_MOUSEOVER) && 
		  !(node->status & NODE_STATUS_MOUSEDOWN))
			drawbox(rr, &btn_rect, &theme->def.highlight_color, &theme->def.btn_secondary_color);
		else
			drawbox(rr, &btn_rect, &theme->def.btn_primary_color, &theme->def.btn_secondary_color);

		drawtext(rr, 
			&node->tex, 
			theme->button_font, 
			node->text, 
			x + node->def.pad_l, 
			y + node->def.pad_t);
	}

	if(node->next)
		theme_render(theme, rr, node->next, relx, rely);
	if(node->child)
		theme_render(theme, rr, node->child, node->x + relx, node->y + rely);
}

void theme_getsize(const theme_t * theme, const emily_Node * node, int * w, int * h)
{
	if(node->def.type == EMILY_LABEL && theme->text_font)
	{
		TTF_SizeText(theme->text_font, node->text, w, h);
	}
	else if(node->def.type == EMILY_BUTTON)
	{
		int tw = 0, th = 0;

		if(theme->button_font)
			TTF_SizeText(theme->button_font, node->text, &tw, &th);

		tw += node->def.pad_l + node->def.pad_r;
		th += node->def.pad_t + node->def.pad_b;

		*w = tw;
		*h = th;
	}
	else
	{
		*w = 0;
		*h = 0;
	}
}
int theme_getbarheight(const theme_t * theme)
{
	if(theme->title_font) return TTF_FontHeight(theme->title_font) + 4;
	else return 15;
}

