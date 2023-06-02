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

/* Flags */

#define set_flagh_add(emu, v1, v2) modify_flag(emu, flag_h, (((uint32_t)v1 & 0xf) + ((uint32_t)v2 & 0xf) > 0xf) ? 1 : 0);
#define set_flagh_sub(emu, v1, v2) modify_flag(emu, flag_h, ((v1 & 0xf) - (v2 & 0xf) & 0x10) ? 1 : 0)
#define set_flagh_addu16(emu, v1, v2) modify_flag(emu, flag_h, (((uint32_t)v1 & 0xfff) + ((uint32_t)v2 & 0xfff) > 0xfff) ? 0 : 1)
#define set_flagc_addu16(emu, v1, v2) modify_flag(emu, flag_c, ((uint32_t)(v1) + (uint32_t)(v2)) > 0xFFFF ? 1 : 0)
#define set_flagc_add(emu, v1, v2) modify_flag(emu, flag_c, ((uint16_t)(v1) + (uint16_t)(v2)) > 0xFF ? 1 : 0)
#define set_flagc_subu16(emu, v1, v2) modify_flag(emu, flag_c, ((int)(v1) - (int)(v2)) < 0 ? 1 : 0)
#define set_flagc_sub(emu, v1, v2) modify_flag(emu, flag_c, ((int)(v1) - (int)(v2)) < 0 ? 1 : 0)

#define LD_u8(emu, reg) reg = read_u8(emu);
#define LD_u16(emu, reg) reg = read_u16(emu);
#define LD_addr_reg(emu, addr, reg) write(emu, addr,  reg);
#define LD_R_u8(emu, reg, u_8) reg = u_8;

#define INC_RR(emu, reg) reg ++;
#define DEC_RR(emy, reg) reg --;

#define INC(emu, reg) inc_r8(emu, reg); reg ++;
#define DEC(emu, reg) dec_r8(emu, reg); reg --;

#define JUMP(emu, u_8) emu->PC.entireByte += (u8)u_8;
#define ADD(emu, v1, v2) 

#define ROTATE_LEFT(emu, reg, zflag, cflag) reg = rotate_left(emu, reg, zflag, cflag);
#define ROTATE_RIGHT(emu, reg, zflag, cflag) reg = rotate_right(emu, reg, zflag, cflag);

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

u8 read_u8(Emulator* emu){
    return (u8)(read(emu, emu->PC.entireByte ++));
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
    /* Old val is reg's value before incrementing. The actual incrementing is done after this function.*/
    
    modify_flag(emu, flag_z, !(oldval + 1));
    modify_flag(emu, flag_n, 0);
    set_flagh_add(emu, oldval, 1);
}

static void dec_r8(Emulator* emu, u8 oldval){
    modify_flag(emu, flag_z, !(oldval - 1));
    modify_flag(emu, flag_n, 1);
    set_flagh_sub(emu, oldval, 1);
}

static u8 rotate_left(Emulator* emu, u8 reg_value, bool zflag, bool carry_flag){

    /* Rotate left (Circular?) with Carry - Accumulator:
     * This function performs a bitwise left shift (by 1) on the provided register value (REG A).
     * If the carry_flag option is enabled, it sets the most significant bit (bit 7) to the original 
       value of the Carry flag (C).
     * If the zflag option is disabled, the Zero flag (Z) is cleared; otherwise, it is set based 
       on the resulting register value.
     * 
     * The Carry flag (C) is always set to the original value of bit 7 in all cases.
     */

    u8 lastbit = reg_value >> 7;
    reg_value <<= 1;
    reg_value |= carry_flag ? getflag(emu, flag_c) : lastbit;

    modify_flag(emu, flag_z, zflag ? !reg_value : 0);
    modify_flag(emu, flag_h, 0);
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_c, lastbit);

    return reg_value;
}

