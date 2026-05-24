CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -O2

all:
	$(CC) src/tarsau.c -o tarsau $(CFLAGS)

clean:
	rm -f tarsau *.sau
