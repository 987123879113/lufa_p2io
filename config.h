#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <avr/pgmspace.h>

#define INPUT_METHOD_PS4USB 1
#define INPUT_METHOD_PSX 2

#define EEPROM_IDX_PROFILE 0
#define EEPROM_IDX_DIPSW 1
#define EEPROM_IDX_CHECK1 2
#define EEPROM_IDX_CHECK2 3
#define EEPROM_IDX_CHECK3 4

enum GameType : uint8_t {
    GAMETYPE_DM = 0,
    GAMETYPE_GF,
    GAMETYPE_DDR,
    GAMETYPE_TOYSMARCH,
    GAMETYPE_THRILLDRIVE,
    GAMETYPE_DANCE864,
};

enum CardState : uint8_t {
    CARD_EJECTED = 0,
    CARD_INSERTED,
    CARD_ERROR,
};

typedef struct
{
    const char name[16];
    const GameType gameType;
    const uint8_t dongleBlack[40];
} GameProject;

typedef struct
{
    const char pin[4];
    const uint8_t cardId[40];
} CardInfo;

extern CardState cardStatus[2];
extern GameType gameType;
extern int curLoadedProfileId;
extern uint8_t dipsw;

#include "config_private.h"

const int gameProfileCount = sizeof(gameProfiles) / sizeof(gameProfiles[0]);

#endif
