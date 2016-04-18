CC=gcc
CFLAGS=-c
OBJS=graphics.o isa.o debug.o main.o

all: $(OBJS)
	$(CC) $(OBJS) -o 8086emulator -lSDL 

main.o: main.c 
	$(CC) $(CFLAGS) main.c

debug.o: debug.c
	$(CC) $(CFLAGS) debug.c

isa.o: isa.c
	$(CC) $(CFLAGS) isa.c

graphics.o: graphics.c
	$(CC) $(CFLAGS) graphics.c


clean:
	rm -rf *.o 8086emulator