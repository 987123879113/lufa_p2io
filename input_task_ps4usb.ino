#include "config.h"

#if INPUT_METHOD == INPUT_METHOD_PS4USB
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.cpp
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.h

#include <PS4USB.h>
#include <stdint.h>

#include "common.h"
#include "lcd_task.h"

// TODO: Move the required variables elsewhere?
#include "acio.h"
#include "io_bits.h"
#include "p2io_task.h"
#include "thrilldrive_handle.h"

bool coinWasInserted[2] = {false, false};
bool guitarKnobHeld[2] = {false, false};
bool drumInputHit[5] = {};
unsigned long drumInputTimer[5] = {};

USB Usb;
PS4USB PS4(&Usb);

void Input_Init_PS4USB() {
    Usb.Init();
}

void Input_Reset_PS4USB() {
    // TODO: Call this somewhere
    coinWasInserted[0] = coinWasInserted[1] = false;
    guitarKnobHeld[0] = guitarKnobHeld[1] = false;

    for (int i = 0; i < 5; i++) {
        drumInputHit[i] = false;
        drumInputTimer[i] = 0;
    }
}

void Input_Task_PS4USB() {
    Usb.Task();

    if (!PS4.connected())
        return;

    jammaIoStatus = jammaIoStatus & 2;
    otherIoStatus = 0;
    // cardIoStatus = 0;

    const bool is_pressed_ps = PS4.getButtonPress(PS);
    const bool is_pressed_l1 = PS4.getButtonPress(L1);
    const bool is_pressed_l2 = PS4.getButtonPress(L2);
    const bool is_pressed_l3 = PS4.getButtonPress(L3);
    const bool is_pressed_r1 = PS4.getButtonPress(R1);
    const bool is_pressed_r2 = PS4.getButtonPress(R2);
    const bool is_pressed_r3 = PS4.getButtonPress(R3);
    const bool is_pressed_options = PS4.getButtonPress(OPTIONS);
    const bool is_pressed_share = PS4.getButtonPress(SHARE);
    const bool is_pressed_touchpad = PS4.getButtonPress(TOUCHPAD);
    const bool is_pressed_circle = PS4.getButtonPress(CIRCLE);
    const bool is_pressed_square = PS4.getButtonPress(SQUARE);
    const bool is_pressed_triangle = PS4.getButtonPress(TRIANGLE);
    const bool is_pressed_cross = PS4.getButtonPress(CROSS);
    const bool is_pressed_up = PS4.getButtonPress(UP);
    const bool is_pressed_down = PS4.getButtonPress(DOWN);
    const bool is_pressed_left = PS4.getButtonPress(LEFT);
    const bool is_pressed_right = PS4.getButtonPress(RIGHT);

    bool isTouchPadLeftPressed = false;
    bool isTouchPadRightPressed = false;
    if (is_pressed_touchpad) {
        uint16_t x = PS4.getX(PS4.isTouching(1) ? 1 : 0);
        isTouchPadLeftPressed = x <= 500;
        isTouchPadRightPressed = x >= 1500;
    }

    if (is_pressed_ps) {
        // Map some P2 controls and other inputs to holding select
        if (is_pressed_l2) {
            jammaIoStatus |= P2IO_JAMMA_IO_TEST;
        }

        if (is_pressed_r2) {
            jammaIoStatus |= P2IO_JAMMA_IO_SERVICE;
        }

        if (gameType == GAMETYPE_DDR) {
            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P2_START;
            }

            if (is_pressed_l1) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P2_LEFT;
            }

            if (is_pressed_r1) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P2_RIGHT;
            }
        } else if (gameType == GAMETYPE_GF) {
            if (isTouchPadRightPressed) {
                if (!guitarKnobHeld[1]) {
                    guitarKnobHeld[1] = true;
                    guitarKnobValue[1]++;
                }
            } else if (isTouchPadLeftPressed) {
                if (!guitarKnobHeld[1]) {
                    guitarKnobHeld[1] = true;
                    guitarKnobValue[1]--;
                }
            } else {
                guitarKnobHeld[1] = false;
            }

            if (guitarKnobValue[1] < 0)
                guitarKnobValue[1] = 3;
            else
                guitarKnobValue[1] %= 4;

            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_GF_P2_START;
            }
        } else if (gameType == GAMETYPE_TOYSMARCH) {
            if (isTouchPadLeftPressed) {
                jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P2_LEFT;
            }

            if (isTouchPadRightPressed) {
                jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P2_RIGHT;
            }
        }
    } else {
        if (is_pressed_l3) {
            jammaIoStatus |= P2IO_JAMMA_IO_COIN1;

            if (!coinWasInserted[0])
                coinsInserted[0]++;

            coinWasInserted[0] = true;
        } else {
            coinWasInserted[0] = false;
        }

        coinWasInserted[1] = false;
        if (is_pressed_r3) {
            jammaIoStatus |= P2IO_JAMMA_IO_COIN2;

            if (!coinWasInserted[1])
                coinsInserted[1]++;
        } else {
            coinWasInserted[1] = true;
        }

        if (gameType == GAMETYPE_DDR) {
            if (isTouchPadLeftPressed) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_LEFT;
            }

            if (isTouchPadRightPressed) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_RIGHT;
            }

            if (is_pressed_up || is_pressed_triangle) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_UP;
            }
            if (is_pressed_left || is_pressed_square) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_LEFT;
            }
            if (is_pressed_right || is_pressed_circle) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
            }
            if (is_pressed_down || is_pressed_cross) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_DOWN;
            }

            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_DDR_P1_START;
            }
        } else if (gameType == GAMETYPE_GF) {
            if (isTouchPadRightPressed) {
                if (!guitarKnobHeld[0]) {
                    guitarKnobHeld[0] = true;
                    guitarKnobValue[0]++;
                }
            } else if (isTouchPadLeftPressed) {
                if (!guitarKnobHeld[0]) {
                    guitarKnobHeld[0] = true;
                    guitarKnobValue[0]--;
                }
            } else {
                guitarKnobHeld[0] = false;
            }

            if (guitarKnobValue[0] < 0)
                guitarKnobValue[0] = 3;
            else
                guitarKnobValue[0] %= 4;

            if (is_pressed_l2) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_R;
            }

            if (is_pressed_l1) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_G;
            }

            if (is_pressed_down) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_B;
            }

            if (is_pressed_r1 || is_pressed_r2) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_WAILING;
            }

            if (is_pressed_circle || is_pressed_cross) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_PICK;
            }

            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_GF_P1_START;
            }
        } else if (gameType == GAMETYPE_DM) {
            if (isTouchPadLeftPressed) {
                jammaIoStatus |= P2IO_JAMMA_DM_SELECT_L;
            }

            if (isTouchPadRightPressed) {
                jammaIoStatus |= P2IO_JAMMA_DM_SELECT_R;
            }

            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_DM_START;
            }

            if (is_pressed_down || is_pressed_cross) {
                jammaIoStatus |= P2IO_JAMMA_DM_BASS_DRUM;
            }

            unsigned long curtime = millis();
            if (is_pressed_l1 || is_pressed_l2) {
                if (PS4.getButtonClick(L1) || PS4.getButtonClick(L2)) {
                    drumInputTimer[0] = curtime;
                    drumInputHit[0] = true;
                }
            } else {
                drumInputHit[0] = false;
            }

            if (is_pressed_r1 || is_pressed_r2) {
                if (PS4.getButtonClick(R1) || PS4.getButtonClick(R2)) {
                    drumInputTimer[1] = curtime;
                    drumInputHit[1] = true;
                }
            } else {
                drumInputHit[1] = false;
            }

            if (is_pressed_up || is_pressed_triangle) {
                if (PS4.getButtonClick(UP) || PS4.getButtonClick(TRIANGLE)) {
                    drumInputTimer[2] = curtime;
                    drumInputHit[2] = true;
                }
            } else {
                drumInputHit[2] = false;
            }

            if (is_pressed_left || is_pressed_square) {
                if (PS4.getButtonClick(LEFT) || PS4.getButtonClick(SQUARE)) {
                    drumInputTimer[3] = curtime;
                    drumInputHit[3] = true;
                }
            } else {
                drumInputHit[3] = false;
            }

            if (is_pressed_right || is_pressed_circle) {
                if (PS4.getButtonClick(RIGHT) || PS4.getButtonClick(CIRCLE)) {
                    drumInputTimer[4] = curtime;
                    drumInputHit[4] = true;
                }
            } else {
                drumInputHit[4] = false;
            }

            // These need to be on a timer or else the game doesn't see the inputs in time
            const int DRUM_INPUT_TIMEOUT = 25;
            if (drumInputHit[0] && curtime - drumInputTimer[0] < DRUM_INPUT_TIMEOUT)
                jammaIoStatus |= P2IO_JAMMA_DM_HIHAT;
            if (drumInputHit[1] && curtime - drumInputTimer[1] < DRUM_INPUT_TIMEOUT)
                jammaIoStatus |= P2IO_JAMMA_DM_CYMBAL;
            if (drumInputHit[2] && curtime - drumInputTimer[2] < DRUM_INPUT_TIMEOUT)
                jammaIoStatus |= P2IO_JAMMA_DM_HIGH_TOM;
            if (drumInputHit[3] && curtime - drumInputTimer[3] < DRUM_INPUT_TIMEOUT)
                jammaIoStatus |= P2IO_JAMMA_DM_SNARE;
            if (drumInputHit[4] && curtime - drumInputTimer[4] < DRUM_INPUT_TIMEOUT)
                jammaIoStatus |= P2IO_JAMMA_DM_LOW_TOM;
        } else if (gameType == GAMETYPE_THRILLDRIVE) {
            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_START;
            }

            if (is_pressed_share) {
                otherIoStatus |= P2IO_OTHER_THRILLDRIVE_BELT;
            }

            if (is_pressed_triangle) {
                jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_DOWN;
            } else if (is_pressed_square) {
                jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_UP;
            }

            uint16_t wheel = 0xffff - (0xffff * (PS4.getAnalogHat(LeftHatX) / 255.0f));
            uint16_t accel = 0xffff * (PS4.getAnalogButton(R2) / 255.0f);
            uint16_t brake = 0xffff * (PS4.getAnalogButton(L2) / 255.0f);

            // Override analog inputs with digital inputs
            if (is_pressed_circle) {
                accel = 0xffff;
            }

            if (is_pressed_cross) {
                brake = 0xffff;
            }

            if (is_pressed_left) {
                wheel = 0xffff;
            } else if (is_pressed_right) {
                wheel = 0;
            }

            acio_device* aciodev = (acio_device*)serialDevices[1];
            thrilldrive_handle_device* handle = (thrilldrive_handle_device*)aciodev->devices[0];
            if (handle->wheelCalibrationHack) {
                // The game wants to be able to use force feedback to force the wheel into specific positions for calibration testing during boot.
                // I don't know enough about FFB to try to emulate it in software so just force the wheel position during calibration only.
                wheel = 0xffff - (0xffff * (handle->wheelForceFeedback / 127.0f));
            }

            analogIoStatus[0] = BigEndian16(wheel);
            analogIoStatus[1] = BigEndian16(accel);
            analogIoStatus[2] = BigEndian16(brake);
        } else if (gameType == GAMETYPE_TOYSMARCH) {
            if (isTouchPadLeftPressed) {
                jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_LEFT;
            }

            if (isTouchPadRightPressed) {
                jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_RIGHT;
            }

            if (is_pressed_options) {
                jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_START;
            }

            if (is_pressed_l1 || is_pressed_r1) {
                otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_CYMBAL;
            }

            if (is_pressed_left || is_pressed_right || is_pressed_down) {
                otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_DRUML;
            }

            if (is_pressed_circle || is_pressed_square || is_pressed_cross) {
                otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_DRUMR;
            }
        }
    }
}
#endif