// TODO: Make FORCE_31KHZ configurable on menus?

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include <EEPROM.h>

#include "config.h"
#include "lcd_task.h"
#include "profile.h"

#include "io_bits.h"
#include "p2io_task.h"

const int lcdButtonPin = A0;

LiquidCrystal lcd(8, 11, 4, 5, 6, 7);

enum : uint8_t {
    BUTTON_NONE,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_SELECT,
};

enum MenuState : uint8_t {
    MENU_HOME = 0,
    MENU_GAME_PROFILES,
    MENU_CARD_OPERATION,
    MENU_DIPSW,

    MENU_BOOT
};

MenuState menuStateId = MENU_BOOT;
MenuState lastMenuStateId = MENU_HOME;
MenuState lastMenuCurSelectionMenuId = MENU_HOME;
int8_t menuCurSelection = 0, lastMenuCurSelection = 0;

const char *MAIN_MENU_ITEMS[] = {
    "Game Profiles",
    "Card Operations",
    "Dip Switches",
};

const uint8_t MENU_ITEM_COUNTS[] = {
    sizeof(MAIN_MENU_ITEMS) / sizeof(MAIN_MENU_ITEMS[0]),
    gameProfileCount,
    0,
    0,
};

uint8_t lastButtonPressed = BUTTON_NONE;
int8_t cardSelectionId = 0;
uint8_t curPinIdx[2] = {};
int8_t dipswSelectIdx = 0;

uint8_t getLcdButtonState() {
    int val = analogRead(lcdButtonPin);

    if (val <= 60)
        return BUTTON_RIGHT;
    else if (val <= 200)
        return BUTTON_UP;
    else if (val <= 400)
        return BUTTON_DOWN;
    else if (val <= 600)
        return BUTTON_LEFT;
    else if (val <= 800)
        return BUTTON_SELECT;

    return BUTTON_NONE;
}

void LCD_Init() {
    lcd.begin(16, 2);
    LCD_ResetState();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Loading...");
    lcd.setCursor(0, 1);
    lcd.print((const __FlashStringHelper *)(gameProfiles[curLoadedProfileId].name));
    delay(500);
}

void LCD_ResetState() {
    menuStateId = MENU_BOOT;
    lastMenuStateId = lastMenuCurSelectionMenuId = MENU_HOME;
    menuCurSelection = lastMenuCurSelection = 0;
    cardSelectionId = 0;
    curPinIdx[0] = curPinIdx[1] = 0;
    cardIoStatus = 0;
    dipswSelectIdx = 0;
}

