#include "emulator.h"

Emulator* initEmulator(Emulator* emu){
    emu->AF.bytes.higher = 0x01;
    emu->AF.bytes.lower = 0x0000;

    emu->BC.bytes.higher = 0xff;
    emu->BC.bytes.lower = 0x13;

    emu->DE.bytes.higher = 0x00;
    emu->DE.bytes.lower = 0xc1;

    emu->HL.bytes.higher = 0x84;
    emu->HL.bytes.lower = 0x03;

    emu->PC.entireByte = 0xff;
    emu->SP.entireByte = 0xfffe;
    
    emu->run = false;
}

void modify_flag(Emulator* emu, flags flag, u8 value){
    emu->AF.bytes.lower &= ~(1 << flag); emu->AF.bytes.lower |= value << flag;
}

u8 getflag(Emulator* emu, flags flag){
    return (emu->AF.bytes.lower >> (flag + 4) & 1);
}