static u8 rotate_right(Emulator* emu, u8 reg_value, bool zflag, bool carry_flag){

    /* Rotate Right (Circular?) with Carry - Accumulator:
     * This function performs a bitwise right shift (by 1) on the provided register value (REG A).
     * If the carry_flag option is enabled, it sets the least significant bit (bit 0) to the original 
       value of the Carry flag (C).
     * If the zflag option is disabled, the Zero flag (Z) is cleared; otherwise, it is set based
       on the resulting register value.
     * 
     * The Carry flag (C) is always set to the original value of bit 0 in all cases.
     */

    u8 firstbit = reg_value & 1;

    reg_value >>= 1;
    reg_value |= (carry_flag ? getflag(emu, flag_c) : firstbit) << 7;

    modify_flag(emu, flag_z, zflag ? !reg_value : 0);
    modify_flag(emu, flag_h, 0);
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_c, firstbit);

    return reg_value;
}

static void add_u16_RR(Emulator* emu, res res1, res res2){

    u16 rr1 = res1.entireByte;
    u16 rr2 = res2.entireByte;

    modify_flag(emu, flag_n, 0);
    set_flagh_addu16(emu, rr1, rr2);
    set_flagc_addu16(emu, rr1, rr2);

    res1.entireByte = rr1 + rr2;
}

void dispatch(Emulator* emu){

    u8 opcode = read(emu, emu->PC.entireByte);

    printInstruction(emu); /* Prints the current instruction */
    printRegisters(emu); /* Shows what changes were made to regs after exec last instr. */

    switch (opcode){
        case 0x00: break;
        case 0x01: LD_u16(emu, BC(emu)); break;
        case 0x02: LD_addr_reg(emu, read(emu, BC(emu)), A(emu)); break;
        case 0x03: INC_RR(emu, BC(emu)); break;
        case 0x04: INC(emu, B(emu)); break;
        case 0x05: DEC(emu, B(emu)); break;
        case 0x06: LD_u8(emu, B(emu)); break;
        case 0x07: ROTATE_LEFT(emu, A(emu), false, false); break;
        case 0x08: {
            u16 twobytes = read_u16(emu);
            u16 sp = emu->SP.entireByte;

            write(emu, twobytes + 1, sp >> 8);
            write(emu, twobytes, sp & 0xff);
            break;
        }
        case 0x09: add_u16_RR(emu, emu->HL, emu->BC); break;
        case 0x0A: LD_R_u8(emu, A(emu), read(emu, BC(emu))); break;  // LD A, (BC)
        case 0x0B: DEC_RR(emu, BC(emu)); break;
        case 0x0C: INC(emu, C(emu)); break;
        case 0x0D: DEC(emu, C(emu)); break;
        case 0x0E: LD_u8(emu, C(emu)); break;
        case 0x0F: ROTATE_RIGHT(emu, A(emu), false, false); break;

        case 0x10: break;
        case 0x11: LD_u16(emu, DE(emu)); break;
        case 0x12: LD_addr_reg(emu, read(emu, DE(emu)), A(emu)); break;
        case 0x13: INC_RR(emu, DE(emu)); break;
        case 0x14: INC(emu, D(emu)); break;
        case 0x15: DEC(emu, D(emu)); break;
        case 0x16: LD_u8(emu, D(emu)); break;
        case 0x17: ROTATE_LEFT(emu, A(emu), false, true); break;
        case 0x18: JUMP(emu, read_u8(emu)); break;
        case 0x19: add_u16_RR(emu, emu->HL, emu->DE); break;
        case 0x1A: LD_R_u8(emu, A(emu), read(emu, DE(emu))); break;  // LD A, (BC)
        case 0x1B: DEC_RR(emu, DE(emu)); break;
        case 0x1C: INC(emu, E(emu)); break;
        case 0x1D: DEC(emu, E(emu)); break;
        case 0x1E: LD_u8(emu, E(emu)); break;
        case 0x1F: ROTATE_RIGHT(emu, A(emu), false, true); break;
        
    }

    emu->PC.entireByte ++;
}