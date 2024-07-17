build:
	gcc -Wall -std=c99 src/*.c -lSDL2 -lm -o renderer

debug:
	gcc -g -Wall -std=c99 src/*.c -lSDL2 -lm -o renderer; gdb ./renderer;

run:
	./renderer

clean:
	rm renderer
