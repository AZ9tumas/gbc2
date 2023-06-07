#include "debug.h"

void log_fatal(Emulator* emu, const char* string) {
    printf("[FATAL]");
    printf(" %s", string);
    printf("\n");

    exit(99);
}

void log_warning(Emulator* emu, const char* string) {
    printf("[WARNING]");
    printf(" %s", string);
    printf("\n");
}

static uint16_t read2Bytes(Emulator* emu) {
    uint8_t b1 = read(emu, emu->PC.entireByte + 1);
    uint8_t b2 = read(emu, emu->PC.entireByte + 2);
    uint16_t D16 = (b2 << 8) | b1;
    return D16;
}

static void printFlags(Emulator* emu) {
    uint8_t flagState = emu->AF.bytes.lower;

    printf("[Z%d", flagState >> 7);
    printf(" N%d", (flagState >> 6) & 1);
    printf(" H%d", (flagState >> 5) & 1);
    printf(" C%d]", (flagState >> 4) & 1);
}

static void simpleInstruction(Emulator* emu, char* ins) {
    printf("%s\n", ins);;
}

static void d16(Emulator* emu, char* ins) {
    printf("%s (0x%04x)\n", ins, read2Bytes(emu));
}

static void d8(Emulator* emu, char* ins) {
    printf("%s (0x%02x)\n", ins, read(emu, emu->PC.entireByte + 1));
}

static void a16(Emulator* emu, char* ins) {
    printf("%s (0x%04x)\n", ins, read2Bytes(emu));
}

static void r8(Emulator* emu, char* ins) {
    printf("%s (%d)\n", ins, (int8_t)read(emu, emu->PC.entireByte + 1));
}

