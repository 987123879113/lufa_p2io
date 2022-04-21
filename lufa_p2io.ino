/**
   You should have a LUFAConfig.h for this to work.
*/
#include "LUFAConfig.h"

/**
   Include LUFA.h after LUFAConfig.h
*/
#include <LUFA.h>

/**
   Finally include the LUFA device setup header
*/
#include "p2io.h"
#include "p2io_task.h"

void setup()
{
   SetupHardware();         // ask LUFA to setup the hardware
   GlobalInterruptEnable(); // enable global interrupts
}

void loop()
{
   CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
   Input_Task();
   P2IO_Task();
   USB_USBTask();
}
