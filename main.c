
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#undef main

#include "emily.h"
#include "luaapi.h"

#include <stdio.h>
#include <stdbool.h>

SDL_Window * muh_window = 0;
SDL_Renderer * muh_renderer = 0;

void destroy_window()
{
	if(muh_renderer)
	{
		SDL_DestroyRenderer(muh_renderer);
		muh_renderer = 0;
	}
	if(muh_window)
	{
		SDL_DestroyWindow(muh_window);
		muh_window = 0;
	}
}
bool create_window(const char * name, size_t w, size_t h)
{
	destroy_window();

	muh_window = SDL_CreateWindow(
		name, 
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		SDL_WINDOW_OPENGL);

	muh_renderer = SDL_CreateRenderer(muh_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	//SDL_SetRenderDrawColor(muh_render, 0, 0, 0, 0);
	//SDL_RenderClear(muh_render);
	SDL_RenderPresent(muh_renderer);

	if(!muh_window)
		printf("Couldn't create window!\n");

	if(!muh_renderer)
		printf("Couldn't create renderer! (SDL_RENDERER_TARGETTEXTURE graphics required)\n");

	return muh_window;
}

int main(int argc, char ** argv)
{
	setvbuf(stdout, 0, _IONBF, 0);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	SDL_Init(SDL_INIT_VIDEO);

	create_window("emily-gui-test", 640, 480);


	emily_ThemeDef td = {
		.text_font = { .path = "arial.ttf", .pt = 12 },
		.title_font = { .path = "arial.ttf", .pt = 16 },
		.button_font = { .path = "arial.ttf", .pt = 14 },

		.text_color = { 255, 255, 255, 255 },
		.primary_color = { 50, 50, 50, 100 },
		.secondary_color = { 25, 25, 25, 200 },
		.btn_primary_color = { 80, 80, 82, 100 },
		.btn_secondary_color = { 40, 40, 41, 100 },
		.highlight_color = { 100, 100, 100, 100 },

		.window_margin_t = 5,
		.window_margin_b = 5,
		.window_margin_l = 5,
		.window_margin_r = 5,
		.button_margin_t = 5,
		.button_margin_b = 5,
		.button_margin_l = 5,
		.button_margin_r = 5,
	};


	lua_State * L = luaL_newstate();
	emily_UI * E = emily_createui(muh_renderer, &td);

	luaL_openlibs(L);
	emily_luainit(L);

	emily_pushui(L, E); lua_setglobal(L, "muh_ui");


	// Call main.lua in protected mode, calling debug.traceback() when an error occurs
	lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_replace(L, -2);

	luaL_loadfile(L, "main.lua");
	if(lua_pcall(L, 0, 0, -2))
	{
		printf("UNHANDLED LUA ERROR:  %s\n", lua_tostring(L, -1));

		lua_pop(L, 1); //Remove error from stack
	}

	lua_pop(L, 1); //Remove traceback from stack

	emily_pack(E);
	emily_dump(E);
	emily_render(E);


	SDL_Surface * bg_surf = IMG_Load("background.jpg");
	SDL_Texture * bg_tex = SDL_CreateTextureFromSurface(SDL_GetRenderer(muh_window), bg_surf);
	SDL_RenderCopy(SDL_GetRenderer(muh_window), bg_tex, 0, 0);


	emily_render(E);

	bool run = true;
	while(run)
	{
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if(event.type == SDL_KEYDOWN)
			{
				if(event.key.keysym.sym == SDLK_0)
					run = false;
			}
			if(event.type == SDL_QUIT)
			{
				run = false;
			}

			emily_Event em_event;
			if(emily_event(E, &em_event, &event))
			{
				if(em_event.type == EMILY_CLICK)
				{
					printf("Clicked on node %p\n", em_event.node);
				}
			}

			SDL_RenderCopy(SDL_GetRenderer(muh_window), bg_tex, 0, 0);
			emily_render(E);
		}
	}

	emily_destroyui(E);
	E = 0;

	lua_close(L);
	L = 0;

	destroy_window();

	IMG_Quit();
	SDL_Quit();

	return 0;
}
