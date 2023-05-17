#ifndef gbc_emulator
#define gbc_emulator

#include "cartridge.h"

typedef union {
    struct {
        u8 lower;
        u8 higher;
    } bytes;
    u16 entireByte;
} Register;


typedef Register res;

typedef struct {
    /* Registers */
    
    res AF, BC, DE, HL, SP, PC;

    u8 vram[0x2000];  /* 8 kb */
    u8 wram1[0x1000]; /* wram1 + wram2 = 8 kb */
    u8 wram2[0x1000];
    u8 hram[0x7f]; /*  */
    u8 IO[0x80]; 


    Cartridge* cart;
} Emulator;

Emulator* initEmulator(Emulator* emu);


#endif
