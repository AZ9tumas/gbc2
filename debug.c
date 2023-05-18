#include "debug.h"

void printInstruction(Emulator* emu) {
    switch (emu->PC.entireByte) {
        case 0x00: printf("NOP\n"); break;
        case 0x01: printf("LD BC, u16\n"); break;
        case 0x02: printf("LD (BC), A\n"); break;
        case 0x03: printf("INC BC\n"); break;
        case 0x04: printf("INC B\n"); break;
        case 0x05: printf("DEC B\n"); break;
        case 0x06: printf("LD B, u8\n"); break;
        case 0x07: printf("RLCA\n"); break;
        case 0x08: printf("LD (u16), SP\n"); break;
        case 0x09: printf("ADD HL, BC\n"); break;
        case 0x0A: printf("LD A, (BC)\n"); break;
        case 0x0B: printf("DEC BC\n"); break;
        case 0x0C: printf("INC C\n"); break;
        case 0x0D: printf("DEC C\n"); break;
        case 0x0E: printf("LD C, u8\n"); break;
        case 0x0F: printf("RRCA\n"); break;
        // Add more cases for other instructions here
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