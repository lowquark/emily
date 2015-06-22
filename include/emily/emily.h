#ifndef EMILY_H
#define EMILY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "types.h"

emily_UI *   emily_createui  (SDL_Renderer * rr, const emily_ThemeDef * themedef);
void         emily_destroyui (emily_UI * E);

emily_Node * emily_addroot(emily_UI * E, const emily_NodeDef * def);

emily_Node * emily_add(emily_Node * self, const emily_NodeDef * def);
void         emily_pack(emily_UI * E);

void         emily_dump   (emily_UI * E);

void         emily_render (emily_UI * E);

bool         emily_event  (emily_UI * E, emily_Event * event, const SDL_Event * sdl_event);

#endif
