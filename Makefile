CC = gcc
CFLAGS = -Isrc/Include
LDFLAGS = -Lsrc/lib -lmingw32

all: gbc

gbc: main.o cartridge.o
	$(CC) -o gbc main.o cartridge.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

cartridge.o: cartridge.h cartridge.c
	$(CC) $(CFLAGS) -c cartridge.c