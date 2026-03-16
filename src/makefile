CC=gcc
CFLAGS=-I. -O0 -g -ggdb
DEPS = html.h lang.h
OBJ = main.o interp.o parser.o internal.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nwl: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)