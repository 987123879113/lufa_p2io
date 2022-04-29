#include "profile.h"

#include <Arduino.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#include "acio.h"
#include "common.h"
#include "config.h"
#include "ddr_extio.h"
#include "icca.h"
#include "lcd_task.h"
#include "p2io_task.h"
#include "thrilldrive_belt.h"
#include "thrilldrive_handle.h"
#include "toysmarch_drumpad.h"

void loadGameProfile(int profileId) {
    curLoadedProfileId = profileId;

    if (curLoadedProfileId < 0 || curLoadedProfileId >= gameProfileCount)
        curLoadedProfileId = 0;

    // Don't save the corrected profile ID to avoid writing to EEPROM as much as possible.
    // The next time the user selects a new profile it should save properly.
    EEPROM.update(0, profileId);

    gameType = pgm_read_byte(&gameProfiles[curLoadedProfileId].gameType);

    // Reset device states
    if (serialDevices[0] != nullptr)
        delete serialDevices[0];

    if (serialDevices[1] != nullptr)
        delete serialDevices[1];

    serialDevices[0] = nullptr;
    serialDevices[1] = nullptr;

    if (gameType == GAMETYPE_DM) {
        auto aciodev = new acio_device();
        aciodev->add_acio_device(0, new acio_icca_device(&playerCards[0]));
        serialDevices[0] = aciodev;
        serialDeviceAvailability = SERIAL_DEVICE_0;
    } else if (gameType == GAMETYPE_GF) {
        auto aciodev = new acio_device();
        aciodev->add_acio_device(0, new acio_icca_device(&playerCards[0]));
        aciodev->add_acio_device(1, new acio_icca_device(&playerCards[1]));
        serialDevices[0] = aciodev;
        serialDeviceAvailability = SERIAL_DEVICE_0;
    } else if (gameType == GAMETYPE_DDR) {
        serialDevices[0] = new extio_device();

        auto aciodev = new acio_device();
        aciodev->add_acio_device(0, new acio_icca_device(&playerCards[0]));
        aciodev->add_acio_device(1, new acio_icca_device(&playerCards[1]));
        serialDevices[1] = aciodev;

        serialDeviceAvailability = SERIAL_DEVICE_1 | SERIAL_DEVICE_0;
    } else if (gameType == GAMETYPE_THRILLDRIVE) {
        auto aciodev = new acio_device();
        aciodev->add_acio_device(0, new thrilldrive_handle_device());
        aciodev->add_acio_device(1, new thrilldrive_belt_device());
        serialDevices[1] = aciodev;
        serialDeviceAvailability = SERIAL_DEVICE_1;
    } else if (gameType == GAMETYPE_TOYSMARCH) {
        serialDevices[0] = new toysmarch_drumpad_device();
        serialDeviceAvailability = SERIAL_DEVICE_0;
    }

    P2IO_Reset();
    LCD_ResetState();

    delay(250);
}