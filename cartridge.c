#include "cartridge.h"

void initCartridge(Cartridge* cart, uint8_t* fileData, size_t fileSize){
    if (fileData == NULL || fileSize < 0x4000) printf("Some issues with the file provided.\n");

    cart->file = fileData;

    memcpy(cart->logoCheckSum, &fileData[0x104], 0x30);    
    cart->licensee_code = fileData[0x145];

    memcpy(cart->title, &fileData[0x134], 11);
    memcpy(cart->manufacturer_code, &fileData[0x13f], 4);

    cart->cartridge_type = fileData[0x147];
    cart->romSize = fileData[0x148];
    cart->ramsize = fileData[0x149];
}

const char* stringify_new_cartridge_code(CARTRIDGE_TYPE code) {
    switch (code) {
        case CT_ROM_ONLY: return "ROM ONLY";
        case CT_MBC1: return "MBC1";
        case CT_MBC1_RAM: return "MBC1+RAM";
        case CT_MBC1_RAM_BATTERY: return "MBC1+RAM+BATTERY";
        case CT_MBC2: return "MBC2";
        case CT_MBC2_BATTERY: return "MBC2+BATTERY";
        case CT_ROM_RAM1: return "ROM+RAM 1";
        case CT_ROM_RAM1_BATTERY1: return "ROM+RAM+BATTERY 1";
        case CT_MMM01: return "MMM01";
        case CT_MMM01_RAM: return "MMM01+RAM";
        case CT_MMM01_RAM_BATTERY: return "MMM01+RAM+BATTERY";
        case CT_MBC3_TIMER_BATTERY: return "MBC3+TIMER+BATTERY";
        case CT_MBC3_TIMER_RAM_BATTERY2: return "MBC3+TIMER+RAM+BATTERY 2";
        case CT_MBC3: return "MBC3";
        case CT_MBC3_RAM2: return "MBC3+RAM 2";
        case CT_MBC3_RAM_BATTERY2: return "MBC3+RAM+BATTERY 2";
        case CT_MBC5: return "MBC5";
        case CT_MBC5_RAM: return "MBC5+RAM";
        case CT_MBC5_RAM_BATTERY: return "MBC5+RAM+BATTERY";
        case CT_MBC5_RUMBLE: return "MBC5+RUMBLE";
        case CT_MBC5_RUMBLE_RAM: return "MBC5+RUMBLE+RAM";
        case CT_MBC5_RUMBLE_RAM_BATTERY: return "MBC5+RUMBLE+RAM+BATTERY";
        case CT_MBC6: return "MBC6";
        case CT_MBC7_SENSOR_RUMBLE_RAM_BATTERY: return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
        case CT_POCKET_CAMERA: return "POCKET CAMERA";
        case CT_BANDAI_TAMA5: return "BANDAI TAMA5";
        case CT_HuC3: return "HuC3";
        case CT_HuC1_RAM_BATTERY: return "HuC1+RAM+BATTERY";
        default: return "Unknown";  /* Or not added yet */
    }
}


