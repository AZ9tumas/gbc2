#include "cpu.h"

/* Flags */

#define set_flagz(emu, v1) modify_flag(emu, flag_z, !v1);
#define set_flagh_add(emu, v1, v2) modify_flag(emu, flag_h, (((uint32_t)v1 & 0xf) + ((uint32_t)v2 & 0xf) > 0xf) ? 1 : 0);
#define set_flagh_sub(emu, v1, v2) modify_flag(emu, flag_h, ((v1 & 0xf) - (v2 & 0xf) & 0x10) ? 1 : 0)
#define set_flagh_addu16(emu, v1, v2) modify_flag(emu, flag_h, (((uint32_t)v1 & 0xfff) + ((uint32_t)v2 & 0xfff) > 0xfff) ? 0 : 1)
#define set_flagc_addu16(emu, v1, v2) modify_flag(emu, flag_c, ((uint32_t)(v1) + (uint32_t)(v2)) > 0xFFFF ? 1 : 0)
#define set_flagc_add(emu, v1, v2) modify_flag(emu, flag_c, ((u16)(v1) + (u16)(v2)) > 0xFF ? 1 : 0)
#define set_flagc_subu16(emu, v1, v2) modify_flag(emu, flag_c, ((int)(v1) - (int)(v2)) < 0 ? 1 : 0)
#define set_flagc_sub(emu, v1, v2) modify_flag(emu, flag_c, ((int)(v1) - (int)(v2)) < 0 ? 1 : 0)

/* Flag conditions */
#define CONDITION_NZ(emu) getflag(emu, flag_z) != 1
#define CONDITION_NC(emu) getflag(emu, flag_c) != 1
#define CONDITION_Z(emu) getflag(emu, flag_z) == 1
#define CONDITION_C(emu) getflag(emu, flag_z) == 1

#define LD_u8(emu, reg) reg = read_u8(emu);
#define LD_u16(emu, reg) reg = read_u16(emu);
#define LD_addr_reg(emu, addr, reg) write(emu, addr,  reg);
#define LD_R_u8(emu, reg, u_8) reg = u_8;
#define LD_RR(emu, r1, r2) r1 = r2;
#define LD_u16_R(emu, addr, r) write(emu, addr, r);

#define INC_RR(emu, reg) reg ++;
#define DEC_RR(emy, reg) reg --;

#define INC(emu, reg) inc_r8(emu, reg); reg ++;
#define DEC(emu, reg) dec_r8(emu, reg); reg --;

/* Direct jump macros */
#define JUMP(emu, u_16v)  emu->PC.entireByte = u_16v;

/* Jump relative macros */
#define JUMP_RELATIVE(emu, u_8) emu->PC.entireByte += (u8)u_8;

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
    
    //printf("Found some address, 0x%04x, which cannot be actually accessed.", addr);

    return 0xff;
}

u16 read_u16(Emulator* emu){
    u8 lower = read(emu, ++ emu->PC.entireByte);
    u8 higher = read(emu, ++ emu->PC.entireByte);

    u16 total = lower | (higher << 8);

    return total;
}

u8 read_u8(Emulator* emu){
    return (u8)(read(emu, ++ emu->PC.entireByte));
}

static bool perform_IO_actions(Emulator* emu, u16 diff, u8 byte){
    /* Returns whether we have to continue writing to IO after this execution. */
    switch (diff){
        case R_SC: {
            if (byte == 0x81){
                printf("%c", emu->IO[R_SB]);
                emu->IO[R_SC] = 0x00;
            }
        }
    }
}

static void write(Emulator* emu, u16 addr, u8 byte){
    if ((addr >= ECHO_RAM && addr <= ECHO_RAM_END) || (addr >= NOT_USABLE && addr <= NOT_USABLE_END)) return;
    
    if (addr >= VRAM_8KB && addr <= VRAM_8KB_END) emu->vram[addr - VRAM_8KB] = byte;
    else if (addr >= WRAM_4KB && addr <= WRAM_4KB_END) emu->wram1[addr - WRAM_4KB] = byte;
    else if (addr >= WRAM_SWITCHABLE_4KB && addr <= WRAM_SWITCHABLE_4KB_END) emu->wram2[addr - WRAM_SWITCHABLE_4KB] = byte;
    else if (addr >= HIGH_RAM && addr <= HIGH_RAM_END) emu->hram[addr - HIGH_RAM] = byte;
    else if (addr >= IO_REGISTERS && addr <= IO_REGISTERS_END){
        if (perform_IO_actions(emu, addr - IO_REGISTERS, byte)) return;
        emu->IO[addr - IO_REGISTERS] = byte;
    }
}

