#ifndef gbc_cpu
#define gbc_cpu

#include "debug.h"

typedef enum {
    ROM_N0_16KB = 0x0000,
    ROM_N0_16KB_END = 0x3FFF,

    ROM_N1_NN_16KB = 0x4000,          // ROM Bank 01~NN
    ROM_N1_NN_16KB_END = 0x7FFF,

    VRAM_8KB = 0x8000,                // Video RAM
    VRAM_8KB_END = 0x9FFF,

    EXTERNAL_RAM_8KB = 0xA000,        // External RAM
    EXTERNAL_RAM_8KB_END = 0xBFFF,

    WRAM_4KB = 0xC000,                // Work RAM
    WRAM_4KB_END = 0xCFFF,

    WRAM_SWITCHABLE_4KB = 0xD000,     // Work RAM (Switchable Bank)
    WRAM_SWITCHABLE_4KB_END = 0xDFFF,

    ECHO_RAM = 0xE000,                // Mirror of C000~DDFF (ECHO RAM)
    ECHO_RAM_END = 0xFDFF,

    OAM = 0xFE00,                     // Sprite attribute table (OAM)
    OAM_END = 0xFE9F,

    NOT_USABLE = 0xFEA0,              // Not Usable
    NOT_USABLE_END = 0xFEFF,

    IO_REGISTERS = 0xFF00,            // I/O Registers
    IO_REGISTERS_END = 0xFF7F,

    HIGH_RAM = 0xFF80,                // High RAM (HRAM)
    HIGH_RAM_END = 0xFFFE,

    INTERRUPT_ENABLE = 0xFFFF          // Interrupt Enable register (IE)
} addresses;

void Start(Cartridge* cart, Emulator* emu);
void dispatch(Emulator* emu);


#endif
