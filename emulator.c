#include "emulator.h"

Emulator* initEmulator(){

    Emulator* emu = (Emulator*) malloc(sizeof(Emulator));
    
    A(emu) = 0x11;
    F(emu) = 8;

    D(emu) = 0xff;
    E(emu) = 0x56;

    HL(emu) = 0x000d;

    emu->PC.entireByte = 0x100;
    emu->SP.entireByte = 0xfffe;

    emu->run = false;

    return emu;
}

char* getFlag(flags flag){
    if (flag == flag_z) return "Flag Z";
    else if (flag == flag_h) return "Flag H";
    else if (flag == flag_n) return "Flag N";
    else if (flag == flag_c) return "Flag C";

    return "Nan";
}

/*u8 getflag(Emulator* emu, flags flag){

    u8 flag_value = (emu->AF.bytes.lower >> (flag + 4) & 1);
    printf("(0x%04x) Fetching flag %s with value 0x%02x\n", F(emu), getFlag(flag), flag_value);

    return flag_value;
}*/

/*void modify_flag(Emulator* emu, flags flag, u8 value){
    emu->AF.bytes.lower &= ~(1 << flag); emu->AF.bytes.lower |= value << flag;
}*/

void modify_flag(Emulator* emu, flags flag, u8 value){
    // reset that specific bit
    F(emu) &=  ~(1 << (flag - 1));
    // and update it
    F(emu) |= (value << (flag - 1));
}

u8 getflag(Emulator* emu, flags flag){
    u8 flagValue = F(emu) >> (flag - 1) & 1;
    //printf("(0x%04x) Fetching flag %s with value 0x%02x\n", F(emu), getFlag(flag), flagValue);

    return flagValue;
}

void cyclesSync_4(Emulator* emu){
    /* Update clock cycles and sync relevant hardware, such as 
    * display and DMA/HMA */

    emu->clock += 4;
}