void Start(Cartridge* cart, Emulator* emu){
    emu->cart = cart;

    int dispatch_count = 0;

    emu->run = true;

    while (dispatch_count < 2074879 && emu->run) {
        dispatch_count += 1;
        //printf("\n-- DISPATCH %d --\n", dispatch_count);
        dispatch(emu);
    }
}

static void inc_r8(Emulator* emu, u8 oldval){
    /* Old val is reg's value before incrementing. The actual incrementing is done after this function.*/
    
    set_flagz(emu, oldval + 1);
    set_flagh_add(emu, oldval, 1);
    modify_flag(emu, flag_n, 0);
}

static void dec_r8(Emulator* emu, u8 oldval){
    set_flagz(emu, oldval + 1);
    set_flagh_sub(emu, oldval, 1);
    modify_flag(emu, flag_n, 1);
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

static void decimal_adjust_accumulator(Emulator* emu){

    /* Decimal Adjust Accumulator 
     * This instruction is known by other names, such as "decimal adjust", or simply "DDA".
     * Since BCD arithmetic is supported, this representation of numbers must remain valid and
       properly. 
     * BCD Arithmetic means that numbers can be represented and manipulated in decimal format 
       using two decimal digits per byte. This instruction ensures that BCD operation is properly
       formatted. BCD stands for "Binary coded decimal".
    
    ** How the decimal adjust works **
    
      * If the least significant 4 bits of register A are greater than 9, or the half carry flag (H) is
      set, 6 is added to the accumulator.
      * If the most significant 4 bigs of register A are greater than 9 or if the carry flag (C) is 
      set, then 0x60 (96 in base 10) is added to the accumulator.
      * If the substract flag (N) is set, the above adjustments are substracted instead of added.

    ** For more information regarding this, visit : 'https://ehaskins.com/2018-01-30%20Z80%20DAA/'
    */


    u8 val = A(emu);

    if (getflag(emu, flag_n)) {

        if (getflag(emu, flag_h)) val -= 0x06;
        if (getflag(emu, flag_c)) val -= 0x60;

    } else {

        if (getflag(emu, flag_h) || (val & 0xf) > 0x9){
            u8 orginal = val;
            val += 0x6;

            if (orginal > val) modify_flag(emu, flag_c, 1);
        }

        if (getflag(emu, flag_c) || val > 0x9f) val += 0x60; modify_flag(emu, flag_c, 1);
    }

    set_flagz(emu, val);
    modify_flag(emu, flag_h, 0);
    A(emu) = val;
}

static void complement(Emulator* emu){
    /* Also known as "cpl", this instruction is used to flip values of all 
        bits in accumulator. */
    
    A(emu) = ~A(emu);

    modify_flag(emu, flag_n, 1);
    modify_flag(emu, flag_h, 1);
}

static void complement_carry(Emulator* emu){
    /* Also known as "ccf", which stands for "Carry flag complement".
    
    1. Complements the carry flag: If the carry flag is set (1),
     it is cleared (0), and if it is cleared (0), it is set (1).
    2. The half-carry flag (H) is reset to 0.
    3. The half-carry flag (H) is reset to 0.

    */

    modify_flag(emu, flag_c, !getflag(emu, flag_c));
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_h, 0);
}

static void halt_emulator(Emulator* emu){
    /* The HALT instruction
     * This is used to temporarily halt the execution of the CPU until an interrupt occurs.
     * When the HALT instruction is executed, the CPU enters a low-power state :
     * Minimal power is consumed until an interrupt signal is passed, to resume normal operation.
    */

   emu->run = false;
   printf("Came across the HALT instruction : : Stopping all execution.\n");
}

