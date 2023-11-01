CC = gcc
CFLAGS = -Wall -pedantic -std=gnu99 -o
LIBCFLAGS=-fPIC -Wall -pedantic -std=gnu99
LDFLAGS = -I/local/courses/csse2310/include -L/local/courses/csse2310/lib -lcsse2310a3 -lcsse2310a4
SERVERFLAGS = -lstringstore -pthread -lm


all: dbclient dbserver stringstore.o libstringstore.so
dbclient: dbclient.c
	$(CC) -g $(CFLAGS) $@ $< $(LDFLAGS)
dbserver: dbserver.c
	$(CC) $(CFLAGS) $@ $< $(LDFLAGS) $(SERVERFLAGS)
stringstore.o: stringstore.c
	$(CC) $(LIBCFLAGS) -c $<
libstringstore.so: stringstore.o
	$(CC) -shared -o $@ stringstore.o
clean:
	rm -f dbclient
	rm -f dbserver
