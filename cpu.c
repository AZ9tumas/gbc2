#include "cpu.h"

/* Some very imp functions */

u8 read(Emulator* emu, u16 addr){
    if (addr >= 0x0 && addr <= 0x7fff) return emu->cart->file[addr];
    if (addr >= VRAM_8KB && addr <= VRAM_8KB_END) return emu->vram[addr - VRAM_8KB];
    if (addr >= WRAM_4KB && addr <= WRAM_4KB_END) return emu->wram1[addr - WRAM_4KB];
    if (addr >= WRAM_SWITCHABLE_4KB && addr <= WRAM_SWITCHABLE_4KB_END) return emu->wram2[addr - WRAM_SWITCHABLE_4KB];
    if (addr >= HIGH_RAM && addr >= HIGH_RAM_END) return emu->hram[addr - HIGH_RAM];
    if (addr >= IO_REGISTERS && addr <= IO_REGISTERS_END) return emu->IO[addr - IO_REGISTERS];
    
    printf("Found some address, 0x%04x, which cannot be actually accessed.", addr);

    return 0xff;
}

void write(Emulator* emu, u16 addr, u8 byte){
    if ((addr >= ECHO_RAM && addr <= ECHO_RAM_END) || (addr >= NOT_USABLE && addr <= NOT_USABLE_END)) return;
    if (addr >= VRAM_8KB && addr <= VRAM_8KB_END) emu->vram[addr - VRAM_8KB] = byte;
    if (addr >= WRAM_4KB && addr <= WRAM_4KB_END) emu->wram1[addr - WRAM_4KB] = byte;
    if (addr >= WRAM_SWITCHABLE_4KB && addr <= WRAM_SWITCHABLE_4KB_END) emu->wram2[addr - WRAM_SWITCHABLE_4KB] = byte;
    if (addr >= HIGH_RAM && addr <= HIGH_RAM_END) emu->hram[addr - HIGH_RAM] = byte;
    if (addr >= IO_REGISTERS && addr <= IO_REGISTERS_END) emu->IO[addr - IO_REGISTERS] = byte;
}

void Start(Cartridge* cart, Emulator* emu){
    printf("Starting dispatch\n");
    emu->cart = cart;

    int dispatch_count = 0;

    u8 opcode;

    while (dispatch_count < 10) {
        dispatch_count += 1;
        dispatch(emu);
    }
}

void dispatch(Emulator* emu){

    u8 opcode = read(emu, emu->PC.entireByte);

    switch (opcode){
        case 0x01: printf("NOP"); break;
        case 0x02: printf("LD BC, u16"); break;
        case 0x03: printf("INC BC"); break;
    }

    emu->PC.entireByte += 1;
}