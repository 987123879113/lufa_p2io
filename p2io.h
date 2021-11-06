#ifndef _P2IO_H_
#define _P2IO_H_

/* Includes: */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>

#include "Descriptors.h"

#include <LUFA/LUFA/Drivers/Board/LEDs.h>
#include <LUFA/LUFA/Drivers/Board/Joystick.h>
#include <LUFA/LUFA/Drivers/USB/USB.h>
#include <LUFA/LUFA/Platform/Platform.h>

/** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
#define LEDMASK_USB_NOTREADY LEDS_LED1

/** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
#define LEDMASK_USB_ENUMERATING (LEDS_LED2 | LEDS_LED3)

/** LED mask for the library LED driver, to indicate that the USB interface is ready. */
#define LEDMASK_USB_READY (LEDS_LED2 | LEDS_LED4)

/** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
#define LEDMASK_USB_ERROR (LEDS_LED1 | LEDS_LED3)

/* Function Prototypes: */
#ifdef __cplusplus
extern "C"
{
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
