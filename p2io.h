#ifndef _P2IO_H_
#define _P2IO_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/LUFA/Drivers/USB/USB.h>
#include <LUFA/LUFA/Platform/Platform.h>

/* Function Prototypes: */
#ifdef __cplusplus
extern "C" {
#endif
void SetupHardware(void);

void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_ControlRequest(void);

/* Data Types: */
extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;

#ifdef __cplusplus
}
#endif
#endif
