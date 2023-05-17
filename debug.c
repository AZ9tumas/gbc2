#include "debug.h"

void printInstruction(Emulator* emu){
    switch (emu->PC.entireByte) {
        
        case 0x01: printf("NOP\n"); break;
    
        default: printf("Invalid instruction\n");
    }
}

void printRegisters(Emulator* emu){
    printf("|A 0x%02x |F 0x%02x |B 0x%02x |C 0x%02x |D 0x%02x |E 0x%02x |H 0x%02x |L 0x%02x\n",
            emu->AF.bytes.higher, emu->AF.bytes.lower, /* AF */
            emu->BC.bytes.higher, emu->BC.bytes.lower, /* BC */
            emu->DE.bytes.higher, emu->DE.bytes.lower, /* DE */
            emu->HL.bytes.higher, emu->HL.bytes.lower  /* HL */
        );
}