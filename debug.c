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
        
        case 0x10: printf("STOP\n"); break;
        case 0x11: printf("LD DE, u16\n"); break;
        case 0x12: printf("LD (DE), A\n"); break;
        case 0x13: printf("INC DE\n"); break;
        case 0x14: printf("INC D\n"); break;
        case 0x15: printf("DEC D\n"); break;
        case 0x16: printf("LD D, u8\n"); break;
        case 0x17: printf("RLA\n"); break;
        case 0x18: printf("JR r8\n"); break;
        case 0x19: printf("ADD HL, DE\n"); break;
        case 0x1A: printf("LD A, (DE)\n"); break;
        case 0x1B: printf("DEC DE\n"); break;
        case 0x1C: printf("INC E\n"); break;
        case 0x1D: printf("DEC E\n"); break;
        case 0x1E: printf("LD E, u8\n"); break;
        case 0x1F: printf("RRA\n"); break;

        case 0x20: printf("JR NZ, r8\n"); break;
        case 0x21: printf("LD HL, u16\n"); break;
        case 0x22: printf("LDI (HL), A\n"); break;
        case 0x23: printf("INC HL\n"); break;
        case 0x24: printf("INC H\n"); break;
        case 0x25: printf("DEC H\n"); break;
        case 0x26: printf("LD H, u8\n"); break;
        case 0x27: printf("DAA\n"); break;
        case 0x28: printf("JR Z, r8\n"); break;
        case 0x29: printf("ADD HL, HL\n"); break;
        case 0x2A: printf("LDI A, (HL)\n"); break;
        case 0x2B: printf("DEC HL\n"); break;
        case 0x2C: printf("INC L\n"); break;
        case 0x2D: printf("DEC L\n"); break;
        case 0x2E: printf("LD L, u8\n"); break;
        case 0x2F: printf("CPL\n"); break;

        case 0x30: printf("JR NC, r8\n"); break;
        case 0x31: printf("LD SP, u16\n"); break;
        case 0x32: printf("LDD (HL), A\n"); break;
        case 0x33: printf("INC SP\n"); break;
        case 0x34: printf("INC (HL)\n"); break;
        case 0x35: printf("DEC (HL)\n"); break;
        case 0x36: printf("LD (HL), u8\n"); break;
        case 0x37: printf("SCF\n"); break;
        case 0x38: printf("JR C, r8\n"); break;
        case 0x39: printf("ADD HL, SP\n"); break;
        case 0x3A: printf("LDD A, (HL)\n"); break;
        case 0x3B: printf("DEC SP\n"); break;
        case 0x3C: printf("INC A\n"); break;
        case 0x3D: printf("DEC A\n"); break;
        case 0x3E: printf("LD A, u8\n"); break;
        case 0x3F: printf("CCF\n"); break;

        case 0x40: printf("LD B, B\n"); break;
        case 0x41: printf("LD B, C\n"); break;
        case 0x42: printf("LD B, D\n"); break;
        case 0x43: printf("LD B, E\n"); break;
        case 0x44: printf("LD B, H\n"); break;
        case 0x45: printf("LD B, L\n"); break;
        case 0x46: printf("LD B, (HL)\n"); break;
        case 0x47: printf("LD B, A\n"); break;
        case 0x48: printf("LD C, B\n"); break;
        case 0x49: printf("LD C, C\n"); break;
        case 0x4A: printf("LD C, D\n"); break;
        case 0x4B: printf("LD C, E\n"); break;
        case 0x4C: printf("LD C, H\n"); break;
        case 0x4D: printf("LD C, L\n"); break;
        case 0x4E: printf("LD C, (HL)\n"); break;
        case 0x4F: printf("LD C, A\n"); break;

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