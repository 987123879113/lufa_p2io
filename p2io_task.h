#ifndef _P2IO_TASK_H_
#define _P2IO_TASK_H_

#include <stdint.h>

#include "input_device.h"

/* Function Prototypes: */
#ifdef __cplusplus
extern "C" {
#endif
void P2IO_Task();

extern uint32_t jammaIoStatus, otherIoStatus, cardIoStatus;
extern uint16_t analogIoStatus[3];
extern uint16_t coinsInserted[2];
extern input_device* serialDevices[2];

#ifdef __cplusplus
}
#endif
#endif
