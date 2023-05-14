#include <stdio.h>
#include <stdlib.h>

#include "cartridge.h"

int main(int argc, char* argv[]){

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

    } else {
        printf("No input file has been provided.\n");
    }
}