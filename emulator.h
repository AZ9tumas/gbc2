#ifndef gbc_emulator
#define gbc_emulator

#include "cartridge.h"

#define A(emu) emu->AF.bytes.higher
#define F(emu) emu->AF.bytes.lower

#define B(emu) emu->BC.bytes.higher
#define C(emu) emu->BC.bytes.lower

#define D(emu) emu->DE.bytes.higher
#define E(emu) emu->DE.bytes.lower

#define H(emu) emu->HL.bytes.higher
#define L(emu) emu->HL.bytes.lower

#define AF(emu) emu->AF.entireByte
#define BC(emu) emu->BC.entireByte
#define DE(emu) emu->DE.entireByte
#define HL(emu) emu->HL.entireByte

typedef union {
    struct {
        u8 lower;
        u8 higher;
    } bytes;
    u16 entireByte;
} Register;

typedef enum {
    flag_z = 4,
    flag_n = 3,
    flag_h = 2, 
    flag_c = 1
} flags;
typedef Register res;

typedef enum {
    R_SC = 0x01,
    R_SB = 0x02
} io_reg_addr;

typedef struct {

    /* Registers */ 
    res AF, BC, DE, HL, SP, PC;

    u8 vram[0x2000];  /*  */

    /* WRAM1 contains 1 Byte (8 bits) x 4096 (0x1000) = 4 KB of data */
    /* WRAM1 + WRAM2 = 8 KB */
    u8 wram1[0x1000];
    u8 wram2[0x1000];

    /* High Ram 0x7f = 127 */
    u8 hram[0x7f];
    u8 IO[0x80]; 

    bool run;

    unsigned long clock;

    Cartridge* cart;
} Emulator;

Emulator* initEmulator();
void modify_flag(Emulator* emu, flags flag, u8 val);
u8 getflag(Emulator* emu, flags flag);

u8 read(Emulator* emu, u16 addr);
void cyclesSync_4(Emulator* emu);

#endif
