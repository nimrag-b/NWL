CC=gcc
CFLAGS=-I.
DEPS = html.h
OBJ = main.o interp.o parser.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

nwl: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)