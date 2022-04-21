// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.cpp
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.h

#include <stdint.h>
#include <PsxControllerBitBang.h>

#include "config.h"

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

constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_START = 0x00000100;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_LEFT = 0x00000800;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P1_RIGHT = 0x00001000;

constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_START = 0x00010000;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_LEFT = 0x00080000;
constexpr uint32_t P2IO_JAMMA_TOYSMARCH_P2_RIGHT = 0x00100000;

constexpr uint8_t PIN_PS2_CMD = 16;
constexpr uint8_t PIN_PS2_DAT = 14;
constexpr uint8_t PIN_PS2_CLK = 15;
constexpr uint8_t PIN_PS2_ATT = 10;

const unsigned long POLLING_INTERVAL = 1000U / 50U; // Setting too low results in too much time being wasted on I/O polling

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

boolean haveController = false;
bool coinWasInserted[2] = {false, false};

void Input_Task()
{
  static unsigned long last = 0;

  if (millis () - last >= POLLING_INTERVAL) {
    last = millis ();
    if (!haveController) {
      if (psx.begin()) {
        if (psx.enterConfigMode()) {
          psx.enableAnalogSticks();
          psx.exitConfigMode();
          }

        haveController = true;
      }
    } else {
      if (!psx.read()) {
        haveController = false;
      } else {
        jammaIoStatus = 0xfffffffc | (jammaIoStatus & 2);

        if (psx.buttonPressed(PSB_SELECT)) {
          // Map some P2 controls and other inputs to holding select
          if (psx.buttonPressed(PSB_L2)) {
            jammaIoStatus &= ~P2IO_JAMMA_IO_TEST;
          } else {
            jammaIoStatus |= P2IO_JAMMA_IO_TEST;
          }

          if (psx.buttonPressed(PSB_R2)) {
            jammaIoStatus &= ~P2IO_JAMMA_IO_SERVICE;
          } else {
            jammaIoStatus |= P2IO_JAMMA_IO_SERVICE;
          }

#if GAME_TYPE == GAMETYPE_DDR
          if (psx.buttonPressed(PSB_START)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_START;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P2_START;
          }

          if (psx.buttonPressed(PSB_L1)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_LEFT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P2_LEFT;
          }

          if (psx.buttonPressed(PSB_R1)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P2_RIGHT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P2_RIGHT;
          }
#elif GAME_TYPE == GAMETYPE_GF
          if (psx.buttonPressed(PSB_START)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P2_START;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P2_START;
          }
#endif
        } else {
          if (psx.buttonPressed(PSB_L3)) {
            jammaIoStatus &= ~P2IO_JAMMA_IO_COIN1;

            if (!coinWasInserted[0])
              coinsInserted[0]++;

            coinWasInserted[0] = true;
          } else {
            jammaIoStatus |= P2IO_JAMMA_IO_COIN1;
            coinWasInserted[0] = false;
          }

          if (psx.buttonPressed(PSB_R3)) {
            jammaIoStatus &= ~P2IO_JAMMA_IO_COIN2;

            if (!coinWasInserted[1])
              coinsInserted[1]++;

            coinWasInserted[1] = true;
          } else {
            jammaIoStatus |= P2IO_JAMMA_IO_COIN2;
            coinWasInserted[1] = false;
          }

#if GAME_TYPE == GAMETYPE_DDR
          // P1 controls
          if (psx.buttonPressed(PSB_PAD_UP) || psx.buttonPressed(PSB_TRIANGLE)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_UP;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_UP;
          }
          if (psx.buttonPressed(PSB_PAD_LEFT) || psx.buttonPressed(PSB_SQUARE)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_LEFT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_LEFT;
          }
          if (psx.buttonPressed(PSB_PAD_RIGHT) || psx.buttonPressed(PSB_CIRCLE)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
          }
          if (psx.buttonPressed(PSB_PAD_DOWN) || psx.buttonPressed(PSB_CROSS)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_FOOT_DOWN;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_DOWN;
          }

          if (psx.buttonPressed(PSB_L1)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_LEFT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_LEFT;
          }

          if (psx.buttonPressed(PSB_R1)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_RIGHT;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_RIGHT;
          }

          if (psx.buttonPressed(PSB_START)) {
            jammaIoStatus &= ~P2IO_JAMMA_DDR_P1_START;
          } else {
            jammaIoStatus |= P2IO_JAMMA_DDR_P1_START;
          }
#elif GAME_TYPE == GAMETYPE_GF
          if (psx.buttonPressed(PSB_L2)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_R;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_R;
          }

          if (psx.buttonPressed(PSB_L1)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_G;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_G;
          }

          if (psx.buttonPressed(PSB_PAD_DOWN)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_B;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_B;
          }

          if (psx.buttonPressed(PSB_R1) || psx.buttonPressed(PSB_R2)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_WAILING;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_WAILING;
          }

          if (psx.buttonPressed(PSB_CIRCLE)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_PICK;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_PICK;
          }

          if (psx.buttonPressed(PSB_START)) {
            jammaIoStatus &= ~P2IO_JAMMA_GF_P1_START;
          } else {
            jammaIoStatus |= P2IO_JAMMA_GF_P1_START;
          }
#endif
        }
      }
    }
  }
}
