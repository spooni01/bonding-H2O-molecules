CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

ALL: proj2

proj2: proj2.c
	$(CC) $(CFLAGS) proj2.c -o proj2

pack:
	zip proj2.zip *.c *.h

test: proj2
	./proj2 3 5 100 100
	bash test/kontrola-vystupu.sh < proj2.out

test_py: proj2
	python3 test/test.py

clean:
	rm -f *.exe *.out *.core
	rm -f proj2