void printCBInstruction(Emulator* emu, uint8_t byte) {
#ifdef DEBUG_PRINT_ADDRESS
    printf("[0x%04x]", emu->PC.entireByte.entireByte - 1);
#endif
#ifdef DEBUG_PRINT_FLAGS
    printFlags(emu);
#endif
#ifdef DEBUG_PRINT_CYCLES
    printf("[%ld]", emu->clock);
#endif
#ifdef DEBUG_PRINT_JOYPAD_REG
    printf("[sel:%x|", (emu->IO[R_P1_JOYP] >> 4) & 0x3);
    printf("sig:%x]", (emu->IO[R_P1_JOYP] & 0b00001111));
#endif
#ifdef DEBUG_PRINT_TIMERS
    printf("[%x|%x|%x|%x]", emu->IO[R_DIV], emu->IO[R_TIMA], emu->IO[R_TMA], emu->IO[R_TAC]);
#endif
    printf(" %5s", "");

    switch (byte) {
        case 0x00: return simpleInstruction(emu, "RLC B");
        case 0x01: return simpleInstruction(emu, "RLC C");
        case 0x02: return simpleInstruction(emu, "RLC D");
        case 0x03: return simpleInstruction(emu, "RLC E");
        case 0x04: return simpleInstruction(emu, "RLC H");
        case 0x05: return simpleInstruction(emu, "RLC L");
        case 0x06: return simpleInstruction(emu, "RLC (HL)");
        case 0x07: return simpleInstruction(emu, "RLC A");
        case 0x08: return simpleInstruction(emu, "RRC B");
        case 0x09: return simpleInstruction(emu, "RRC C");
        case 0x0A: return simpleInstruction(emu, "RRC D");
        case 0x0B: return simpleInstruction(emu, "RRC E");
        case 0x0C: return simpleInstruction(emu, "RRC H");
        case 0x0D: return simpleInstruction(emu, "RRC L");
        case 0x0E: return simpleInstruction(emu, "RRC (HL)");
        case 0x0F: return simpleInstruction(emu, "RRC A");
        case 0x10: return simpleInstruction(emu, "RL B");
        case 0x11: return simpleInstruction(emu, "RL C");
        case 0x12: return simpleInstruction(emu, "RL D");
        case 0x13: return simpleInstruction(emu, "RL E");
        case 0x14: return simpleInstruction(emu, "RL H");
        case 0x15: return simpleInstruction(emu, "RL L");
        case 0x16: return simpleInstruction(emu, "RL (HL)");
        case 0x17: return simpleInstruction(emu, "RL A");
        case 0x18: return simpleInstruction(emu, "RR B");
        case 0x19: return simpleInstruction(emu, "RR C");
        case 0x1A: return simpleInstruction(emu, "RR D");
        case 0x1B: return simpleInstruction(emu, "RR E");
        case 0x1C: return simpleInstruction(emu, "RR H");
        case 0x1D: return simpleInstruction(emu, "RR L");
        case 0x1E: return simpleInstruction(emu, "RR (HL)");
        case 0x1F: return simpleInstruction(emu, "RR A");
        case 0x20: return simpleInstruction(emu, "SLA B");
        case 0x21: return simpleInstruction(emu, "SLA C");
        case 0x22: return simpleInstruction(emu, "SLA D");
        case 0x23: return simpleInstruction(emu, "SLA E");
        case 0x24: return simpleInstruction(emu, "SLA H");
        case 0x25: return simpleInstruction(emu, "SLA L");
        case 0x26: return simpleInstruction(emu, "SLA (HL)");
        case 0x27: return simpleInstruction(emu, "SLA A");
        case 0x28: return simpleInstruction(emu, "SRA B");
        case 0x29: return simpleInstruction(emu, "SRA C");
        case 0x2A: return simpleInstruction(emu, "SRA D");
        case 0x2B: return simpleInstruction(emu, "SRA E");
        case 0x2C: return simpleInstruction(emu, "SRA H");
        case 0x2D: return simpleInstruction(emu, "SRA L");
        case 0x2E: return simpleInstruction(emu, "SRA (HL)");
        case 0x2F: return simpleInstruction(emu, "SRA A");
        case 0x30: return simpleInstruction(emu, "SWAP B");
        case 0x31: return simpleInstruction(emu, "SWAP C");
        case 0x32: return simpleInstruction(emu, "SWAP D");
        case 0x33: return simpleInstruction(emu, "SWAP E");
        case 0x34: return simpleInstruction(emu, "SWAP H");
        case 0x35: return simpleInstruction(emu, "SWAP L");
        case 0x36: return simpleInstruction(emu, "SWAP (HL)");
        case 0x37: return simpleInstruction(emu, "SWAP A");
        case 0x38: return simpleInstruction(emu, "SRL B");
        case 0x39: return simpleInstruction(emu, "SRL C");
        case 0x3A: return simpleInstruction(emu, "SRL D");
        case 0x3B: return simpleInstruction(emu, "SRL E");
        case 0x3C: return simpleInstruction(emu, "SRL H");
        case 0x3D: return simpleInstruction(emu, "SRL L");
        case 0x3E: return simpleInstruction(emu, "SRL (HL)");
        case 0x3F: return simpleInstruction(emu, "SRL A");
        case 0x40: return simpleInstruction(emu, "BIT 0, B");
        case 0x41: return simpleInstruction(emu, "BIT 0, C");
        case 0x42: return simpleInstruction(emu, "BIT 0, D");
        case 0x43: return simpleInstruction(emu, "BIT 0, E");
        case 0x44: return simpleInstruction(emu, "BIT 0, H");
        case 0x45: return simpleInstruction(emu, "BIT 0, L");
        case 0x46: return simpleInstruction(emu, "BIT 0, (HL)");
        case 0x47: return simpleInstruction(emu, "BIT 0, A");
        case 0x48: return simpleInstruction(emu, "BIT 1, B");
        case 0x49: return simpleInstruction(emu, "BIT 1, C");
        case 0x4A: return simpleInstruction(emu, "BIT 1, D");
        case 0x4B: return simpleInstruction(emu, "BIT 1, E");
        case 0x4C: return simpleInstruction(emu, "BIT 1, H");
        case 0x4D: return simpleInstruction(emu, "BIT 1, L");
        case 0x4E: return simpleInstruction(emu, "BIT 1, (HL)");
        case 0x4F: return simpleInstruction(emu, "BIT 1, A");
        case 0x50: return simpleInstruction(emu, "BIT 2, B");
        case 0x51: return simpleInstruction(emu, "BIT 2, C");
        case 0x52: return simpleInstruction(emu, "BIT 2, D");
        case 0x53: return simpleInstruction(emu, "BIT 2, E");
        case 0x54: return simpleInstruction(emu, "BIT 2, H");
        case 0x55: return simpleInstruction(emu, "BIT 2, L");
        case 0x56: return simpleInstruction(emu, "BIT 2, (HL)");
        case 0x57: return simpleInstruction(emu, "BIT 2, A");
        case 0x58: return simpleInstruction(emu, "BIT 3, B");
        case 0x59: return simpleInstruction(emu, "BIT 3, C");
        case 0x5A: return simpleInstruction(emu, "BIT 3, D");
        case 0x5B: return simpleInstruction(emu, "BIT 3, E");
        case 0x5C: return simpleInstruction(emu, "BIT 3, H");
        case 0x5D: return simpleInstruction(emu, "BIT 3, L");
        case 0x5E: return simpleInstruction(emu, "BIT 3, (HL)");
        case 0x5F: return simpleInstruction(emu, "BIT 3, A");
        case 0x60: return simpleInstruction(emu, "BIT 4, B");
        case 0x61: return simpleInstruction(emu, "BIT 4, C");
        case 0x62: return simpleInstruction(emu, "BIT 4, D");
        case 0x63: return simpleInstruction(emu, "BIT 4, E");
        case 0x64: return simpleInstruction(emu, "BIT 4, H");
        case 0x65: return simpleInstruction(emu, "BIT 4, L");
        case 0x66: return simpleInstruction(emu, "BIT 4, (HL)");
        case 0x67: return simpleInstruction(emu, "BIT 4, A");
        case 0x68: return simpleInstruction(emu, "BIT 5, B");
        case 0x69: return simpleInstruction(emu, "BIT 5, C");
        case 0x6A: return simpleInstruction(emu, "BIT 5, D");
        case 0x6B: return simpleInstruction(emu, "BIT 5, E");
        case 0x6C: return simpleInstruction(emu, "BIT 5, H");
        case 0x6D: return simpleInstruction(emu, "BIT 5, L");
        case 0x6E: return simpleInstruction(emu, "BIT 5, (HL)");
        case 0x6F: return simpleInstruction(emu, "BIT 5, A");
        case 0x70: return simpleInstruction(emu, "BIT 6, B");
        case 0x71: return simpleInstruction(emu, "BIT 6, C");
        case 0x72: return simpleInstruction(emu, "BIT 6, D");
        case 0x73: return simpleInstruction(emu, "BIT 6, E");
        case 0x74: return simpleInstruction(emu, "BIT 6, H");
        case 0x75: return simpleInstruction(emu, "BIT 6, L");
        case 0x76: return simpleInstruction(emu, "BIT 6, (HL)");
        case 0x77: return simpleInstruction(emu, "BIT 6, A");
        case 0x78: return simpleInstruction(emu, "BIT 7, B");
        case 0x79: return simpleInstruction(emu, "BIT 7, C");
        case 0x7A: return simpleInstruction(emu, "BIT 7, D");
        case 0x7B: return simpleInstruction(emu, "BIT 7, E");
        case 0x7C: return simpleInstruction(emu, "BIT 7, H");
        case 0x7D: return simpleInstruction(emu, "BIT 7, L");
        case 0x7E: return simpleInstruction(emu, "BIT 7, (HL)");
        case 0x7F: return simpleInstruction(emu, "BIT 7, A");
        case 0x80: return simpleInstruction(emu, "RES 0, B");
        case 0x81: return simpleInstruction(emu, "RES 0, C");
        case 0x82: return simpleInstruction(emu, "RES 0, D");
        case 0x83: return simpleInstruction(emu, "RES 0, E");
        case 0x84: return simpleInstruction(emu, "RES 0, H");
        case 0x85: return simpleInstruction(emu, "RES 0, L");
        case 0x86: return simpleInstruction(emu, "RES 0, (HL)");
        case 0x87: return simpleInstruction(emu, "RES 0, A");
        case 0x88: return simpleInstruction(emu, "RES 1, B");
        case 0x89: return simpleInstruction(emu, "RES 1, C");
        case 0x8A: return simpleInstruction(emu, "RES 1, D");
        case 0x8B: return simpleInstruction(emu, "RES 1, E");
        case 0x8C: return simpleInstruction(emu, "RES 1, H");
        case 0x8D: return simpleInstruction(emu, "RES 1, L");
        case 0x8E: return simpleInstruction(emu, "RES 1, (HL)");
        case 0x8F: return simpleInstruction(emu, "RES 1, A");
        case 0x90: return simpleInstruction(emu, "RES 2, B");
        case 0x91: return simpleInstruction(emu, "RES 2, C");
        case 0x92: return simpleInstruction(emu, "RES 2, D");
        case 0x93: return simpleInstruction(emu, "RES 2, E");
        case 0x94: return simpleInstruction(emu, "RES 2, H");
        case 0x95: return simpleInstruction(emu, "RES 2, L");
        case 0x96: return simpleInstruction(emu, "RES 2, (HL)");
        case 0x97: return simpleInstruction(emu, "RES 2, A");
        case 0x98: return simpleInstruction(emu, "RES 3, B");
        case 0x99: return simpleInstruction(emu, "RES 3, C");
        case 0x9A: return simpleInstruction(emu, "RES 3, D");
        case 0x9B: return simpleInstruction(emu, "RES 3, E");
        case 0x9C: return simpleInstruction(emu, "RES 3, H");
        case 0x9D: return simpleInstruction(emu, "RES 3, L");
        case 0x9E: return simpleInstruction(emu, "RES 3, (HL)");
        case 0x9F: return simpleInstruction(emu, "RES 3, A");
        case 0xA0: return simpleInstruction(emu, "RES 4, B");
        case 0xA1: return simpleInstruction(emu, "RES 4, C");
        case 0xA2: return simpleInstruction(emu, "RES 4, D");
        case 0xA3: return simpleInstruction(emu, "RES 4, E");
        case 0xA4: return simpleInstruction(emu, "RES 4, H");
        case 0xA5: return simpleInstruction(emu, "RES 4, L");
        case 0xA6: return simpleInstruction(emu, "RES 4, (HL)");
        case 0xA7: return simpleInstruction(emu, "RES 4, A");
        case 0xA8: return simpleInstruction(emu, "RES 5, B");
        case 0xA9: return simpleInstruction(emu, "RES 5, C");
        case 0xAA: return simpleInstruction(emu, "RES 5, D");
        case 0xAB: return simpleInstruction(emu, "RES 5, E");
        case 0xAC: return simpleInstruction(emu, "RES 5, H");
        case 0xAD: return simpleInstruction(emu, "RES 5, L");
        case 0xAE: return simpleInstruction(emu, "RES 5, (HL)");
        case 0xAF: return simpleInstruction(emu, "RES 5, A");
        case 0xB0: return simpleInstruction(emu, "RES 6, B");
        case 0xB1: return simpleInstruction(emu, "RES 6, C");
        case 0xB2: return simpleInstruction(emu, "RES 6, D");
        case 0xB3: return simpleInstruction(emu, "RES 6, E");
        case 0xB4: return simpleInstruction(emu, "RES 6, H");
        case 0xB5: return simpleInstruction(emu, "RES 6, L");
        case 0xB6: return simpleInstruction(emu, "RES 6, (HL)");
        case 0xB7: return simpleInstruction(emu, "RES 6, A");
        case 0xB8: return simpleInstruction(emu, "RES 7, B");
        case 0xB9: return simpleInstruction(emu, "RES 7, C");
        case 0xBA: return simpleInstruction(emu, "RES 7, D");
        case 0xBB: return simpleInstruction(emu, "RES 7, E");
        case 0xBC: return simpleInstruction(emu, "RES 7, H");
        case 0xBD: return simpleInstruction(emu, "RES 7, L");
        case 0xBE: return simpleInstruction(emu, "RES 7, (HL)");
        case 0xBF: return simpleInstruction(emu, "RES 7, A");
        case 0xC0: return simpleInstruction(emu, "SET 0, B");
        case 0xC1: return simpleInstruction(emu, "SET 0, C");
        case 0xC2: return simpleInstruction(emu, "SET 0, D");
        case 0xC3: return simpleInstruction(emu, "SET 0, E");
        case 0xC4: return simpleInstruction(emu, "SET 0, H");
        case 0xC5: return simpleInstruction(emu, "SET 0, L");
        case 0xC6: return simpleInstruction(emu, "SET 0, (HL)");
        case 0xC7: return simpleInstruction(emu, "SET 0, A");
        case 0xC8: return simpleInstruction(emu, "SET 1, B");
        case 0xC9: return simpleInstruction(emu, "SET 1, C");
        case 0xCA: return simpleInstruction(emu, "SET 1, D");
        case 0xCB: return simpleInstruction(emu, "SET 1, E");
        case 0xCC: return simpleInstruction(emu, "SET 1, H");
        case 0xCD: return simpleInstruction(emu, "SET 1, L");
        case 0xCE: return simpleInstruction(emu, "SET 1, (HL)");
        case 0xCF: return simpleInstruction(emu, "SET 1, A");
        case 0xD0: return simpleInstruction(emu, "SET 2, B");
        case 0xD1: return simpleInstruction(emu, "SET 2, C");
        case 0xD2: return simpleInstruction(emu, "SET 2, D");
        case 0xD3: return simpleInstruction(emu, "SET 2, E");
        case 0xD4: return simpleInstruction(emu, "SET 2, H");
        case 0xD5: return simpleInstruction(emu, "SET 2, L");
        case 0xD6: return simpleInstruction(emu, "SET 2, (HL)");
        case 0xD7: return simpleInstruction(emu, "SET 2, A");
        case 0xD8: return simpleInstruction(emu, "SET 3, B");
        case 0xD9: return simpleInstruction(emu, "SET 3, C");
        case 0xDA: return simpleInstruction(emu, "SET 3, D");
        case 0xDB: return simpleInstruction(emu, "SET 3, E");
        case 0xDC: return simpleInstruction(emu, "SET 3, H");
        case 0xDD: return simpleInstruction(emu, "SET 3, L");
        case 0xDE: return simpleInstruction(emu, "SET 3, (HL)");
        case 0xDF: return simpleInstruction(emu, "SET 3, A");
        case 0xE0: return simpleInstruction(emu, "SET 4, B");
        case 0xE1: return simpleInstruction(emu, "SET 4, C");
        case 0xE2: return simpleInstruction(emu, "SET 4, D");
        case 0xE3: return simpleInstruction(emu, "SET 4, E");
        case 0xE4: return simpleInstruction(emu, "SET 4, H");
        case 0xE5: return simpleInstruction(emu, "SET 4, L");
        case 0xE6: return simpleInstruction(emu, "SET 4, (HL)");
        case 0xE7: return simpleInstruction(emu, "SET 4, A");
        case 0xE8: return simpleInstruction(emu, "SET 5, B");
        case 0xE9: return simpleInstruction(emu, "SET 5, C");
        case 0xEA: return simpleInstruction(emu, "SET 5, D");
        case 0xEB: return simpleInstruction(emu, "SET 5, E");
        case 0xEC: return simpleInstruction(emu, "SET 5, H");
        case 0xED: return simpleInstruction(emu, "SET 5, L");
        case 0xEE: return simpleInstruction(emu, "SET 5, (HL)");
        case 0xEF: return simpleInstruction(emu, "SET 5, A");
        case 0xF0: return simpleInstruction(emu, "SET 6, B");
        case 0xF1: return simpleInstruction(emu, "SET 6, C");
        case 0xF2: return simpleInstruction(emu, "SET 6, D");
        case 0xF3: return simpleInstruction(emu, "SET 6, E");
        case 0xF4: return simpleInstruction(emu, "SET 6, H");
        case 0xF5: return simpleInstruction(emu, "SET 6, L");
        case 0xF6: return simpleInstruction(emu, "SET 6, (HL)");
        case 0xF7: return simpleInstruction(emu, "SET 6, A");
        case 0xF8: return simpleInstruction(emu, "SET 7, B");
        case 0xF9: return simpleInstruction(emu, "SET 7, C");
        case 0xFA: return simpleInstruction(emu, "SET 7, D");
        case 0xFB: return simpleInstruction(emu, "SET 7, E");
        case 0xFC: return simpleInstruction(emu, "SET 7, H");
        case 0xFD: return simpleInstruction(emu, "SET 7, L");
        case 0xFE: return simpleInstruction(emu, "SET 7, (HL)");
        case 0xFF: return simpleInstruction(emu, "SET 7, A");
    }
}

