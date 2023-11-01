CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -o
LDFLAGS = -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a3


all: sigcat hq
sigcat: sigcat.c
	$(CC) $(CFLAGS) $@ $< $(LDFLAGS)
hq: hq.c
	$(CC) -g $(CFLAGS) $@ $< $(LDFLAGS)
clean:
	rm -f sigcat
	rm -f hq
