CC=gcc
CFLAGS=-c
OBJS=debug.o main.o

run: all
	./8086emulator

all: $(OBJS)
	$(CC) $(OBJS) -o 8086emulator

main.o: main.c 
	$(CC) $(CFLAGS) main.c

debug.o: debug.c
	$(CC) $(CFLAGS) debug.c

clean:
	rm -rf *.o 8086emulator