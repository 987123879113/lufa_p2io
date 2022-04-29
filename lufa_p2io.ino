#include "LUFAConfig.h"

#include <EEPROM.h>
#include <LUFA.h>

#include "config.h"

#include "p2io.h"

#include "profile.h"

#include "lcd_task.h"
#include "p2io_task.h"

void setup() {
    // Initialize the EEPROM if needed.
    // Checking if various values in EEPROM are 5, 7, 3 should be enough to know that the EEPROM has been initialized for this program.
    if (EEPROM.read(EEPROM_IDX_CHECK1) != 5 || EEPROM.read(EEPROM_IDX_CHECK2) != 7 || EEPROM.read(EEPROM_IDX_CHECK3) != 3) {
        EEPROM.update(EEPROM_IDX_PROFILE, 0); // Current profile ID
        EEPROM.update(EEPROM_IDX_DIPSW, 0); // Current DIPSW value
        EEPROM.update(EEPROM_IDX_CHECK1, 5);
        EEPROM.update(EEPROM_IDX_CHECK2, 7);
        EEPROM.update(EEPROM_IDX_CHECK3, 3);
    }

    loadGameProfile(EEPROM.read(0));

    LCD_Init();

#if INPUT_METHOD == INPUT_METHOD_PS4USB
    Input_Init_PS4USB();
#elif INPUT_METHOD == INPUT_METHOD_PSX
    Input_Init_PSX();
#endif

    SetupHardware();          // ask LUFA to setup the hardware
    GlobalInterruptEnable();  // enable global interrupts
}

void loop() {
    LCD_Task();

#if INPUT_METHOD == INPUT_METHOD_PS4USB
    Input_Task_PS4USB();
#elif INPUT_METHOD == INPUT_METHOD_PSX
    Input_Task_PSX();
#endif

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    P2IO_Task();
    USB_USBTask();
}