static void add_u16_RR(Emulator* emu, res res1, res res2){

    u16 rr1 = res1.entireByte;
    u16 rr2 = res2.entireByte;

    modify_flag(emu, flag_n, 0);
    set_flagh_addu16(emu, rr1, rr2);
    set_flagc_addu16(emu, rr1, rr2);

    res1.entireByte = rr1 + rr2;
}

static void test_adc_hc_flags(Emulator* emu, u8 old, u8 toAdd, u8 result, u8 carry) {
    /* ADC (Add with Carry) has a slightly different behavior compared to regular addition.
     * It adds the carry flag to the result, which introduces additional considerations for
     * evaluating the Half Carry (H) and Carry (C) flags. In this custom test function, we
     * carefully examine the relevant conditions for the flags. */

    bool halfCarryOccurred = false;  // Variable to track if a Half Carry (H) occurred
    bool carryOccurred = false;      // Variable to track if a Carry (C) occurred

    /* Half Carry (H) flag:
     * The Half Carry flag can occur in two cases:
     * 1. When the lower nibble of the initial value is added to the lower nibble of the value to be added.
     * 2. When the lower nibble of the result is added to the lower nibble of the carry flag.
     * We check for the occurrence of a Half Carry in both cases. */

    if ((old & 0xF) + (toAdd & 0xF) > 0xF || (result & 0xF) + (carry & 0xF) > 0xF) halfCarryOccurred = true;

    /* Carry (C) flag:
     * The Carry flag is set when there is an integer overflow during an 8-bit addition.
     * We calculate the overflow separately for both the initial addition and the addition
     * involving the carry flag. If the sum exceeds the maximum value of 8 bits (0xFF),
     * a Carry is considered to have occurred. */

    if ((u16)old + (u16)toAdd > 0xFF || (u16)result + (u16)carry > 0xFF) carryOccurred = true;

    /* Set the Half Carry (H) and Carry (C) flags based on the calculated results */
    modify_flag(emu, flag_h, halfCarryOccurred);
    modify_flag(emu, flag_c, carryOccurred);
}

static void test_sbc_hc_flags(Emulator* emu, u8 old, u8 toSub, u8 result, u8 carry){
    bool halfCarryOccured = false;
    bool carryOccured = false;

    u8 oldLow = old & 0xF;
    u8 resultLow = result & 0xF;

    if ((u8)(oldLow - (toSub & 0xF)) > oldLow) halfCarryOccured = true;
    if ((u8)(resultLow - carry) > resultLow) halfCarryOccured = true;

    if ((u8)((u16)old - (u16)toSub) > old) carryOccured = true;
    if ((u8)((u16)result - carry) > result) carryOccured = true;

    modify_flag(emu, flag_h, halfCarryOccured);
    modify_flag(emu, flag_c, carryOccured);
}

////////////////////////////////////////////////////////////////////////////////////////////////

static u8 add_u8_u8(Emulator* emu, u8 val1, u8 val2){
    /* Add instructions are used to add reg / immediate values 
     * Used to perform arithmetic operations by cpu
    */

    u8 result = val1 + val2;

    set_flagz(emu, result);
    modify_flag(emu, flag_n, 0);
    set_flagh_add(emu, val1, val2);
    set_flagc_add(emu, val1, val2);

    return result;
}

static u8 adc_u8_u8(Emulator* emu, u8 val1, u8 val2) {
    /* ADC (Add with Carry) instructions are used to add reg / immediate values
     * with the carry flag
    */

    u8 carry = getflag(emu, flag_c);
    u8 result = val1 + val2 + carry;

    set_flagz(emu, result);
    modify_flag(emu, flag_n, 0);

    test_adc_hc_flags(emu, val1, val2, result - carry, carry);

    return result;
}

static u8 sub_u8_u8(Emulator* emu, u8 val1, u8 val2){
    /* Subtract instructions are used to subtract reg / immediate values 
     * Used to perform arithmetic operations by cpu
    */

    u8 result = val1 - val2;

    set_flagz(emu, result);
    modify_flag(emu, flag_n, 1);
    set_flagh_sub(emu, val1, val2);
    set_flagc_sub(emu, val1, val2);

    return result;
}

