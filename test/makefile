
all: main.o ../libemily.a
	gcc -o emily_test $^ -g -lluajit -lmingw32 -lSDL2 -lSDL2_ttf -lSDL2_image -std=c99 -Wall

../libemily.a:
	make -C ..

%.o: %.c
	gcc -c -o $@ $< -Wall -std=c99 -g -I../include/