static char* stringify_new_license_code(LICENSEE_CODE code) {
    switch (code) {
        case LC_NONE: return "None";
        case LC_NINTENDO_RD1: return "Nintendo R&D1";
        case LC_CAPCOM: return "Capcom";
        case LC_ELECTRONIC_ARTS: return "Electronic Arts";
        case LC_HUDSON_SOFT: return "Hudson Soft";
        case LC_B_AI: return "b-ai";
        case LC_KSS: return "kss";
        case LC_POW: return "pow";
        case LC_PCM_COMPLETE: return "PCM Complete";
        case LC_SAN_X: return "san-x";
        case LC_KEMCO_JAPAN: return "Kemco Japan";
        case LC_SETA: return "seta";
        case LC_VIACOM: return "Viacom";
        case LC_NINTENDO: return "Nintendo";
        case LC_BANDAI: return "Bandai";
        case LC_OCEAN_ACCLAIM: return "Ocean/Acclaim";
        case LC_KONAMI: return "Konami";
        case LC_HECTOR: return "Hector";
        case LC_TAITO: return "Taito";
        case LC_HUDSON: return "Hudson";
        case LC_BANPRESTO: return "Banpresto";
        case LC_UBI_SOFT: return "Ubi Soft";
        case LC_ATLUS: return "Atlus";
        case LC_MALIBU: return "Malibu";
        case LC_ANGEL: return "angel";
        case LC_BULLET_PROOF: return "Bullet-Proof";
        case LC_IREM: return "irem";
        case LC_ABSOLUTE: return "Absolute";
        case LC_ACTIVISION: return "Activision";
        case LC_AMERICAN_SAMMY: return "American sammy";
        case LC_HI_TECH_ENTERTAINMENT: return "Hi tech entertainment";
        case LC_LJN: return "LJN";
        case LC_MATCHBOX: return "Matchbox";
        case LC_MATTEL: return "Mattel";
        case LC_MILTON_BRADLEY: return "Milton Bradley";
        case LC_TITUS: return "Titus";
        case LC_VIRGIN: return "Virgin";
        case LC_LUCASARTS: return "LucasArts";
        case LC_INTERPLAY: return "Interplay";
        case LC_BRODERBUND: return "Broderbund";
        case LC_SCI: return "sci";
        case LC_THQ: return "THQ";
        case LC_ACCOLADE: return "Accolade";
        case LC_MISAWA: return "misawa";
        case LC_LOZC: return "lozc";
        case LC_TOKUMA_SHOTEN_INTERMEDIA: return "Tokuma Shoten Intermedia";
        case LC_TSUKUDA_ORIGINAL: return "Tsukuda Original";
        case LC_CHUNSOFT: return "Chunsoft";
        case LC_VIDEO_SYSTEM: return "Video system";
        case LC_VARIE: return "Varie";
        case LC_YONEZAWA_S_PAL: return "Yonezawa/s’pal";
        case LC_KANEKO: return "Kaneko";
        case LC_PACK_IN_SOFT: return "Pack in soft";
        case LC_KONAMI_YUGIOH: return "Konami (Yu-Gi-Oh!)";
        default: return "No Code Found";
    }
}

static char* stringify_rom_size(ROM_SIZE code) {
    switch (code) {
        case ROM_32KB: return "32 KB";
        case ROM_64KB: return "64 KB";
        case ROM_128KB: return "128 KB";
        case ROM_256KB: return "256 KB";
        case ROM_512KB: return "512 KB";
        case ROM_1MB: return "1 MB";
        case ROM_2MB: return "2 MB";
        case ROM_4MB: return "4 MB";
        case ROM_8MB: return "8 MB";
        default: return "Unknown";
    }
}

static char* stringify_ram_size(RAM_SIZE code) {
    switch (code) {
        case EXT_RAM_0: return "No External Ram";
        case EXT_RAM_2KB: return "2 KB EXT RAM";
        case EXT_RAM_8KB: return "8 KB EXT RAM";
        case EXT_RAM_32KB: return "32 KB EXT RAM";
        case EXT_RAM_64KB: return "84 KB EXT RAM";
        case EXT_RAM_128KB: return "128 KB EXT RAM";
        default: return "Unknown";
    }
}

void print_cartridge(Cartridge* cart){
    printf("============== CARTRIDGE DETAILS ==============\n");
    printf("Title: %s\n", cart->title);
    printf("License Code (%u): %s\n", cart->licensee_code, 
        stringify_new_license_code(cart->licensee_code));
    printf("Manufacturer Code: %s\n", cart->manufacturer_code);
    printf("Cartridge type: %s\n", stringify_new_cartridge_code(cart->cartridge_type));
    printf("Ram Size: %s\n", stringify_ram_size(cart->ramsize));
    printf("Rom Size: %s\n", stringify_rom_size(cart->romSize));
    printf("===============================================\n");
}