void printInstruction(Emulator* emu) {
    printf("PC 0x%04x -> ", emu->PC.entireByte);
    u8 inst = read(emu, emu->PC.entireByte);
    printf("OP 0x%02x ", inst);
    switch (inst) {
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

        case 0x50: printf("LD D, B\n"); break;
        case 0x51: printf("LD D, C\n"); break;
        case 0x52: printf("LD D, D\n"); break;
        case 0x53: printf("LD D, E\n"); break;
        case 0x54: printf("LD D, H\n"); break;
        case 0x55: printf("LD D, L\n"); break;
        case 0x56: printf("LD D, (HL)\n"); break;
        case 0x57: printf("LD D, A\n"); break;
        case 0x58: printf("LD E, B\n"); break;
        case 0x59: printf("LD E, C\n"); break;
        case 0x5A: printf("LD E, D\n"); break;
        case 0x5B: printf("LD E, E\n"); break;
        case 0x5C: printf("LD E, H\n"); break;
        case 0x5D: printf("LD E, L\n"); break;
        case 0x5E: printf("LD E, (HL)\n"); break;
        case 0x5F: printf("LD E, A\n"); break;

        case 0x60: printf("LD H, B\n"); break;
        case 0x61: printf("LD H, C\n"); break;
        case 0x62: printf("LD H, D\n"); break;
        case 0x63: printf("LD H, E\n"); break;
        case 0x64: printf("LD H, H\n"); break;
        case 0x65: printf("LD H, L\n"); break;
        case 0x66: printf("LD H, (HL)\n"); break;
        case 0x67: printf("LD H, A\n"); break;
        case 0x68: printf("LD L, B\n"); break;
        case 0x69: printf("LD L, C\n"); break;
        case 0x6A: printf("LD L, D\n"); break;
        case 0x6B: printf("LD L, E\n"); break;
        case 0x6C: printf("LD L, H\n"); break;
        case 0x6D: printf("LD L, L\n"); break;
        case 0x6E: printf("LD L, (HL)\n"); break;
        case 0x6F: printf("LD L, A\n"); break;

        case 0x70: printf("LD (HL), B\n"); break;
        case 0x71: printf("LD (HL), C\n"); break;
        case 0x72: printf("LD (HL), D\n"); break;
        case 0x73: printf("LD (HL), E\n"); break;
        case 0x74: printf("LD (HL), H\n"); break;
        case 0x75: printf("LD (HL), L\n"); break;
        case 0x76: printf("HALT\n"); break;
        case 0x77: printf("LD (HL), A\n"); break;
        case 0x78: printf("LD A, B\n"); break;
        case 0x79: printf("LD A, C\n"); break;
        case 0x7A: printf("LD A, D\n"); break;
        case 0x7B: printf("LD A, E\n"); break;
        case 0x7C: printf("LD A, H\n"); break;
        case 0x7D: printf("LD A, L\n"); break;
        case 0x7E: printf("LD A, (HL)\n"); break;
        case 0x7F: printf("LD A, A\n"); break;

        case 0x80: printf("ADD A, B\n"); break;
        case 0x81: printf("ADD A, C\n"); break;
        case 0x82: printf("ADD A, D\n"); break;
        case 0x83: printf("ADD A, E\n"); break;
        case 0x84: printf("ADD A, H\n"); break;
        case 0x85: printf("ADD A, L\n"); break;
        case 0x86: printf("ADD A, (HL)\n"); break;
        case 0x87: printf("ADD A, A\n"); break;
        case 0x88: printf("ADC A, B\n"); break;
        case 0x89: printf("ADC A, C\n"); break;
        case 0x8A: printf("ADC A, D\n"); break;
        case 0x8B: printf("ADC A, E\n"); break;
        case 0x8C: printf("ADC A, H\n"); break;
        case 0x8D: printf("ADC A, L\n"); break;
        case 0x8E: printf("ADC A, (HL)\n"); break;
        case 0x8F: printf("ADC A, A\n"); break;

        case 0x90: printf("SUB B\n"); break;
        case 0x91: printf("SUB C\n"); break;
        case 0x92: printf("SUB D\n"); break;
        case 0x93: printf("SUB E\n"); break;
        case 0x94: printf("SUB H\n"); break;
        case 0x95: printf("SUB L\n"); break;
        case 0x96: printf("SUB (HL)\n"); break;
        case 0x97: printf("SUB A\n"); break;
        case 0x98: printf("SBC A, B\n"); break;
        case 0x99: printf("SBC A, C\n"); break;
        case 0x9A: printf("SBC A, D\n"); break;
        case 0x9B: printf("SBC A, E\n"); break;
        case 0x9C: printf("SBC A, H\n"); break;
        case 0x9D: printf("SBC A, L\n"); break;
        case 0x9E: printf("SBC A, (HL)\n"); break;
        case 0x9F: printf("SBC A, A\n"); break;

        case 0xA0: printf("AND B\n"); break;
        case 0xA1: printf("AND C\n"); break;
        case 0xA2: printf("AND D\n"); break;
        case 0xA3: printf("AND E\n"); break;
        case 0xA4: printf("AND H\n"); break;
        case 0xA5: printf("AND L\n"); break;
        case 0xA6: printf("AND (HL)\n"); break;
        case 0xA7: printf("AND A\n"); break;
        case 0xA8: printf("XOR B\n"); break;
        case 0xA9: printf("XOR C\n"); break;
        case 0xAA: printf("XOR D\n"); break;
        case 0xAB: printf("XOR E\n"); break;
        case 0xAC: printf("XOR H\n"); break;
        case 0xAD: printf("XOR L\n"); break;
        case 0xAE: printf("XOR (HL)\n"); break;
        case 0xAF: printf("XOR A\n"); break;

        case 0xB0: printf("OR B\n"); break;
        case 0xB1: printf("OR C\n"); break;
        case 0xB2: printf("OR D\n"); break;
        case 0xB3: printf("OR E\n"); break;
        case 0xB4: printf("OR H\n"); break;
        case 0xB5: printf("OR L\n"); break;
        case 0xB6: printf("OR (HL)\n"); break;
        case 0xB7: printf("OR A\n"); break;
        case 0xB8: printf("CP B\n"); break;
        case 0xB9: printf("CP C\n"); break;
        case 0xBA: printf("CP D\n"); break;
        case 0xBB: printf("CP E\n"); break;
        case 0xBC: printf("CP H\n"); break;
        case 0xBD: printf("CP L\n"); break;
        case 0xBE: printf("CP (HL)\n"); break;
        case 0xBF: printf("CP A\n"); break;

        case 0xC0: printf("RET NZ\n"); break;
        case 0xC1: printf("POP BC\n"); break;
        case 0xC2: printf("JP NZ, u16\n"); break;
        case 0xC3: printf("JP u16\n"); break;
        case 0xC4: printf("CALL NZ, u16\n"); break;
        case 0xC5: printf("PUSH BC\n"); break;
        case 0xC6: printf("ADD A, u8\n"); break;
        case 0xC7: printf("RST 00H\n"); break;
        case 0xC8: printf("RET Z\n"); break;
        case 0xC9: printf("RET\n"); break;
        case 0xCA: printf("JP Z, u16\n"); break;
        case 0xCB: printf("PREFIX CB\n"); break;
        case 0xCC: printf("CALL Z, u16\n"); break;
        case 0xCD: printf("CALL u16\n"); break;
        case 0xCE: printf("ADC A, u8\n"); break;
        case 0xCF: printf("RST 08H\n"); break;

        case 0xD0: printf("RET NC\n"); break;
        case 0xD1: printf("POP DE\n"); break;
        case 0xD2: printf("JP NC, u16\n"); break;
        case 0xD4: printf("CALL NC, u16\n"); break;
        case 0xD5: printf("PUSH DE\n"); break;
        case 0xD6: printf("SUB u8\n"); break;
        case 0xD7: printf("RST 10H\n"); break;
        case 0xD8: printf("RET C\n"); break;
        case 0xD9: printf("RETI\n"); break;
        case 0xDA: printf("JP C, u16\n"); break;
        case 0xDC: printf("CALL C, u16\n"); break;
        case 0xDE: printf("SBC A, u8\n"); break;
        case 0xDF: printf("RST 18H\n"); break;

        case 0xE0: printf("LDH (u8), A\n"); break;
        case 0xE1: printf("POP HL\n"); break;
        case 0xE2: printf("LD (C), A\n"); break;
        case 0xE5: printf("PUSH HL\n"); break;
        case 0xE6: printf("AND u8\n"); break;
        case 0xE7: printf("RST 20H\n"); break;
        case 0xE8: printf("ADD SP, i8\n"); break;
        case 0xE9: printf("JP (HL)\n"); break;
        case 0xEA: printf("LD (u16), A\n"); break;
        case 0xEE: printf("XOR u8\n"); break;
        case 0xEF: printf("RST 28H\n"); break;

        case 0xF0: printf("LDH A, (u8)\n"); break;
        case 0xF1: printf("POP AF\n"); break;
        case 0xF2: printf("LD A, (C)\n"); break;
        case 0xF3: printf("DI\n"); break;
        case 0xF5: printf("PUSH AF\n"); break;
        case 0xF6: printf("OR u8\n"); break;
        case 0xF7: printf("RST 30H\n"); break;
        case 0xF8: printf("LD HL, SP+i8\n"); break;
        case 0xF9: printf("LD SP, HL\n"); break;
        case 0xFA: printf("LD A, (u16)\n"); break;
        case 0xFB: printf("EI\n"); break;
        case 0xFE: printf("CP u8\n"); break;
        case 0xFF: printf("RST 38H\n"); break;

        default: printf("Invalid instruction\n");
    }
}

void printRegisters(Emulator* emu) {
    printf("[A%02x|B%02x|C%02x|D%02x|E%02x|H%02x|L%02x|SP%04x]\n", A(emu), B(emu), C(emu), D(emu), E(emu), H(emu), L(emu), emu->SP.entireByte);
}