#ifndef gbc_cartridge
#define gbc_cartridge
#include "common.h"

typedef enum {
    LC_NONE = 0x00,
    LC_NINTENDO_RD1 = 0x01,
    LC_CAPCOM = 0x08,
    LC_ELECTRONIC_ARTS = 0x13,
    LC_HUDSON_SOFT = 0x18,
    LC_B_AI = 0x19,
    LC_KSS = 0x20,
    LC_POW = 0x22,
    LC_PCM_COMPLETE = 0x24,
    LC_SAN_X = 0x25,
    LC_KEMCO_JAPAN = 0x28,
    LC_SETA = 0x29,
    LC_VIACOM = 0x30,
    LC_NINTENDO = 0x31,
    LC_BANDAI = 0x32,
    LC_OCEAN_ACCLAIM = 0x33,
    LC_KONAMI = 0x34,
    LC_HECTOR = 0x35,
    LC_TAITO = 0x37,
    LC_HUDSON = 0x38,
    LC_BANPRESTO = 0x39,
    LC_UBI_SOFT = 0x41,
    LC_ATLUS = 0x42,
    LC_MALIBU = 0x44,
    LC_ANGEL = 0x46,
    LC_BULLET_PROOF = 0x47,
    LC_IREM = 0x49,
    LC_ABSOLUTE = 0x50,
    LC_ACTIVISION = 0x52,
    LC_AMERICAN_SAMMY = 0x53,
    LC_HI_TECH_ENTERTAINMENT = 0x55,
    LC_LJN = 0x56,
    LC_MATCHBOX = 0x57,
    LC_MATTEL = 0x58,
    LC_MILTON_BRADLEY = 0x59,
    LC_TITUS = 0x60,
    LC_VIRGIN = 0x61,
    LC_LUCASARTS = 0x64,
    LC_INTERPLAY = 0x71,
    LC_BRODERBUND = 0x72,
    LC_SCI = 0x75,
    LC_THQ = 0x78,
    LC_ACCOLADE = 0x79,
    LC_MISAWA = 0x80,
    LC_LOZC = 0x83,
    LC_TOKUMA_SHOTEN_INTERMEDIA = 0x86,
    LC_TSUKUDA_ORIGINAL = 0x87,
    LC_CHUNSOFT = 0x91,
    LC_VIDEO_SYSTEM = 0x92,
    LC_VARIE = 0x95,
    LC_YONEZAWA_S_PAL = 0x96,
    LC_KANEKO = 0x97,
    LC_PACK_IN_SOFT = 0x99,
    LC_KONAMI_YUGIOH = 0xA4
} LICENSEE_CODE;

typedef enum {
    CT_ROM_ONLY = 0x00,
    CT_MBC1 = 0x01,
    CT_MBC1_RAM = 0x02,
    CT_MBC1_RAM_BATTERY = 0x03,
    CT_MBC2 = 0x05,
    CT_MBC2_BATTERY = 0x06,
    CT_ROM_RAM1 = 0x08,
    CT_ROM_RAM1_BATTERY1 = 0x09,
    CT_MMM01 = 0x0B,
    CT_MMM01_RAM = 0x0C,
    CT_MMM01_RAM_BATTERY = 0x0D,
    CT_MBC3_TIMER_BATTERY = 0x0F,
    CT_MBC3_TIMER_RAM_BATTERY2 = 0x10,
    CT_MBC3 = 0x11,
    CT_MBC3_RAM2 = 0x12,
    CT_MBC3_RAM_BATTERY2 = 0x13,
    CT_MBC5 = 0x19,
    CT_MBC5_RAM = 0x1A,
    CT_MBC5_RAM_BATTERY = 0x1B,
    CT_MBC5_RUMBLE = 0x1C,
    CT_MBC5_RUMBLE_RAM = 0x1D,
    CT_MBC5_RUMBLE_RAM_BATTERY = 0x1E,
    CT_MBC6 = 0x20,
    CT_MBC7_SENSOR_RUMBLE_RAM_BATTERY = 0x22,
    CT_POCKET_CAMERA = 0xFC,
    CT_BANDAI_TAMA5 = 0xFD,
    CT_HuC3 = 0xFE,
    CT_HuC1_RAM_BATTERY = 0xFF
} CARTRIDGE_TYPE;

typedef enum {
    ROM_32KB,
    ROM_64KB,
    ROM_128KB,
    ROM_256KB,
    ROM_512KB,
    ROM_1MB,
    ROM_2MB,
    ROM_4MB,
    ROM_8MB
} ROM_SIZE;

typedef enum {
    /* External Ram */
    EXT_RAM_0,
    EXT_RAM_2KB,
    EXT_RAM_8KB,
    EXT_RAM_32KB,
    EXT_RAM_128KB,
    EXT_RAM_64KB
} RAM_SIZE;

typedef struct {
    uint8_t* file;
    uint8_t* logoCheckSum[0x30];

    char title[11];
    char manufacturer_code[4];
    
    LICENSEE_CODE licensee_code;
    CARTRIDGE_TYPE cartridge_type;

    ROM_SIZE romSize;
    RAM_SIZE ramsize;

} Cartridge;


void initCartridge(Cartridge* cart, uint8_t* fileData, size_t fileSize);
void print_cartridge(Cartridge* cart);

#endif