static u8 sbc_u8_u8(Emulator* emu, u8 val1, u8 val2){
    /* SBC (Subtract with carry) instructions are used to subtract reg / immediate values 
     * Used to perform arithmetic operations by cpu
    */

    u8 carry = getflag(emu, flag_c);
    u8 result = val1 - val2 - carry;

    set_flagz(emu, result);
    modify_flag(emu, flag_n, 1);
    test_sbc_hc_flags(emu, val1, val2, result + carry, carry);

    return result;
}

static u8 and_u8_u8(Emulator* emu, u8 val1, u8 val2){
    u8 result = val1 & val2;

    set_flagz(emu, result);
    
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_h, 1);
    modify_flag(emu, flag_c, 0);

    return result;
}

static u8 xor_u8_u8(Emulator* emu, u8 val1, u8 val2){
    u8 result = val1 ^ val2;

    set_flagz(emu, result);
    
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_h, 0);
    modify_flag(emu, flag_c, 0);

    return result;
}

static u8 or_u8_u8(Emulator* emu, u8 val1, u8 val2){
    u8 result = val1 | val2;

    set_flagz(emu, result);
    
    modify_flag(emu, flag_n, 0);
    modify_flag(emu, flag_h, 0);
    modify_flag(emu, flag_c, 0);

    return result;
}

static void cp_u8_u8(Emulator* emu, u8 val1, u8 val2){
    u8 result = val1 - val2;
    
    modify_flag(emu, flag_n, 1);

    set_flagh_sub(emu, val1, val2);
    set_flagc_sub(emu, val1, val2);
}

static void jump_relative_condition(Emulator* emu, bool condition_status){
    int8_t jp_count = (int8_t) read_u8(emu); /* 4 cycles */
    if (condition_status){
        emu->PC.entireByte += jp_count;
        /* 4 Cycles */
    }
}

