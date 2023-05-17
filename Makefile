CC = gcc
CFLAGS = -Isrc/Include
LDFLAGS = -Lsrc/lib -lmingw32

all: gbc

gbc: main.o cartridge.o emulator.o cpu.o debug.o
	$(CC) -o gbc main.o cartridge.o emulator.o cpu.o debug.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

cartridge.o: cartridge.h cartridge.c
	$(CC) $(CFLAGS) -c cartridge.c

emulator.o: emulator.h emulator.c
	$(CC) $(CFLAGS) -c emulator.c

cpu.o: cpu.h cpu.c
	$(CC) $(CFLAGS) -c cpu.c

debug.o: debug.h debug.c
	$(CC) $(CFLAGS) -c debug.c
