
all: 
	gcc -o emily_gui_test luatype.c main.c luaapi.c emily.c theme.c pack.c -g -lluajit -lmingw32 -lSDL2 -lSDL2_ttf -lSDL2_image -std=c99 -Wall
