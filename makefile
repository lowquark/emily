

libemily.a: $(patsubst %.c,%.o,$(wildcard src/*.c))
	ar rcs $@ $^

%.o: %.c
	gcc -c -o $@ $< -Wall -std=c99 -g -I./include/


