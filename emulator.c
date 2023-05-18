#include "emulator.h"

Emulator* initEmulator(Emulator* emu){
    printf("Going to initialize the emulator.\n");
    
    emu->AF.bytes.higher = 0x01;
    emu->AF.bytes.lower = 0x0000;

    emu->BC.bytes.higher = 0xff;
    emu->BC.bytes.lower = 0x13;

    emu->DE.bytes.higher = 0x00;
    emu->DE.bytes.lower = 0xc1;

    emu->HL.bytes.higher = 0x84;
    emu->HL.bytes.lower = 0x03;

    emu->PC.entireByte = 0x100;
    emu->SP.entireByte = 0xfffe;

    printf("Finished initializing the emulator.\n");
}

void modify_flag(Emulator* emu, flags flag, u8 value){
    emu->AF.bytes.lower &= ~(1 << flag); emu->AF.bytes.lower |= value << flag;
}