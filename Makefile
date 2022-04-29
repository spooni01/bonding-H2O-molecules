CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

ALL: proj2

proj2: proj2.c
	$(CC) $(CFLAGS) proj2.c -o proj2

pack:
	zip proj2.zip *.c *.h Makefile

test: proj2
	./proj2 3 5 100 100
	bash test/kontrola-vystupu.sh < proj2.out

clean:
	rm -f *.exe *.out *.core *.zip
	rm -f proj2
