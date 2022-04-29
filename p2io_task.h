#ifndef _P2IO_TASK_H_
#define _P2IO_TASK_H_

#include <stdint.h>

#include "input_device.h"

constexpr int SERIAL_DEVICE_0 = 1;
constexpr int SERIAL_DEVICE_1 = 2;

void P2IO_Reset();
void P2IO_Task();

extern uint32_t jammaIoStatus, otherIoStatus, cardIoStatus;
extern uint16_t analogIoStatus[3];
extern uint16_t coinsInserted[2];
extern input_device* serialDevices[2];
extern uint8_t serialDeviceAvailability;
extern int8_t guitarKnobValue[2];

#endif