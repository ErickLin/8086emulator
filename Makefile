CC=gcc
CFLAGS=-c
OBJS=isa.o debug.o main.o

all: $(OBJS)
	$(CC) $(OBJS) -o 8086emulator

main.o: main.c 
	$(CC) $(CFLAGS) main.c

debug.o: debug.c
	$(CC) $(CFLAGS) debug.c

isa.o: isa.c
	$(CC) $(CFLAGS) isa.c


clean:
	rm -rf *.o 8086emulator