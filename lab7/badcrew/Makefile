CC=gcc
SOURCES=*.c
PROGRAM=badcrew
BINDIR=./bin
SRCDIR=./src

CFLAGS=-Wall -Wextra -Wpedantic -g


main:
	$(CC) $(SRCDIR)/$(SOURCES) -o $(BINDIR)/$(PROGRAM) $(CFLAGS)

clean:
	rm $(BINDIR)/$(PROGRAM)

run:
	./$(BINDIR)/$(PROGRAM)
