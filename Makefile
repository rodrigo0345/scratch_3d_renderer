build:
	gcc -Wall -std=99 src/*.c -o renderer

run:
	./renderer

clean:
	rm renderer
