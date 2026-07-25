CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Os

OBJS=main.o terminal.o editor.o file.o

unano: $(OBJS)
	$(CC) $(CFLAGS) -o unano $(OBJS)

clean:
	rm -f *.o unano
