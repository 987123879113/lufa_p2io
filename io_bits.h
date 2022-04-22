#ifndef __IO_BITS_H__
#define __IO_BITS_H__

constexpr uint32_t P2IO_JAMMA_IO_TEST = 0x10000000;
constexpr uint32_t P2IO_JAMMA_IO_COIN1 = 0x20000000;
constexpr uint32_t P2IO_JAMMA_IO_COIN2 = 0x80000000;
constexpr uint32_t P2IO_JAMMA_IO_SERVICE = 0x40000000;
constexpr uint32_t P2IO_JAMMA_IO_SERVICE2 = 0x00000080;

constexpr uint32_t P2IO_JAMMA_GF_P1_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_GF_P1_PICK = 0x00000200;
constexpr uint32_t P2IO_JAMMA_GF_P1_WAILING = 0x00000400;
constexpr uint32_t P2IO_JAMMA_GF_P1_EFFECT2 = 0x00000800;
constexpr uint32_t P2IO_JAMMA_GF_P1_EFFECT1 = 0x00001000;
constexpr uint32_t P2IO_JAMMA_GF_P1_EFFECT3 = P2IO_JAMMA_GF_P1_EFFECT1 | P2IO_JAMMA_GF_P1_EFFECT2;
constexpr uint32_t P2IO_JAMMA_GF_P1_R = 0x00002000;
constexpr uint32_t P2IO_JAMMA_GF_P1_G = 0x00004000;
constexpr uint32_t P2IO_JAMMA_GF_P1_B = 0x00008000;

constexpr uint32_t P2IO_JAMMA_GF_P2_START = 0x00010000;
constexpr uint32_t P2IO_JAMMA_GF_P2_PICK = 0x00020000;
constexpr uint32_t P2IO_JAMMA_GF_P2_WAILING = 0x00040000;
constexpr uint32_t P2IO_JAMMA_GF_P2_EFFECT2 = 0x00080000;
constexpr uint32_t P2IO_JAMMA_GF_P2_EFFECT1 = 0x00100000;
constexpr uint32_t P2IO_JAMMA_GF_P2_EFFECT3 = P2IO_JAMMA_GF_P2_EFFECT1 | P2IO_JAMMA_GF_P2_EFFECT2;
constexpr uint32_t P2IO_JAMMA_GF_P2_R = 0x00200000;
constexpr uint32_t P2IO_JAMMA_GF_P2_G = 0x00400000;
constexpr uint32_t P2IO_JAMMA_GF_P2_B = 0x00800000;

constexpr uint32_t P2IO_JAMMA_DM_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_DM_HIHAT = 0x00000200;
constexpr uint32_t P2IO_JAMMA_DM_SNARE = 0x00000400;
constexpr uint32_t P2IO_JAMMA_DM_HIGH_TOM = 0x00000800;
constexpr uint32_t P2IO_JAMMA_DM_LOW_TOM = 0x00001000;
constexpr uint32_t P2IO_JAMMA_DM_CYMBAL = 0x00002000;
constexpr uint32_t P2IO_JAMMA_DM_BASS_DRUM = 0x00008000;
constexpr uint32_t P2IO_JAMMA_DM_SELECT_L = 0x00080000;
constexpr uint32_t P2IO_JAMMA_DM_SELECT_R = 0x00100000;

constexpr uint32_t P2IO_JAMMA_DDR_P1_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_DDR_P1_LEFT = 0x00004000;
constexpr uint32_t P2IO_JAMMA_DDR_P1_RIGHT = 0x00008000;
constexpr uint32_t P2IO_JAMMA_DDR_P1_FOOT_UP = 0x00000200;
constexpr uint32_t P2IO_JAMMA_DDR_P1_FOOT_DOWN = 0x00000400;
constexpr uint32_t P2IO_JAMMA_DDR_P1_FOOT_LEFT = 0x00000800;
constexpr uint32_t P2IO_JAMMA_DDR_P1_FOOT_RIGHT = 0x00001000;

constexpr uint32_t P2IO_JAMMA_DDR_P2_START = 0x00010000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_LEFT = 0x00400000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_RIGHT = 0x00800000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_FOOT_UP = 0x00020000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_FOOT_DOWN = 0x00040000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_FOOT_LEFT = 0x00080000;
constexpr uint32_t P2IO_JAMMA_DDR_P2_FOOT_RIGHT = 0x00100000;

constexpr uint32_t P2IO_JAMMA_THRILLDRIVE_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_DOWN = 0x00000200;
constexpr uint32_t P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_UP = 0x00000400;
constexpr uint32_t P2IO_OTHER_THRILLDRIVE_BELT = 0x00000010;

constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_LEFT = 0x00000800;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_RIGHT = 0x00001000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P1_CYMBAL = 0x00002000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P1_DRUML = 0x00004000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P1_DRUMR = 0x00008000;

constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_START = 0x00010000;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_LEFT = 0x00080000;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_RIGHT = 0x00100000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P2_CYMBAL = 0x00020000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P2_DRUML = 0x00040000;
constexpr uint32_t P2IO_OTHER_TOYSMARCH_P2_DRUMR = 0x00200000;
#endif