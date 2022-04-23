#include "LUFAConfig.h"

#include <LUFA.h>

#include "p2io.h"

#include "p2io_task.h"

#include "acio.h"
#include "common.h"
#include "config.h"
#include "ddr_extio.h"
#include "icca.h"
#include "thrilldrive_belt.h"
#include "thrilldrive_handle.h"
#include "toysmarch_drumpad.h"

void setup() {
    SetupHardware();          // ask LUFA to setup the hardware
    GlobalInterruptEnable();  // enable global interrupts

    for (int j = 0; j < 2; j++) {
        dongleIsLoaded[j] = false;
        for (int i = 0; i < 40; i++) {
            if (donglePayload[j][i] != 0) {
                dongleIsLoaded[j] = true;
                break;
            }
        }
    }

#if GAME_TYPE == GAMETYPE_DM
    auto aciodev = new acio_device();
    aciodev->add_acio_device(0, new acio_icca_device(0));
    serialDevices[0] = aciodev;
    serialDeviceAvailability = SERIAL_DEVICE_0;
#elif GAME_TYPE == GAMETYPE_GF
    auto aciodev = new acio_device();
    aciodev->add_acio_device(0, new acio_icca_device(0));
    aciodev->add_acio_device(1, new acio_icca_device(1));
    serialDevices[0] = aciodev;
    serialDeviceAvailability = SERIAL_DEVICE_0;
#elif GAME_TYPE == GAMETYPE_DDR
    serialDevices[0] = new extio_device();

    auto aciodev = new acio_device();
    aciodev->add_acio_device(0, new acio_icca_device(0));
    aciodev->add_acio_device(1, new acio_icca_device(1));
    serialDevices[1] = aciodev;

    serialDeviceAvailability = SERIAL_DEVICE_1 | SERIAL_DEVICE_0;
#elif GAME_TYPE == GAMETYPE_THRILLDRIVE
    auto aciodev = new acio_device();
    aciodev->add_acio_device(0, new thrilldrive_handle_device());
    aciodev->add_acio_device(1, new thrilldrive_belt_device());
    serialDevices[1] = aciodev;
    serialDeviceAvailability = SERIAL_DEVICE_1;
#elif GAME_TYPE == GAMETYPE_TOYSMARCH
    serialDevices[0] = new toysmarch_drumpad_device();
    serialDeviceAvailability = SERIAL_DEVICE_0;
#endif
}

void loop() {
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    Input_Task();
    P2IO_Task();
    USB_USBTask();
}
