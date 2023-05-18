#include "cpu.h"

/* Register definations */

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

#define LD_u8(emu, reg) reg = read(emu, emu->PC.entireByte++);
#define LD_u16(emu, reg) reg = read_u16(emu);
#define LD_addr_reg(emu, addr, reg) write(emu, addr,  reg);

#define INC(emu, reg) inc_r8(emu, reg); reg ++;
#define DEC(emu, reg) dec_r8(emu, reg); reg --;
#define RLCA(emu, reg) reg = rotate_left_circular_carry_accumulator(reg);

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

u16 read_u16(Emulator* emu){
    return (u16)(read(emu, emu->PC.entireByte++) | (read(emu, emu->PC.entireByte++) << 8));
}

static void write(Emulator* emu, u16 addr, u8 byte){
    if ((addr >= ECHO_RAM && addr <= ECHO_RAM_END) || (addr >= NOT_USABLE && addr <= NOT_USABLE_END)) return;
    
    if (addr >= VRAM_8KB && addr <= VRAM_8KB_END) emu->vram[addr - VRAM_8KB] = byte;
    else if (addr >= WRAM_4KB && addr <= WRAM_4KB_END) emu->wram1[addr - WRAM_4KB] = byte;
    else if (addr >= WRAM_SWITCHABLE_4KB && addr <= WRAM_SWITCHABLE_4KB_END) emu->wram2[addr - WRAM_SWITCHABLE_4KB] = byte;
    else if (addr >= HIGH_RAM && addr <= HIGH_RAM_END) emu->hram[addr - HIGH_RAM] = byte;
    else if (addr >= IO_REGISTERS && addr <= IO_REGISTERS_END) emu->IO[addr - IO_REGISTERS] = byte;
}

void Start(Cartridge* cart, Emulator* emu){
    printf("Starting dispatch\n");
    emu->cart = cart;

    int dispatch_count = 0;

    u8 opcode;

    while (dispatch_count < 10) {
        dispatch_count += 1;
        printf("\n-- DISPATCH %d --\n", dispatch_count);
        dispatch(emu);
    }
}

static void inc_r8(Emulator* emu, u8 oldval){
    printf("Old val: %02x\n", oldval);
}

static void dec_r8(Emulator* emu, u8 oldval){
    printf("Old val: %02x\n", oldval);
}

static u8 rotate_left_circular_carry_accumulator(u8 reg_value){
    /* Rotate Left Circular through Carry Accumulator */
    u8 lastbit = reg_value;
    reg_value <<= 1;
    reg_value |= lastbit;

    return reg_value;
}

void dispatch(Emulator* emu){

    u8 opcode = read(emu, emu->PC.entireByte);

    printInstruction(emu); /* Prints the current instruction */
    printRegisters(emu); /* Shows what changes were made to regs after exec last instr. */

    switch (opcode){
        case 0x00: break;
        case 0x01: LD_u16(emu, BC(emu)); break;
        case 0x02: LD_addr_reg(emu, read(emu, BC(emu)), A(emu)); break;
        case 0x04: INC(emu, B(emu)); break;
        case 0x05: DEC(emu, B(emu)); break;
        case 0x06: LD_u8(emu, B(emu)); break;
        case 0x07: RLCA(emu, A(emu)); break;
    }

    emu->PC.entireByte ++;
}