#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

int main(int argc, char* argv[]){

    Emulator* emu;

    initEmulator(emu);

    if (argc > 1) {
        char* filePath = argv[1];
        FILE* file = fopen(filePath, "r");

        if (file == NULL) {
            printf("Cannot open file.\n");
            exit(10);
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        uint8_t* memory = (uint8_t*)malloc(size);
        size_t e = fread(memory, size, 1, file);
        fclose(file);

        Cartridge cart;
        initCartridge(&cart, memory, size);
        print_cartridge(&cart);

        Start(&cart, emu);

    } else {
        printf("No input file has been provided.\n");
    }
}
