
gbc: main.o cartridge.o emulator.o cpu.o debug.o
	gcc -o gbc main.o cartridge.o emulator.o cpu.o debug.o $(shell sdl2-config --libs)

main.o: main.c
	gcc -c main.c $(shell sdl2-config --cflags)

cartridge.o: cartridge.c cartridge.h
	gcc -c cartridge.c

emulator.o: emulator.c emulator.h
	gcc -c emulator.c

cpu.o: cpu.c cpu.h
	gcc -c cpu.c

debug.o: debug.c debug.h
	gcc -c debug.c

clean:
	rm -f gbc *.o

