CC=gcc
CFLAGS=-I. -g -ggdb -O0
DEPS = html.h lang.h
OBJ = main.o interp.o parser.o internal.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nwl: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)