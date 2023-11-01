all: wordle
wordle: wordle.c
	gcc -Wall -pedantic -std=gnu99 -o $@ $< -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a1
debug: wordle.c
	gcc -Wall -g -pedantic -std=gnu99 -o $@ $< -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a1
clean:
	rm wordle
