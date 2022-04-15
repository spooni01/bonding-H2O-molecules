CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

ALL: proj2

proj2: proj2.c
	$(CC) $(CFLAGS) proj2.c -o proj2

clean:
	rm -f *.exe *.out *.core
	rm proj2
