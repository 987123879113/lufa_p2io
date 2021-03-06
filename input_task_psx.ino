#include "config.h"

#if INPUT_METHOD == INPUT_METHOD_PSX
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.cpp
// Reference: https://github.com/987123879113/pcsx2/blob/p2io/pcsx2/USB/usb-python2/usb-python2.h

#include <PsxControllerBitBang.h>
#include <stdint.h>

#include "common.h"

// TODO: Move the required variables elsewhere?
#include "acio.h"
#include "io_bits.h"
#include "p2io_task.h"
#include "thrilldrive_handle.h"

constexpr uint8_t PIN_PS2_CMD = 16;
constexpr uint8_t PIN_PS2_DAT = 14;
constexpr uint8_t PIN_PS2_CLK = 15;
constexpr uint8_t PIN_PS2_ATT = 10;

const unsigned long POLLING_INTERVAL_MS = 15;  // Setting too low results in too much time being wasted on I/O polling

PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

boolean haveController = false;
bool coinWasInserted[2] = {false, false};

bool isGuitarController = false;
bool isDrumController = false;

void Input_Init_PSX() {
}

void Input_Task_PSX() {
    static unsigned long last = 0;
    unsigned long cur_ms = millis();

    if (cur_ms - last >= POLLING_INTERVAL_MS) {
        last = cur_ms;
        if (!haveController) {
            if (psx.begin()) {
                if (psx.enterConfigMode()) {
                    psx.enableAnalogSticks();
                    psx.exitConfigMode();
                }

                isDrumController = psx.buttonPressed(PSB_PAD_LEFT) && psx.buttonPressed(PSB_PAD_RIGHT) && psx.buttonPressed(PSB_PAD_UP);
                isGuitarController = psx.buttonPressed(PSB_PAD_LEFT) && psx.buttonPressed(PSB_PAD_RIGHT) && !isDrumController;

                haveController = true;
            }
        } else {
            if (!psx.read()) {
                haveController = false;
            } else {
                jammaIoStatus = jammaIoStatus & 3;
                otherIoStatus = 0;
                // cardIoStatus = 0;

                if (psx.buttonPressed(PSB_SELECT)) {
                    // Map some P2 controls and other inputs to holding select
                    if (psx.buttonPressed(PSB_L2)) {
                        jammaIoStatus |= P2IO_JAMMA_IO_TEST;
                    }

                    if (psx.buttonPressed(PSB_R2)) {
                        jammaIoStatus |= P2IO_JAMMA_IO_SERVICE;
                    }

                    if (gameType == GAMETYPE_DDR) {
                        if (psx.buttonPressed(PSB_START)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P2_START;
                        }

                        if (psx.buttonPressed(PSB_L1)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P2_LEFT;
                        }

                        if (psx.buttonPressed(PSB_R1)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P2_RIGHT;
                        }
                    } else if (gameType == GAMETYPE_GF) {
                        if (psx.buttonPressed(PSB_START)) {
                            jammaIoStatus |= P2IO_JAMMA_GF_P2_START;
                        }

                        if (isGuitarController) {
                            if (!psx.buttonPressed(PSB_START)) {
                                jammaIoStatus |= P2IO_JAMMA_IO_COIN1;

                                if (!coinWasInserted[0])
                                    coinsInserted[0]++;

                                coinWasInserted[0] = true;
                            } else {
                                coinWasInserted[0] = false;
                            }
                        }
                    } else if (gameType == GAMETYPE_DM) {
                        if (psx.buttonPressed(PSB_L1)) {
                            jammaIoStatus |= P2IO_JAMMA_DM_SELECT_L;
                        }

                        if (psx.buttonPressed(PSB_R1)) {
                            jammaIoStatus |= P2IO_JAMMA_DM_SELECT_R;
                        }

                        if (isDrumController) {
                            if (!psx.buttonPressed(PSB_L1) && !psx.buttonPressed(PSB_R1)) {
                                jammaIoStatus |= P2IO_JAMMA_IO_COIN1;

                                if (!coinWasInserted[0])
                                    coinsInserted[0]++;

                                coinWasInserted[0] = true;
                            } else {
                                coinWasInserted[0] = false;
                            }
                        }
                    } else if (gameType == GAMETYPE_TOYSMARCH) {
                        if (psx.buttonPressed(PSB_L1)) {
                            jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_LEFT;
                        }

                        if (psx.buttonPressed(PSB_R1)) {
                            jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_RIGHT;
                        }

                        if (psx.buttonPressed(PSB_L2)) {
                            jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P2_LEFT;
                        }

                        if (psx.buttonPressed(PSB_R2)) {
                            jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P2_RIGHT;
                        }
                    } else if (gameType == GAMETYPE_THRILLDRIVE) {
                        if (psx.buttonPressed(PSB_TRIANGLE)) {
                            otherIoStatus |= P2IO_OTHER_THRILLDRIVE_BELT;
                        }
                    }
                } else {
                    if (psx.buttonPressed(PSB_L3)) {
                        jammaIoStatus |= P2IO_JAMMA_IO_COIN1;

                        if (!coinWasInserted[0])
                            coinsInserted[0]++;

                        coinWasInserted[0] = true;
                    } else {
                        coinWasInserted[0] = false;
                    }

                    if (psx.buttonPressed(PSB_R3)) {
                        jammaIoStatus |= P2IO_JAMMA_IO_COIN2;

                        if (!coinWasInserted[1])
                            coinsInserted[1]++;

                        coinWasInserted[1] = true;
                    } else {
                        coinWasInserted[1] = false;
                    }

                    if (gameType == GAMETYPE_DDR) {
                        // P1 controls
                        if (psx.buttonPressed(PSB_PAD_UP) || psx.buttonPressed(PSB_TRIANGLE)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_UP;
                        }
                        if (psx.buttonPressed(PSB_PAD_LEFT) || psx.buttonPressed(PSB_SQUARE)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_LEFT;
                        }
                        if (psx.buttonPressed(PSB_PAD_RIGHT) || psx.buttonPressed(PSB_CIRCLE)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_RIGHT;
                        }
                        if (psx.buttonPressed(PSB_PAD_DOWN) || psx.buttonPressed(PSB_CROSS)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_FOOT_DOWN;
                        }

                        if (psx.buttonPressed(PSB_L1)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_LEFT;
                        }

                        if (psx.buttonPressed(PSB_R1)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_RIGHT;
                        }
                        if (psx.buttonPressed(PSB_START)) {
                            jammaIoStatus |= P2IO_JAMMA_DDR_P1_START;
                        }
                    } else if (gameType == GAMETYPE_GF) {
                        // PS1 guitar mapping
                        if (isGuitarController) {
                            jammaIoStatus |= P2IO_JAMMA_IO_COIN1;
                            coinWasInserted[0] = false;

                            if (psx.buttonPressed(PSB_R2)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_R;
                            }

                            if (psx.buttonPressed(PSB_CIRCLE)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_G;
                            }

                            if (psx.buttonPressed(PSB_TRIANGLE)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_B;
                            }

                            if (psx.buttonPressed(PSB_PAD_UP)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_PICK;
                            }

                            if (psx.buttonPressed(PSB_L2)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_WAILING;
                            }

                            if (psx.buttonPressed(PSB_START)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_START;
                            }
                        } else {
                            if (psx.buttonPressed(PSB_L2)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_R;
                            }

                            if (psx.buttonPressed(PSB_L1)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_G;
                            }

                            if (psx.buttonPressed(PSB_PAD_DOWN)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_B;
                            }

                            if (psx.buttonPressed(PSB_R1) || psx.buttonPressed(PSB_R2)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_WAILING;
                            }

                            if (psx.buttonPressed(PSB_CIRCLE) || psx.buttonPressed(PSB_CROSS)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_PICK;
                            }

                            if (psx.buttonPressed(PSB_START)) {
                                jammaIoStatus |= P2IO_JAMMA_GF_P1_START;
                            }
                        }
                    } else if (gameType == GAMETYPE_DM) {
                        if (isDrumController) {
                            if (psx.buttonPressed(PSB_START)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_START;
                            }

                            if (psx.buttonJustPressed(PSB_TRIANGLE)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_HIHAT;
                            }

                            if (psx.buttonJustPressed(PSB_CIRCLE)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_SNARE;
                            }

                            if (psx.buttonJustPressed(PSB_R2)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_HIGH_TOM;
                            }

                            if (psx.buttonJustPressed(PSB_R1)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_LOW_TOM;
                            }

                            if (psx.buttonJustPressed(PSB_CROSS)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_CYMBAL;
                            }

                            if (psx.buttonPressed(PSB_L2)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_BASS_DRUM;
                            }
                        } else {
                            if (psx.buttonPressed(PSB_START)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_START;
                            }

                            if (psx.buttonJustPressed(PSB_L1) || psx.buttonJustPressed(PSB_L2)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_HIHAT;
                            }

                            if (psx.buttonJustPressed(PSB_R1) || psx.buttonJustPressed(PSB_R2)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_CYMBAL;
                            }

                            if (psx.buttonJustPressed(PSB_PAD_UP) || psx.buttonJustPressed(PSB_TRIANGLE)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_HIGH_TOM;
                            }

                            if (psx.buttonJustPressed(PSB_PAD_LEFT) || psx.buttonJustPressed(PSB_SQUARE)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_SNARE;
                            }

                            if (psx.buttonPressed(PSB_PAD_DOWN) || psx.buttonJustPressed(PSB_CROSS)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_BASS_DRUM;
                            }

                            if (psx.buttonJustPressed(PSB_PAD_RIGHT) || psx.buttonJustPressed(PSB_CIRCLE)) {
                                jammaIoStatus |= P2IO_JAMMA_DM_LOW_TOM;
                            }
                        }
                    } else if (gameType == GAMETYPE_THRILLDRIVE) {
                        if (psx.buttonPressed(PSB_START)) {
                            jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_START;
                        }

                        if (psx.buttonPressed(PSB_TRIANGLE)) {
                            jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_DOWN;
                        } else if (psx.buttonPressed(PSB_SQUARE)) {
                            jammaIoStatus |= P2IO_JAMMA_THRILLDRIVE_GEARSHIFT_UP;
                        }

                        uint16_t wheel = 0x7fdf;
                        uint16_t accel = 0;
                        uint16_t brake = 0;

                        uint8_t x, y;
                        if (psx.getLeftAnalog(x, y)) {
                            float xf = x / 255.0f;

                            // Calculate for dead zones to make sure it centers properly when stick is released.
                            // Adjust as your controller needs it.
                            if (xf > 0.4f && xf < 0.6f)
                                xf = 0.5f;

                            wheel = 0xffff - (0xffff * xf);
                        }

                        if (psx.getRightAnalog(x, y)) {
                            float yf = y / 255.0f;

                            // Calculate for dead zones. Adjust as needed for your controller.
                            if (yf <= 0.42f) {
                                accel = 0xffff * ((0.42f - yf) / 0.42f);
                            } else if (yf >= 0.55f) {
                                brake = 0xffff * ((yf - 0.55f) / (1.0f - 0.55f));
                            }
                        }

                        // Override analog inputs with digital inputs
                        if (psx.buttonPressed(PSB_CIRCLE)) {
                            accel = 0xffff;
                        }

                        if (psx.buttonPressed(PSB_CROSS)) {
                            brake = 0xffff;
                        }

                        if (psx.buttonPressed(PSB_PAD_LEFT)) {
                            wheel = 0xffff;
                        } else if (psx.buttonPressed(PSB_PAD_RIGHT)) {
                            wheel = 0;
                        }

                        acio_device* aciodev = (acio_device*)serialDevices[1];
                        thrilldrive_handle_device* handle = (thrilldrive_handle_device*)aciodev->devices[0];
                        if (handle->wheelCalibrationHack) {
                            // The game wants to be able to use force feedback to force the wheel into specific positions for calibration testing during boot.
                            // I don't know enough about FFB to try to emulate it in software so just force the wheel position during calibration only.
                            wheel = 0xffff - (0xffff * (handle->wheelForceFeedback / 127.0));
                        }

                        analogIoStatus[0] = BigEndian16(wheel);
                        analogIoStatus[1] = BigEndian16(accel);
                        analogIoStatus[2] = BigEndian16(brake);
                    } else if (gameType == GAMETYPE_TOYSMARCH) {
                        if (psx.buttonPressed(PSB_START)) {
                            jammaIoStatus |= P2IO_JAMMA_TOYSMARCH_P1_START;
                        }

                        if (psx.buttonPressed(PSB_L1) || psx.buttonPressed(PSB_R1) || psx.buttonPressed(PSB_L2) || psx.buttonPressed(PSB_R2) || psx.buttonPressed(PSB_PAD_UP) || psx.buttonPressed(PSB_TRIANGLE)) {
                            otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_CYMBAL;
                        }

                        if (psx.buttonPressed(PSB_PAD_LEFT) || psx.buttonPressed(PSB_PAD_RIGHT) || psx.buttonPressed(PSB_PAD_DOWN)) {
                            otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_DRUML;
                        }

                        if (psx.buttonPressed(PSB_CROSS) || psx.buttonPressed(PSB_CIRCLE) || psx.buttonPressed(PSB_SQUARE)) {
                            otherIoStatus |= P2IO_OTHER_TOYSMARCH_P1_DRUMR;
                        }
                    }
                }
            }
        }
    }
}
#endif