void dispatch(Emulator* emu){
    /* This increments PC, so we're already at the next byte. */
    u8 opcode = read_u8(emu);

    printRegisters(emu);
    printInstruction(emu);

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
        case 0x18: JUMP_RELATIVE(emu, read_u8(emu)); break;
        case 0x19: add_u16_RR(emu, emu->HL, emu->DE); break;
        case 0x1A: LD_R_u8(emu, A(emu), read(emu, DE(emu))); break;  // LD A, (DE)
        case 0x1B: DEC_RR(emu, DE(emu)); break;
        case 0x1C: INC(emu, E(emu)); break;
        case 0x1D: DEC(emu, E(emu)); break;
        case 0x1E: LD_u8(emu, E(emu)); break;
        case 0x1F: ROTATE_RIGHT(emu, A(emu), false, true); break;

        case 0x20: jump_relative_condition(emu, CONDITION_NZ(emu)); break;
        case 0x21: LD_u16(emu, HL(emu)); break;
        case 0x22: LD_addr_reg(emu, read(emu, HL(emu)), A(emu)); INC_RR(emu, HL(emu)); break;
        case 0x23: INC_RR(emu, HL(emu)); break;
        case 0x24: INC(emu, H(emu)); break;
        case 0x25: DEC(emu, H(emu)); break;
        case 0x26: LD_u8(emu, H(emu)); break;
        case 0x27: decimal_adjust_accumulator(emu); break;
        case 0x28: jump_relative_condition(emu, CONDITION_Z(emu)); break;
        case 0x29: add_u16_RR(emu, emu->HL, emu->HL); break;
        case 0x2A: LD_R_u8(emu, A(emu), read(emu, HL(emu))); INC_RR(emu, HL(emu)); break;
        case 0x2B: DEC_RR(emu, HL(emu)); break;
        case 0x2C: INC(emu, L(emu)); break;
        case 0x2D: DEC(emu, L(emu)); break;
        case 0x2E: LD_u8(emu, L(emu)); break;
        case 0x2F: complement(emu); break;

        case 0x30: jump_relative_condition(emu, CONDITION_NC(emu)); break;
        case 0x31: LD_u16(emu, emu->SP.entireByte); break;
        case 0x32: LD_addr_reg(emu, read(emu, HL(emu)), A(emu)); DEC_RR(emu, HL(emu)); break;
        case 0x33: INC_RR(emu, emu->SP.entireByte); break;
        case 0x34: {
            /* INC (HL) */
            u16 addr = HL(emu);
            u8 old = read(emu, addr);
            u8 new = old + 1;
            write(emu, addr, new);

            set_flagz(emu, new);
            set_flagh_addu16(emu, old, 1);
            modify_flag(emu, flag_n, 0);

            break;
        }
        case 0x35: {
            /* DEC (HL) */
            u16 addr = HL(emu);
            u8 old = read(emu, addr);
            u8 new = old - 1;
            write(emu, addr, new);

            set_flagz(emu, new);
            set_flagc_subu16(emu, old, 1);
            modify_flag(emu, flag_n, 1);

            break;
        }
        case 0x36: LD_addr_reg(emu, read(emu, HL(emu)), read_u8(emu)); break;
        case 0x37: {
            modify_flag(emu, flag_c, 1);
            modify_flag(emu, flag_n, 0);
            modify_flag(emu, flag_h, 0);
            break;
        }
        case 0x38: jump_relative_condition(emu, CONDITION_C(emu)); break;
        case 0x39: add_u16_RR(emu, emu->HL, emu->SP); break;
        case 0x3A: LD_R_u8(emu, A(emu), read(emu, HL(emu))); DEC_RR(emu, HL(emu)); break;
        case 0x3B: DEC_RR(emu, emu->SP.entireByte); break;
        case 0x3C: INC(emu, A(emu)); break;
        case 0x3D: DEC(emu, A(emu)); break;
        case 0x3E: LD_u8(emu, A(emu)); break;
        case 0x3F: complement_carry(emu); break;

        case 0x40: LD_RR(emu, B(emu), B(emu)); break;
        case 0x41: LD_RR(emu, B(emu), C(emu)); break;
        case 0x42: LD_RR(emu, B(emu), D(emu)); break;
        case 0x43: LD_RR(emu, B(emu), E(emu)); break;
        case 0x44: LD_RR(emu, B(emu), H(emu)); break;
        case 0x45: LD_RR(emu, B(emu), L(emu)); break;
        case 0x46: LD_RR(emu, B(emu), read(emu, HL(emu))); break;
        case 0x47: LD_RR(emu, B(emu), A(emu)); break;
        case 0x48: LD_RR(emu, C(emu), B(emu)); break;
        case 0x49: LD_RR(emu, C(emu), C(emu)); break;
        case 0x4A: LD_RR(emu, C(emu), D(emu)); break;
        case 0x4B: LD_RR(emu, C(emu), E(emu)); break;
        case 0x4C: LD_RR(emu, C(emu), H(emu)); break;
        case 0x4D: LD_RR(emu, C(emu), L(emu)); break;
        case 0x4E: LD_RR(emu, C(emu), read(emu, HL(emu))); break;
        case 0x4F: LD_RR(emu, C(emu), A(emu)); break;

        case 0x50: LD_RR(emu, D(emu), B(emu)); break;
        case 0x51: LD_RR(emu, D(emu), C(emu)); break;
        case 0x52: LD_RR(emu, D(emu), D(emu)); break;
        case 0x53: LD_RR(emu, D(emu), E(emu)); break;
        case 0x54: LD_RR(emu, D(emu), H(emu)); break;
        case 0x55: LD_RR(emu, D(emu), L(emu)); break;
        case 0x56: LD_RR(emu, D(emu), read(emu, HL(emu))); break;
        case 0x57: LD_RR(emu, D(emu), A(emu)); break;
        case 0x58: LD_RR(emu, E(emu), B(emu)); break;
        case 0x59: LD_RR(emu, E(emu), C(emu)); break;
        case 0x5A: LD_RR(emu, E(emu), D(emu)); break;
        case 0x5B: LD_RR(emu, E(emu), E(emu)); break;
        case 0x5C: LD_RR(emu, E(emu), H(emu)); break;
        case 0x5D: LD_RR(emu, E(emu), L(emu)); break;
        case 0x5E: LD_RR(emu, E(emu), read(emu, HL(emu))); break;
        case 0x5F: LD_RR(emu, E(emu), A(emu)); break;

        case 0x60: LD_RR(emu, H(emu), B(emu)); break;
        case 0x61: LD_RR(emu, H(emu), C(emu)); break;
        case 0x62: LD_RR(emu, H(emu), D(emu)); break;
        case 0x63: LD_RR(emu, H(emu), E(emu)); break;
        case 0x64: LD_RR(emu, H(emu), H(emu)); break;
        case 0x65: LD_RR(emu, H(emu), L(emu)); break;
        case 0x66: LD_RR(emu, H(emu), read(emu, HL(emu))); break;
        case 0x67: LD_RR(emu, H(emu), A(emu)); break;
        case 0x68: LD_RR(emu, L(emu), B(emu)); break;
        case 0x69: LD_RR(emu, L(emu), C(emu)); break;
        case 0x6A: LD_RR(emu, L(emu), D(emu)); break;
        case 0x6B: LD_RR(emu, L(emu), E(emu)); break;
        case 0x6C: LD_RR(emu, L(emu), H(emu)); break;
        case 0x6D: LD_RR(emu, L(emu), L(emu)); break;
        case 0x6E: LD_RR(emu, L(emu), read(emu, HL(emu))); break;
        case 0x6F: LD_RR(emu, L(emu), A(emu)); break;

        case 0x70: LD_u16_R(emu, HL(emu), B(emu)); break;
        case 0x71: LD_u16_R(emu, HL(emu), C(emu)); break;
        case 0x72: LD_u16_R(emu, HL(emu), D(emu)); break;
        case 0x73: LD_u16_R(emu, HL(emu), E(emu)); break;
        case 0x74: LD_u16_R(emu, HL(emu), H(emu)); break;
        case 0x75: LD_u16_R(emu, HL(emu), L(emu)); break;
        case 0x76: halt_emulator(emu); break;
        case 0x77: LD_u16_R(emu, HL(emu), A(emu)); break;
        case 0x78: LD_RR(emu, A(emu), B(emu)); break;
        case 0x79: LD_RR(emu, A(emu), C(emu)); break;
        case 0x7A: LD_RR(emu, A(emu), D(emu)); break;
        case 0x7B: LD_RR(emu, A(emu), E(emu)); break;
        case 0x7C: LD_RR(emu, A(emu), H(emu)); break;
        case 0x7D: LD_RR(emu, A(emu), L(emu)); break;
        case 0x7E: LD_RR(emu, A(emu), read(emu, HL(emu))); break;
        case 0x7F: LD_RR(emu, A(emu), A(emu)); break;

        case 0x80: A(emu) = add_u8_u8(emu, A(emu), B(emu)); break;
        case 0x81: A(emu) = add_u8_u8(emu, A(emu), C(emu)); break;
        case 0x82: A(emu) = add_u8_u8(emu, A(emu), D(emu)); break;
        case 0x83: A(emu) = add_u8_u8(emu, A(emu), E(emu)); break;
        case 0x84: A(emu) = add_u8_u8(emu, A(emu), H(emu)); break;
        case 0x85: A(emu) = add_u8_u8(emu, A(emu), L(emu)); break;
        case 0x86: A(emu) = add_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0x87: A(emu) = add_u8_u8(emu, A(emu), A(emu)); break;
        case 0x88: A(emu) = adc_u8_u8(emu, A(emu), B(emu)); break;
        case 0x89: A(emu) = adc_u8_u8(emu, A(emu), C(emu)); break;
        case 0x8A: A(emu) = adc_u8_u8(emu, A(emu), D(emu)); break;
        case 0x8B: A(emu) = adc_u8_u8(emu, A(emu), E(emu)); break;
        case 0x8C: A(emu) = adc_u8_u8(emu, A(emu), H(emu)); break;
        case 0x8D: A(emu) = adc_u8_u8(emu, A(emu), L(emu)); break;
        case 0x8E: A(emu) = adc_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0x8F: A(emu) = adc_u8_u8(emu, A(emu), A(emu)); break;

        case 0x90: A(emu) = sub_u8_u8(emu, A(emu), B(emu)); break;
        case 0x91: A(emu) = sub_u8_u8(emu, A(emu), C(emu)); break;
        case 0x92: A(emu) = sub_u8_u8(emu, A(emu), D(emu)); break;
        case 0x93: A(emu) = sub_u8_u8(emu, A(emu), E(emu)); break;
        case 0x94: A(emu) = sub_u8_u8(emu, A(emu), H(emu)); break;
        case 0x95: A(emu) = sub_u8_u8(emu, A(emu), L(emu)); break;
        case 0x96: A(emu) = sub_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0x97: A(emu) = sub_u8_u8(emu, A(emu), A(emu)); break;
        case 0x98: A(emu) = sbc_u8_u8(emu, A(emu), B(emu)); break;
        case 0x99: A(emu) = sbc_u8_u8(emu, A(emu), C(emu)); break;
        case 0x9A: A(emu) = sbc_u8_u8(emu, A(emu), D(emu)); break;
        case 0x9B: A(emu) = sbc_u8_u8(emu, A(emu), E(emu)); break;
        case 0x9C: A(emu) = sbc_u8_u8(emu, A(emu), H(emu)); break;
        case 0x9D: A(emu) = sbc_u8_u8(emu, A(emu), L(emu)); break;
        case 0x9E: A(emu) = sbc_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0x9F: A(emu) = sbc_u8_u8(emu, A(emu), A(emu)); break;

        case 0xA0: A(emu) = and_u8_u8(emu, A(emu), B(emu)); break;
        case 0xA1: A(emu) = and_u8_u8(emu, A(emu), C(emu)); break;
        case 0xA2: A(emu) = and_u8_u8(emu, A(emu), D(emu)); break;
        case 0xA3: A(emu) = and_u8_u8(emu, A(emu), E(emu)); break;
        case 0xA4: A(emu) = and_u8_u8(emu, A(emu), H(emu)); break;
        case 0xA5: A(emu) = and_u8_u8(emu, A(emu), L(emu)); break;
        case 0xA6: A(emu) = and_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0xA7: A(emu) = and_u8_u8(emu, A(emu), A(emu)); break;
        case 0xA8: A(emu) = xor_u8_u8(emu, A(emu), B(emu)); break;
        case 0xA9: A(emu) = xor_u8_u8(emu, A(emu), C(emu)); break;
        case 0xAA: A(emu) = xor_u8_u8(emu, A(emu), D(emu)); break;
        case 0xAB: A(emu) = xor_u8_u8(emu, A(emu), E(emu)); break;
        case 0xAC: A(emu) = xor_u8_u8(emu, A(emu), H(emu)); break;
        case 0xAD: A(emu) = xor_u8_u8(emu, A(emu), L(emu)); break;
        case 0xAE: A(emu) = xor_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0xAF: A(emu) = xor_u8_u8(emu, A(emu), A(emu)); break;

        case 0xB0: A(emu) = or_u8_u8(emu, A(emu), B(emu)); break;
        case 0xB1: A(emu) = or_u8_u8(emu, A(emu), C(emu)); break;
        case 0xB2: A(emu) = or_u8_u8(emu, A(emu), D(emu)); break;
        case 0xB3: A(emu) = or_u8_u8(emu, A(emu), E(emu)); break;
        case 0xB4: A(emu) = or_u8_u8(emu, A(emu), H(emu)); break;
        case 0xB5: A(emu) = or_u8_u8(emu, A(emu), L(emu)); break;
        case 0xB6: A(emu) = or_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0xB7: A(emu) = or_u8_u8(emu, A(emu), A(emu)); break;
        case 0xB8: cp_u8_u8(emu, A(emu), B(emu)); break;
        case 0xB9: cp_u8_u8(emu, A(emu), C(emu)); break;
        case 0xBA: cp_u8_u8(emu, A(emu), D(emu)); break;
        case 0xBB: cp_u8_u8(emu, A(emu), E(emu)); break;
        case 0xBC: cp_u8_u8(emu, A(emu), H(emu)); break;
        case 0xBD: cp_u8_u8(emu, A(emu), L(emu)); break;
        case 0xBE: cp_u8_u8(emu, A(emu), read(emu, HL(emu))); break;
        case 0xBF: cp_u8_u8(emu, A(emu), A(emu)); break;

        case 0xC3: {
            u16 stuff = read_u16(emu); 
            emu->PC.entireByte = stuff - 1; 
            break;
        }
        case 0xCE: A(emu) = adc_u8_u8(emu, A(emu), read_u8(emu)); break;
        
        default: {
            printf("This instruction hasn't been implemented yet.\n");
            emu->run = false;
            break;
        }
    }
}