void LCD_Task() {
    uint8_t curButton = getLcdButtonState();

    if (menuStateId == MENU_BOOT)
        menuStateId = MENU_HOME;
    else if (curButton == BUTTON_NONE || curButton == lastButtonPressed)
        goto skipMenuUpdate;

    // Button operations
    if (curButton == BUTTON_SELECT) {
        if (menuStateId == MENU_HOME) {
            switch (menuCurSelection) {
                case 0:
                    menuStateId = MENU_GAME_PROFILES;
                    break;
                case 1:
                    menuStateId = MENU_CARD_OPERATION;
                    break;
                case 2:
                    menuStateId = MENU_DIPSW;
                    break;
            }
        } else if (menuStateId == MENU_GAME_PROFILES) {
            menuStateId = MENU_HOME;
        } else if (menuStateId == MENU_CARD_OPERATION) {
            menuStateId = MENU_HOME;
        } else if (menuStateId == MENU_DIPSW) {
            menuStateId = MENU_HOME;

            // Only save dipswitch to EEPROM when exiting menu
            EEPROM.update(EEPROM_IDX_DIPSW, dipsw);
        }

        // TODO: This only works for one depth. If required, implement for further depths.
        if (menuStateId != lastMenuStateId) {
            if (menuStateId == lastMenuCurSelectionMenuId) {
                menuCurSelection = lastMenuCurSelection;
            } else {
                lastMenuCurSelection = menuCurSelection;
                menuCurSelection = 0;
            }

            lastMenuCurSelectionMenuId = lastMenuStateId;
            lastMenuStateId = menuStateId;
        }
    }

    if (menuStateId == MENU_CARD_OPERATION) {
        if (curButton == BUTTON_UP) {
            cardSelectionId--;

            if (cardSelectionId < 0)
                cardSelectionId = 1;

            cardIoStatus &= ~(0b011111 << (cardSelectionId * 6));
        } else if (curButton == BUTTON_DOWN) {
            cardSelectionId++;
            cardSelectionId %= 2;
            cardIoStatus &= ~(0b011111 << (cardSelectionId * 6));
        } else if (curButton == BUTTON_LEFT) {
            // Insert/Eject card
            cardStatus[cardSelectionId] = cardStatus[cardSelectionId] == CARD_EJECTED ? CARD_INSERTED : CARD_EJECTED;
            curPinIdx[cardSelectionId] = 0;
            cardIoStatus &= ~(0b111111 << (cardSelectionId * 6));

            if (cardStatus[cardSelectionId] == CARD_INSERTED)
              cardIoStatus |= P2IO_OTHER_CARDREADER_P1_TOGGLE_CARD << (cardSelectionId * 6);
        } else if (curButton == BUTTON_RIGHT) {
            // Insert next digit in pin
            const uint32_t cardBits[] = {
                P2IO_OTHER_CARDREADER_P1_KEYPAD_0,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_1,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_2,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_3,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_4,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_5,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_6,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_7,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_8,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_9,
                P2IO_OTHER_CARDREADER_P1_KEYPAD_00,
            };

            cardIoStatus &= ~(0b011111 << (cardSelectionId * 6));
            cardIoStatus |= cardBits[playerCards[cardSelectionId].pin[curPinIdx[cardSelectionId]] - '0'] << (cardSelectionId * 6);
            curPinIdx[cardSelectionId] = (curPinIdx[cardSelectionId] + 1) % 4;
        }
    } else if (menuStateId == MENU_GAME_PROFILES) {
        if (curButton == BUTTON_UP) {
            menuCurSelection--;
        } else if (curButton == BUTTON_DOWN) {
            menuCurSelection++;
        } else if (curButton == BUTTON_LEFT) {
            // Load game configuration
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Loading...");
            lcd.setCursor(0, 1);
            lcd.print((const __FlashStringHelper *)(gameProfiles[menuCurSelection].name));
            loadGameProfile(menuCurSelection);
        }

        if (menuCurSelection < 0)
            menuCurSelection = MENU_ITEM_COUNTS[menuStateId] - 1;
        else
            menuCurSelection %= MENU_ITEM_COUNTS[menuStateId];
    } else if (menuStateId == MENU_DIPSW) {
        if (curButton == BUTTON_UP || curButton == BUTTON_DOWN) {
            dipsw ^= 1 << (3 - dipswSelectIdx);
        } else if (curButton == BUTTON_LEFT) {
            dipswSelectIdx--;
        } else if (curButton == BUTTON_RIGHT) {
            dipswSelectIdx++;
        }

        if (dipswSelectIdx < 0)
            dipswSelectIdx = 3;
        else
            dipswSelectIdx %= 4;
    } else {
        if (curButton == BUTTON_UP) {
            menuCurSelection--;
        } else if (curButton == BUTTON_DOWN) {
            menuCurSelection++;
        }

        if (menuCurSelection < 0)
            menuCurSelection = MENU_ITEM_COUNTS[menuStateId] - 1;
        else
            menuCurSelection %= MENU_ITEM_COUNTS[menuStateId];
    }

    // Display menu
    lcd.clear();
    lcd.setCursor(0, 0);
    if (menuStateId == MENU_HOME) {
        lcd.print(">");
        lcd.print(MAIN_MENU_ITEMS[menuCurSelection]);

        if (menuCurSelection + 1 < MENU_ITEM_COUNTS[MENU_HOME]) {
            lcd.setCursor(0, 1);
            lcd.print(MAIN_MENU_ITEMS[menuCurSelection + 1]);
        }
    } else if (menuStateId == MENU_GAME_PROFILES) {
        lcd.print(">");

        for (int i = 0; i < 2 && menuCurSelection + i < gameProfileCount; i++) {
            lcd.setCursor(1, i);
            if (menuCurSelection + i == curLoadedProfileId) {
                lcd.print("*");
            }

            lcd.print((const __FlashStringHelper *)(gameProfiles[menuCurSelection + i].name));
        }
    } else if (menuStateId == MENU_CARD_OPERATION) {
        for (int i = 0; i < 2; i++) {
            lcd.setCursor(0, i);

            lcd.print(i == cardSelectionId ? ">" : " ");
            lcd.print("P");
            lcd.print(i + 1);

            if (cardStatus[i] == CARD_INSERTED)
                lcd.print(" INSERTED");
            else
                lcd.print(" EJECTED");

          lcd.setCursor(15, i);
          lcd.print(curPinIdx[i] + 1);
        }
    } else if (menuStateId == MENU_DIPSW) {
        lcd.setCursor(2 * dipswSelectIdx, 0);
        lcd.print("v");

        for (int i = 0; i < 4; i++) {
            lcd.setCursor(2 * i, 1);
            lcd.print(!!(dipsw & (1 << (3 - i))));
        }
    }

skipMenuUpdate:
    lastButtonPressed = curButton;
}