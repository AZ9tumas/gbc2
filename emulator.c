#include "emulator.h"

Emulator* initEmulator(Emulator* emu){
    A(emu) = 0x11;
    F(emu) = 0b10000000;

    BC(emu) = 0x0000;

    D(emu) = 0xff;
    E(emu) = 0x56;

    HL(emu) = 0x000d;

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