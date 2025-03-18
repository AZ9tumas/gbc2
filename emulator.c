#include "emulator.h"

Emulator* initEmulator(){

    Emulator* emu = (Emulator*) malloc(sizeof(Emulator));
    emu->AF.bytes.higher = 0x11;
    emu->AF.bytes.lower = 0x0000;

    D(emu) = 0xff;
    E(emu) = 0x56;

    HL(emu) = 0x000d;

    emu->HL.bytes.higher = 0x00;
    emu->HL.bytes.lower = 0x0d;

    emu->PC.entireByte = 0x100;
    emu->SP.entireByte = 0xfffe;

    modify_flag(emu, flag_z, 1);

    emu->run = false;

    return emu;
}

void modify_flag(Emulator* emu, flags flag, u8 value){
    emu->AF.bytes.lower &= ~(1 << flag); emu->AF.bytes.lower |= value << flag;
}

u8 getflag(Emulator* emu, flags flag){
    return (emu->AF.bytes.lower >> (flag + 4) & 1);
}

void cyclesSync_4(Emulator* emu){
    /* Update clock cycles and sync relevant hardware, such as 
    * display and DMA/HMA */

    emu->clock += 4;
}
